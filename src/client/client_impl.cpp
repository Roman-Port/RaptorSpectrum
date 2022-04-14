#include "client_impl.h"
#include "../fft/fft_dispatcher_threaded.h"
#include "../fft/fft_dispatcher_realtime.h"

void dummy_handler_frames_processed(void* ctx, float* frame, int frame_size) {}
void dummy_handler_samples_dropped(void* ctx, int dropped_samples) {}

template<class IN_T>
fft_client_impl<IN_T>::fft_client_impl(int fft_bins, int output_size, unsigned int flags) :
	fft_bins(fft_bins),
	frame_processed_cb(&dummy_handler_frames_processed),
	frame_processed_ctx(0),
	samples_dropped_cb(&dummy_handler_samples_dropped),
	samples_dropped_ctx(0),
	dispatcher(NULL),
	target_sample_rate(0),
	target_fps(0),
	output_size(output_size)
{
	//Create the correct dispatcher from the flags
	if (flags & RAPTORSPECTRUM_CLIENT_FLAG_REALTIME)
		dispatcher = new fft_dispatcher_realtime<IN_T>(this, fft_bins);
	else
		dispatcher = new fft_dispatcher_threaded<IN_T>(this, fft_bins);
}

template<class IN_T>
fft_client_impl<IN_T>::~fft_client_impl()
{
	//Clean up dispatcher
	if (dispatcher != NULL) {
		delete (dispatcher);
		dispatcher = NULL;
	}
}

template<class IN_T>
void fft_client_impl<IN_T>::push(IN_T* samples, int count)
{
	input_lock.lock();
	dispatcher->push(samples, count);
	input_lock.unlock();
}

template<class IN_T>
void fft_client_impl<IN_T>::set_target_fps(float fps)
{
	input_lock.lock();
	target_fps = std::max(fps, 0.0f);
	configure_skip();
	input_lock.unlock();
}

template<class IN_T>
float fft_client_impl<IN_T>::get_target_fps()
{
	input_lock.lock();
	float value = target_fps;
	input_lock.unlock();
	return value;
}

template<class IN_T>
void fft_client_impl<IN_T>::set_target_sample_rate(float sample_rate)
{
	input_lock.lock();
	target_sample_rate = std::max(sample_rate, 0.0f);
	configure_skip();
	input_lock.unlock();
}

template<class IN_T>
float fft_client_impl<IN_T>::get_target_sample_rate()
{
	input_lock.lock();
	float value = target_sample_rate;
	input_lock.unlock();
	return value;
}

template<class IN_T>
void fft_client_impl<IN_T>::set_output_size(int size)
{
	output_lock.lock();
	output_size = size;
	output_lock.unlock();
}

template<class IN_T>
int fft_client_impl<IN_T>::get_output_size()
{
	output_lock.lock();
	int value = output_size;
	output_lock.unlock();
	return value;
}

template<class IN_T>
void fft_client_impl<IN_T>::set_attack(float attack)
{
	post.set_attack(attack);
}

template<class IN_T>
float fft_client_impl<IN_T>::get_attack()
{
	return post.get_attack();
}

template<class IN_T>
void fft_client_impl<IN_T>::set_decay(float decay)
{
	post.set_decay(decay);
}

template<class IN_T>
float fft_client_impl<IN_T>::get_decay()
{
	return post.get_decay();
}

template<class IN_T>
void fft_client_impl<IN_T>::set_callback_frame_processed(raptorspectrum_client_frame_processed_cb cb, void* ctx)
{
	output_lock.lock();
	frame_processed_cb = cb;
	frame_processed_ctx = ctx;
	output_lock.unlock();
}

template<class IN_T>
void fft_client_impl<IN_T>::set_callback_samples_dropped(raptorspectrum_client_samples_dropped_cb cb, void* ctx)
{
	output_lock.lock();
	samples_dropped_cb = cb;
	samples_dropped_ctx = ctx;
	output_lock.unlock();
}

template<class IN_T>
void fft_client_impl<IN_T>::configure_skip()
{
	//Calculate the samples per frame
	int samplesPerFrame;
	if (target_fps == 0)
		samplesPerFrame = 0;
	else
		samplesPerFrame = (int)(target_sample_rate / target_fps);

	//Set skip
	dispatcher->set_skip(samplesPerFrame - fft_bins);
}

template<class IN_T>
void fft_client_impl<IN_T>::fft_frame_computed(fft_generator<IN_T>* fft)
{
	//Lock
	output_lock.lock();

	//Apply post-processing
	int size = output_size;
	float* buffer = post.process(fft, size);

	//Unlock
	output_lock.unlock();

	//Send event
	frame_processed_cb(frame_processed_ctx, buffer, size);
}

template<class IN_T>
void fft_client_impl<IN_T>::fft_samples_dropped(int nsamples)
{
	//Send event
	samples_dropped_cb(samples_dropped_ctx, nsamples);
}

template class fft_client_impl<fftwf_complex>;
template class fft_client_impl<float>;