/*
 * MPU9250.cpp
 *
 *  Created on: 21.01.2019
 *      Author: arthur.buchta
 */

#include "MPU9250.h"

MPU9250::MPU9250() {
	// TODO Auto-generated constructor stub
}


void MPU9250::read_register(uint8_t readRegister, uint8_t rx_buf[], uint8_t size_rx){

	spi.set_spi_settings(NRF_SPI_FREQ_500K, NRF_SPI_MODE_3, NRF_SPI_BIT_ORDER_MSB_FIRST);

	nrf_drv_gpiote_out_clear(CS);
	rreg = SPI_READ | readRegister;
	spi.nrf52_transfer(&rreg, 1, rx_buf, size_rx);
	nrf_drv_gpiote_out_set(CS);
}

void MPU9250::write_register(uint8_t write_register, uint8_t data){

	spi.set_spi_settings(NRF_SPI_FREQ_500K, NRF_SPI_MODE_3, NRF_SPI_BIT_ORDER_MSB_FIRST);
	nrf_drv_gpiote_out_clear(CS);
	nrf_delay_ms(2);
	m_wx_buf[0] = write_register;
	m_wx_buf[1] = data;
	spi.nrf52_transfer(m_wx_buf, 2, nullptr, 0);
	nrf_delay_ms(2);
	nrf_drv_gpiote_out_set(CS);
}


uint8_t MPU9250::begin(Nrf52_spi t_spi, uint8_t m_cs){
	CS = m_cs;

	spi.set_spi_settings(NRF_SPI_FREQ_500K, NRF_SPI_MODE_3, NRF_SPI_BIT_ORDER_MSB_FIRST);
	/*
	spi.begin(0,SPI_MISO_PIN, SPI_MOSI_PIN, SPI_SCK_PIN,
					NRF_DRV_SPI_MODE_3, NRF_DRV_SPI_FREQ_250K);
	 */


	read_register(WHO_AM_I, m_rx_buf, 2);

	if(m_rx_buf[1] != 0x73) return 3;
	return m_rx_buf[1];

}


void MPU9250::set_accel_range(uint8_t range){
	write_register(ACCEL_CONFIG, range);
}

uint8_t MPU9250::get_accel_range(){
	read_register(ACCEL_CONFIG, m_rx_buf, 2);
	return m_rx_buf[1];
}

void MPU9250::set_gyro_range(uint8_t range){
	write_register(GYRO_CONFIG, range);
}

uint8_t MPU9250::get_gyro_range(){
	read_register(GYRO_CONFIG, m_rx_buf, 2);
	return m_rx_buf[1];
}

void MPU9250::set_dlpf_bandwith(uint8_t accel_dlpf, uint8_t gyro_dlpf){
	write_register(ACCEL_CONFIG2, accel_dlpf);
	write_register(GYRO_CONFIG2, gyro_dlpf);
}

uint8_t MPU9250::get_dlpf_bandwith_acc(){
	read_register(ACCEL_CONFIG2, m_rx_buf, 2);
	return m_rx_buf[1];
}

uint8_t MPU9250::get_dlpf_bandwith_gyro(){
	read_register(GYRO_CONFIG2, m_rx_buf, 2);
	return m_rx_buf[1];
}

void MPU9250::set_enable_wake_up_on_motion(float womThresh_mg, int odr){

	// reset the MPU9250
	write_register(PWR_MGMNT_1,PWR_RESET);

	write_register(PWR_MGMNT_1, 0x00); // cycle 0, sleep 0, standby 0
	write_register(PWR_MGMNT_2,DIS_GYRO); // disable gyro measurements
	write_register(ACCEL_CONFIG2,ACCEL_DLPF_184); // setting accel bandwidth to 184Hz
	write_register(INT_ENABLE,INT_WOM_EN); // enabling interrupt to wake on motion
	write_register(MOT_DETECT_CTRL,(ACCEL_INTEL_EN | ACCEL_INTEL_MODE)); // enabling accel hardware intelligence

	write_register(WOM_THR, (uint8_t) (womThresh_mg/4)), //mappend WomThresh_mg form (0,1020) to (0, 255)
	write_register(LP_ACCEL_ODR,(uint8_t)odr); // set frequency of wakeup
	write_register(PWR_MGMNT_1,PWR_CYCLE); // switch to accel low power mode
}

void MPU9250::sleep(){
	write_register(PWR_MGMNT_1,PWR_SLEEP); // switch to accel low power mode
}

void MPU9250::wakeup(){
	write_register(PWR_MGMNT_1,0x00);
}
void MPU9250::get_acc(int16_t acc_data[3]){
	read_register(ACCEL_OUT, m_rx_data_buf, 7);
	acc_data[0] = (((int16_t)m_rx_data_buf[1]) << 8) | m_rx_data_buf[2];
	acc_data[1] = (((int16_t)m_rx_data_buf[3]) << 8) | m_rx_data_buf[4];
	acc_data[2] = (((int16_t)m_rx_data_buf[5]) << 8) | m_rx_data_buf[6];
}

void MPU9250::get_acc_raw(uint8_t acc_data[6]){
	read_register(ACCEL_OUT, m_rx_data_buf, 7);
	acc_data[0] = m_rx_data_buf[1];
	acc_data[1] = m_rx_data_buf[2];
	acc_data[2] = m_rx_data_buf[3];
	acc_data[3] = m_rx_data_buf[4];
	acc_data[4] = m_rx_data_buf[5];
	acc_data[5] = m_rx_data_buf[6];
}


void MPU9250::get_gyro(int16_t gyro_data[3]){
	read_register(GYRO_OUT, m_rx_data_buf, 7);
	gyro_data[0] = (((int16_t)m_rx_data_buf[1]) << 8) | m_rx_data_buf[2];
	gyro_data[1] = (((int16_t)m_rx_data_buf[3]) << 8) | m_rx_data_buf[4];
	gyro_data[2] = (((int16_t)m_rx_data_buf[5]) << 8) | m_rx_data_buf[6];
}

