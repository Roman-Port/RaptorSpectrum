#pragma once

#include <raptorspectrum.h>

class ifft_client : public raptorspectrum_client_t {

public:
	virtual void set_target_fps(float fps) = 0;
	virtual float get_target_fps() = 0;

	virtual void set_target_sample_rate(float sample_rate) = 0;
	virtual float get_target_sample_rate() = 0;

	virtual void set_output_size(int size) = 0;
	virtual int get_output_size() = 0;

	virtual void set_attack(float attack) = 0;
	virtual float get_attack() = 0;

	virtual void set_decay(float decay) = 0;
	virtual float get_decay() = 0;

	virtual void set_callback_frame_processed(raptorspectrum_client_frame_processed_cb cb, void* ctx) = 0;
	virtual void set_callback_samples_dropped(raptorspectrum_client_samples_dropped_cb cb, void* ctx) = 0;

};