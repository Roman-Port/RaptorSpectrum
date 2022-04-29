#pragma once

#include "../fft/fft.h"
#include <fftw3.h>
#include "../threads.h"

template <class IN_T>
class post_processor {

public:
	post_processor();
	~post_processor();

	/// <summary>
	/// Gets power, resizes, and smoothens the output and returns a pointer to the result, valid until the next call.
	/// </summary>
	/// <returns></returns>
	float* process(fft_generator<IN_T>* fft, int outputCount);

	void set_attack(float attack);
	float get_attack();
	void set_decay(float decay);
	float get_decay();

private:
	std::mutex lock;

	int input_size;
	int output_size;

	float attack;
	float decay;

	bool first_output_frame;

	float* power_buffer; // Len=input_size; Holds the power
	float* resized_buffer; // Len=output_size; Holds resized samples
	float* output_buffer; // Len=output_size; Holds resized, smoothened samples


};
