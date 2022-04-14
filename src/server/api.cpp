#include <raptorspectrum.h>
#include "iserver.h"
#include "server_impl.h"

#define CONTEXT ((ifft_server*)ctx)

int raptorspectrum_server_create(raptorspectrum_server_t** result, unsigned int type)
{
	switch (type) {
	case RAPTORSPECTRUM_TYPE_COMPLEX:
		(*result) = new fft_server_impl<raptorspectrum_complex>();
		return RAPTORSPECTRUM_RESULT_OK;
	case RAPTORSPECTRUM_TYPE_REAL:
		(*result) = new fft_server_impl<float>();
		return RAPTORSPECTRUM_RESULT_OK;
	default:
		(*result) = 0;
		return RAPTORSPECTRUM_RESULT_INCORRECT_TYPE;
	}
}

int raptorspectrum_server_push_complex(raptorspectrum_server_t* ctx, raptorspectrum_complex* samples, int count) {
	return CONTEXT->push_complex(samples, count);
}

int raptorspectrum_server_push_real(raptorspectrum_server_t* ctx, float* samples, int count) {
	return CONTEXT->push_real(samples, count);
}

void raptorspectrum_server_set_sample_rate(raptorspectrum_server_t* ctx, float sample_rate) {
	CONTEXT->set_sample_rate(sample_rate);
}

float raptorspectrum_server_get_sample_rate(raptorspectrum_server_t* ctx) {
	return CONTEXT->get_sample_rate();
}

int raptorspectrum_server_add_client(raptorspectrum_server_t* ctx, raptorspectrum_client_t** result, int fft_bins, int output_size, float output_fps, unsigned int flags)
{
	return CONTEXT->create_client(result, fft_bins, output_size, output_fps, flags);
}

int raptorspectrum_server_remove_client(raptorspectrum_server_t* ctx, raptorspectrum_client_t* client)
{
	return CONTEXT->destroy_client((ifft_client*)client);
}

void raptorspectrum_server_destroy(raptorspectrum_server_t* ctx)
{
	delete(CONTEXT);
}