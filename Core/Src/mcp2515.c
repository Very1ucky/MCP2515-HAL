#include "mcp2515.h"

extern SPI_HandleTypeDef hspi1;

void SPI_transmit(uint8_t data);
void SPI_recieve(uint8_t *data);

void SPI_transmit_buffer(uint8_t *data, uint8_t buf_len);
void SPI_recieve_buffer(uint8_t *data, uint8_t buf_len);

void mcp2515_init()
{
    mcp2515_ss_high();
}

void mcp2515_read_byte(uint8_t address, uint8_t *data)
{
    mcp2515_ss_low();

    SPI_transmit(MCP2515_READ);
    SPI_transmit(address);

    SPI_recieve(data);

    mcp2515_ss_high();
}

process_status_t mcp2515_read_rx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool read_only_data)
{
    if (length > sizeof(tx_rx_reg_packet_t)
    || (read_only_data && length > DATA_MAX_SIZE_IN_BYTES))
    {
        return INCORRECT_INPUT;
    }

    uint8_t read_inst;

    mcp2515_ss_low();

    switch (buffer_number)
    {
    case 1:
        read_inst = MCP2515_READ_RXB0SIDH;
        break;
    case 2:
        read_inst = MCP2515_READ_RXB1SIDH;
        break;
    default:
        return INCORRECT_INPUT;
    }

    if (read_only_data)
    {
        read_inst |= 1;
    }

    SPI_transmit(read_inst);

    SPI_recieve_buffer(data, length);

    mcp2515_ss_high();

    return OK;
}

process_status_t mcp2515_write_tx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool load_only_data)
{

    if (length > sizeof(tx_rx_reg_packet_t)
    || (load_only_data && length > DATA_MAX_SIZE_IN_BYTES))
    {
        return INCORRECT_INPUT;
    }

    uint8_t load_inst;
    uint8_t rts_inst;

    mcp2515_ss_low();

    switch (buffer_number)
    {
    case 1:
        load_inst = MCP2515_LOAD_TXB0SIDH;
        rts_inst = MCP2515_RTX_TX0;
        break;
    case 2:
        load_inst = MCP2515_LOAD_TXB1SIDH;
        rts_inst = MCP2515_RTX_TX1;
        break;
    case 3:
        load_inst = MCP2515_LOAD_TXB2SIDH;
        rts_inst = MCP2515_RTX_TX2;
        break;
    default:
        return INCORRECT_INPUT;
    }

    if (load_only_data)
    {
        load_inst |= 1;
    }

    // send instr to start writing data
    SPI_transmit(load_inst);

    // load data to buffer
    SPI_transmit_buffer(data, length);

    // request to transmit (set TxBnCTRL.TXREQ reg to 1)
    SPI_transmit(rts_inst);

    mcp2515_ss_high();

    return OK;
}

void mcp2515_write_byte(uint8_t address, uint8_t *data)
{
    mcp2515_ss_low();

    SPI_transmit(MCP2515_WRITE);
    SPI_transmit(address);
    SPI_transmit(*data);

    mcp2515_ss_high();
}

void mcp2515_get_read_status(uint8_t *status)
{
    mcp2515_ss_low();

    SPI_transmit(MCP2515_READ_STATUS);
    SPI_recieve(status);

    mcp2515_ss_high();
}

void mcp2515_get_rx_status(uint8_t *status)
{
    mcp2515_ss_low();

    SPI_transmit(MCP2515_RX_STATUS);
    SPI_recieve(status);

    mcp2515_ss_high();
}

void SPI_transmit(uint8_t data)
{
    HAL_SPI_Transmit(SPI_CAN, &data, 1, 10);
}

void SPI_transmit_buffer(uint8_t *data, uint8_t buf_len)
{
    HAL_SPI_Transmit(SPI_CAN, data, buf_len, 10);
}

void SPI_recieve(uint8_t *data)
{
    HAL_SPI_Receive(SPI_CAN, data, 1, 10);
}

void SPI_recieve_buffer(uint8_t *data, uint8_t buf_len)
{
    HAL_SPI_Receive(SPI_CAN, data, buf_len, 10);
}