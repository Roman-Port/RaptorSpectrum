#include "fft_dispatcher_realtime.h"
#include <algorithm>
#include <cassert>
#include <cstring>

template<class IN_T>
fft_dispatcher_realtime<IN_T>::fft_dispatcher_realtime(fft_dispatcher_handler<IN_T>* handler, int fft_bins) :
	handler(handler),
	bins(fft_bins),
	fft(fft_bins),
	use(0)
{
}

template<class IN_T>
fft_dispatcher_realtime<IN_T>::~fft_dispatcher_realtime()
{
}

template<class IN_T>
int fft_dispatcher_realtime<IN_T>::push(IN_T* samples, int count)
{
	int written = 0;
	while (count > 0) {
		//Skip
		this->apply_skip(&samples, &count);

		//Determine how much can be written at the moment
		int writable = std::min(count, bins - use);

		//Write to the current buffer
		memcpy(&fft.buffer_in[use], samples, sizeof(IN_T) * writable);

		//Advance state
		use += writable;
		samples += writable;
		written += writable;
		count -= writable;

		//Check if we can submit a frame
		if (use == bins) {
			//Compute
			fft.compute();

			//Push
			handler->fft_frame_computed(&fft);

			//Reset, keeping the needed samples
			int keep = this->reset_skip();
			assert(keep >= 0 && keep <= fft.bins);
			use = keep;
			memcpy(fft.buffer_in, &fft.buffer_in[fft.bins - keep], sizeof(IN_T) * keep); // Move latest samples to the beginning
		}
	}
	return 0;
}

template class fft_dispatcher_realtime<fftwf_complex>;
template class fft_dispatcher_realtime<float>;
