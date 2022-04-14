#pragma once

#include "fft.h"
#include "fft_dispatcher_base.h"
#include <mutex>
#include <thread>

#define GENERATOR_THREADED_BUFFERS 2

template <class IN_T>
class fft_dispatcher_threaded : public fft_dispatcher_base<IN_T> {

public:
	fft_dispatcher_threaded(fft_dispatcher_handler<IN_T>* handler, int fft_bins);
	virtual ~fft_dispatcher_threaded() override;
	virtual int push(IN_T* samples, int count) override;

private:
	fft_dispatcher_handler<IN_T>* handler;
	int bins;
	fft_generator<IN_T> buffers[GENERATOR_THREADED_BUFFERS];
	int active_writer;
	int active_writer_use;

	std::mutex lock;
	std::condition_variable evt;
	int pending_operations; //Number of buffers with data waiting to be processed; Set to -1 when exiting. Mutex protected.

	std::thread worker_thread;

	void worker();
	static void* worker_static(void* opaque);

};