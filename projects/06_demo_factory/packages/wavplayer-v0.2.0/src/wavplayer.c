/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-07-15     Zero-Free    first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <wavhdr.h>
#include <wavplayer.h>

#define DBG_TAG              "WAV_PLAYER"
#define DBG_LVL              DBG_INFO
#include <rtdbg.h>

#define VOLUME_MIN (0)
#define VOLUME_MAX (99)

#define WP_BUFFER_SIZE (2048)
#define WP_VOLUME_DEFAULT (55)
#define WP_MSG_SIZE (10)
#define WP_THREAD_STATCK_SIZE (2048)
#define WP_THREAD_PRIORITY (15)

enum MSG_TYPE
{
    MSG_NONE   = 0,
    MSG_START  = 1,
    MSG_STOP   = 2,
    MSG_PAUSE  = 3,
    MSG_RESUME = 4,
};

enum PLAYER_EVENT
{
    PLAYER_EVENT_NONE   = 0,
    PLAYER_EVENT_PLAY   = 1,
    PLAYER_EVENT_STOP   = 2,
    PLAYER_EVENT_PAUSE  = 3,
    PLAYER_EVENT_RESUME = 4,
};

struct play_msg
{
    int type;
    void *data;
};

struct wavplayer
{
    int state;
    char *uri;
    char *buffer;
    rt_device_t device;
    rt_mq_t mq;
    rt_mutex_t lock;
    struct rt_completion ack;
    FILE *fp;
    int volume;
};

static struct wavplayer player;

#if (DBG_LEVEL >= DBG_LOG)

static const char *state_str[] =
{
    "STOPPED",
    "PLAYING",
    "PAUSED",
};

static const char *event_str[] =
{
    "NONE",
    "PLAY"
    "STOP"
    "PAUSE"
    "RESUME"
};

#endif

static void play_lock(void)
{
    rt_mutex_take(player.lock, RT_WAITING_FOREVER);
}

static void play_unlock(void)
{
    rt_mutex_release(player.lock);
}

static rt_err_t play_msg_send(struct wavplayer *player, int type, void *data)
{
    struct play_msg msg;

    msg.type = type;
    msg.data = data;

    return rt_mq_send(player->mq, &msg, sizeof(struct play_msg));
}

int wavplayer_play(char *uri)
{
    rt_err_t result = RT_EOK;

    rt_completion_init(&player.ack);

    play_lock();
    if (player.state != PLAYER_STATE_STOPED)
    {
        wavplayer_stop();
    }
    if (player.uri)
    {
        rt_free(player.uri);
    }
    player.uri = rt_strdup(uri);
    result = play_msg_send(&player, MSG_START, RT_NULL);
    rt_completion_wait(&player.ack, RT_WAITING_FOREVER);
    play_unlock();

    return result;
}

int wavplayer_stop(void)
{
    rt_err_t result = RT_EOK;

    rt_completion_init(&player.ack);

    play_lock();
    if (player.state != PLAYER_STATE_STOPED)
    {
        result = play_msg_send(&player, MSG_STOP, RT_NULL);
        rt_completion_wait(&player.ack, RT_WAITING_FOREVER);
    }
    play_unlock();

    return result;
}

int wavplayer_pause(void)
{
    rt_err_t result = RT_EOK;

    rt_completion_init(&player.ack);

    play_lock();
    if (player.state == PLAYER_STATE_PLAYING)
    {
        result = play_msg_send(&player, MSG_PAUSE, RT_NULL);
        rt_completion_wait(&player.ack, RT_WAITING_FOREVER);
    }
    play_unlock();

    return result;
}

int wavplayer_resume(void)
{
    rt_err_t result = RT_EOK;

    rt_completion_init(&player.ack);

    play_lock();
    if (player.state == PLAYER_STATE_PAUSED)
    {
        result = play_msg_send(&player, MSG_RESUME, RT_NULL);
        rt_completion_wait(&player.ack, RT_WAITING_FOREVER);
    }
    play_unlock();

    return result;
}

