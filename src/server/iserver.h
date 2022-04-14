#pragma once

#include <raptorspectrum.h>
#include "../client/iclient.h"

class ifft_server : public raptorspectrum_server_t {

public:
	virtual int push_complex(raptorspectrum_complex* samples, int count) = 0;
	virtual int push_real(float* samples, int count) = 0;
	virtual void set_sample_rate(float sample_rate) = 0;
	virtual float get_sample_rate() = 0;
	virtual int create_client(raptorspectrum_client_t** result, int fft_bins, int output_size, float output_fps, unsigned int flags) = 0;
	virtual int destroy_client(ifft_client* client) = 0;

};