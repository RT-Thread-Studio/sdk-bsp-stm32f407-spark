/*
 * COPYRIGHT (C) 2018, Real-Thread Information Technology Ltd
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-07-31     aozima       the first version
 * 2014-09-18     aozima       update command & response.
 */

#ifndef SPI_WIFI_H_INCLUDED
#define SPI_WIFI_H_INCLUDED

#include <stdint.h>
#include <rtdevice.h>
typedef enum
{
    master_cmd_phase = 0x01,
    master_data_phase,
    slave_cmd_phase,
    slave_data_phase,
}phase_type;

typedef enum
{
    TRANSFER_DATA_SUCCESS = 0x00,
    TRANSFER_DATA_ERROR,
    TRANSFER_DATA_CONTINUE,
}rw007_transfer_state;
/* little-endian */
struct spi_master_request
{
    uint8_t reserve1;
    uint8_t flag: 4;
    uint8_t type: 4;
    uint16_t reserve2;
    uint16_t seq;
    uint16_t M2S_len; // master to slave data len.
    uint32_t magic1;
    uint32_t magic2;
};

#define MASTER_MAGIC1 (0x67452301)
#define MASTER_MAGIC2 (0xEFCDAB89)

#define MASTER_FLAG_MRDY (0x01)

/* little-endian */
struct spi_slave_response
{
    uint8_t reserve1;
    uint8_t flag: 4;
    uint8_t type: 4;
    uint8_t slave_rx_buf: 4;
    uint8_t slave_tx_buf: 4;
    uint8_t reserve2;
    uint16_t seq;
    uint16_t S2M_len; // slave to master data len.
    uint32_t magic1;
    uint32_t magic2;
};

#define SLAVE_MAGIC1 (0x98BADCFE)
#define SLAVE_MAGIC2 (0x10325476)
#define SLAVE_FLAG_SRDY (0x01)

#define SLAVE_DATA_FULL (0x01)

/* spi buffer configure. */
#define SPI_MAX_DATA_LEN 1520
#define SPI_TX_POOL_SIZE 4
#define SPI_RX_POOL_SIZE 4
/*  The slave interrupts wait timeout */
#define SLAVE_INT_TIMEOUT  100

typedef enum
{
    RW007_SLAVE_INT = 1,
    RW007_MASTER_DATA = 1<<1,
} wifi_event;

typedef enum
{
    DATA_TYPE_STA_ETH_DATA = 1,
    DATA_TYPE_AP_ETH_DATA,
    DATA_TYPE_PROMISC_ETH_DATA,
    DATA_TYPE_CMD,
    DATA_TYPE_RESP,
    DATA_TYPE_CB,
} app_data_type_t;

struct spi_data_packet
{
    uint32_t data_len;  /* length for buffer */
    uint32_t data_type; /* app_data_type_t */
    char buffer[SPI_MAX_DATA_LEN];
};

typedef struct rw007_ap_info_value
{
    struct rt_wlan_info info;
    char passwd[RT_WLAN_PASSWORD_MAX_LENGTH];
} * rw007_ap_info_value_t;

/* littel endian */
typedef struct rw007_cmd
{
    uint32_t cmd;
    uint32_t len;

    /* command parameter */
    union
    {
        uint32_t int_value;
        uint8_t mac_value[8]; /* padding 2bytes */
        struct rw007_ap_info_value ap_info_value;
        char string_value[UINT16_MAX];
    } value;
} * rw007_cmd_t;

struct rw007_resp
{
    uint32_t cmd;
    uint32_t len;

    int32_t result; /* result of CMD. */

    /* response value */
    union
    {
        uint32_t int_value;
        uint8_t mac_value[8]; /* padding 2bytes */
        struct rw007_ap_info_value ap_info_value;
        char string_value[UINT16_MAX];
    } value;
};

/* tools */
#define node_entry(node, type, member) ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))
#define member_offset(type, member) ((unsigned long)(&((type *)0)->member))

#define MAX_SPI_PACKET_SIZE (member_offset(struct spi_data_packet, buffer) + SPI_MAX_DATA_LEN)

typedef enum 
{
    RW00x_CMD_INIT = 0x00,
    RW00x_CMD_SET_MODE,
    RW00x_CMD_MAC_GET,
    RW00x_CMD_MAC_SET,
    RW00x_CMD_GET_SN,
    RW00x_CMD_GET_VSR,
    RW00x_CMD_SCAN,
    RW00x_CMD_JOIN,
    RW00x_CMD_SOFTAP,
    RW00x_CMD_DISCONNECT,
    RW00x_CMD_AP_STOP,
    RW00x_CMD_AP_DEAUTH,
    RW00x_CMD_SCAN_STOP,
    RW00x_CMD_GET_RSSI,
    RW00x_CMD_SET_PWR_SAVE,
    RW00x_CMD_GET_PWR_SAVE,
    RW00x_CMD_CFG_PROMISC,
    RW00x_CMD_CFG_FILTER,
    RW00x_CMD_SET_CHANNEL,
    RW00x_CMD_GET_CHANNEL,
    RW00x_CMD_SET_COUNTRY,
    RW00x_CMD_GET_COUNTRY,
    RW00x_CMD_AP_MAC_GET,
    RW00x_CMD_AP_MAC_SET,
    RW00x_CMD_MAX_NUM
}RW00x_CMD;

struct rw007_spi
{
    /* Device handle for spi device */
    struct rt_spi_device *spi_device;

    /* Tx mempool and mailbox */
    struct rt_mempool spi_tx_mp;
    ALIGN(RT_ALIGN_SIZE)
    rt_uint8_t spi_tx_mempool[(sizeof(struct spi_data_packet) + 4) * SPI_TX_POOL_SIZE];
    struct rt_mailbox spi_tx_mb;
    rt_ubase_t spi_tx_mb_pool[SPI_TX_POOL_SIZE + 1];

    /* Rx mempool and mailbox */
    struct rt_mempool spi_rx_mp;
    ALIGN(RT_ALIGN_SIZE)
    rt_uint8_t spi_rx_mempool[(sizeof(struct spi_data_packet) + 4) * SPI_RX_POOL_SIZE];
    struct rt_mailbox spi_rx_mb;
    rt_ubase_t spi_rx_mb_pool[SPI_RX_POOL_SIZE + 1];

    /* response event */
    rt_event_t rw007_cmd_event;
    /* response data */
    struct rw007_resp *resp[RW00x_CMD_MAX_NUM];
};

#define RW00x_CMD_RESP_EVENT(n)     (0x01UL << n)

struct rw007_wifi
{
    /* inherit from ethernet device */
    struct rt_wlan_device *wlan;
    /* spi transfer layer handle */
    struct rw007_spi * hspi;
};

/* porting */
extern void spi_wifi_hw_init(void);

/* end porting */

/* api exclude in wlan framework */
extern rt_err_t rw007_sn_get(char sn[24]);
extern rt_err_t rw007_version_get(char version[16]);
/* end api exclude in wlan framework */

extern rt_err_t rt_hw_wifi_init(const char *spi_device_name);

#endif /* SPI_WIFI_H_INCLUDED */
