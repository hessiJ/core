/*
 * MPU9250.h
 *
 *  Created on: 21.01.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_MPU9250_H_
#define CORE_MPU9250_H_


#include "nrf52spi.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "nrf_delay.h"



class MPU9250 {
private:
	uint8_t CS;
	uint8_t m_rx_buf[2]; /**< RX buffer. */
	uint8_t m_wx_buf[2];
	uint8_t m_rx_data_buf[7];

	uint8_t rreg;
	Nrf52_spi spi;
	void read_register(uint8_t readRegister, uint8_t rx_buf[], uint8_t size_rx);
	void write_register(uint8_t write_register, uint8_t data);
public:

    const uint8_t SPI_READ = 0x80;

    // MPU9250 registers
    const uint8_t ACCEL_OUT = 0x3B;
    const uint8_t GYRO_OUT = 0x43;
    const uint8_t TEMP_OUT = 0x41;
    const uint8_t EXT_SENS_DATA_00 = 0x49;
    const uint8_t ACCEL_CONFIG = 0x1C;

    const uint8_t ACCEL_FS_SEL_2G = 0x00;
    const uint8_t ACCEL_FS_SEL_4G = 0x08;
    const uint8_t ACCEL_FS_SEL_8G = 0x10;
    const uint8_t ACCEL_FS_SEL_16G = 0x18;

    const uint8_t GYRO_CONFIG = 0x1B;
    const uint8_t GYRO_FS_SEL_250DPS = 0x00;
    const uint8_t GYRO_FS_SEL_500DPS = 0x08;
    const uint8_t GYRO_FS_SEL_1000DPS = 0x10;
    const uint8_t GYRO_FS_SEL_2000DPS = 0x18;
    const uint8_t ACCEL_CONFIG2 = 0x1D;
    const uint8_t ACCEL_DLPF_184 = 0x01;
    const uint8_t ACCEL_DLPF_92 = 0x02;
    const uint8_t ACCEL_DLPF_41 = 0x03;
    const uint8_t ACCEL_DLPF_20 = 0x04;
    const uint8_t ACCEL_DLPF_10 = 0x05;
    const uint8_t ACCEL_DLPF_5 = 0x06;
    const uint8_t GYRO_CONFIG2 = 0x1A;
    const uint8_t GYRO_DLPF_184 = 0x01;
    const uint8_t GYRO_DLPF_92 = 0x02;
    const uint8_t GYRO_DLPF_41 = 0x03;
    const uint8_t GYRO_DLPF_20 = 0x04;
    const uint8_t GYRO_DLPF_10 = 0x05;
    const uint8_t GYRO_DLPF_5 = 0x06;
    const uint8_t SMPDIV = 0x19;
    const uint8_t INT_PIN_CFG = 0x37;
    const uint8_t INT_ENABLE = 0x38;
    const uint8_t INT_DISABLE = 0x00;
    const uint8_t INT_PULSE_50US = 0x00;
    const uint8_t INT_WOM_EN = 0x40;
    const uint8_t INT_RAW_RDY_EN = 0x01;
    const uint8_t PWR_MGMNT_1 = 0x6B;
    const uint8_t PWR_CYCLE = 0x20;
    const uint8_t PWR_SLEEP = 0x40;
    const uint8_t PWR_RESET = 0x80;
    const uint8_t CLOCK_SEL_PLL = 0x01;
    const uint8_t PWR_MGMNT_2 = 0x6C;
    const uint8_t SEN_ENABLE = 0x00;
    const uint8_t DIS_GYRO = 0x07;
    const uint8_t USER_CTRL = 0x6A;
    const uint8_t MOT_DETECT_CTRL = 0x69;
    const uint8_t ACCEL_INTEL_EN = 0x80;
    const uint8_t ACCEL_INTEL_MODE = 0x40;
    const uint8_t LP_ACCEL_ODR = 0x1E;
    const uint8_t WOM_THR = 0x1F;
    const uint8_t WHO_AM_I = 0x75;
    const uint8_t FIFO_EN = 0x23;
    const uint8_t FIFO_TEMP = 0x80;
    const uint8_t FIFO_GYRO = 0x70;
    const uint8_t FIFO_ACCEL = 0x08;
    const uint8_t FIFO_MAG = 0x01;
    const uint8_t FIFO_COUNT = 0x72;
    const uint8_t FIFO_READ = 0x74;
    // AK8963 registers
    const uint8_t AK8963_I2C_ADDR = 0x0C;
    const uint8_t AK8963_HXL = 0x03;
    const uint8_t AK8963_CNTL1 = 0x0A;
    const uint8_t AK8963_PWR_DOWN = 0x00;
    const uint8_t AK8963_CNT_MEAS1 = 0x12;
    const uint8_t AK8963_CNT_MEAS2 = 0x16;
    const uint8_t AK8963_FUSE_ROM = 0x0F;
    const uint8_t AK8963_CNTL2 = 0x0B;
    const uint8_t AK8963_RESET = 0x01;
    const uint8_t AK8963_ASA = 0x10;
    const uint8_t AK8963_WHO_AM_I = 0x00;

	MPU9250();
	uint8_t begin(Nrf52_spi t_spi, uint8_t CS);

	void set_accel_range(uint8_t range);
	uint8_t get_accel_range();

	void set_gyro_range(uint8_t range);
	uint8_t get_gyro_range();

	void set_dlpf_bandwith(uint8_t accel_dlpf, uint8_t gyro_dlpf);
	uint8_t get_dlpf_bandwith_acc();
	uint8_t get_dlpf_bandwith_gyro();

	void set_srd(uint8_t srd);
	uint8_t get_srd();

	void set_enable_data_ready_interrupt();
	uint8_t get_enable_data_ready_interrupt();

	void set_enable_wake_up_on_motion(float wom_thresh_mg, int odr);
	//uint8_t get_enable_wake_up_on_motion();

	void get_all(); //reads acc gyro and mag

	void get_acc(int16_t accel_data[3]);
	void get_acc_raw(uint8_t acc_data[6]);

	void get_gyro(int16_t gyro_data[3]);
	int16_t get_mag();
	int16_t get_acc_gyro();

	void read_temp();

	void sleep();

	void wakeup();
};

#endif /* CORE_MPU9250_H_ */
