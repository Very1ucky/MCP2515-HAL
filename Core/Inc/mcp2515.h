#ifndef MCP2515_H_
#define MCP2515_H_

#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "stm32f4xx_hal.h"

#include "proc_status.h"

#define SPI_CAN hspi2
#define CS_PORT_RX GPIOA
#define CS_PIN_RX 6
#define CS_PORT_TX GPIOA
#define CS_PIN_TX 5

#define INT_PIN GPIO_PIN_8

#define CS_RX() CS_PORT_RX->BSRR = 1 << (CS_PIN_RX + 16)
#define CdS_RX() CS_PORT_RX->BSRR = 1 << (CS_PIN_RX)
#define CS_TX() CS_PORT_TX->BSRR = 1 << (CS_PIN_TX + 16)
#define CdS_TX() CS_PORT_TX->BSRR = 1 << (CS_PIN_TX)

#define DATA_MAX_SIZE_IN_BYTES 8

#define MCP2515_ERROR_FLG 0x2D

#define MCP2515_TXB0CTRL 0x30
#define MCP2515_TXB1CTRL 0x40
#define MCP2515_TXB2CTRL 0x50

#define MCP2515_RXB0CTRL 0x60
#define MCP2515_RXB1CTRL 0x70

#define MCP2515_CNF3_ADDR 0x28
#define MCP2515_CNF2_ADDR 0x29
#define MCP2515_CNF1_ADDR 0x2A

#define MCP2515_CANINTE_ADDR 0x2B
#define MCP2515_CANINTF_ADDR 0x2C

#define MCP2515_CANCTRL_ADDR 0x0F
#define MCP2515_CANSTAT_ADDR 0x0E

#define MCP2515_RESET 0xC0
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
    RX_CAN_SLAVE,
    TX_CAN_SLAVE
} can_slaves_t;

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

process_status_t mcp2515_write_tx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool load_only_data, can_slaves_t slave);
process_status_t mcp2515_read_rx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool read_only_data, can_slaves_t slave);

process_status_t mcp2515_write_byte(uint8_t address, uint8_t data, can_slaves_t slave);
process_status_t mcp2515_read_byte(uint8_t address, uint8_t *data, can_slaves_t slave);

process_status_t mcp2515_get_read_status(uint8_t *status, can_slaves_t slave);
process_status_t mcp2515_get_rx_status(uint8_t *status, can_slaves_t slave);

process_status_t mcp2515_enter_mode(mcp2515_mode_t mode, can_slaves_t slave);

process_status_t mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data, can_slaves_t slave);

void mcp2515_init();

process_status_t mcp2515_reset(can_slaves_t slave);

#endif // MCP2515_H_