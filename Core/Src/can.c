#include "can.h"

static void conv_can_frame_2_regs(can_frame_t *frame, tx_rx_reg_packet_t *regs);
static void conv_regs_2_can_frame(can_frame_t *frame, tx_rx_reg_packet_t *regs);
static uint8_t get_first_free_tx_buf(can_slaves_t slave);

process_status_t send_can_frame(can_frame_t *frame)
{
    return send_can_frame_to_slave(frame, TX_CAN_SLAVE);
}

process_status_t send_can_frame_to_slave(can_frame_t *frame, can_slaves_t slave)
{
    static tx_rx_reg_packet_t tx_regs;

    conv_can_frame_2_regs(frame, &tx_regs);

    uint8_t free_buf_num = get_first_free_tx_buf(slave);
    if (free_buf_num == 0)
    {
        return NO_FREE_TX_BUF;
    }

    return mcp2515_write_tx_buffer(free_buf_num, (uint8_t *)&tx_regs,
                                   sizeof(tx_rx_reg_packet_t), false, slave);
}

process_status_t recieve_can_frame(can_frame_t *frame, uint8_t full_buf_num)
{
    return recieve_can_frame_from_slave(frame, full_buf_num, RX_CAN_SLAVE);
}

process_status_t recieve_can_frame_from_slave(can_frame_t *frame, uint8_t full_buf_num, can_slaves_t slave)
{
    static tx_rx_reg_packet_t rx_regs;

    process_status_t status;

    status = mcp2515_read_rx_buffer(full_buf_num, (uint8_t *)&rx_regs,
                                    sizeof(tx_rx_reg_packet_t), false, slave);

    conv_regs_2_can_frame(frame, &rx_regs);

    return status;
}

static uint8_t get_first_free_tx_buf(can_slaves_t slave)
{

    static uint8_t status;

    mcp2515_get_read_status(&status, slave);

    for (int buf_num = 1; buf_num <= TX_BUF_COUNT; ++buf_num)
    {
        if (IS_TX_BUF_FREE(buf_num, status))
            return buf_num;
    }

    return 0;
}

static void conv_can_frame_2_regs(can_frame_t *frame, tx_rx_reg_packet_t *regs)
{
    regs->TXRXBnSIDH = frame->st_id >> 3;
    regs->TXRXBnSIDL = (frame->st_id << 5) & 0xE0;

    if (IS_FRAME_EXTENDED(frame))
    {
        // set extended identifier enable
        regs->TXRXBnSIDL |= 0x8;

        regs->TXRXBnSIDL |= frame->ext_id >> 16;

        regs->TXRXBnEID8 = frame->ext_id >> 8;
        regs->TXRXBnEID0 = frame->ext_id;
    }
    else
    {
        regs->TXRXBnEID8 = 0;
        regs->TXRXBnEID0 = 0;
    }

    regs->TXRXBnDLC = frame->dlc & 0xF;

    if (IS_FRAME_RMT(frame))
    {
        regs->TXRXBnDLC |= 0x40;
    }

    for (int i = 0; i < frame->dlc; ++i)
    {
        regs->TXRXBnDx[i] = frame->data[i];
    }
}

static void conv_regs_2_can_frame(can_frame_t *frame, tx_rx_reg_packet_t *regs)
{
    frame->st_id = regs->TXRXBnSIDH << 3 | regs->TXRXBnSIDL >> 5;
    frame->type = (regs->TXRXBnSIDL & 0x18) >> 3;

    if (IS_FRAME_EXTENDED(frame))
    {
        frame->ext_id = (regs->TXRXBnSIDL & 0x3) | regs->TXRXBnEID8 << 8 |
                        regs->TXRXBnEID0;
    }

    frame->dlc = regs->TXRXBnDLC & 0xF;
    for (int i = 0; i < frame->dlc; ++i)
    {
        frame->data[i] = regs->TXRXBnDx[i];
    }
}

/*void check_interrupt_flag()
{

    mcp2515_read_byte(MCP2515_CANINTF_ADDR);
}
*/

process_status_t can_init(can_slaves_t slave)
{
    process_status_t status;

    mcp2515_init();

    HAL_Delay(10);

    // reset and set config mode
    status = mcp2515_reset(slave);
    if (status)
    {
        return status;
    }

    HAL_Delay(10);
    

    /* enable rollover messages to RXB1 if RXB0 is full
        disable all filters and masks to recieve all messages
    */
    mcp2515_write_byte(MCP2515_RXB0CTRL, 0b01100100, slave);
    mcp2515_write_byte(MCP2515_RXB1CTRL, 0b01100000, slave);

    /*
        setup Tx buffers (equal message priority of all buffers)
    */
    mcp2515_write_byte(MCP2515_TXB0CTRL, 0x00, slave);
    mcp2515_write_byte(MCP2515_TXB1CTRL, 0x00, slave);
    mcp2515_write_byte(MCP2515_TXB2CTRL, 0x00, slave);

    /*
        enable interrupts on full recieve buffer and message error for rx slave
    */
    if (slave == RX_CAN_SLAVE)
    {
        mcp2515_write_byte(MCP2515_CANINTE_ADDR, 0b10000011, slave);
    }

    /*
      Tq = 2 * (brp + 1) / 80000000 = 0.5us
      Tbit = (SYNCSEG=1 + PRSEG + PHSEG1 + PHSEG2)
      Tbit = 1tq + 5tq + 6tq + 4tq = 16tq
      16Tq = 4us = 125kbps

      PRSEG+PHSEG1>=PHSEG2
      PRSEG+PHSEG1>=Tdelay=1-2Tq
      PHSEG2>SJW
    */

    /* 7-6(SJW)5-0(BRP)
        0b00 000001 (BRP = 1, SJW = 1 tq)
    */
    mcp2515_write_byte(MCP2515_CNF1_ADDR, 0b00000001, slave);

    /* 7(BTLMODE)6(SamplePointConf)5-3(PHSEG1)2-0(PRSEG)
        0b1 0 110 101 (def PHSEG2 in CNF3, poll once in SP, set PHSEG1 and PRSEG)
    */
    mcp2515_write_byte(MCP2515_CNF2_ADDR, 0b10110101, slave);

    /* 7(SOF)6(WAKFIL)2-0(PHSEG2)
        0b0 0 xxx 100 (wake-up filter forbidden, set PHSEG2)
    */
    mcp2515_write_byte(MCP2515_CNF3_ADDR, 0b00000100, slave);

    status = mcp2515_enter_mode(NORMAL, slave);

    return status;
}