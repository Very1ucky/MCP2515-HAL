#ifndef ACCEL_AMPERKA_H_
#define ACCEL_AMPERKA_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include "proc_status.h"

#define LIS331DLN_ADDR 0x30
#define L3G4200D_ADDR 0xD0
#define I2C_INT hi2c1

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

#define HP_FILTER_RESET 0x25

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

// set MSB bit to 1 for multireg reading
#define OUT_XYZ_START OUT_X_L|0x80
/* enable HPF on data
*/
#define LIS331DLN_CTRL_REG2_DATA 0b10010000
#define LIS331DLN_CTRL_REG3_DATA 0b00000000
/* FS = 2g
*/
#define LIS331DLN_CTRL_REG4_DATA 0b00000000

#define LIS331DLN_CTRL_REG5_DATA 0b00000000


#define L3G4200D_CTRL_REG1_DATA 0b00001111
#define L3G4200D_CTRL_REG2_DATA 0b00000000
#define L3G4200D_CTRL_REG3_DATA 0b00000000
// enable BDU (scale 250 dps)
#define L3G4200D_CTRL_REG4_DATA 0b10000000
#define L3G4200D_CTRL_REG5_DATA 0b00000000

typedef enum
{
    HalfHz = 0x47,
    OneHz = 0x67,
    TwoHz = 0x87,
    FiveHz = 0xA7,
    TenHz = 0xC7, 
    FiftyHz = 0x27,
    OneHundredHz = 0x2F,
    FourHundredHz = 0x37,
    OnekHz = 0x3F
} acel_poll_rate_t;

typedef struct {
    int16_t X;
    int16_t Y;
    int16_t Z;
} axes_sensor_data_t;

process_status_t read_axes_sensor_data(uint16_t dev_addr,axes_sensor_data_t *data_frame);
process_status_t LIS331DLN_init(acel_poll_rate_t poll_rate);
process_status_t L3G4200D_init();

#endif // ACCEL_AMPERKA_H_