int wavplayer_volume_set(int volume)
{
    struct rt_audio_caps caps;

    if (volume < VOLUME_MIN)
        volume = VOLUME_MIN;
    else if (volume > VOLUME_MAX)
        volume = VOLUME_MAX;

    player.device = rt_device_find(PKG_WP_PLAY_DEVICE);
    if (player.device == RT_NULL)
        return -RT_ERROR;

    player.volume = volume;
    caps.main_type = AUDIO_TYPE_MIXER;
    caps.sub_type  = AUDIO_MIXER_VOLUME;
    caps.udata.value = volume;

    LOG_D("set volume = %d", volume);
    return rt_device_control(player.device, AUDIO_CTL_CONFIGURE, &caps);
}

int wavplayer_volume_get(void)
{
    return player.volume;
}

int wavplayer_state_get(void)
{
    return player.state;
}

char *wavplayer_uri_get(void)
{
    return player.uri;
}

static rt_err_t wavplayer_open(struct wavplayer *player)
{
    rt_err_t result = RT_EOK;
    struct rt_audio_caps caps;
    struct wav_header wav;

    /* find device */
    player->device = rt_device_find(PKG_WP_PLAY_DEVICE);
    if (player->device == RT_NULL)
    {
        LOG_E("device %s not find", PKG_WP_PLAY_DEVICE);
        return - RT_ERROR;
    }

    /* open file */
    player->fp = fopen(player->uri, "rb");
    if (player->fp == RT_NULL)
    {
        LOG_E("open file %s failed", player->uri);
        result = -RT_ERROR;
        goto __exit;
    }

    /* open sound device */
    result = rt_device_open(player->device, RT_DEVICE_OFLAG_WRONLY);
    if (result != RT_EOK)
    {
        LOG_E("open %s device faield", PKG_WP_PLAY_DEVICE);
        goto __exit;
    }

    LOG_D("open wavplayer, device %s", PKG_WP_PLAY_DEVICE);
    /* read wavfile header information from file */
    wavheader_read(&wav, player->fp);

    rt_kprintf("Information:\n");
    rt_kprintf("samplerate %d\n", wav.fmt_sample_rate);
    rt_kprintf("channels %d\n", wav.fmt_channels);
    rt_kprintf("sample bits width %d\n", wav.fmt_bit_per_sample);

    /* set sampletate,channels, samplebits */
    caps.main_type = AUDIO_TYPE_OUTPUT;
    caps.sub_type  = AUDIO_DSP_PARAM;
    caps.udata.config.samplerate = wav.fmt_sample_rate;
    caps.udata.config.channels = wav.fmt_channels;
    caps.udata.config.samplebits = wav.fmt_bit_per_sample;
    rt_device_control(player->device, AUDIO_CTL_CONFIGURE, &caps);

    /* set volume according to configuration */
    caps.main_type = AUDIO_TYPE_MIXER;
    caps.sub_type  = AUDIO_MIXER_VOLUME;
    caps.udata.value = player->volume;
    rt_device_control(player->device, AUDIO_CTL_CONFIGURE, &caps);

    return RT_EOK;

__exit:
    if (player->fp)
    {
        fclose(player->fp);
        player->fp = RT_NULL;
    }

    if (player->device)
    {
        rt_device_close(player->device);
        player->device = RT_NULL;
    }

    return result;
}


static void wavplayer_close(struct wavplayer *player)
{
    if (player->fp)
    {
        fclose(player->fp);
        player->fp = RT_NULL;
    }

    if (player->device)
    {
        rt_device_close(player->device);
        player->device = RT_NULL;
    }

    LOG_D("close wavplayer");
}

