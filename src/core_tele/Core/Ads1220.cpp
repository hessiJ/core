/*
 * Ads1220.cpp
 *
 *  Created on: 22.01.2019
 *      Author: arthur.buchta
 */

#include "Ads1220.h"

Ads1220::Ads1220() {
	// TODO Auto-generated constructor stub
}

void Ads1220::writeRegister(uint8_t address, uint8_t value)
{

	spi.set_spi_settings(NRF_SPI_FREQ, NRF_SPI_MODE_1, NRF_SPI_BIT_ORDER_MSB_FIRST);
	nrf_drv_gpiote_out_clear(m_cs_pin);
	//nrf_delay_ms(2);
	m_wx_buf[0] = WREG|(address<<2);
	m_wx_buf[1] = value;
	spi.nrf52_transfer(m_wx_buf, 2, nullptr, 0);
	//nrf_delay_ms(2);
	nrf_drv_gpiote_out_set(m_cs_pin);
}

uint8_t Ads1220::readRegister(uint8_t address)
{

	spi.set_spi_settings(NRF_SPI_FREQ, NRF_SPI_MODE_1, NRF_SPI_BIT_ORDER_MSB_FIRST);
    nrf_drv_gpiote_out_clear(m_cs_pin);
    rreg = RREG|(address<<2);
    spi.nrf52_transfer(&rreg, 1, m_rx_buf, rx_buf_size);
    nrf_drv_gpiote_out_set(m_cs_pin);

    return m_rx_buf[1];

}


void Ads1220::begin(Nrf52_spi t_spi, uint8_t cs_pin, uint8_t drdy_pin)
{
    m_drdy_pin=drdy_pin;
    m_cs_pin=cs_pin;
    //spi = t_spi;

    spi.set_spi_settings(NRF_SPI_FREQ, NRF_SPI_MODE_1, NRF_SPI_BIT_ORDER_MSB_FIRST);
    nrf_delay_ms(100);
    ads1220_Reset();
    nrf_delay_ms(100);

    m_config_reg0 = 0x00;   //Default settings: AINP=AIN0, AINN=AIN1, Gain 1, PGA enabled
    m_config_reg1 = 0x04;   //Default settings: DR=20 SPS, Mode=Normal, Conv mode=continuous, Temp Sensor disabled, Current Source off
    m_config_reg2 = 0x10;   //Default settings: Vref internal, 50/60Hz rejection, power open, IDAC off
    m_config_reg3 = 0x00;   //Default settings: IDAC1 disabled, IDAC2 disabled, DRDY pin only

    writeRegister( CONFIG_REG0_ADDRESS , m_config_reg0);
    writeRegister( CONFIG_REG1_ADDRESS , m_config_reg1);
    writeRegister( CONFIG_REG2_ADDRESS , m_config_reg2);
    writeRegister( CONFIG_REG3_ADDRESS , m_config_reg3);

    nrf_delay_ms(100);
}

void Ads1220::SPI_Command(uint8_t data_in)
{
    spi.set_spi_settings(NRF_SPI_FREQ, NRF_SPI_MODE_1, NRF_SPI_BIT_ORDER_MSB_FIRST);
	nrf_drv_gpiote_out_clear(m_cs_pin);
    nrf_delay_ms(2);
    nrf_drv_gpiote_out_set(m_cs_pin);
    nrf_delay_ms(2);
	nrf_drv_gpiote_out_clear(m_cs_pin);
    nrf_delay_ms(2);
	spi.nrf52_transfer(&data_in, 1, nullptr, 0);
    nrf_delay_ms(2);
    nrf_drv_gpiote_out_set(m_cs_pin);
}

void Ads1220::ads1220_Reset()
{
    SPI_Command(RESET);
}

void Ads1220::Start_Conv()
{
    SPI_Command(START);
}

void Ads1220::Power_Down(){
	SPI_Command(PWRDOWN);
}

