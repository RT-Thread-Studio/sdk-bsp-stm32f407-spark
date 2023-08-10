/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-07-16     Zero-Free    first implementation
 */

#ifndef __WAVRECORDER_H__
#define __WAVRECORDER_H__

#include <rtthread.h>

struct wavrecord_info
{
    char *uri;
    rt_uint32_t samplerate;
    rt_uint16_t channels;
    rt_uint16_t samplebits;
};

/**
 * @brief             Start to record
 *
 * @param info        wavfile informations
 *
 * @return
 *      - RT_EOK      Success
 *      - < 0         Failed
 */
rt_err_t wavrecorder_start(struct wavrecord_info *info);

/**
 * @brief             Stop record
 *
 * @return
 *      - RT_EOK      Success
 *      - < 0         Failed
 */
rt_err_t wavrecorder_stop(void);

/**
 * @brief             Get recorder status
 *
 * @return
 *      - RT_TRUE     actived
 *      - RT_FALSE    non-actived
 */
rt_bool_t wavrecorder_is_actived(void);

#endif
