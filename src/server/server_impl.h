#pragma once

#include "iserver.h"
#include "../client/client_impl.h"
#include <mutex>

#define SERVER_MAX_CLIENTS 16

template <class IN_T>
class fft_server_impl : public ifft_server {

public:
	fft_server_impl();
	virtual ~fft_server_impl();
	virtual int push_complex(raptorspectrum_complex* samples, int count) override;
	virtual int push_real(float* samples, int count) override;
	virtual void set_sample_rate(float sample_rate) override;
	virtual float get_sample_rate() override;
	virtual int create_client(raptorspectrum_client_t** result, int fft_bins, int output_size, float output_fps, unsigned int flags) override;
	virtual int destroy_client(ifft_client* client) override;

private:
	void push(IN_T* samples, int count);

	std::mutex lock;
	fft_client_impl<IN_T>* clients[SERVER_MAX_CLIENTS];
	float sample_rate;

};