#ifndef CAN_H_
#define CAN_H_

#include "mcp2515.h"

typedef struct {
    uint16_t st_id;
    uint32_t ext_id;
    bool is_extended;
    bool is_rtr;
    uint8_t dlc;
    uint8_t data[8];
} can_frame_t;

#endif // CAN_H_