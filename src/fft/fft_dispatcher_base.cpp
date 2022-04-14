#include "fft_dispatcher_base.h"
#include <algorithm>

template<class IN_T>
fft_dispatcher_base<IN_T>::fft_dispatcher_base() :
	target_skip(0),
	remaining_skip(0)
{
}

template<class IN_T>
fft_dispatcher_base<IN_T>::~fft_dispatcher_base()
{
}

template<class IN_T>
void fft_dispatcher_base<IN_T>::set_skip(int skip)
{
	if (skip >= 0) {
		target_skip = skip;
		remaining_skip = skip;
		keep = 0;
	}
	else {
		target_skip = 0;
		remaining_skip = 0;
		keep = -skip;
	}
}

template<class IN_T>
void fft_dispatcher_base<IN_T>::apply_skip(IN_T** samples, int* count)
{
	int skippable = std::min((*count), remaining_skip);
	(*samples) += skippable;
	(*count) -= skippable;
	remaining_skip -= skippable;
}

template<class IN_T>
int fft_dispatcher_base<IN_T>::reset_skip()
{
	remaining_skip = target_skip;
	return keep;
}

template class fft_dispatcher_base<fftwf_complex>;
template class fft_dispatcher_base<float>;