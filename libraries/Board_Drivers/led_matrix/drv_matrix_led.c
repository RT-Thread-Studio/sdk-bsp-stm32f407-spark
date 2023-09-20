/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-22     yuanjie      first version, transfer using PWM method.
 * 2023-09-13     yuanjie      twick few functions.
 */

/**
 *  WS2812B serial LED data timing flow:
 *  | T0H | H | 350ns  | ±150ns   |
 *  | T0L | L | 800ns  | ±150ns   |
 *  | T1H | H | 700ns  | ±150ns   |
 *  | T1L | L | 600ns  | ±150ns   |
 *  | RES | L | ≥50us  | --       |
 *  When using TIM peripheral, to meet 800kHz (1250ns) refresh rate:
 *  - period is:  1250ns
 *  - logic 0 is: 400ns(H) + 900ns(L)
 *  - logic 1 is: 900ns(H) + 400ns(L)
 *  As shown we made a tile to achieve above logic IO transfer:
 *      tile[] = {2, 7};
 *          {2} stands for logic 0's reload value
 *          {7} stands for logic 1's reload value
 *
 *  raw_led_buffer: led pwm counter reload buffers to imitate WS2812B signal.
 *      1 LED color needs 3Byte, contains 24 bit (RGB888),
 *      And 1 bit needs 2 reload value to imitate H and L logic.
 *      So 1 LED need 48 Bytes of RAW data to store.
 */

#include <board.h>
#include <rtthread.h>
#include <drv_matrix_led.h>

#ifdef BSP_USING_LED_MATRIX

#ifndef BSP_ONBOAR_LED_NUMS
    #define BSP_ONBOAR_LED_NUMS 19 // 默认板载LED灯珠个数
#endif /* BSP_ONBOAR_LED_NUMS */

#ifndef BSP_EXT_LED_NUMS
    #define BSP_EXT_LED_NUMS    10 // 默认外部LED灯珠个数
#endif /* BSP_EXT_LED_NUMS */

TIM_HandleTypeDef htim3;

#ifdef BSP_USING_ONBOARD_LED_MATRIX
DMA_HandleTypeDef hdma_tim3_ch2;
#endif /* BSP_USING_ONBOARD_LED_MATRIX */

#ifdef BSP_USING_EXT_LED_MATRIX
// TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim3_ch1;
#endif /* BSP_USING_EXT_LED_MATRIX */

#if defined (BSP_USING_ONBOARD_LED_MATRIX) && defined (BSP_USING_EXT_LED_MATRIX)
    #define LED_TOTAL_NUMS (BSP_ONBOAR_LED_NUMS + BSP_EXT_LED_NUMS)
#elif defined (BSP_USING_ONBOARD_LED_MATRIX)
    #define LED_TOTAL_NUMS  BSP_ONBOAR_LED_NUMS
#elif defined(BSP_USING_EXT_LED_MATRIX)
    #define LED_TOTAL_NUMS  BSP_EXT_LED_NUMS
#endif

#define LED_RAW_BUF_SIZE (LED_TOTAL_NUMS * 24 * 2)

ALIGN(4)
uint8_t raw_led_buffer[LED_RAW_BUF_SIZE];

// 模拟bit码: 2为逻辑0, 7为逻辑1
const uint8_t tile[2] = {2, 7};

const pixel_rgb_t DARK = {0, 0, 0};
const pixel_rgb_t GREEN = {0, 255, 0};
const pixel_rgb_t RED = {255, 0, 0};
const pixel_rgb_t WHITE = {255, 255, 255};
const pixel_rgb_t BLUE = {0, 0, 255};
const pixel_rgb_t LT_RED = {32, 0, 0};
const pixel_rgb_t LT_GREEN = {0, 32, 0};
const pixel_rgb_t LT_BLUE = {0, 0, 32};
const pixel_rgb_t LT_WHITE = {16, 16, 16};

void led_matrix_clear(void);
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

#ifdef BSP_USING_EXT_LED_MATRIX
/**
 * @brief This function handles DMA1 stream4 global interrupt.
 */
void DMA1_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_tim3_ch1);
}
#endif /* BSP_USING_EXT_LED_MATRIX */

#ifdef BSP_USING_ONBOARD_LED_MATRIX
/**
 * @brief This function handles DMA1 stream5 global interrupt.
 */
void DMA1_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_tim3_ch2);
}
#endif /* BSP_USING_ONBOARD_LED_MATRIX */

