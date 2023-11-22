#include "can.h"

void can_init()
{
}

void conv_can_frame_2_regs(can_frame_t *frame, tx_rx_reg_packet_t *regs)
{
    regs->TXRXBnSIDH = frame->st_id >> 3;
    regs->TXRXBnSIDL = frame->st_id << 5 && 0xE0;

    if (frame->is_extended)
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

    if (frame->is_rtr) {
        regs->TXRXBnDLC |= 0x40;
    }

    for (int i = 0; i < frame->dlc; ++i) {
        regs->TXRXBnDx[i] = frame->data[i];
    }
}