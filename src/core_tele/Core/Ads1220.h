/*
 * Ads1220.h
 *
 *  Created on: 22.01.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_ADS1220_H_
#define CORE_ADS1220_H_

#include "nrf52spi.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#define _BV(bit) (1<<(bit))

class Ads1220 {
private:

	Nrf52_spi spi;
	//ADS1220 SPI commands

	//SPI configs
	const nrf_spi_frequency_t NRF_SPI_FREQ = NRF_SPI_FREQ_2M;

public:
	const uint8_t SPI_MASTER_DUMMY    = 0xFF;
	const uint8_t RESET               = 0x06;   //Send the RESET command (06h) to make sure the ADS1220 is properly reset after power-up
	const uint8_t START               = 0x08;    //Send the START/SYNC command (08h) to start converting in continuous conversion mode
	const uint8_t PWRDOWN			  = 0x02;   //Enter power-down mode
	const uint8_t WREG  = 0x40;
	const uint8_t RREG  = 0x20;
	const uint8_t RDATA = 0x10;

	//Config registers
	const uint8_t CONFIG_REG0_ADDRESS = 0x00;
	const uint8_t CONFIG_REG1_ADDRESS = 0x01;
	const uint8_t CONFIG_REG2_ADDRESS = 0x02;
	const uint8_t CONFIG_REG3_ADDRESS = 0x03;


	const uint8_t REG_CONFIG0_PGA_GAIN_MASK = 0x0E;
	const uint8_t REG_CONFIG0_MUX_MASK      = 0xF0;
	const uint8_t REG_CONFIG0_BYPASS		= 0x01;

	const uint8_t REG_CONFIG1_DR_MASK   =   0xE0;
	const uint8_t REG_CONFIG1_OP_MASK =  0x18;
	const uint8_t REG_CONFIG1_CON_MASK =  0x04;
	const uint8_t REG_CONFIG1_TEM_MASK =  0x02;
	const uint8_t REG_CONFIG1_BCS_MASK =  0x01;

	const uint8_t REG_CONFIG2_IDAC_MASK =	  0x07;
	const uint8_t REG_CONFIG2_PSW_MASK  =    0x08;
	const uint8_t REG_CONFIG2_FIR_MASK 	=  0x30;
	const uint8_t REG_CONFIG2_VREF_MASK =	  0xC0;

	const uint8_t REG_CONFIG3_DRDYM_MASK	=  0x02;
	const uint8_t REG_CONFIG3_I2MUX_MASK	=  0x1C;
	const uint8_t REG_CONFIG3_I1MUX_MASK  =  0xE0;

	//Settings
	//CONFIG0
	const uint8_t MUX_AIN0_AIN1 =  0x00;
	const uint8_t MUX_AIN0_AIN2 =  0x10;
	const uint8_t MUX_AIN0_AIN3 =  0x20;
	const uint8_t MUX_AIN1_AIN2 =  0x30;
	const uint8_t MUX_AIN1_AIN3 =  0x40;
	const uint8_t MUX_AIN2_AIN3 =  0x50;
	const uint8_t MUX_AIN1_AIN0 =  0x60;
	const uint8_t MUX_AIN3_AIN2 =  0x70;
	const uint8_t MUX_AIN0_AVSS =  0x80;
	const uint8_t MUX_AIN1_AVSS =  0x90;
	const uint8_t MUX_AIN2_AVSS =  0xA0;
	const uint8_t MUX_AIN3_AVSS =  0xB0;

	const uint8_t PGA_GAIN_1   = 0x00;
	const uint8_t PGA_GAIN_2   = 0x02;
	const uint8_t PGA_GAIN_4   = 0x04;
	const uint8_t PGA_GAIN_8   = 0x06;
	const uint8_t PGA_GAIN_16  = 0x08;
	const uint8_t PGA_GAIN_32  = 0x0A;
	const uint8_t PGA_GAIN_64  = 0x0C;
	const uint8_t PGA_GAIN_128 = 0x0E;

	const uint8_t  PGA_BYPASS_ENABLED	= 0;
	const uint8_t  PGA_BYPASS_DISABLED	= 1;

	//CONFIG1
	const uint8_t DR_20SPS    =0x00;
	const uint8_t DR_45SPS    =0x20;
	const uint8_t DR_90SPS    =0x40;
	const uint8_t DR_175SPS   =0x60;
	const uint8_t DR_330SPS   =0x80;
	const uint8_t DR_600SPS   =0xA0;
	const uint8_t DR_1000SPS  =0xC0;

	const uint8_t DR_TM_40SPS    = 0x00 ;
	const uint8_t DR_TM_90SPS    = 0x20 ;
	const uint8_t DR_TM_180SPS   = 0x40;
	const uint8_t DR_TM_350SPS   = 0x60 ;
	const uint8_t DR_TM_660SPS   = 0x80 ;
	const uint8_t DR_TM_1200SPS  = 0xA0;
	const uint8_t DR_TM_2000SPS  = 0xC0 ;

	const uint8_t OP_MODE_NM	= 0x00;
	const uint8_t OP_MODE_DCM	= 0x08;
	const uint8_t OP_MODE_TM	= 0x10;

	const uint8_t CON_MODE_SS	= 0x00;
	const uint8_t CON_MODE_CC	= 0x04;


	const uint8_t TEMPS_DISABLED= 0x00;
	const uint8_t TEMPS_ENABLED	= 0x02;

	const uint8_t BCS_OFF			= 0x00;
	const uint8_t BCS_ON			= 0x01;

	//CONFIG2
	const uint8_t IDAC_OFF		= 0x00;
	const uint8_t IDAC_10uA		= 0x01;
	const uint8_t IDAC_50uA		= 0x02;
	const uint8_t IDAC_100uA		= 0x03;
	const uint8_t IDAC_250uA		= 0x04;
	const uint8_t IDAC_500uA		= 0x05;
	const uint8_t IDAC_1000uA		= 0x06;
	const uint8_t IDAC_1500uA		= 0x07;

	const uint8_t PSW_ALWAYS_OPEN	= 0x00;
	const uint8_t PSW_ALWAYS_AUTO	= 0x08;

	const uint8_t FIR_NO_50_60	= 0x00;
	const uint8_t FIR_50_60		= 0x10;
	const uint8_t FIR_50			= 0x20;
	const uint8_t FIR_60			= 0x30;

	const uint8_t VREF_INTERNAL	= 0x00;
	const uint8_t VREF_EXT_REFP0_REFN0	= 0x40;
	const uint8_t VREF_EXT_REFP1_REFN1	= 0x80;
	const uint8_t VREF_ANALOG_SUPPLY		= 0xC0;

	//CONFIG3
	const uint8_t DRDY_ONLY				= 0x00;
	const uint8_t DRDY_DOUT_DRDY			= 0x01;


	const uint8_t IDAC2_DISABLED			= 0x00;
	const uint8_t IDAC2_AIN0				= 0x04;
	const uint8_t IDAC2_AIN1				= 0x08;
	const uint8_t IDAC2_AIN2				= 0x0C;
	const uint8_t IDAC2_AIN3				= 0x10;
	const uint8_t IDAC2_REFP0				= 0x14;
	const uint8_t IDAC2_REFN0				= 0x18;

	const uint8_t IDAC1_DISABLED			= 0x00;
	const uint8_t IDAC1_AIN0				= 0x20;
	const uint8_t IDAC1_AIN1				= 0x40;
	const uint8_t IDAC1_AIN2				= 0x60;
	const uint8_t IDAC1_AIN3				= 0x80;
	const uint8_t IDAC1_REFP0				= 0xA0;
	const uint8_t IDAC1_REFN0				= 0xC0;

	const uint8_t MUX_SE_CH0      = 0x80;
	const uint8_t MUX_SE_CH1      = 0x90;
	const uint8_t MUX_SE_CH2      = 0xA0;
	const uint8_t MUX_SE_CH3      = 0xB0;

    uint8_t m_config_reg0;
    uint8_t m_config_reg1;
    uint8_t m_config_reg2;
    uint8_t m_config_reg3;

    uint8_t Config_Reg0;
    uint8_t Config_Reg1;
    uint8_t Config_Reg2;
    uint8_t Config_Reg3;

    uint8_t m_drdy_pin=6;
    uint8_t m_cs_pin=7;

	uint8_t mask_collection[15]{
	  REG_CONFIG0_PGA_GAIN_MASK, // 0   0x00
	  REG_CONFIG0_MUX_MASK,      // 1   0x01
	  REG_CONFIG0_BYPASS,		 // 2   0x02
	                             //
	  REG_CONFIG1_DR_MASK,       // 3   0x03
	  REG_CONFIG1_OP_MASK, 	     // 4   0x04
	  REG_CONFIG1_CON_MASK, 	 // 5   0x05
	  REG_CONFIG1_TEM_MASK, 	 // 6   0x06
	  REG_CONFIG1_BCS_MASK, 	 // 7   0x07
	                             //
	  REG_CONFIG2_IDAC_MASK, 	 // 8   0x08
	  REG_CONFIG2_PSW_MASK ,     // 9   0x09
	  REG_CONFIG2_FIR_MASK, 	 // 10  0x0A
	  REG_CONFIG2_VREF_MASK, 	 // 11  0x0B
	                             //
	  REG_CONFIG3_DRDYM_MASK,	 // 12  0x0C
	  REG_CONFIG3_I2MUX_MASK,	 // 13  0x0D
	  REG_CONFIG3_I1MUX_MASK	 //	14  0x0E
	};

	const uint8_t master_dummy[3] = {0xff, 0xff, 0xff};
	uint8_t m_rx_buf[2]; /**< RX buffer. */
	uint8_t m_rx_data_buf[4];
	uint16_t m_rx_data_buf_size = 4;

	uint16_t rx_buf_size = 2;
	uint8_t m_wx_buf[2]; /**< WX buffer. */

	uint8_t rreg = 0;

