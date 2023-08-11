/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-07-15     Zero-Free    first implementation
 */

#ifndef __WAVPLAYER_H__
#define __WAVPLAYER_H__

/**
 * wav player status
 */
enum PLAYER_STATE
{
    PLAYER_STATE_STOPED  = 0,
    PLAYER_STATE_PLAYING = 1,
    PLAYER_STATE_PAUSED  = 2,
};

/**
 * @brief             Play wav music
 *
 * @param uri         the pointer for file path
 *
 * @return
 *      - 0      Success
 *      - others Failed
 */
int wavplayer_play(char *uri);

/**
 * @brief             Stop music
 *
 * @return
 *      - 0      Success
 *      - others Failed
 */
int wavplayer_stop(void);

/**
 * @brief             Pause music
 *
 * @return
 *      - 0      Success
 *      - others Failed
 */
int wavplayer_pause(void);

/**
 * @brief             Resume music
 *
 * @return
 *      - 0      Success
 *      - others Failed
 */
int wavplayer_resume(void);

/**
 * @brief             Sev volume
 *
 * @param volume      volume value(0 ~ 99)
 *
 * @return
 *      - 0      Success
 *      - others Failed
 */
int wavplayer_volume_set(int volume);

/**
 * @brief             Get volume
 *
 * @return            volume value(0~00)
 */
int wavplayer_volume_get(void);

/**
 * @brief             Get wav player state
 *
 * @return
 *      - PLAYER_STATE_STOPED   stoped status
 *      - PLAYER_STATE_PLAYING  playing status
 *      - PLAYER_STATE_PAUSED   paused
 */
int wavplayer_state_get(void);

/**
 * @brief             Get the uri that is currently playing
 *
 * @return            uri that is currently playing
 */
char *wavplayer_uri_get(void);

#endif
