/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-07-16     Zero-Free    first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <wavhdr.h>
#include <wavrecorder.h>

#define DBG_TAG              "WAV_RECORDER"
#define DBG_LVL              DBG_INFO
#include <rtdbg.h>

struct recorder
{
    rt_device_t device;
    struct wavrecord_info info;
    struct rt_event *event;
    struct rt_completion ack;
    rt_uint8_t *buffer;
    FILE *fp;
    rt_bool_t activated;
};

enum RECORD_EVENT
{
    RECORD_EVENT_STOP  = 0x01,
    RECORD_EVENT_START = 0x02,
};

#define WR_BUFFER_SIZE (2048)

static struct recorder record;

static rt_err_t wavrecorder_open(struct recorder *record)
{
    rt_err_t result = RT_EOK;

    record->device = rt_device_find(PKG_WP_RECORD_DEVICE);
    if (record->device == RT_NULL)
    {
        LOG_E("device %s not find", PKG_WP_RECORD_DEVICE);
        return -RT_ERROR;
    }

    /* malloc internal buffer */
    record->buffer = rt_malloc(WR_BUFFER_SIZE);
    if (record->buffer == RT_NULL)
    {
        result = -RT_ENOMEM;
        LOG_E("malloc internal buffer for recorder failed");
        goto __exit;
    }
    rt_memset(record->buffer, 0, WR_BUFFER_SIZE);

    /* open file */
    record->fp = fopen(record->info.uri, "wb+");
    if (record->fp == RT_NULL)
    {
        result = -RT_ERROR;
        LOG_E("open file %s failed", record->info.uri);
        goto __exit;
    }

    /* open micphone device */
    result = rt_device_open(record->device, RT_DEVICE_OFLAG_RDONLY);
    if (result != RT_EOK)
    {
        result = -RT_ERROR;
        LOG_E("open %s device faield", PKG_WP_RECORD_DEVICE);
        goto __exit;
    }

    record->event = rt_event_create("wav_r", RT_IPC_FLAG_FIFO);
    if (record->event == RT_NULL)
    {
        result = -RT_ERROR;
        LOG_E("create event for wav recorder failed");
        goto __exit;
    }

    return RT_EOK;

__exit:
    if (record->buffer)
    {
        rt_free(record->buffer);
        record->buffer = RT_NULL;
    }

    if (record->fp)
    {
        fclose(record->fp);
        record->fp = RT_NULL;
    }

    if (record->device)
    {
        rt_device_close(record->device);
        record->device = RT_NULL;
    }

    if (record->event)
    {
        rt_event_delete(record->event);
        record->event = RT_NULL;
    }

    return result;
}

void wavrecorder_close(struct recorder *record)
{
    if (record->buffer)
    {
        rt_free(record->buffer);
        record->buffer = RT_NULL;
    }

    if (record->fp)
    {
        fclose(record->fp);
        record->fp = RT_NULL;
    }

    if (record->device)
    {
        rt_device_close(record->device);
        record->device = RT_NULL;
    }

    if (record->event)
    {
        rt_event_delete(record->event);
        record->event = RT_NULL;
    }
}

static void wavrecord_entry(void *parameter)
{
    rt_err_t result;
    rt_size_t size;
    struct wav_header wav = {0};
    struct rt_audio_caps caps;
    rt_uint32_t recv_evt, total_length = 0;

    result = wavrecorder_open(&record);
    if (result != RT_EOK)
    {
        LOG_E("open wav recorder failed");
        return;
    }

    record.activated = RT_TRUE;

    /* write 44 bytes wavheader */
    fwrite(&wav, 44, 1, record.fp);

    rt_kprintf("Information:\n");
    rt_kprintf("samplerate %d\n", record.info.samplerate);
    rt_kprintf("channels %d\n", record.info.channels);

    /* set sampletate,channels, samplebits */
    caps.main_type = AUDIO_TYPE_INPUT;
    caps.sub_type  = AUDIO_DSP_PARAM;
    caps.udata.config.samplerate = record.info.samplerate;
    caps.udata.config.channels = record.info.channels;
    caps.udata.config.samplebits = 16;
    rt_device_control(record.device, AUDIO_CTL_CONFIGURE, &caps);

    LOG_D("ready to record, device %s, uri %s", PKG_WP_PLAY_DEVICE, record.info.uri);

    while (1)
    {
        /* read raw data from sound device */
        size =  rt_device_read(record.device, 0, record.buffer, WR_BUFFER_SIZE);
        if (size)
        {
            fwrite(record.buffer, size, 1, record.fp);
            total_length += size;
        }

        /* recive stop event */
        if (rt_event_recv(record.event, RECORD_EVENT_STOP,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, &recv_evt) == RT_EOK)
        {

            /* re-write wav header */
            wavheader_init(&wav, record.info.samplerate, record.info.channels, total_length);
            fseek(record.fp, 0, SEEK_SET);
            wavheader_write(&wav, record.fp);
            wavrecorder_close(&record);

            LOG_D("total_length = %d", total_length);

            /* ack event */
            rt_completion_done(&record.ack);
            record.activated = RT_FALSE;

            break;
        }
    }

    return;
}

rt_err_t wavrecorder_start(struct wavrecord_info *info)
{
    if (record.activated != RT_TRUE)
    {
        rt_thread_t tid;

        if (record.info.uri)
            rt_free(record.info.uri);
        record.info.uri = rt_strdup(info->uri);

        record.info.samplerate = info->samplerate;
        record.info.channels   = info->channels;
        record.info.samplebits = info->samplebits;

        tid = rt_thread_create("wav_r", wavrecord_entry, RT_NULL, 2048, 19, 20);
        if (tid)
            rt_thread_startup(tid);
    }

    return RT_EOK;
}

rt_err_t wavrecorder_stop(void)
{
    if (record.activated == RT_TRUE)
    {
        rt_completion_init(&record.ack);
        rt_event_send(record.event, RECORD_EVENT_STOP);
        rt_completion_wait(&record.ack, RT_WAITING_FOREVER);
    }

    return RT_EOK;
}

rt_bool_t wavrecorder_is_actived(void)
{
    return record.activated;
}
