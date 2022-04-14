#include <raptorspectrum.h>
#include "iclient.h"
#include "client_impl.h"

#define CONTEXT ((ifft_client*)ctx)

void raptorspectrum_client_set_callback_frame_processed(raptorspectrum_client_t* ctx, raptorspectrum_client_frame_processed_cb cb, void* cbctx)
{
	CONTEXT->set_callback_frame_processed(cb, cbctx);
}

void raptorspectrum_client_set_callback_samples_dropped(raptorspectrum_client_t* ctx, raptorspectrum_client_samples_dropped_cb cb, void* cbctx)
{
	CONTEXT->set_callback_samples_dropped(cb, cbctx);
}

void raptorspectrum_client_set_target_fps(raptorspectrum_client_t* ctx, float fps)
{
	CONTEXT->set_target_fps(fps);
}

float raptorspectrum_client_get_target_fps(raptorspectrum_client_t* ctx)
{
	return CONTEXT->get_target_fps();
}

void raptorspectrum_client_set_output_size(raptorspectrum_client_t* ctx, int size)
{
	CONTEXT->set_output_size(size);
}

float raptorspectrum_client_get_output_size(raptorspectrum_client_t* ctx)
{
	return CONTEXT->get_output_size();
}

void raptorspectrum_client_set_attack(raptorspectrum_client_t* ctx, float attack)
{
	CONTEXT->set_attack(attack);
}

float raptorspectrum_client_get_attack(raptorspectrum_client_t* ctx)
{
	return CONTEXT->get_attack();
}

void raptorspectrum_client_set_decay(raptorspectrum_client_t* ctx, float decay)
{
	CONTEXT->set_decay(decay);
}

float raptorspectrum_client_get_decay(raptorspectrum_client_t* ctx)
{
	return CONTEXT->get_decay();
}
