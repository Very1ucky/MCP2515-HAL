#ifndef CAN_H_
#define CAN_H_

#include "mcp2515.h"

#define TX_BUF_COUNT 3
#define RX_BUF_COUNT 2

#define IS_TX_BUF_FREE(__BUF_NUM__, __STATUS__) ((1<<(2*(__BUF_NUM__)))&(__STATUS__))==0

#define IS_FRAME_EXTENDED(__FRAME__) __FRAME__->type&0x2
#define IS_FRAME_RMT(__FRAME__) __FRAME__->type&0x1

typedef enum {
    ST_DATA = 0x0,
    EXT_DATA,
    ST_RMT,
    EXT_RMT
} can_frame_type_t;

typedef struct {
    uint16_t st_id;
    uint32_t ext_id;
    can_frame_type_t type;
    uint8_t dlc;
    uint8_t data[8];
} can_frame_t;

process_status_t can_init(can_slaves_t slave);

process_status_t send_can_frame(can_frame_t *frame);
process_status_t recieve_can_frame(can_frame_t *frame, uint8_t full_buf_num);

process_status_t send_can_frame_to_slave(can_frame_t *frame, can_slaves_t slave);
process_status_t recieve_can_frame_from_slave(can_frame_t *frame, uint8_t full_buf_num, can_slaves_t slave);

#endif // CAN_H_