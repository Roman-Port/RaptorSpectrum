#pragma once

#include <raptorspectrum.h>
#include "fft.h"

template <class IN_T>
class fft_dispatcher_handler {

public:
	virtual void fft_frame_computed(fft_generator<IN_T>* fft) = 0;
	virtual void fft_samples_dropped(int nsamples) = 0;

};

template <class IN_T>
class fft_dispatcher_base {

public:
	fft_dispatcher_base();
	virtual ~fft_dispatcher_base();
	virtual int push(IN_T* samples, int count) = 0;

	/// <summary>
	/// Sets the number of input samples to skip PER OUTPUT FRAME.
	/// </summary>
	/// <param name="skip"></param>
	void set_skip(int skip);

protected:
	void apply_skip(IN_T** samples, int* count); // Should be run just before pushing; advances sample and decrements count
	int reset_skip(); // Should be called right after a new frame is pushed; returns the number of samples to KEEP

private:
	int target_skip; // The value set via set_skip
	int remaining_skip; // Number of samples needed to be skipped
	int keep; // Number of samples to keep after each frame

};