#include "fft.h"
#include <raptorspectrum.h>
#include <cassert>
#include <math.h>

template <class IN_T>
fft_generator<IN_T>::fft_generator(size_t bins) :
	bins(bins),
	buffer_in(0),
	buffer_out(0)
{
	//Sanity check
	assert(bins > 0);

	//Create buffers
	buffer_in = (IN_T*)fftwf_malloc(sizeof(IN_T) * bins);
	assert(buffer_in != 0);
	buffer_out = (raptorspectrum_complex*)fftwf_malloc(sizeof(raptorspectrum_complex) * bins);
	assert(buffer_out != 0);

	//Create the plan
	plan = create_plan((int)bins, buffer_in, buffer_out);

	//Calculate normalization
	power_normalization = 1.0f / get_power_size();
}

template <class IN_T>
void fft_generator<IN_T>::compute() {
	//Process
	fftwf_execute(plan);
}

template <class IN_T>
fft_generator<IN_T>::~fft_generator() {
	//Destroy plan
	fftwf_destroy_plan(plan);

	//Destroy buffers
	fftwf_free(buffer_in);
	fftwf_free(buffer_out);
}

template<class IN_T>
void fft_generator<IN_T>::calculate_power(float* output, size_t output_size) {
	assert(output_size == get_power_size());
	float real;
	float imag;
	int in_index;
	for (int i = 0; i < output_size; i++)
	{
		in_index = wrap_output_index(i);
		real = buffer_out[in_index][0] * power_normalization;
		imag = buffer_out[in_index][1] * power_normalization;
		output[i] = 10.0f * log10f(((real * real) + (imag * imag)) + 1e-20);
	}
}

#define DEFAULT_FLAGS FFTW_MEASURE

/* COMPLEX IMPLEMENTATION */

template class fft_generator<raptorspectrum_complex>;

template <>
fftwf_plan fft_generator<raptorspectrum_complex>::create_plan(int bins, raptorspectrum_complex* input, fftwf_complex* output) {
	return fftwf_plan_dft_1d(bins, (fftwf_complex*)input, output, FFTW_FORWARD, DEFAULT_FLAGS);
}

template<>
size_t fft_generator<raptorspectrum_complex>::get_power_size()
{
	return bins;
}

template<>
int fft_generator<raptorspectrum_complex>::wrap_output_index(int index)
{
	return (index + (bins / 2)) % bins;
}

/* REAL IMPLEMENTATION */

template class fft_generator<float>;

template <>
fftwf_plan fft_generator<float>::create_plan(int bins, float* input, fftwf_complex* output) {
	return fftwf_plan_dft_r2c_1d(bins, input, output, DEFAULT_FLAGS);
}

template<>
size_t fft_generator<float>::get_power_size()
{
	return bins / 2; // Real spectrums only use half of the bins
}

template<>
int fft_generator<float>::wrap_output_index(int index)
{
	return index;
}
