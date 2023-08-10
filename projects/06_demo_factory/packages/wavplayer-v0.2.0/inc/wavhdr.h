/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-07-15     Zero-Free    first implementation
 */

#ifndef __WAVHDR_H__
#define __WAVHDR_H__

#include <stdio.h>

struct wav_header
{
    char  riff_id[4];                       /* "RIFF" */
    int   riff_datasize;                    /* RIFF chunk data size,exclude riff_id[4] and riff_datasize,total - 8 */

    char  riff_type[4];                     /* "WAVE" */

    char  fmt_id[4];                        /* "fmt " */
    int   fmt_datasize;                     /* fmt chunk data size,16 for pcm */
    short fmt_compression_code;             /* 1 for PCM */
    short fmt_channels;                     /* 1(mono) or 2(stereo) */
    int   fmt_sample_rate;                  /* samples per second */
    int   fmt_avg_bytes_per_sec;            /* sample_rate * channels * bit_per_sample / 8 */
    short fmt_block_align;                  /* number bytes per sample, bit_per_sample * channels / 8 */
    short fmt_bit_per_sample;               /* bits of each sample(8,16,32). */

    char  data_id[4];                       /* "data" */
    int   data_datasize;                    /* data chunk size,pcm_size - 44 */
};

/**
 * @brief             Initialize wavfile header
 *
 * @param header      the pointer for wavfile header
 * @param channels    wavfile channels
 * @param sample_rate wavfile samplerate
 * @param datasize    wavfile total data size
 *
 * @return
 *      - 0  Success
 *      - -1 Error
 */
int wavheader_init(struct wav_header *header, int sample_rate, int channels, int datasize);

/**
 * @brief             Read wavfile head information from file stream
 *
 * @param header      the pointer for wavfile header
 * @param fp          file stream
 *
 * @return
 *      - 0  Success
 *      - -1 Error
 */
int wavheader_read(struct wav_header *header, FILE *fp);

/**
 * @brief             Write wavfile head information to file stream
 *
 * @param header      the pointer for wavfile header
 * @param fp          file stream
 *
 * @return
 *      - 0  Success
 *      - -1 Error
 */
int wavheader_write(struct wav_header *header, FILE *fp);

/**
 * @brief             Print wavfile header information
 *
 * @param header      the pointer for wavfile header
 *
 * @return
 *      - None
 */
void wavheader_print(struct wav_header *header);

#endif