void Ads1220::PGA_ON(void)
{
    m_config_reg0 &= ~_BV(0);
    writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void Ads1220::PGA_OFF(void)
{
    m_config_reg0 |= _BV(0);
    writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void Ads1220::set_conv_mode_continuous(void)
{
    m_config_reg1 |= _BV(2);
    writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);
}

void Ads1220::set_conv_mode_single_shot(void)
{
    m_config_reg1 &= ~_BV(2);
    writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);
}

void Ads1220::set_data_rate(int datarate)
{

    m_config_reg1 &= ~REG_CONFIG1_DR_MASK;
    m_config_reg1 |= datarate;

    writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);
}

void Ads1220::Single_shot_mode_ON(){
	m_config_reg1 &= ~REG_CONFIG1_CON_MASK;
    m_config_reg1 |= CON_MODE_SS;
    writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);
}

void Ads1220::select_mux_channels(int channels_conf)
{
    m_config_reg0 &= ~REG_CONFIG0_MUX_MASK;
    m_config_reg0 |= channels_conf;
    writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void Ads1220::set_pga_gain(int pgagain)
{
    m_config_reg0 &= ~REG_CONFIG0_PGA_GAIN_MASK;
    m_config_reg0 |= pgagain ;
    writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void Ads1220::get_config_reg(uint8_t config_Buff[4])
{

    m_config_reg0 = readRegister(CONFIG_REG0_ADDRESS);
    m_config_reg1 = readRegister(CONFIG_REG1_ADDRESS);
    m_config_reg2 = readRegister(CONFIG_REG2_ADDRESS);
    m_config_reg3 = readRegister(CONFIG_REG3_ADDRESS);

    config_Buff[0] = m_config_reg0 ;
    config_Buff[1] = m_config_reg1 ;
    config_Buff[2] = m_config_reg2 ;
    config_Buff[3] = m_config_reg3 ;

}



int32_t Ads1220::read_Sensor_data(uint8_t spi_buff[3]){
	int32_t mResult32=0;
    long int bit24;

    spi.set_spi_settings(NRF_SPI_FREQ, NRF_SPI_MODE_1, NRF_SPI_BIT_ORDER_MSB_FIRST);

    nrf_drv_gpiote_out_clear(m_cs_pin);
    spi.nrf52_transfer(&RDATA, 1, m_rx_data_buf, m_rx_data_buf_size);

    nrf_drv_gpiote_out_set(m_cs_pin);
    spi_buff[0] = m_rx_data_buf[1];
    spi_buff[1] = m_rx_data_buf[2];
    spi_buff[2] = m_rx_data_buf[3];

    bit24 = m_rx_data_buf[1];
    bit24 = (bit24 << 8) | m_rx_data_buf[2];
    bit24 = (bit24 << 8) | m_rx_data_buf[3];                                 // Converting 3 bytes to a 24 bit int

    bit24 = ( bit24 << 8 );
    mResult32 = ( bit24 >> 8 );                      // Converting 24 bit two's complement to 32 bit two's complement

    return mResult32;
}

int32_t Ads1220::Read_WaitForData(uint8_t spi_buff[3])
{
	int32_t mResult32=0;
    long int bit24;

    //if((digitalRead(m_drdy_pin)) == LOW)             //        Wait for DRDY to transition low
    //{

		spi.set_spi_settings(NRF_SPI_FREQ, NRF_SPI_MODE_1, NRF_SPI_BIT_ORDER_MSB_FIRST);
    	nrf_drv_gpiote_out_clear(m_cs_pin);
    	/*
        for (int i = 0; i < 3; i++)
        {
          SPI_Buff[i] = SPI.transfer(SPI_MASTER_DUMMY);
        }
        */

        spi.nrf52_transfer(master_dummy, 3, spi_buff, 3);

        nrf_drv_gpiote_out_set(m_cs_pin);

        bit24 = spi_buff[0];
        bit24 = (bit24 << 8) | spi_buff[1];
        bit24 = (bit24 << 8) | spi_buff[2];                                 // Converting 3 bytes to a 24 bit int

        bit24 = ( bit24 << 8 );
        mResult32 = ( bit24 >> 8 );                      // Converting 24 bit two's complement to 32 bit two's complement
    //}
    return mResult32;
}

int32_t Ads1220::Read_SingleShot_WaitForData(uint8_t spi_buff[3])
{
    int32_t mResult32=0;
    long int bit24;

    Start_Conv();

    //if((digitalRead(m_drdy_pin)) == LOW)             //        Wait for DRDY to transition low
    //{
    	nrf_drv_gpiote_out_clear(m_cs_pin);
    	spi.nrf52_transfer(master_dummy, 3, spi_buff, 3);
    	nrf_drv_gpiote_out_set(m_cs_pin);


        bit24 = spi_buff[0];
        bit24 = (bit24 << 8) | spi_buff[1];
        bit24 = (bit24 << 8) | spi_buff[2];                                 // Converting 3 bytes to a 24 bit int

        bit24= ( bit24 << 8 );
        mResult32 = ( bit24 >> 8 );                      // Converting 24 bit two's complement to 32 bit two's complement
    //}
    return mResult32;
}

void Ads1220::set_settings(uint8_t settings[2]){

	if( (settings[0]>=0) & (settings[0]<=2) ){ //Settings config Register 0
		m_config_reg0 &= ~mask_collection[settings[0]];
		m_config_reg0 |= settings[1];
		writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);

	}else if( (settings[0]>=3) & (settings[0]<=7) ){ //Settings config Register 1
		m_config_reg1 &= ~mask_collection[settings[0]];
		m_config_reg1 |= settings[1];
		writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);

	}else if( (settings[0]>=8) & (settings[0]<=11) ){ //Settings config Register 2
		m_config_reg2 &= ~mask_collection[settings[0]];
		m_config_reg2 |= settings[1];
		writeRegister(CONFIG_REG2_ADDRESS,m_config_reg2);

	}else if( (settings[0]>=12) & (settings[0]<=14) ){ //Settings config Register 3
		m_config_reg3 &= ~mask_collection[settings[0]];
		m_config_reg3 |= settings[3];
		writeRegister(CONFIG_REG3_ADDRESS,m_config_reg3);
	}else{
		//configuration not available error
	}

}


