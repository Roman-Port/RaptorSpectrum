#pragma once

#include "iclient.h"
#include "post_process.h"
#include "../fft/fft_dispatcher_base.h"

template <class IN_T>
class fft_client_impl : public ifft_client, private fft_dispatcher_handler<IN_T> {

public:
	fft_client_impl(int fft_bins, int output_size, unsigned int flags);
	~fft_client_impl();
	void push(IN_T* samples, int count);

	virtual void set_target_fps(float fps) override; //we must make thread-safe
	virtual float get_target_fps() override; //we must make thread-safe

	virtual void set_target_sample_rate(float sample_rate) override; //we must make thread-safe
	virtual float get_target_sample_rate() override; //we must make thread-safe

	virtual void set_output_size(int size) override; //we must make thread-safe
	virtual int get_output_size() override;  //we must make thread-safe

	virtual void set_attack(float attack) override;
	virtual float get_attack() override;

	virtual void set_decay(float decay) override;
	virtual float get_decay() override;

	virtual void set_callback_frame_processed(raptorspectrum_client_frame_processed_cb cb, void* ctx) override;
	virtual void set_callback_samples_dropped(raptorspectrum_client_samples_dropped_cb cb, void* ctx) override;

private:
	raptorspectrum_client_frame_processed_cb frame_processed_cb;
	void* frame_processed_ctx;
	raptorspectrum_client_samples_dropped_cb samples_dropped_cb;
	void* samples_dropped_ctx;

	std::mutex input_lock;
	std::mutex output_lock;

	int fft_bins;
	fft_dispatcher_base<IN_T>* dispatcher;

	int output_size;
	post_processor<IN_T> post;

	float target_sample_rate;
	float target_fps;

	void configure_skip(); // Called when the target FPS or sample rate is changed; Sets up skip again

	virtual void fft_frame_computed(fft_generator<IN_T>* fft) override;
	virtual void fft_samples_dropped(int nsamples) override;

};