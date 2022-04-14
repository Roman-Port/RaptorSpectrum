#pragma once

#include "fft.h"
#include "fft_dispatcher_base.h"

template <class IN_T>
class fft_dispatcher_realtime : public fft_dispatcher_base<IN_T> {

public:
	fft_dispatcher_realtime(fft_dispatcher_handler<IN_T>* handler, int fft_bins);
	virtual ~fft_dispatcher_realtime() override;
	virtual int push(IN_T* samples, int count) override;

private:
	fft_dispatcher_handler<IN_T>* handler;
	int bins;
	fft_generator<IN_T> fft;
	int use;

};