static int wavplayer_event_handler(struct wavplayer *player, int timeout)
{
    int event;
    rt_err_t result;
    struct play_msg msg;
#if (DBG_LEVEL >= DBG_LOG)
    rt_uint8_t last_state;
#endif

    result = rt_mq_recv(player->mq, &msg, sizeof(struct play_msg), timeout);
    if (result != RT_EOK)
    {
        event = PLAYER_EVENT_NONE;
        return event;
    }

#if (DBG_LEVEL >= DBG_LOG)
    last_state = player->state;
#endif

    switch (msg.type)
    {
    case MSG_START:
        event = PLAYER_EVENT_PLAY;
        player->state = PLAYER_STATE_PLAYING;
        break;

    case MSG_STOP:
        event = PLAYER_EVENT_STOP;
        player->state = PLAYER_STATE_STOPED;
        break;

    case MSG_PAUSE:
        event = PLAYER_EVENT_PAUSE;
        player->state = PLAYER_STATE_PAUSED;
        break;

    case MSG_RESUME:
        event = PLAYER_EVENT_RESUME;
        player->state = PLAYER_STATE_PLAYING;
        break;

    default:
        event = PLAYER_EVENT_NONE;
        break;
    }

    rt_completion_done(&player->ack);

#if (DBG_LEVEL >= DBG_LOG)
    LOG_D("EVENT:%s, STATE:%s -> %s", event_str[event], state_str[last_state], state_str[player->state]);
#endif

    return event;
}

static void wavplayer_entry(void *parameter)
{
    rt_err_t result = RT_EOK;
    rt_int32_t size;
    int event;

    player.buffer = rt_malloc(WP_BUFFER_SIZE);
    if (player.buffer == RT_NULL)
        return;
    rt_memset(player.buffer, 0, WP_BUFFER_SIZE);

    player.mq = rt_mq_create("wav_p", 10, sizeof(struct play_msg), RT_IPC_FLAG_FIFO);
    if (player.mq == RT_NULL)
        goto __exit;

    player.lock = rt_mutex_create("wav_p", RT_IPC_FLAG_FIFO);
    if (player.lock == RT_NULL)
        goto __exit;

    player.volume = WP_VOLUME_DEFAULT;

    while (1)
    {
        /* wait play event forever */
        event = wavplayer_event_handler(&player, RT_WAITING_FOREVER);
        if (event != PLAYER_EVENT_PLAY)
            continue;

        /* open wavplayer */
        result = wavplayer_open(&player);
        if (result != RT_EOK)
        {
            player.state = PLAYER_STATE_STOPED;
            LOG_I("open wav player failed");
            continue;
        }

        LOG_I("play start, uri=%s", player.uri);
        while (1)
        {
            event = wavplayer_event_handler(&player, RT_WAITING_NO);

            switch (event)
            {
            case PLAYER_EVENT_NONE:
            {
                /* read raw data from file stream */
                size = fread(player.buffer, WP_BUFFER_SIZE, 1, player.fp);
                if (size != 1)
                {
                    /* FILE END*/
                    player.state = PLAYER_STATE_STOPED;
                }
                else
                {
                    /*witte data to sound device*/
                    rt_device_write(player.device, 0, player.buffer, WP_BUFFER_SIZE);
                }
                break;
            }

            case PLAYER_EVENT_PAUSE:
            {
                /* wait resume or stop event forever */
                event = wavplayer_event_handler(&player, RT_WAITING_FOREVER);
            }

            default:
                break;
            }

            if (player.state == PLAYER_STATE_STOPED)
                break;
        }

        /* close wavplayer */
        wavplayer_close(&player);
        LOG_I("play end");
    }

__exit:
    if (player.buffer)
    {
        rt_free(player.buffer);
        player.buffer = RT_NULL;
    }

    if (player.mq)
    {
        rt_mq_delete(player.mq);
        player.mq = RT_NULL;
    }

    if (player.lock)
    {
        rt_mutex_delete(player.lock);
        player.lock = RT_NULL;
    }
}


int wavplayer_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("wav_p",
                           wavplayer_entry,
                           RT_NULL,
                           WP_THREAD_STATCK_SIZE,
                           WP_THREAD_PRIORITY, 10);
    if (tid)
        rt_thread_startup(tid);

    return RT_EOK;
}

INIT_APP_EXPORT(wavplayer_init);