/**
 * @brief 时序发送完成回调，手动清除占空比（拉低IO）防止误点灯
 * 
 * @param htim 
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        __HAL_TIM_SetCompare(htim, TIM_CHANNEL_1, 0); // 占空比清0
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        __HAL_TIM_SetCompare(htim, TIM_CHANNEL_2, 0); // 占空比清0
    }
}
/**
 * @brief matrix Initialization Function
 * @param None
 * @retval None
 */
int led_matrix_init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* TIM3_CH2 Init */
    __HAL_RCC_TIM3_CLK_ENABLE();

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 10 - 1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 10 - 1; // 840kHz
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.RepetitionCounter = 0;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim3);

    /* TIM3 DMA Init */
    __HAL_RCC_DMA1_CLK_ENABLE();
#ifdef BSP_USING_ONBOARD_LED_MATRIX
    hdma_tim3_ch2.Instance = DMA1_Stream5;
    hdma_tim3_ch2.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch2.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch2.Init.Mode = DMA_NORMAL;
    hdma_tim3_ch2.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_tim3_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim3_ch2) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_LINKDMA(&htim3, hdma[TIM_DMA_ID_CC2], hdma_tim3_ch2);
    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    rt_pin_mode(LED_ONBOARD_MATRIX_EN_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_ONBOARD_MATRIX_EN_PIN, PIN_LOW);
#endif /* BSP_USING_ONBOARD_LED_MATRIX */

#ifdef BSP_USING_EXT_LED_MATRIX
    hdma_tim3_ch1.Instance = DMA1_Stream4;
    hdma_tim3_ch1.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch1.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch1.Init.Mode = DMA_NORMAL;
    hdma_tim3_ch1.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_tim3_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim3_ch1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(&htim3, hdma[TIM_DMA_ID_CC1], hdma_tim3_ch1);
    __HAL_LINKDMA(&htim3, hdma[TIM_DMA_ID_TRIGGER], hdma_tim3_ch1);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

#endif /* BSP_USING_EXT_LED_MATRIX */

    led_matrix_clear();
    return RT_EOK;
}

INIT_APP_EXPORT(led_matrix_init);


/**
 * @brief 使用PWM DMA方式连续发送LED时序
 *
 * @param raw_buffer    含有LED时序的PWM计数值
 * @param size          16bit(2Byte)为一个发送单位
 */
//TODO 阻塞超时处理
//TODO 多线程重入处理
static void tim_dma_send_raw(uint8_t *raw_buffer, uint32_t size)
{
    rt_base_t level = rt_hw_interrupt_disable(); // DMA send 期间多线程访问保护
#if defined (BSP_USING_ONBOARD_LED_MATRIX) && defined (BSP_USING_EXT_LED_MATRIX)
    uint16_t offset = (BSP_ONBOAR_LED_NUMS * 24 * 2);
    int16_t ext_size = size - (BSP_ONBOAR_LED_NUMS * 24);
    if ((ext_size > 0) || (ext_size <= (BSP_EXT_LED_NUMS * 24)))
    {
        // 阻塞判断上次DMA有没有传输完成
        while (HAL_DMA_GetState(&hdma_tim3_ch2) != HAL_DMA_STATE_READY)
            ;
        //先发送板载灯珠
        HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)raw_buffer, size);
        rt_thread_mdelay(1);    //wait for DMA channel throughput down.
        // 阻塞判断上次DMA有没有传输完成
        while (HAL_DMA_GetState(&hdma_tim3_ch1) != HAL_DMA_STATE_READY)
            ;
        //计算index偏移，发送板外灯珠
        HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)(raw_buffer + offset), ext_size);
    }

#elif defined (BSP_USING_ONBOARD_LED_MATRIX)
    // 阻塞判断上次DMA有没有传输完成
    while (HAL_DMA_GetState(&hdma_tim3_ch2) != HAL_DMA_STATE_READY)
        ;
    // 发送一个24bit的RGB数据
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)raw_buffer, size);
#elif defined(BSP_USING_EXT_LED_MATRIX)
    // 阻塞判断上次DMA有没有传输完成
    while (HAL_DMA_GetState(&hdma_tim3_ch1) != HAL_DMA_STATE_READY)
        ;
    // 发送一个24bit的RGB数据
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)raw_buffer, size);
#endif
    rt_hw_interrupt_enable(level);
}

/**
 * @brief reflash raw data to LED Strip using PWM DMA
 *
 */
