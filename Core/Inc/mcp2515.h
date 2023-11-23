#ifndef MCP2515_H_
#define MCP2515_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "stdint.h"

#define SPI_CAN &hspi1
#define SS_PORT GPIOA
#define SS_PIN 5
#define mcp2515_ss_high() SS_PORT->BSRR = 1 << SS_PIN
#define mcp2515_ss_low() SS_PORT->BSRR = 1 << (SS_PIN + 16)

#define DATA_MAX_SIZE_IN_BYTES 8

#define MCP2515_CANCTRL_ADDR 0x0F
#define MCP2515_CANSTAT_ADDR 0x0E

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
#define MCP2515_LOAD_TXB0D0 0b01000001
// start send start at TXB1SIDH register (all packet info)
#define MCP2515_LOAD_TXB1SIDH 0b01000010
// start send start at TXB1D0 register (only data packet info)
#define MCP2515_LOAD_TXB1D0 0b01000011
// start send start at TXB2SIDH register (all packet info)
#define MCP2515_LOAD_TXB2SIDH 0b01000100
// start send start at TXB2D0 register (only data packet info)
#define MCP2515_LOAD_TXB2D0 0b01000101

// start read start at RXB0SIDH register (all packet info)
#define MCP2515_READ_RXB0SIDH 0b10010000
// start send start at RXB0D0 register (only data packet info)
#define MCP2515_LOAD_RXB0D0 0b10010001
// start read start at RXB1SIDH register (all packet info)
#define MCP2515_READ_RXB1SIDH 0b10010010
// start send start at RXB1D0 register (only data packet info)
#define MCP2515_LOAD_RXB1D0 0b10010011

typedef enum
{
    OK,
    INCORRECT_INPUT,
    TRANSFER_ERROR,
    NO_FREE_TX_BUF,
    NO_FULL_RX_BUF,
    FAILED
} process_status_t;

typedef enum
{
    NORMAL,
    SLEEP,
    LOOPBACK,
    LISTENONLY,
    CONFIGURATION
} mcp2515_mode_t;

typedef struct
{
    uint8_t TXRXBnSIDH;
    uint8_t TXRXBnSIDL;
    uint8_t TXRXBnEID8;
    uint8_t TXRXBnEID0;
    uint8_t TXRXBnDLC;
    uint8_t TXRXBnDx[8];
} tx_rx_reg_packet_t;

void mcp2515_init();

process_status_t mcp2515_write_tx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool load_only_data);
process_status_t mcp2515_read_rx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool read_only_data);

void mcp2515_get_read_status(uint8_t *status);
void mcp2515_get_rx_status(uint8_t *status);

#endif // MCP2515_H_