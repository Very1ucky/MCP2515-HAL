#include "can.h"

static void conv_can_frame_2_regs(can_frame_t *frame, tx_rx_reg_packet_t *regs);
static void conv_regs_2_can_frame(can_frame_t *frame, tx_rx_reg_packet_t *regs, can_frame_type_t frame_type);
static uint8_t get_first_free_tx_buf();
static uint8_t get_rx_full_buf(can_frame_type_t *frame_type);

process_status_t send_can_frame(can_frame_t *frame)
{
    static tx_rx_reg_packet_t tx_regs;

    conv_can_frame_2_regs(frame, &tx_regs);

    uint8_t free_buf_num = get_first_free_tx_buf();
    if (free_buf_num == 0)
    {
        return NO_FREE_TX_BUF;
    }

    return mcp2515_write_tx_buffer(free_buf_num, (uint8_t *)&tx_regs,
                            sizeof(tx_rx_reg_packet_t), false);
}

process_status_t recieve_can_frame(can_frame_t *frame)
{
    static tx_rx_reg_packet_t rx_regs;

    process_status_t status;

    can_frame_type_t frame_type;
    uint8_t full_buf_num = get_rx_full_buf(&frame_type);
    if (full_buf_num == 0)
    {
        return NO_FULL_RX_BUF;
    }

    status = mcp2515_read_rx_buffer(full_buf_num, (uint8_t *)&rx_regs,
                           sizeof(tx_rx_reg_packet_t), false);

    conv_regs_2_can_frame(frame, &rx_regs, frame_type);

    return status;
}

static uint8_t get_rx_full_buf(can_frame_type_t *frame_type)
{
    static uint8_t status;

    mcp2515_get_rx_status(&status);

    *frame_type = (can_frame_type_t)((status >> 3) & 0x3);

    for (int buf_num = 1; buf_num <= RX_BUF_COUNT; ++buf_num)
    {
        if (READ_BIT(status, buf_num + 1))
            return buf_num;
    }

    return 0;
}

static uint8_t get_first_free_tx_buf()
{

    static uint8_t status;

    mcp2515_get_read_status(&status);

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

    regs->TXRXBnDLC = frame->dlc && 0xF;

    if (IS_FRAME_RMT(frame))
    {
        regs->TXRXBnDLC |= 0x40;
    }

    for (int i = 0; i < frame->dlc; ++i)
    {
        regs->TXRXBnDx[i] = frame->data[i];
    }
}

static void conv_regs_2_can_frame(can_frame_t *frame, tx_rx_reg_packet_t *regs, can_frame_type_t frame_type)
{
    frame->st_id = regs->TXRXBnSIDH << 3 | regs->TXRXBnSIDL >> 5;
    frame->type = frame_type;

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

process_status_t can_init()
{
    process_status_t status;

    mcp2515_init();

    status = mcp2515_enter_mode(CONFIGURATION);
    if (status != OK)
    {
        return status;
    }

    

    status = mcp2515_enter_mode(NORMAL);

    return status;
}