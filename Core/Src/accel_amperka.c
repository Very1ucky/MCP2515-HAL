#include "accel_amperka.h"

extern I2C_HandleTypeDef I2C_INT;

process_status_t I2C_read_byte(uint16_t addr, uint8_t *data);
process_status_t I2C_write_byte(uint16_t addr, uint8_t data);

process_status_t I2C_read_buffer(uint16_t addr, uint8_t *data, uint8_t length);
process_status_t I2C_write_buffer(uint16_t addr, uint8_t *data, uint8_t length);

process_status_t LIS331DLN_init(acel_poll_rate_t poll_rate)
{
    process_status_t status = OK;

    status = LIS331DLN_turn_on(poll_rate);

    if (status)
    {
        return status;
    }

    // set ctrl_regs 2,3,4,5 data
    uint8_t ctrl_regs_data[] = {LIS331DLN_CTRL_REG2_DATA, LIS331DLN_CTRL_REG3_DATA,
                                LIS331DLN_CTRL_REG4_DATA, LIS331DLN_CTRL_REG5_DATA};

    status = I2C_write_buffer(LIS331DLN_CTRL_REG2 | 0x80, ctrl_regs_data, 4);

    /* dummy read to force the HP filter to actual acceleration value (set reference acceleretion),
    if needed (set zero poz of acelerometr)
    */
    uint8_t dummy;
    status = I2C_read_buffer(LIS331DLN_HP_FILTER_RESET, &dummy, 1);


    return status;
}

process_status_t LIS331DLN_turn_on(acel_poll_rate_t poll_rate)
{
    process_status_t status = OK;

    status = I2C_write_byte(LIS331DLN_CTRL_REG1, poll_rate);

    // wait turn on time (AN2847)
    HAL_Delay(21);
    acel_poll_rate_t set_poll_rate;
    status = I2C_read_byte(LIS331DLN_CTRL_REG1, (uint8_t *)(&set_poll_rate));

    if (set_poll_rate != poll_rate)
    {
        return FAILED;
    }

    return status;
}

process_status_t LIS331DLN_turn_off()
{
    process_status_t status = OK;
    status = I2C_write_byte(LIS331DLN_CTRL_REG1, 0x00);

    return status;
}

process_status_t LIS331DLN_calibrate()
{
    process_status_t status = OK;
    uint8_t ctrl_reg2_data = 0;

    I2C_read_byte(LIS331DLN_CTRL_REG2, &ctrl_reg2_data);
    // active HPF on data
    ctrl_reg2_data |= 0x10;
    status = I2C_write_byte(LIS331DLN_CTRL_REG2, ctrl_reg2_data);

    uint8_t dummy;
    status = I2C_read_byte(LIS331DLN_HP_FILTER_RESET, &dummy);

    return status;
}

process_status_t read_velocity(acel_data_t *data_frame)
{
    process_status_t status = OK;

    status = I2C_read_buffer(LIS331DLN_OUT_XYZ_START, (uint8_t *)data_frame, 6);

    return status;
}

process_status_t I2C_read_byte(uint16_t addr, uint8_t *data)
{
    return I2C_read_buffer(addr, data, 1);
}

process_status_t I2C_read_buffer(uint16_t addr, uint8_t *data, uint8_t length)
{
    if (HAL_I2C_Mem_Read(&I2C_INT, LIS331DLN_ADDR, addr,
                         1, data, length, 200))
    {
        return TRANSFER_ERROR;
    }

    return OK;
}

process_status_t I2C_write_byte(uint16_t addr, uint8_t data)
{
    return I2C_write_buffer(addr, &data, 1);
}

process_status_t I2C_write_buffer(uint16_t addr, uint8_t *data, uint8_t length)
{
    if (HAL_I2C_Mem_Write(&I2C_INT, LIS331DLN_ADDR, addr,
                          1, data, length, 200))
    {
        return TRANSFER_ERROR;
    }

    return OK;
}