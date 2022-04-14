#include "server_impl.h"

template <class IN_T>
fft_server_impl<IN_T>::fft_server_impl() :
	sample_rate(0)
{
	//Clear clients
	for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
		clients[i] = 0;
}

template <class IN_T>
fft_server_impl<IN_T>::~fft_server_impl() {
	//Destroy all clients and their structs
	for (int i = 0; i < SERVER_MAX_CLIENTS; i++) {
		if (clients[i] != 0) {
			delete(clients[i]);
			clients[i] = 0;
		}
	}
}

template <class IN_T>
void fft_server_impl<IN_T>::push(IN_T* samples, int count) {
	//Lock for thread safety
	lock.lock();

	//Forward to each
	for (int i = 0; i < SERVER_MAX_CLIENTS; i++) {
		if (clients[i] != 0) {
			clients[i]->push(samples, count);
		}
	}

	//Unlock
	lock.unlock();
}

template <class IN_T>
void fft_server_impl<IN_T>::set_sample_rate(float sample_rate) {
	//Lock for thread safety
	lock.lock();

	//Set internally
	this->sample_rate = sample_rate;

	//Set on each
	for (int i = 0; i < SERVER_MAX_CLIENTS; i++) {
		if (clients[i] != 0) {
			clients[i]->set_target_sample_rate(sample_rate);
		}
	}

	//Unlock
	lock.unlock();
}

template <class IN_T>
float fft_server_impl<IN_T>::get_sample_rate() {
	//Lock, fetch, and unlock to make thread safe
	lock.lock();
	float rate = sample_rate;
	lock.unlock();
	return rate;
}

template<class IN_T>
int fft_server_impl<IN_T>::create_client(raptorspectrum_client_t** result, int fft_bins, int output_size, float output_fps, unsigned int flags)
{
	//Lock
	lock.lock();

	//Find a free index to use
	(*result) = 0;
	for (int i = 0; i < SERVER_MAX_CLIENTS; i++) {
		if (clients[i] == 0) {
			//Create
			fft_client_impl<IN_T>* client = new fft_client_impl<IN_T>(fft_bins, output_size, flags);
			clients[i] = client;
			(*result) = client;

			//Configure
			client->set_target_fps(output_fps);
			client->set_target_sample_rate(sample_rate);

			break;
		}
	}

	//Unlock
	lock.unlock();

	return (*result) == 0 ? RAPTORSPECTRUM_RESULT_MAX_CLIENTS_REACHED : RAPTORSPECTRUM_RESULT_OK;;
}

template<class IN_T>
int fft_server_impl<IN_T>::destroy_client(ifft_client* target)
{
	//Lock for thread safety
	lock.lock();

	//Search
	int result = RAPTORSPECTRUM_RESULT_CLIENT_NOT_FOUND;
	for (int i = 0; i < SERVER_MAX_CLIENTS; i++) {
		if (clients[i] != 0 && clients[i] == target) {
			delete(clients[i]);
			clients[i] = 0;
			result = RAPTORSPECTRUM_RESULT_OK;
		}
	}

	//Unlock
	lock.unlock();

	return result;
}

/* COMPLEX IMPLEMENTATION */

template class fft_server_impl<raptorspectrum_complex>;

template <>
int fft_server_impl<raptorspectrum_complex>::push_complex(raptorspectrum_complex* samples, int count) {
	push(samples, count);
	return RAPTORSPECTRUM_RESULT_OK;
}

template <>
int fft_server_impl<raptorspectrum_complex>::push_real(float* samples, int count) {
	return RAPTORSPECTRUM_RESULT_INCORRECT_TYPE;
}

/* REAL IMPLEMENTATION */

template class fft_server_impl<float>;

template <>
int fft_server_impl<float>::push_complex(raptorspectrum_complex* samples, int count) {
	return RAPTORSPECTRUM_RESULT_INCORRECT_TYPE;
}

template <>
int fft_server_impl<float>::push_real(float* samples, int count) {
	push(samples, count);
	return RAPTORSPECTRUM_RESULT_OK;
}