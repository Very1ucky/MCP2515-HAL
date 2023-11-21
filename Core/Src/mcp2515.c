#include "mcp2515.h"

extern SPI_HandleTypeDef hspi1;

void SPI_transmite(uint8_t *data);
void SPI_recieve(uint8_t *data);

void mcp2515_init()
{
    mcp2515_ss_high();
}

void mcp2515_read_byte(uint8_t address, uint8_t *data)
{
    mcp2515_ss_low();

    SPI_transmite(MCP2515_READ);
    SPI_transmite(address);

    SPI_recieve(data);

    mcp2515_ss_high();
}

void mcp2515_read_rx_buffer(RXBn buffer_number, uint8_t *data, uint8_t length)
{
    mcp2515_ss_low();

    switch (buffer_number)
    {
    case RXB0:
        SPI_transmite(MCP2515_READ_RXB0SIDH);
        break;
    case RXB1:
        SPI_transmite(MCP2515_READ_RXB1SIDH);
        break;
    }

    SPI_recieve_buffer(data, length);

    mcp2515_ss_high();
}

void mcp2515_write_tx_buffer(TXBn buffer_number, uint8_t *data, uint8_t length)
{

    uint8_t load_addr;
    uint8_t rts_addr;

    mcp2515_ss_low();

    switch (buffer_number)
    {
    case TXB0:
        load_addr = MCP2515_LOAD_TXB0SIDH;
        rts_addr = MCP2515_RTX_TX0;
        break;
    case TXB1:
        load_addr = MCP2515_LOAD_TXB1SIDH;
        rts_addr = MCP2515_RTX_TX1;
        break;
    case TXB2:
        load_addr = MCP2515_LOAD_TXB2SIDH;
        rts_addr = MCP2515_RTX_TX2;
        break;
    }

    // send instr to start writing data
    SPI_transmite(load_addr);
    
    // load data to buffer
    SPI_transmite_buffer(data, length);
    
    // request to transmit
    SPI_transmite(rts_addr);

    mcp2515_ss_high();
}

void mcp2515_write_byte(uint8_t address, uint8_t *data)
{
    mcp2515_ss_low();

    SPI_transmite(MCP2515_WRITE);
    SPI_transmite(address);
    SPI_transmite(data);

    mcp2515_ss_high();
}

mcp2515_read_status(uint8_t *status)
{
    mcp2515_ss_low();

    SPI_transmite(MCP2515_READ_STATUS);
    SPI_recieve(status);

    mcp2515_ss_high();
}

void SPI_transmite(uint8_t *data)
{
    HAL_SPI_Transmit(SPI_CAN, data, 1, 10);
}

void SPI_transmite_buffer(uint8_t *data, uint8_t buf_len)
{
    HAL_SPI_Transmite(SPI_CAN, data, buf_len, 10);
}

void SPI_recieve(uint8_t *data)
{
    HAL_SPI_Receive(SPI_CAN, data, 1, 10);
}

void SPI_recieve_buffer(uint8_t *data, uint8_t buf_len)
{
    HAL_SPI_Receive(SPI_CAN, data, buf_len, 10);
}