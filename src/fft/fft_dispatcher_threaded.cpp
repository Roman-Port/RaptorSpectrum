#include "fft_dispatcher_threaded.h"
#include <cstring>
#include <cassert>

template<class IN_T>
fft_dispatcher_threaded<IN_T>::fft_dispatcher_threaded(fft_dispatcher_handler<IN_T>* handler, int fft_bins) :
	handler(handler),
	bins(fft_bins),
	buffers {
		fft_generator<IN_T>(fft_bins),
		fft_generator<IN_T>(fft_bins)
	},
	active_writer(0),
	active_writer_use(0),
	pending_operations(0),
	worker_thread(fft_dispatcher_threaded<IN_T>::worker_static, this)
{
}

template<class IN_T>
fft_dispatcher_threaded<IN_T>::~fft_dispatcher_threaded()
{
	//Signal worker to end
	{
		std::unique_lock<std::mutex> uLock(lock);
		pending_operations = -1;
	}

	//Wait for worker to exit
	evt.notify_one();
	worker_thread.join();
}

template<class IN_T>
int fft_dispatcher_threaded<IN_T>::push(IN_T* samples, int count)
{
	//Enter lock
	lock.lock();

	//Loop
	int written = 0;
	while (count > 0) {
		//Skip
		this->apply_skip(&samples, &count);
		if (count <= 0)
			break;

		//Check if the worker is currently overloaded
		if (pending_operations == GENERATOR_THREADED_BUFFERS) {
			//Overloaded! Drop the frame...
			handler->fft_samples_dropped(count);
			break;
		}
		else {
			//Determine how much can be written at the moment
			int writable = std::min(count, bins - active_writer_use);

			//Write to the current buffer
			memcpy(&(buffers[active_writer].buffer_in[active_writer_use]), samples, sizeof(IN_T) * writable);

			//Advance state
			active_writer_use += writable;
			samples += writable;
			written += writable;
			count -= writable;

			//Check if we can submit a frame
			if (active_writer_use == bins) {
				//Reset state, transferring samples to keep into the next buffer
				int nextWriter = (active_writer + 1) % GENERATOR_THREADED_BUFFERS;
				int keep = this->reset_skip();
				assert(keep >= 0 && keep <= bins);
				active_writer_use = keep;
				memcpy(buffers[nextWriter].buffer_in, &buffers[active_writer].buffer_in[bins - keep], sizeof(IN_T) * keep); // Move latest samples to the beginning
				active_writer = nextWriter;

				//Trigger worker to process this, sending an event
				pending_operations++;
				evt.notify_one();
			}
		}
	}

	//Unlock
	lock.unlock();

	return written;
}

template<class IN_T>
void fft_dispatcher_threaded<IN_T>::worker()
{
	int next_buffer = 0;
	while (true) {
		//Wait for a request
		{
			std::unique_lock<std::mutex> uLock(lock);
			while (pending_operations == 0)
				evt.wait(uLock);
			if (pending_operations == -1)
				break;
		}

		//Compute active item
		buffers[next_buffer].compute();

		//Push
		handler->fft_frame_computed(&buffers[next_buffer]);

		//Update local counter
		next_buffer = (next_buffer + 1) % GENERATOR_THREADED_BUFFERS;

		//Update operations counter
		{
			std::unique_lock<std::mutex> uLock(lock);
			pending_operations--;
		}
	}
}

template<class IN_T>
void* fft_dispatcher_threaded<IN_T>::worker_static(void* opaque)
{
	((fft_dispatcher_threaded<IN_T>*)opaque)->worker();
	return 0;
}

template class fft_dispatcher_threaded<fftwf_complex>;
template class fft_dispatcher_threaded<float>;