void led_matrix_reflash(void)
{
    tim_dma_send_raw(raw_led_buffer, LED_RAW_BUF_SIZE / 2);
}

/**
 * @brief 清除LED灯光显示
 *
 */
void led_matrix_clear(void)
{
    for (uint32_t i = 0; i < (LED_RAW_BUF_SIZE / 2); i++)
    {
        raw_led_buffer[(i << 1)] = tile[0];
        raw_led_buffer[(i << 1) + 1] = 0;
    }
    tim_dma_send_raw(raw_led_buffer, LED_RAW_BUF_SIZE / 2);
}
MSH_CMD_EXPORT(led_matrix_clear, Test led matrix on board)

/**
 * @brief Set a pixel's color to PWM reload value.
 * 
 * @param n Pixel index, starting from 0.
 * @param c pixel_rgb_t value, RGB888 format.
 */
void led_matrix_set_color(uint16_t n, pixel_rgb_t c)
{
    uint8_t dat_r, dat_b, dat_g;
    if (n > LED_TOTAL_NUMS)
    {
        return ;
    }

    dat_r = c.r;
    dat_g = c.g;
    dat_b = c.b;
    // 将数组颜色转化为24个要发送的字节数据
    for (uint8_t j = 0; j < 8; j++)
    {
        raw_led_buffer[(14 + (n * 48)) - (j << 1)] = tile[dat_g & 0x01];
        raw_led_buffer[(14 + (n * 48)) - (j << 1) + 1] = 0;
        raw_led_buffer[(30 + (n * 48)) - (j << 1)] = tile[dat_r & 0x01];
        raw_led_buffer[(30 + (n * 48)) - (j << 1) + 1] = 0;
        raw_led_buffer[(46 + (n * 48)) - (j << 1)] = tile[dat_b & 0x01];
        raw_led_buffer[(46 + (n * 48)) - (j << 1) + 1] = 0;
        dat_g >>= 1;
        dat_r >>= 1;
        dat_b >>= 1;
    }
}

/**
 * @brief Fill all or part of the Pixel strip with a color.
 *
 * @param pix   32-bit color value. Most significant byte is white (for
 *                RGBW pixels) or ignored (for RGB pixels), next is red,
 *                then green, and least significant byte is blue. If all
 *                arguments are unspecified, this will be 0 (off).
 * @param first Index of first pixel to fill, starting from 0. Must be
 *                in-bounds, no clipping is performed. 0 if unspecified.
 * @param count Number of pixels to fill, as a positive value. Passing
 *                0 or leaving unspecified will fill to end of strip.
 */
void led_matrix_fill(pixel_rgb_t pix, uint16_t first, uint16_t count)
{
    uint16_t i, end;

    if (first >= LED_TOTAL_NUMS)
    {
        return; // If first LED is past end of strip, nothing to do
    }

    // Calculate the index ONE AFTER the last pixel to fill
    if (count == 0)
    {
        // Fill to end of strip
        end = LED_TOTAL_NUMS;
    }
    else
    {
        // Ensure that the loop won't go past the last pixel
        end = first + count;
        if (end > LED_TOTAL_NUMS)
            end = LED_TOTAL_NUMS;
    }

    for (i = first; i < end; i++)
    {
        led_matrix_set_color(i, pix);
    }
}

/**
 * @brief full fill the on board LED matrix.
 * 
 * @param r 8bit R channel
 * @param g 8bit G channel
 * @param b 8bit B channel
 */
void led_matrix_fill_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    pixel_rgb_t color = {r, g, b};
    led_matrix_fill(color, 0, 19);
}

void exapmle_led_matrix_fill_show(uint8_t index)
{
    switch (index)
    {
    case 0:
        led_matrix_fill(LT_RED, 0, 19);
        led_matrix_reflash();
        break;
    case 1:
        led_matrix_fill(LT_GREEN, 0, 19);
        led_matrix_reflash();
        break;
    case 2:
        led_matrix_fill(LT_BLUE, 0, 19);
        led_matrix_reflash();
        break;
    case 3:
        led_matrix_fill(LT_WHITE, 0, 19);
        led_matrix_reflash();
        break;
    default:
        break;
    }
}

void led_fill_test()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        exapmle_led_matrix_fill_show(i);
        rt_thread_mdelay(1000);
    }
    led_matrix_clear();
}

MSH_CMD_EXPORT(led_fill_test, Test led matrix on board)

#endif /* BSP_USING_LED_MATRIX */
