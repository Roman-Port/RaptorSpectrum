#include "post_process.h"
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstring>

template<class IN_T>
post_processor<IN_T>::post_processor() :
    input_size(-1),
    output_size(-1),
    attack(1),
    decay(1),
    first_output_frame(true),
    power_buffer(NULL),
    resized_buffer(NULL),
    output_buffer(NULL)
{
}

template<class IN_T>
post_processor<IN_T>::~post_processor()
{
    //Free buffers
    if (power_buffer != NULL) {
        free(power_buffer);
        power_buffer = NULL;
    }
    if (resized_buffer != NULL) {
        free(resized_buffer);
        resized_buffer = NULL;
    }
    if (output_buffer != NULL) {
        free(output_buffer);
        output_buffer = NULL;
    }
}

void resize_power(const float* input, int oldWidth, float* output, int newWidth) {
    float gx;
    int gxi;
    for (int x = 0; x < newWidth; x++) {
        //Calculate
        gx = std::min(x / (float)(newWidth) * (oldWidth)-0.5f, oldWidth - 1.0f);
        gxi = (int)gx;

        //Sanity check
        assert(gxi + 1 < oldWidth);

        //Apply lerp function
        output[x] = input[gxi] + (input[gxi + 1] - input[gxi]) * (gx - gxi);
    }
}

void smoothen_power(const float* input, float* buffer, int count, float attack, float decay)
{
    float ratio;
    for (int i = 0; i < count; i++)
    {
        ratio = buffer[i] < input[i] ? attack : decay;
        buffer[i] = buffer[i] * (1 - ratio) + input[i] * ratio;
    }
}

void recreate_buffer(float** buffer, int newSize) {
    //Free old buffer
    if ((*buffer) != NULL)
        free((*buffer));

    //Create new buffer
    (*buffer) = (float*)malloc(sizeof(float) * newSize);

    //Verify
    assert((*buffer) != NULL);
}

float clamp_stat(float value) {
    if (value > 1)
        return 1;
    if (value < 0)
        return 0;
    return value;
}

template<class IN_T>
float* post_processor<IN_T>::process(fft_generator<IN_T>* fft, int outputCount)
{
    //Lock
    lock.lock();

    //Get the size of the power
    size_t powerSize = fft->get_power_size();

    //Check to see if buffers need to be resized
    if (input_size != powerSize || power_buffer == NULL) {
        recreate_buffer(&power_buffer, powerSize);
        input_size = powerSize;
    }
    if (output_size != outputCount || resized_buffer == NULL || output_buffer == NULL) {
        recreate_buffer(&resized_buffer, outputCount);
        recreate_buffer(&output_buffer, outputCount);
        output_size = outputCount;
        first_output_frame = true;
    }

    //Wrap around and convert to dB
    fft->calculate_power(power_buffer, powerSize);

    //Resize
    resize_power(power_buffer, powerSize, resized_buffer, output_size);

    //Smoothen (or just copy if first)
    if (first_output_frame)
        memcpy(output_buffer, resized_buffer, sizeof(float) * output_size);
    else
        smoothen_power(resized_buffer, output_buffer, output_size, attack, decay);
    first_output_frame = false;

    //Unlock
    lock.unlock();

    return output_buffer;
}

template<class IN_T>
void post_processor<IN_T>::set_attack(float attack)
{
    lock.lock();
    this->attack = clamp_stat(attack);
    lock.unlock();
}

template<class IN_T>
float post_processor<IN_T>::get_attack()
{
    lock.lock();
    float value = attack;
    lock.unlock();
    return value;
}

template<class IN_T>
void post_processor<IN_T>::set_decay(float decay)
{
    lock.lock();
    this->decay = clamp_stat(decay);
    lock.unlock();
}

template<class IN_T>
float post_processor<IN_T>::get_decay()
{
    lock.lock();
    float value = decay;
    lock.unlock();
    return value;
}


template class post_processor<fftwf_complex>;
template class post_processor<float>;
