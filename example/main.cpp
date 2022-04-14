#include <raptorspectrum.h>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <cassert>

#define BUFFER_SIZE 49152

void processed(void* ctx, float* frame, int frame_size) {
	//printf("frame\n");
	fwrite(frame, sizeof(float), frame_size, (FILE*)ctx);
}

void dropped(void* ctx, int dropped_samples) {
	printf("dropped\n");
}

void run_test(const char* input_filename, const char* output_filename, float sample_rate, bool is_complex, bool is_realtime) {
	//Allocate buffers
	int16_t* buffer_file = (int16_t*)malloc(sizeof(int16_t) * BUFFER_SIZE);
	assert(buffer_file != 0);
	float* buffer = (float*)malloc(sizeof(float) * BUFFER_SIZE);
	assert(buffer != 0);

	//Open files
	FILE* input = fopen(input_filename, "rb");
	fseek(input, 44, SEEK_SET);
	FILE* output = fopen(output_filename, "wb");

	//Create and configure server
	raptorspectrum_server_t* server;
	raptorspectrum_server_create(&server, is_complex ? RAPTORSPECTRUM_TYPE_COMPLEX : RAPTORSPECTRUM_TYPE_REAL);
	raptorspectrum_server_set_sample_rate(server, sample_rate);

	//Create and configure client
	raptorspectrum_client_t* client;
	raptorspectrum_server_add_client(server, &client, 32768, 1024, 30, is_realtime ? RAPTORSPECTRUM_CLIENT_FLAG_REALTIME : 0);
	raptorspectrum_client_set_callback_frame_processed(client, processed, output);
	raptorspectrum_client_set_callback_samples_dropped(client, dropped, NULL);
	raptorspectrum_client_set_attack(client, 0.3f);
	raptorspectrum_client_set_decay(client, 0.4f);

	//Enter processing loop
	int read;
	do {
		//Read
		read = fread(buffer_file, sizeof(int16_t), BUFFER_SIZE, input);

		//Convert
		for (int i = 0; i < read; i++)
			buffer[i] = buffer_file[i] / 32768.0f;

		//Process
		if (is_complex)
			raptorspectrum_server_push_complex(server, (raptorspectrum_complex*)buffer, read / 2);
		else
			raptorspectrum_server_push_real(server, buffer, read);

		//Wait if we aren't realtime
		if (!is_realtime)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	} while (read != 0);

	//Destroy client and server
	raptorspectrum_server_remove_client(server, client);
	raptorspectrum_server_destroy(server);

	//Close files
	fclose(input);
	fclose(output);

	//Free buffers
	free(buffer_file);
	free(buffer);
}

int main()
{
	run_test("C:\\Users\\Roman\\Desktop\\Unpacked IQ\\92500000Hz KQRS-FM - Bob Seger - Night Moves.wav", "C:\\Users\\Roman\\Desktop\\offset_test.wav", 900001, true, true);
	//run_test("C:\\Users\\Roman\\Desktop\\output.wav", "C:\\Users\\Roman\\Desktop\\offset_test.wav", 187500, false, true);

	return 0;
}