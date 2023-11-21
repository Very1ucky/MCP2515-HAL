#ifndef MCP2515_H_
#define MCP2515_H_


#define SPI_CAN &hspi1
#define SS_PORT GPIOA
#define SS_PIN 5
#define mcp2515_ss_high() SS_PORT->BSRR = 1 << SS_PIN
#define mcp2515_ss_low() SS_PORT->BSRR = 1 << (SS_PIN+16)

#define MCP2515_READ 0x03
#define MCP2515_WRITE 0x02
#define MCP2515_READ_STATUS 0xA0
#define MCP2515_RX_STATUS 0xB0
#define MCP2515_BIT_MOD 0x05

// request to send
#define MCP2515_RTX_TX0 0x81
#define MCP2515_RTX_TX1 0x82
#define MCP2515_RTX_TX2 0x84

// start send start at TXB0SIDH register (all packet info)
#define MCP2515_LOAD_TXB0SIDH 0b01000000 
// start send start at TXB0D0 register (only data packet info)
#define MCP2515_LOAD_TXB0SIDH 0b01000001 
// start send start at TXB1SIDH register (all packet info)
#define MCP2515_LOAD_TXB1SIDH 0b01000010 
// start send start at TXB1D0 register (only data packet info)
#define MCP2515_LOAD_TXB1SIDH 0b01000011 
// start send start at TXB2SIDH register (all packet info)
#define MCP2515_LOAD_TXB2SIDH 0b01000100 
// start send start at TXB2D0 register (only data packet info)
#define MCP2515_LOAD_TXB2SIDH 0b01000101 

// start read start at RXB0SIDH register (all packet info)
#define MCP2515_READ_RXB0SIDH 0b10010000 
// start send start at RXB0D0 register (only data packet info)
#define MCP2515_LOAD_RXB0D0 0b10010010
// start read start at RXB1SIDH register (all packet info)
#define MCP2515_READ_RXB1SIDH 0b10010010 
// start send start at RXB1D0 register (only data packet info)
#define MCP2515_LOAD_RXB1D0 0b10010011

#include "stm32f4xx_hal.h"

typedef enum {
    RXB0,
    RXB1
} RXBn;

typedef enum {
    TXB0,
    TXB1,
    TXB2
} TXBn;

typedef struct {
    uint8_t RXBnSIDH;
    uint8_t RXBnSIDL;
    uint8_t RXBnEID8;
    uint8_t RXBnEID0;
    uint8_t RXBnDLC;
    uint8_t RXBnD0;
    uint8_t RXBnD1;
    uint8_t RXBnD2;
    uint8_t RXBnD3;
    uint8_t RXBnD4;
    uint8_t RXBnD5;
    uint8_t RXBnD6;
    uint8_t RXBnD7;
} rx_reg_packet_t;

typedef struct {
    uint8_t TXBnSIDH;
    uint8_t TXBnSIDL;
    uint8_t TXBnEID8;
    uint8_t TXBnEID0;
    uint8_t TXBnDLC;
    uint8_t TXBnD0;
    uint8_t TXBnD1;
    uint8_t TXBnD2;
    uint8_t TXBnD3;
    uint8_t TXBnD4;
    uint8_t TXBnD5;
    uint8_t TXBnD6;
    uint8_t TXBnD7;
} tx_reg_packet_t;

void mcp2515_init();

#endif // MCP2515_H_