void Ads1220::init_config(uint8_t datarate, uint8_t pga_gain, uint8_t mux_channles, uint8_t power_switch){
    //int ready =
	uint8_t temp_config[2] = {0,0};

    set_data_rate(datarate);

    set_pga_gain(pga_gain);

    select_mux_channels(mux_channles);

    temp_config[0] = REG_CONFIG2_PSW_MASK;
    temp_config[1] = power_switch;
    set_settings(temp_config);

}

/**
 * @brief basic method that converts the 3 byte ads1220 data to int32_t
 * @param ads_1220
 * @return converted adc value in int32_t
 */
static int32_t convert2int(uint8_t *ads_1220){
	int32_t mResult32 = 0;
    long int bit24;

    bit24 = ads_1220[0];
    bit24 = (bit24 << 8) | ads_1220[1];
    bit24 = (bit24 << 8) | ads_1220[2];

    bit24 = ( bit24 << 8 ); // Converting 3 bytes to a 24 bit int
    mResult32 = ( bit24 >> 8 );
    return mResult32;
}

/**
 *
 * @param values amount of values used for tara
 * @param interval_ms delay between value measurement
 * @return offset
 */
int32_t Ads1220::tara(int values, int interval_ms){
	uint8_t ads_1220[3];
	int32_t mResult32_offset = 0;

	for(int i=0; i<values; i++){
		read_Sensor_data(ads_1220);
	    mResult32_offset += convert2int(ads_1220)/values;
		nrf_delay_ms(interval_ms);
	}
	return mResult32_offset;
}

/**
 * @brief subtracts offset
 * @param ads_1220 raw ads1220 sensor data
 * @param offset measured sensor data offset
 */
