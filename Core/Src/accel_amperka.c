#include "accel_amperka.h"

extern I2C_HandleTypeDef I2C_INT;

process_status_t I2C_read_byte(uint16_t dev_addr, uint16_t addr, uint8_t *data);
process_status_t I2C_write_byte(uint16_t dev_addr, uint16_t addr, uint8_t data);

process_status_t I2C_read_buffer(uint16_t dev_addr, uint16_t addr, uint8_t *data, uint8_t length);
process_status_t I2C_write_buffer(uint16_t dev_addr, uint16_t addr, uint8_t *data, uint8_t length);

process_status_t LIS331DLN_init(acel_poll_rate_t poll_rate)
{
    process_status_t status = OK;

    status = I2C_write_byte(LIS331DLN_ADDR, CTRL_REG1, poll_rate);

    HAL_Delay(21);
    acel_poll_rate_t set_poll_rate;
    status = I2C_read_byte(LIS331DLN_ADDR, CTRL_REG1, (uint8_t *)(&set_poll_rate));
    if (set_poll_rate != poll_rate)
    {
        return FAILED;
    }

    // set ctrl_regs 2,3,4,5 data
    uint8_t ctrl_regs_data[] = {LIS331DLN_CTRL_REG2_DATA, LIS331DLN_CTRL_REG3_DATA,
                                LIS331DLN_CTRL_REG4_DATA, LIS331DLN_CTRL_REG5_DATA};

    status = I2C_write_buffer(LIS331DLN_ADDR, CTRL_REG2, ctrl_regs_data, 4);

    if (status)
    {
        return status;
    }

    /* dummy read to force the HP filter to actual acceleration value (set reference acceleretion),
    if needed (set zero poz of acelerometr)
    */
    uint8_t dummy;
    status = I2C_read_buffer(LIS331DLN_ADDR, HP_FILTER_RESET, &dummy, 1);

    return status;
}

process_status_t LIS331DLN_turn_off()
{
    process_status_t status = OK;
    status = I2C_write_byte(LIS331DLN_ADDR, CTRL_REG1, 0x00);

    return status;
}

process_status_t read_axes_sensor_data(uint16_t dev_addr, axes_sensor_data_t *data_frame)
{
    process_status_t status = OK;

    status = I2C_read_buffer(dev_addr, OUT_XYZ_START, (uint8_t *)data_frame, 6);

    return status;
}

process_status_t L3G4200D_init()
{
    process_status_t status = OK;

    // set ctrl_regs  data
    uint8_t ctrl_regs_data[] = {L3G4200D_CTRL_REG1_DATA, L3G4200D_CTRL_REG2_DATA, L3G4200D_CTRL_REG3_DATA,
                                L3G4200D_CTRL_REG4_DATA, L3G4200D_CTRL_REG5_DATA};

    status = I2C_write_buffer(L3G4200D_ADDR, CTRL_REG1, ctrl_regs_data, 5);

    return status;
}

process_status_t I2C_read_byte(uint16_t dev_addr, uint16_t addr, uint8_t *data)
{
    return I2C_read_buffer(dev_addr, addr, data, 1);
}

process_status_t I2C_read_buffer(uint16_t dev_addr, uint16_t addr, uint8_t *data, uint8_t length)
{
    // set MSB bit for multi reading
    if (length > 1)
    {
        addr |= 0x80;
    }

    if (HAL_I2C_Mem_Read(&I2C_INT, dev_addr, addr,
                         1, data, length, 200))
    {
        return TRANSFER_ERROR;
    }

    return OK;
}

process_status_t I2C_write_byte(uint16_t dev_addr, uint16_t addr, uint8_t data)
{
    return I2C_write_buffer(dev_addr, addr, &data, 1);
}

process_status_t I2C_write_buffer(uint16_t dev_addr, uint16_t addr, uint8_t *data, uint8_t length)
{
    // set MSB bit for multi writing
    if (length > 1)
    {
        addr |= 0x80;
    }

    if (HAL_I2C_Mem_Write(&I2C_INT, dev_addr, addr,
                          1, data, length, 200))
    {
        return TRANSFER_ERROR;
    }

    return OK;
}