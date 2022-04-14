#pragma once

#include <fftw3.h>

#if defined(_WIN32)
#define EXPORT_API extern "C" __declspec( dllexport )
#elif defined(__GNUC__)
#define EXPORT_API extern "C" __attribute__((visibility("default")))
#else
#define EXPORT_API extern "C"
#endif

#define RAPTORSPECTRUM_RESULT_OK 0
#define RAPTORSPECTRUM_RESULT_INCORRECT_TYPE -1
#define RAPTORSPECTRUM_RESULT_OUT_OF_MEMORY -2
#define RAPTORSPECTRUM_RESULT_CLIENT_NOT_FOUND -3
#define RAPTORSPECTRUM_RESULT_MAX_CLIENTS_REACHED -4

#define RAPTORSPECTRUM_TYPE_COMPLEX 1
#define RAPTORSPECTRUM_TYPE_REAL 2

/* TYPES */

typedef fftwf_complex raptorspectrum_complex;
typedef struct raptorspectrum_server_t { void* placeholder; };
typedef struct raptorspectrum_client_t { void* placeholder; };

typedef void (*raptorspectrum_client_frame_processed_cb)(void* ctx, float* frame, int frame_size);
typedef void (*raptorspectrum_client_samples_dropped_cb)(void* ctx, int dropped_samples);

/* SERVER */

EXPORT_API int raptorspectrum_server_create(raptorspectrum_server_t** result, unsigned int type);
EXPORT_API int raptorspectrum_server_push_complex(raptorspectrum_server_t* ctx, raptorspectrum_complex* samples, int count);
EXPORT_API int raptorspectrum_server_push_real(raptorspectrum_server_t* ctx, float* samples, int count);
EXPORT_API void raptorspectrum_server_set_sample_rate(raptorspectrum_server_t* ctx, float sample_rate);
EXPORT_API float raptorspectrum_server_get_sample_rate(raptorspectrum_server_t* ctx);
EXPORT_API int raptorspectrum_server_add_client(raptorspectrum_server_t* ctx, raptorspectrum_client_t** result, int fft_bins, int output_size, float output_fps, unsigned int flags);
EXPORT_API int raptorspectrum_server_remove_client(raptorspectrum_server_t* ctx, raptorspectrum_client_t* client);
EXPORT_API void raptorspectrum_server_destroy(raptorspectrum_server_t* ctx);

/* CLIENT */

#define RAPTORSPECTRUM_CLIENT_FLAG_REALTIME (1 << 0)

EXPORT_API void raptorspectrum_client_set_callback_frame_processed(raptorspectrum_client_t* ctx, raptorspectrum_client_frame_processed_cb cb, void* cbctx);
EXPORT_API void raptorspectrum_client_set_callback_samples_dropped(raptorspectrum_client_t* ctx, raptorspectrum_client_samples_dropped_cb cb, void* cbctx);
EXPORT_API void raptorspectrum_client_set_target_fps(raptorspectrum_client_t* ctx, float fps);
EXPORT_API float raptorspectrum_client_get_target_fps(raptorspectrum_client_t* ctx);
EXPORT_API void raptorspectrum_client_set_output_size(raptorspectrum_client_t* ctx, int size);
EXPORT_API float raptorspectrum_client_get_output_size(raptorspectrum_client_t* ctx);
EXPORT_API void raptorspectrum_client_set_attack(raptorspectrum_client_t* ctx, float attack);
EXPORT_API float raptorspectrum_client_get_attack(raptorspectrum_client_t* ctx);
EXPORT_API void raptorspectrum_client_set_decay(raptorspectrum_client_t* ctx, float decay);
EXPORT_API float raptorspectrum_client_get_decay(raptorspectrum_client_t* ctx);