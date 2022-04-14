#pragma once

#include <fftw3.h>

template <class IN_T>
class fft_generator {

public:
	fft_generator(size_t bins);
	~fft_generator();

	size_t bins;
	IN_T* buffer_in;
	fftwf_complex* buffer_out;

	void compute();

	size_t get_power_size(); //Get the size of the output power that will be computed by calculate_power
	void calculate_power(float* output, size_t output_size); //Calculates the FFT power and correctly centers it

private:
	fftwf_plan plan;
	float power_normalization;

	int wrap_output_index(int index); //Converts indices to "unwrap" the output
	static fftwf_plan create_plan(int bins, IN_T* input, fftwf_complex* output);


};