static int32_t substractOffset(uint8_t *ads_1220, int32_t offset){
	int32_t mResult32 = convert2int(ads_1220) - offset;

    ads_1220[2] = mResult32 & 0xff;
    ads_1220[1] = (mResult32>>8) & 0xff;
    ads_1220[0] = (mResult32>>16) & 0xff;
    return mResult32;
}
/*
static int32_t substractOffset2(uint8_t *ads_1220, int32_t offset){
	return (convert2int(ads_1220) - offset);
}
*/
static void convertInt32To8ByteArray(int32_t adcData, uint8_t ads_1220[3]){
	ads_1220[2] = adcData & 0xff;
	ads_1220[1] = (adcData>>8) & 0xff;
	ads_1220[0] = (adcData>>16) & 0xff;
}

/**
 * @brief handles the spi buffer_adc data
 * @param counter check byte that increments with an interval of 6 ms
 * @param buffer_ble new buffer that will be send by ble notification
 * @param buffer_adc spi buffer data from adc
 * @param offset calculated offset
 * @param identifier identifier of ble server
 */
void Ads1220::copy_relavant_data_3byte(uint8_t counter, uint8_t buffer_ble[20], uint8_t buffer_adc[6][7], int32_t offset, uint8_t identifier){

	buffer_ble[0] = counter;

	substractOffset(&buffer_adc[0][2], offset);
	buffer_ble[1] = buffer_adc[0][2];
	buffer_ble[2] = buffer_adc[0][3];
	buffer_ble[3] = buffer_adc[0][4];


	substractOffset(&buffer_adc[1][2], offset);
	buffer_ble[4] = buffer_adc[1][2];
	buffer_ble[5] = buffer_adc[1][3];
	buffer_ble[6] = buffer_adc[1][4];


	substractOffset(&buffer_adc[2][2], offset);
	buffer_ble[7] = buffer_adc[2][2];
	buffer_ble[8] = buffer_adc[2][3];
	buffer_ble[9] = buffer_adc[2][4];

	substractOffset(&buffer_adc[3][2], offset);
	buffer_ble[10] = buffer_adc[3][2];
	buffer_ble[11] = buffer_adc[3][3];
	buffer_ble[12] = buffer_adc[3][4];

	substractOffset(&buffer_adc[4][2], offset);
	buffer_ble[13] = buffer_adc[4][2];
	buffer_ble[14] = buffer_adc[4][3];
	buffer_ble[15] = buffer_adc[4][4];

	substractOffset(&buffer_adc[5][2], offset);
	buffer_ble[16] = buffer_adc[5][2];
	buffer_ble[17] = buffer_adc[5][3];
	buffer_ble[18] = buffer_adc[5][4];
	buffer_ble[19] = identifier;
}


static int32_t dataBigger(int32_t old_data, int32_t new_data){
	if(new_data>old_data){
		return new_data;
	}else{
		return old_data;
	}
}

static int32_t dataSmaller(int32_t oldData, int32_t newData){
	if(newData<oldData){
		return newData;
	}else{
		return oldData;
	}
}


bool Ads1220::averageReady(uint8_t buffer_adc[20], int32_t offset, double intervall, uint8_t average[3], uint8_t min[3], uint8_t max[3]){
	static int countAccumulatedData = 0;
	double localSensorData = 0;
	static double localAverage = 0;
	static double localMax = 0;
	static double localMin = 0;

	for(int i = 1; i<=16; i+=3){
		localSensorData = convert2int(&buffer_adc[i]);
		localAverage += localSensorData/intervall;
		localMax = dataBigger(localMax,localSensorData);
		localMin = dataSmaller(localMin, localSensorData);
	}
	countAccumulatedData += 6;

	if(countAccumulatedData >= intervall){
		convertInt32To8ByteArray((int32_t)localMin, min);
		convertInt32To8ByteArray((int32_t)localMax, max);
		convertInt32To8ByteArray((int32_t)localAverage, average);
		countAccumulatedData = 0;
		localMax = localAverage;
		localMin = localAverage;
		localAverage = 0;
		return true;
	}
	return false;
}


