#ifndef CAN_H_
#define CAN_H_

#include "mcp2515.h"

#define TX_BUF_COUNT 3
#define RX_BUF_COUNT 2

#define IS_BUF_FREE(__BUF_NUM__, __STATUS__) 1<<(2*(__BUF_NUM__))&__STATUS__==0

#define IS_FRAME_EXTENDED(__FRAME__) __FRAME__->frame_type&0x2
#define IS_FRAME_RMT(__FRAME__) __FRAME__->frame_type&0x1

typedef enum {
    ST_DATA = 0x0,
    ST_RMT,
    EXT_DATA,
    EXT_RMT
} can_frame_type_t;

typedef struct {
    uint16_t st_id;
    uint32_t ext_id;
    can_frame_type_t frame_type;
    uint8_t dlc;
    uint8_t data[8];
} can_frame_t;

#endif // CAN_H_