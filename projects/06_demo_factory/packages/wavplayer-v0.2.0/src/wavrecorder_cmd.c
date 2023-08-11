/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-07-16     Zero-Free    first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <optparse.h>
#include <wavrecorder.h>

#include <stdlib.h>

enum WAVRECORDER_ACTTION
{
    WAVRECORDER_ACTION_HELP   = 0,
    WAVRECORDER_ACTION_START  = 1,
    WAVRECORDER_ACTION_STOP   = 2,
};

struct wavrecord_args
{
    int action;
    char *file;
    rt_uint16_t samplerate;
    rt_uint16_t channels;
    rt_uint16_t samplebits;
};

static struct optparse_long opts[] =
{
    {"help", 'h', OPTPARSE_NONE    },       /* 帮助 */
    {"start", 's', OPTPARSE_REQUIRED},      /* 开始录音 */
    {"stop", 't', OPTPARSE_NONE    },       /* 停止录音 */
    { NULL,  0,  OPTPARSE_NONE    }
};

static void usage(void)
{
    rt_kprintf("usage: wavrecord [option] [target] ...\n\n");
    rt_kprintf("usage options:\n");
    rt_kprintf("  -h,     --help                        Print defined help message.\n");
    rt_kprintf("  -s file --start=file  <samplerate> <channels> <samplebits> \n");
    rt_kprintf("                                        record wav music to filesystem.\n");
    rt_kprintf("  -t,     --stop                        Stop record.\n");
}

int wavrecord_args_prase(int argc, char *argv[], struct wavrecord_args *record_args)
{
    int ch;
    int option_index;
    struct optparse options;
    rt_err_t result = RT_EOK;

    if (argc == 1)
    {
        record_args->action = WAVRECORDER_ACTION_HELP;
        return RT_EOK;
    }

    /* Parse cmd */
    optparse_init(&options, argv);
    while ((ch = optparse_long(&options, opts, &option_index)) != -1)
    {
        switch (ch)
        {
        case 'h':
            record_args->action = WAVRECORDER_ACTION_HELP;
            break;

        case 's':
            record_args->action = WAVRECORDER_ACTION_START;
            record_args->file = options.optarg;
            record_args->samplerate = ((argv[3] == RT_NULL) ? 8000 : atoi(argv[3]));
            record_args->channels = ((argv[4] == RT_NULL) ? 2 : atoi(argv[4]));
            record_args->samplebits = ((argv[5] == RT_NULL) ? 16 : atoi(argv[5]));
            break;

        case 't':
            record_args->action = WAVRECORDER_ACTION_STOP;
            break;

        default:
            result = -RT_EINVAL;
            break;
        }
    }

    return result;
}

int wav_recorder(int argc, char *argv[])
{
    int result = RT_EOK;
    struct wavrecord_args record_args = {0};
    struct wavrecord_info info = {0};

    result = wavrecord_args_prase(argc, argv, &record_args);
    if (result != RT_EOK)
    {
        usage();
        return result;
    }

    switch (record_args.action)
    {
    case WAVRECORDER_ACTION_HELP:
        usage();
        break;

    case WAVRECORDER_ACTION_START:
        info.uri = record_args.file;
        info.samplerate = record_args.samplerate;
        info.channels = record_args.channels;
        info.samplebits = record_args.samplebits;
        wavrecorder_start(&info);
        break;

    case WAVRECORDER_ACTION_STOP:
        wavrecorder_stop();
        break;

    default:
        result = -RT_ERROR;
        break;
    }

    return result;
}

MSH_CMD_EXPORT_ALIAS(wav_recorder, wavrecord, record wav music);
