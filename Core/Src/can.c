#include "can.h"

static void conv_can_frame_2_regs(can_frame_t *frame, tx_rx_reg_packet_t *regs);
static conv_regs_2_can_frame(can_frame_t *frame, tx_rx_reg_packet_t *regs, can_frame_type_t frame_type);
static uint8_t get_first_free_tx_buf();
static uint8_t get_rx_full_buf(can_frame_type_t *frame_type);

void send_can_frame(can_frame_t *frame)
{
    static tx_rx_reg_packet_t tx_regs;

    conv_can_frame_2_regs(frame, &tx_regs);

    uint8_t free_buf_num = get_first_free_tx_buf();
    mcp2515_write_tx_buffer(free_buf_num, (uint8_t *)&tx_regs,
                            sizeof(tx_rx_reg_packet_t), false);
}

void recieve_can_frame(can_frame_t *frame)
{
    static tx_rx_reg_packet_t rx_regs;

    can_frame_type_t frame_type;
    uint8_t full_buf_num = get_rx_full_buf(&frame_type);
    mcp2515_read_rx_buffer(full_buf_num, (uint8_t *)&rx_regs,
                           sizeof(tx_rx_reg_packet_t), false);

    conv_regs_2_can_frame(frame, &rx_regs, frame_type);
}

static uint8_t get_rx_full_buf(can_frame_type_t *frame_type)
{
    static status;

    mcp2515_get_rx_status(&status);

    for (int buf_num = 0; buf_num < TX_BUF_COUNT; ++buf_num)
    {
        if (IS_BUF_FREE(buf_num, status))
            return buf_num;
    }
}

static uint8_t get_first_free_tx_buf()
{

    static uint8_t status;

    mcp2515_get_read_status(&status);

    for (int buf_num = 0; buf_num < TX_BUF_COUNT; ++buf_num)
    {
        if (IS_BUF_FREE(buf_num, status))
            return buf_num;
    }
}

static void conv_can_frame_2_regs(can_frame_t *frame, tx_rx_reg_packet_t *regs)
{
    regs->TXRXBnSIDH = frame->st_id >> 3;
    regs->TXRXBnSIDL = frame->st_id << 5 && 0xE0;

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

static conv_regs_2_can_frame(can_frame_t *frame, tx_rx_reg_packet_t *regs, can_frame_type_t frame_type)
{
    
}

void can_init()
{
}