public:
	Ads1220();
	void begin(Nrf52_spi t_spi, uint8_t cs_pin, uint8_t drdy_pin);

	/**
	 * @brief basic fast configuration
	 * @param datarate
	 * @param pga_gain
	 * @param mux_channles
	 * @param power_switch
	 */
	void init_config(uint8_t datarate, uint8_t pga_gain, uint8_t mux_channles, uint8_t power_switch);

    void Start_Conv(void);
    void ads1220_Reset(void);
    void Power_Down();

    /**
     *
     * @param values amount of values used for tara
     * @param interval_ms delay between value measurement
     * @return offset
     */
    int32_t tara(int values, int interval_ms);

    /**
     * @brief basic method that converts the 3 byte ads1220 data to int32_t
     * @param ads_1220
     * @return converted adc value in int32_t
     */
    //static int32_t convert2int(uint8_t *ads_1220);

    /**
     * @brief handles the spi buffer_adc data
     * @param counter check byte that increments with an interval of 6 ms
     * @param buffer_ble new buffer that will be send by ble notification
     * @param buffer_adc spi buffer data from adc
     * @param offset calculated offset
     * @param identifier identifier of ble server
     */
    static void copy_relavant_data_3byte(uint8_t counter, uint8_t buffer_ble[20], uint8_t buffer_adc[6][7], int32_t offset, uint8_t identifier);
    static void copy_relavant_data_2byte(uint8_t counter, uint8_t buffer_ble[20], uint8_t buffer_adc[6][7], int32_t offset, uint8_t identifier);
    static bool averageReady(uint8_t buffer_adc[20], int32_t offset, double intervall,  uint8_t average[3], uint8_t min[3], uint8_t max[3]);

    void SPI_Command(unsigned char data_in);
    void writeRegister(uint8_t address, uint8_t value);
    uint8_t readRegister(uint8_t address);
    uint8_t * Read_Data(void);
    int32_t Read_WaitForData(uint8_t SPI_BUFF[3]);
	void get_config_reg(uint8_t config_Buff[4]);

    void PGA_OFF(void);
    void PGA_ON(void);
    void set_conv_mode_continuous(void);
    void Single_shot_mode_ON(void);
    void set_data_rate(int datarate);
    void set_pga_gain(int pgagain);
    void select_mux_channels(int channels_conf);
    void set_conv_mode_single_shot(void);
    int32_t Read_SingleShot_WaitForData(uint8_t spi_buff[3]);
	uint8_t Read_WaitForByteData(int bytenumber);
    int32_t Read_SingleShot_SingleEnded_WaitForData(uint8_t channel_no);
	int32_t Read_WaitForData_raw(uint8_t * data);
	int32_t read_Sensor_data(uint8_t spi_buff[3]);

	/**
	 * @brief general method to set ads1220 registers
	 * @details example set datarate to 20SPS: uint8_t settings[2] = {REG_CONFIG1_DR_MASK,DR_20SPS}; set_settings(settings);
	 * @param settings array with first element is mask (see mask_collection) second element is settings
	 */
	void set_settings(uint8_t settings[2]);
};

#endif /* CORE_ADS1220_H_ */
