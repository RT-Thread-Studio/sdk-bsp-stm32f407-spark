/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-06-28     Zero-Free    first implementation
 */

#include <wavhdr.h>
#include <string.h>

/* read and write integer from file stream */
static int get_int(FILE *fp)
{
    char *s;
    int i;
    s = (char *)&i;
    size_t len = sizeof(int);
    int n = 0;
    for (; n < len; n++)
    {
        s[n] = getc(fp);
    }
    return i;
}

static int put_int(int i, FILE *fp)
{
    char *s;
    s = (char *)&i;
    size_t len = sizeof(int);
    int n = 0;
    for (; n < len; n++)
    {
        putc(s[n], fp);
    }

    return i;
}

static short int get_sint(FILE *fp)
{
    char *s;
    short int i;
    s = (char *)&i;
    size_t len = sizeof(short);
    int n = 0;
    for (; n < len; n++)
    {
        s[n] = getc(fp);
    }

    return i;
}

static short int put_sint(short int i, FILE *fp)
{
    char *s;
    s = (char *)&i;
    size_t len = sizeof(short);
    int n = 0;
    for (; n < len; n++)
    {
        putc(s[n], fp);
    };

    return i;
}

int wavheader_init(struct wav_header *header, int sample_rate, int channels, int datasize)
{
    if (header == NULL)
        return -1;

    memcpy(header->riff_id, "RIFF", 4);
    header->riff_datasize = datasize + 44 - 8;

    memcpy(header->riff_type, "WAVE", 4);

    memcpy(header->fmt_id, "fmt ", 4);
    header->fmt_datasize = 16;
    header->fmt_compression_code = 1;
    header->fmt_channels = channels;
    header->fmt_sample_rate = sample_rate;
    header->fmt_avg_bytes_per_sec = header->fmt_sample_rate * header->fmt_channels * header->fmt_bit_per_sample / 8;
    header->fmt_block_align = header->fmt_bit_per_sample * header->fmt_channels / 8;
    header->fmt_bit_per_sample = 16;

    memcpy(header->data_id, "data", 4);
    header->data_datasize = datasize;

    return 0;
}

int wavheader_read(struct wav_header *header, FILE *fp)
{
    if (fp == NULL)
        return -1;

    fread(header->riff_id, 4, 1, fp);
    header->riff_datasize = get_int(fp);
    fread(header->riff_type, 4, 1, fp);
    fread(header->fmt_id, 4, 1, fp);
    header->fmt_datasize = get_int(fp);
    header->fmt_compression_code = get_sint(fp);
    header->fmt_channels = get_sint(fp);
    header->fmt_sample_rate = get_int(fp);
    header->fmt_avg_bytes_per_sec = get_int(fp);
    header->fmt_block_align = get_sint(fp);
    header->fmt_bit_per_sample = get_sint(fp);
    fread(header->data_id, 4, 1, fp);
    header->data_datasize = get_int(fp);

    return 0;
}

int wavheader_write(struct wav_header *header, FILE *fp)
{
    if (fp == NULL)
        return -1;

    fwrite(header->riff_id, 4, 1, fp);
    put_int(header->riff_datasize, fp);
    fwrite(header->riff_type, 4, 1, fp);
    fwrite(header->fmt_id, 4, 1, fp);
    put_int(header->fmt_datasize, fp);
    put_sint(header->fmt_compression_code, fp);
    put_sint(header->fmt_channels, fp);
    put_int(header->fmt_sample_rate, fp);
    put_int(header->fmt_avg_bytes_per_sec, fp);
    put_sint(header->fmt_block_align, fp);
    put_sint(header->fmt_bit_per_sample, fp);
    fwrite(header->data_id, 4, 1, fp);
    put_int(header->data_datasize, fp);

    return 0;
}

void wavheader_print(struct wav_header *header)
{
    printf("header.riff_id: %c%c%c%c\n", header->riff_id[0], header->riff_id[1], header->riff_id[2], header->riff_id[3]);
    printf("header.riff_datasize: %d\n", header->riff_datasize);
    printf("header.riff_type: %c%c%c%c\n", header->riff_type[0], header->riff_type[1], header->riff_type[2], header->riff_type[3]);
    printf("header.fmt_id: %c%c%c%c\n", header->fmt_id[0], header->fmt_id[1], header->fmt_id[2], header->fmt_id[3]);
    printf("header.fmt_datasize: %d\n", header->fmt_datasize);
    printf("header.fmt_compression_code: %hd\n", header->fmt_compression_code);
    printf("header.fmt_channels: %hd\n", header->fmt_channels);
    printf("header.fmt_sample_rate: %d\n", header->fmt_sample_rate);
    printf("header.fmt_avg_bytes_per_sec: %d\n", header->fmt_avg_bytes_per_sec);
    printf("header.fmt_block_align: %hd\n", header->fmt_block_align);
    printf("header.fmt_bit_per_sample: %hd\n", header->fmt_bit_per_sample);
    printf("header.data_id: %c%c%c%c\n", header->data_id[0], header->data_id[1], header->data_id[2], header->data_id[3]);
    printf("header.data_datasize: %d\n", header->data_datasize);
}
