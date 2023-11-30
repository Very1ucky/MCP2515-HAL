#include "mcp2515.h"

extern SPI_HandleTypeDef hspi2;

static void slave_select(can_slaves_t slave);
static void slave_deselect(can_slaves_t slave);

process_status_t SPI_read_byte(uint8_t *data);
process_status_t SPI_write_byte(uint8_t data);

process_status_t SPI_read_buffer(uint8_t *data, uint8_t buf_len);
process_status_t SPI_write_buffer(uint8_t *data, uint8_t buf_len);

process_status_t mcp2515_request_to_send(uint8_t rts_inst, can_slaves_t slave);

process_status_t mcp2515_read_byte(uint8_t address, uint8_t *data, can_slaves_t slave)
{

    slave_select(slave);

    if (SPI_write_byte(MCP2515_READ) || SPI_write_byte(address) || SPI_read_byte(data))
    {
        slave_deselect(slave);
        return TRANSFER_ERROR;
    }

    slave_deselect(slave);

    return OK;
}

process_status_t mcp2515_read_rx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool read_only_data, can_slaves_t slave)
{
    if (length > sizeof(tx_rx_reg_packet_t) || (read_only_data && length > DATA_MAX_SIZE_IN_BYTES))
    {
        return INCORRECT_INPUT;
    }

    uint8_t read_inst;

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

    slave_select(slave);

    if (SPI_write_byte(read_inst) || SPI_read_buffer(data, length))
    {
        slave_deselect(slave);
        return TRANSFER_ERROR;
    }

    slave_deselect(slave);

    return OK;
}

process_status_t mcp2515_write_tx_buffer(uint8_t buffer_number, uint8_t *data, uint8_t length, bool load_only_data, can_slaves_t slave)
{

    if (length > sizeof(tx_rx_reg_packet_t) || (load_only_data && length > DATA_MAX_SIZE_IN_BYTES))
    {
        return INCORRECT_INPUT;
    }

    uint8_t load_inst;
    uint8_t rts_inst;

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

    slave_select(slave);

    // send instr to start writing data
    // load data to buffer
    if (SPI_write_byte(load_inst) || SPI_write_buffer(data, length))
    {
        slave_deselect(slave);
        return TRANSFER_ERROR;
    }

    slave_deselect(slave);

    // request to transmit (set TxBnCTRL.TXREQ reg to 1)
    if (mcp2515_request_to_send(rts_inst, slave))
    {
        return TRANSFER_ERROR;
    }

    return OK;
}

process_status_t mcp2515_write_byte(uint8_t address, uint8_t data, can_slaves_t slave)
{
    slave_select(slave);

    if (SPI_write_byte(MCP2515_WRITE) || SPI_write_byte(address) || SPI_write_byte(data))
    {
        slave_deselect(slave);
        return TRANSFER_ERROR;
    }

    slave_deselect(slave);

    return OK;
}

process_status_t mcp2515_get_read_status(uint8_t *status, can_slaves_t slave)
{
    slave_select(slave);

    if (SPI_write_byte(MCP2515_READ_STATUS) || SPI_read_byte(status))
    {
        slave_deselect(slave);
        return TRANSFER_ERROR;
    }

    slave_deselect(slave);

    return OK;
}

process_status_t mcp2515_get_rx_status(uint8_t *status, can_slaves_t slave)
{
    slave_select(slave);

    if (SPI_write_byte(MCP2515_RX_STATUS) || SPI_read_byte(status))
    {
        slave_deselect(slave);
        return TRANSFER_ERROR;
    }

    slave_deselect(slave);

    return OK;
}

process_status_t mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data, can_slaves_t slave)
{
    slave_select(slave);

    if (SPI_write_byte(MCP2515_BIT_MOD) || SPI_write_byte(address) || SPI_write_byte(mask) || SPI_write_byte(data))
    {
        slave_deselect(slave);
        return TRANSFER_ERROR;
    }

    slave_deselect(slave);

    return OK;
}

process_status_t mcp2515_request_to_send(uint8_t rts_inst, can_slaves_t slave)
{
    process_status_t status = OK;

    slave_select(slave);

    status = SPI_write_byte(rts_inst);

    slave_deselect(slave);

    return status;
}

process_status_t mcp2515_enter_mode(mcp2515_mode_t mode, can_slaves_t slave)
{
    uint8_t status = 1;

    mcp2515_bit_modify(MCP2515_CANCTRL_ADDR, 0xE0, (uint8_t)mode, slave);

    HAL_Delay(10);

    mcp2515_read_byte(MCP2515_CANSTAT_ADDR, &status, slave);

    if ((status & 0xE0) == mode)
    {
        return OK;
    }

    return FAILED;
}

process_status_t mcp2515_reset(can_slaves_t slave)
{
    process_status_t status = OK;
    slave_select(slave);

    status = SPI_write_byte(MCP2515_RESET);

    slave_deselect(slave);

    return status;
}

process_status_t SPI_write_byte(uint8_t data)
{
    return SPI_write_buffer(&data, 1);
}

process_status_t SPI_write_buffer(uint8_t *data, uint8_t buf_len)
{

    if (HAL_SPI_Transmit(&SPI_CAN, data, buf_len, 10))
    {
        return TRANSFER_ERROR;
    }

    return OK;
}

process_status_t SPI_read_byte(uint8_t *data)
{
    return SPI_read_buffer(data, 1);
}

process_status_t SPI_read_buffer(uint8_t *data, uint8_t buf_len)
{
    if (HAL_SPI_Receive(&SPI_CAN, data, buf_len, 10))
    {
        return TRANSFER_ERROR;
    }

    return OK;
}

static void slave_select(can_slaves_t slave)
{
    switch (slave)
    {
    case RX_CAN_SLAVE:
        CS_RX();
        break;
    case TX_CAN_SLAVE:
        CS_TX();
        break;
    default:
        break;
    }
}

static void slave_deselect(can_slaves_t slave)
{
    switch (slave)
    {
    case RX_CAN_SLAVE:
        CdS_RX();
        break;
    case TX_CAN_SLAVE:
        CdS_TX();
        break;
    default:
        break;
    }
}

void mcp2515_init()
{
    slave_deselect(RX_CAN_SLAVE);
    slave_deselect(TX_CAN_SLAVE);
}