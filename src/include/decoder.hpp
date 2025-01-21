#ifndef DECODER_HPP
#define DECODER_HPP
#include <cstdint>

uint8_t* decode_mp3_to_pcm(const char *input_filename, int64_t start_time_ms, int64_t *pcm_data_size);
#endif
