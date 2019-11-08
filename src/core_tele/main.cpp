
extern "C"{
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "app_util_platform.h"

#include "bsp_btn_ble.h"
#include "MPU9250.h"
#include "GPIOTE.h"
#include "Ads1220.h"

#include "nrf_drv_clock.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

#include "nrf_drv_saadc.h"
}
#include "Blecore.h"
#include "Fstoragecore.h"
#include "AttributeStorage.h"
#include "Nordicstandard.h"

#define DEAD_BEEF                       0xDEADBEEF
#define UNUSED(x) (void)(x)


APP_TIMER_DEF(m_timer_adv_data);

static const nrf_drv_timer_t m_timer_spi = NRF_DRV_TIMER_INSTANCE(2);
static const nrf_drv_timer_t m_timer_spi_counter = NRF_DRV_TIMER_INSTANCE(3);

static const uint8_t SENSOR_ADC_SETUP = 0x10;
static const uint8_t SENSOR_MPU9250_SETUP = 0x30;
static const uint8_t ATTRIBUTE_CALIBRATION_UNIT = 0x20;
static const uint8_t ATTRIBUTE_CALIBRATION_VALUE = 0x21;
static const uint8_t ATTRIBUTE_IDENTIFIER = 0x22;
static const uint8_t ATTRIBUTE_DEVICENAME = 0x23;
static const uint8_t ATTRIBUTE_RESET_DEVICE = 0x24;
bool setCalibrationUnit = false;
uint8_t receivedCalibrationUnit = 0;

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi_inst = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

static nrf_ppi_channel_t     m_ppi_channel_start_spi;
static nrf_ppi_channel_t     m_ppi_channel_count_spi;

#define SCHED_MAX_EVENT_DATA_SIZE       APP_TIMER_SCHED_EVENT_DATA_SIZE
#define SCHED_QUEUE_SIZE                10

double us_counter = 0;
uint64_t ms_counter = 0;
uint64_t s_counter = 0;


Nrf52_spi spi;
MPU9250 imu_MPU9250;
Ads1220 adc_Ads1220;

core::Ble_core ble;
core::AttributeStorage attributeStorage;
core::Nordic_standard norstd;

uint8_t identifier = 14;

uint8_t const CS_MPU9250 = 16;
uint8_t const CS_ADS1220 = 7;
uint8_t const DRDY_ADS1220 = 15;
static uint8_t test_byte = 0;
uint8_t ble_data_counter = 0;
static uint8_t flash_calib_value[20];
uint16_t sizeOfDeviceName = 7;
uint8_t deviceName2[10];
uint8_t deviceName[10];
double mIntervall = 1200.0;
uint8_t mAds1220Average[3];
uint8_t mAds1220Min[3];
uint8_t mAds1220Max[3];



//app_sched_event_handler_t _app_sched_handler;

uint8_t data_acc_raw[4];
uint16_t size_data_acc_raw = 3;
uint8_t data_adc_raw[3];
uint16_t size_data_adc_raw = 3;

const uint8_t advertiseIdentifier[2] = {10, 7};
uint8_t advertiseAdcValue[3];
uint8_t advBatteryVal[2];
uint8_t advCalibrationUnit;
uint8_t advCalibrationValue[4];

uint8_t adv_data[12] = {
		0x01, 0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x09, 0xa1,
		0xa2, 0xa3};
uint16_t size_adv_data = 12;

bool data_ready = false;
bool startNotification = false;
bool sample_adc = false;


// Start sync; Read_cmd; read data dummy, Power off
uint8_t tx_buffer_adc[6][7] = {
		adc_Ads1220.START, adc_Ads1220.RDATA, 0xff, 0xff, 0xff, adc_Ads1220.PWRDOWN, 0xff,
		adc_Ads1220.START, adc_Ads1220.RDATA, 0xff, 0xff, 0xff, adc_Ads1220.PWRDOWN, 0xff,
		adc_Ads1220.START, adc_Ads1220.RDATA, 0xff, 0xff, 0xff, adc_Ads1220.PWRDOWN, 0xff,
		adc_Ads1220.START, adc_Ads1220.RDATA, 0xff, 0xff, 0xff, adc_Ads1220.PWRDOWN, 0xff,
		adc_Ads1220.START, adc_Ads1220.RDATA, 0xff, 0xff, 0xff, adc_Ads1220.PWRDOWN, 0xff,
		adc_Ads1220.START, adc_Ads1220.RDATA, 0xff, 0xff, 0xff, adc_Ads1220.PWRDOWN, 0xff
};


uint8_t rx_buffer_adc[6][7];

uint8_t rx_buffer_adc_ble[20];
uint16_t rx_buff_size = 20;
int ads_1220_spi_freq = 2000;
int32_t ads1220_offset;

void disable_timer();
void disable_ppi();
void disable_timer_and_ppi();
void start_timer();
void start_ppi();
void start_timer_and_ppi();

nrf_drv_spi_xfer_desc_t const transfer_buffers = {
		.p_tx_buffer = tx_buffer_adc[0],
		.tx_length = 7,
		.p_rx_buffer = rx_buffer_adc[0],
		.rx_length = 7
};


static uint32_t              m_adc_evt_counter;


/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


void sendAdvertismentDataScheduler(void *data, uint16_t size) {
	GPIOTE::gpio_saadc_read();
	ble.advertisementChangeAdcValue(mAds1220Average); //mAds1220Average advertiseAdcValue
	ble.advertisementChangeAdcMinValue(mAds1220Min);
	ble.advertisementChangeAdcMaxValue(mAds1220Max);
	ble.advertisementChangeBatteryValue(advBatteryVal);


	if(sample_adc == false){
		sample_adc = false;
	}

	/* without delay firmware crashes after gpio_saadc_read();
    nrf_delay_us(100); but not when method advertising_change_data is called */
}

static void sendAdvertismentData(void * p_context)
{

}



static void timer_handler_spi(nrf_timer_event_t event_type, void * p_context)
{
}

static void ppi_spi_xfer(){
	nrf_drv_spi_xfer(&spi_inst,
					&transfer_buffers,
					NRF_DRV_SPI_FLAG_HOLD_XFER |
					NRF_DRV_SPI_FLAG_RX_POSTINC |
					NRF_DRV_SPI_FLAG_TX_POSTINC |
					NRF_DRV_SPI_FLAG_REPEATED_XFER |
					NRF_DRV_SPI_FLAG_NO_XFER_EVT_HANDLER);
}

void ads1220_start_conv_scheduler_handler(void *data, uint16_t size){
	disable_timer_and_ppi();
	adc_Ads1220.Start_Conv();
	GPIOTE::gpio_set(CS_ADS1220, false);
	ppi_spi_xfer();
	start_timer_and_ppi();

}



void ads1220_power_down_scheduler_handler(void *data, uint16_t size){
	NRF_LOG_INFO("Disable timer and ppi");
	disable_timer_and_ppi();
	adc_Ads1220.Power_Down();
	GPIOTE::gpio_set(CS_ADS1220, false);
	ppi_spi_xfer();
	start_timer();
	//start_timer_and_ppi();

}


void mpu9250_wake_up_scheduler_handler(void *data, uint16_t size){
	disable_timer_and_ppi();
	GPIOTE::gpio_set(CS_ADS1220, true);
    imu_MPU9250.wakeup();
	GPIOTE::gpio_set(CS_ADS1220, false);
	ppi_spi_xfer();
	start_timer_and_ppi();

}




void mpu9250_sleep_scheduler_handler(void *data, uint16_t size){
	disable_timer_and_ppi();
	GPIOTE::gpio_set(CS_ADS1220, true);
    imu_MPU9250.sleep();
	GPIOTE::gpio_set(CS_ADS1220, false);
	ppi_spi_xfer();
	start_timer_and_ppi();

}


/**@brief Function for the Event Scheduler initialization.
 * @todo  change advertisement data only when not connected to central/server
 */
static void timer_handler_spi_counter(nrf_timer_event_t event_type, void * p_context){
	//Set RX.PTR to start Address
	//copy rx_buffer_adc
	memcpy(rx_buffer_adc_ble, rx_buffer_adc, 21*sizeof(uint8_t));

	rx_buffer_adc_ble[0] = test_byte;
	test_byte++;
	data_ready = true;

	// reset rxd pointer to rx_buffer_adc[0]
	NRF_SPIM0->RXD.PTR = (int32_t)&rx_buffer_adc;
	NRF_SPIM0->TXD.PTR = (int32_t)&tx_buffer_adc;


	Ads1220::copy_relavant_data_3byte(ble_data_counter, rx_buffer_adc_ble, rx_buffer_adc, ads1220_offset, identifier);

	advertiseAdcValue[0] = rx_buffer_adc_ble[1];
	advertiseAdcValue[1] = rx_buffer_adc_ble[2];
	advertiseAdcValue[2] = rx_buffer_adc_ble[3];

	if(Ads1220::averageReady(rx_buffer_adc_ble, ads1220_offset, mIntervall, mAds1220Average, mAds1220Min, mAds1220Max)){
		app_sched_event_put(NULL, 1, sendAdvertismentDataScheduler);
	}






	if(startNotification){
		//ble_nus_data_send_shim(rx_buffer_adc_ble, &rx_buff_size);
		ble.ble_core_data_send(rx_buffer_adc_ble, &rx_buff_size);
	}

	ble_data_counter++;

}

/**
 * @brief Initalize timer scheduler. If schedular is used timer handler will be executed in main loop
 */
static void scheduler_init(void) {
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


static void timer_interrupt_init(void)
{

	ret_code_t err_code;
	UNUSED(err_code);

	err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_timer_adv_data, APP_TIMER_MODE_SINGLE_SHOT, sendAdvertismentData);
    APP_ERROR_CHECK(err_code);


    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;

    err_code = nrf_drv_timer_init(&m_timer_spi, &timer_cfg, timer_handler_spi);
    APP_ERROR_CHECK(err_code);

	uint32_t ticks = nrf_drv_timer_us_to_ticks(&m_timer_spi, ads_1220_spi_freq);

	// don't use timer handler (false)
    nrf_drv_timer_extended_compare(&m_timer_spi,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   false);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer_spi, NRF_TIMER_CC_CHANNEL0);


    //Timer spi counter for counting spi events
    nrf_drv_timer_config_t timer_cfg_spi_counter = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg_spi_counter.mode = NRF_TIMER_MODE_COUNTER;
    err_code = nrf_drv_timer_init(&m_timer_spi_counter, &timer_cfg_spi_counter, timer_handler_spi_counter);

    //
    nrf_drv_timer_extended_compare(&m_timer_spi_counter, NRF_TIMER_CC_CHANNEL0, 6, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    uint32_t timer_count_task_addr = nrf_drv_timer_task_address_get(&m_timer_spi_counter, NRF_TIMER_TASK_COUNT);


    //set spi ppi

   // const nrf_drv_spi_t spi_instance =  spi_inst; // spi.get_instance();

    ppi_spi_xfer();

    uint32_t spi_task_addr   = nrf_drv_spi_start_task_get(&spi_inst);
    uint32_t spi_end_evt_addr = nrf_drv_spi_end_event_get(&spi_inst);

    UNUSED(spi_task_addr);
    UNUSED(spi_end_evt_addr);

    //configure ppi

    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel_start_spi);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel_start_spi,
                                          timer_compare_event_addr,
                                          spi_task_addr);

    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel_count_spi);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel_count_spi,
    										spi_end_evt_addr,
											  timer_count_task_addr);


}

void start_timer(){
	uint8_t err_code;
    //start timer

    nrf_drv_timer_enable(&m_timer_spi);
    nrf_drv_timer_enable(&m_timer_spi_counter);

    err_code = app_timer_start(m_timer_adv_data, APP_TIMER_TICKS(2000), NULL); // APP_TIMER_TICKS(100), NULL)
    APP_ERROR_CHECK(err_code);
}

void start_ppi(){
	uint8_t err_code;
    // start ppi
    err_code = nrf_drv_ppi_channel_enable(m_ppi_channel_start_spi);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_enable(m_ppi_channel_count_spi);
    APP_ERROR_CHECK(err_code);
}

void start_timer_and_ppi(){
	uint8_t err_code;
    //start timer

    nrf_drv_timer_enable(&m_timer_spi);
    nrf_drv_timer_enable(&m_timer_spi_counter);

    err_code = app_timer_start(m_timer_adv_data, APP_TIMER_TICKS(2000), NULL); // APP_TIMER_TICKS(100), NULL)
    APP_ERROR_CHECK(err_code);

    // start ppi
    err_code = nrf_drv_ppi_channel_enable(m_ppi_channel_start_spi);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_enable(m_ppi_channel_count_spi);
    APP_ERROR_CHECK(err_code);

}


void disable_timer(){
	uint8_t err_code;
    //disable timer

    nrf_drv_timer_disable(&m_timer_spi);
    nrf_drv_timer_disable(&m_timer_spi_counter);

    err_code = app_timer_stop(m_timer_adv_data); // APP_TIMER_TICKS(100), NULL)
    APP_ERROR_CHECK(err_code);
}

void disable_ppi(){
    // disable ppi
	uint8_t err_code;
    err_code = nrf_drv_ppi_channel_disable(m_ppi_channel_start_spi);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_disable(m_ppi_channel_count_spi);
    APP_ERROR_CHECK(err_code);
}

void disable_timer_and_ppi(){
	uint8_t err_code;
    //disable timer

    nrf_drv_timer_disable(&m_timer_spi);
    nrf_drv_timer_disable(&m_timer_spi_counter);

    err_code = app_timer_stop(m_timer_adv_data); // APP_TIMER_TICKS(100), NULL)
    APP_ERROR_CHECK(err_code);

    // disable ppi
    err_code = nrf_drv_ppi_channel_disable(m_ppi_channel_start_spi);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_disable(m_ppi_channel_count_spi);
    APP_ERROR_CHECK(err_code);
}


void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
        APP_ERROR_CHECK(err_code);
        //int i;
        uint16_t adc_val = p_event->data.done.p_buffer[0];
       /*
        adv_data[2] = (uint8_t)(adc_val>>8);
        adv_data[3] = (uint8_t)(adc_val & 0xFF);
       */

        advBatteryVal[0] = (uint8_t)(adc_val>>8);
        advBatteryVal[1] = (uint8_t)(adc_val & 0xFF);

        // NRF_LOG_RAW_INFO("\r\n ADC val %i", (int)adc_val);
        m_adc_evt_counter++;
    }

}


static void gpiote_init(){
	GPIOTE gpio;
	gpio.begin();
	gpio.gpio_out_init(CS_MPU9250, true);
	gpio.gpio_out_init(CS_ADS1220, true);

	gpio.gpio_saadc_init(SAADC_CH_PSELP_PSELP_AnalogInput7, saadc_callback);


	gpio.gpio_out_init(LED_3, true);
	nrf_delay_ms(300);
	GPIOTE::gpio_set(LED_3, false);
	nrf_delay_ms(300);
	GPIOTE::gpio_set(LED_3, true);
	nrf_delay_ms(300);
	GPIOTE::gpio_set(LED_3, false);
}


void sensors_setup(){

    spi.begin(0,SPI_MISO_PIN, SPI_MOSI_PIN, SPI_SCK_PIN,
    					NRF_DRV_SPI_MODE_0, NRF_DRV_SPI_FREQ_8M);

	imu_MPU9250.begin(spi, CS_MPU9250);
    imu_MPU9250.set_accel_range(imu_MPU9250.ACCEL_FS_SEL_8G);
    imu_MPU9250.set_gyro_range(imu_MPU9250.GYRO_FS_SEL_2000DPS);
    imu_MPU9250.sleep();


    adc_Ads1220.begin(spi, CS_ADS1220, DRDY_ADS1220);
    adc_Ads1220.init_config(adc_Ads1220.DR_600SPS,
    		adc_Ads1220.PGA_GAIN_128,
			adc_Ads1220.MUX_AIN1_AIN2,
			adc_Ads1220.PSW_ALWAYS_AUTO);

	adc_Ads1220.Start_Conv();

    //Offset Calibration
	ads1220_offset =  adc_Ads1220.tara(100,1);
	NRF_LOG_INFO("ads1220_offset: %i", ads1220_offset);

	adc_Ads1220.Single_shot_mode_ON();
    //adc_Ads1220.Power_Down();

    GPIOTE::gpio_set(CS_ADS1220, false);
	GPIOTE::gpio_set(CS_MPU9250, true);

}


void writeCalibrationUnit(void *data, uint16_t size){
	NRF_LOG_INFO("In writeAttribute Unit");
	attributeStorage.writeCalibrationUnit(*(reinterpret_cast <uint8_t *>(data)));
	NRF_LOG_INFO("Calibration Unit: %x", attributeStorage.readCalibrationUnit());

	ble.advertisementChangeCalibrationUnit(attributeStorage.readCalibrationUnit());
}

void writeCalibrationValue(void *data, uint16_t size){
	NRF_LOG_INFO("In writeAttribute Value");
	uint8_t calibrationValue[4];
	calibrationValue[0] = ((reinterpret_cast <uint8_t *>(data)))[0];
	calibrationValue[1] = ((reinterpret_cast <uint8_t *>(data)))[1];
	calibrationValue[2] = ((reinterpret_cast <uint8_t *>(data)))[2];
	calibrationValue[3] = ((reinterpret_cast <uint8_t *>(data)))[3];

	NRF_LOG_INFO("Data in HEX: %x %x %x %x ", calibrationValue[0], calibrationValue[1], calibrationValue[2], calibrationValue[3]);
	ble.advertisementChangeCalibrationValue(calibrationValue);
	float floatCalibrationValue = attributeStorage.bytes2Float(calibrationValue);
	attributeStorage.writeCalibrationValue(floatCalibrationValue);
	NRF_LOG_INFO("Calibration Value: " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(attributeStorage.readCalibrationValue()));
}

void readIdentifier(void *data, uint16_t size){
	uint16_t intVal = ((*(reinterpret_cast <uint8_t *>(data)))<<8) | (*(reinterpret_cast <uint8_t *>(data)+1));
	//attributeStorage.writeIdentifier(intVal);
	intVal = attributeStorage.readIdentifier();
	uint8_t intInByte[2];
	intInByte[0] = (uint8_t)(intVal>>8);
	intInByte[1] = (uint8_t)(intVal & 0xff);
	NRF_LOG_INFO("Error Code %x: ", ble.bleCoreDataSetRxVal(intInByte, 2));
}

void resetDevice(void *data, uint16_t size){
	NRF_LOG_INFO("Reset device function");
	NVIC_SystemReset();
}

void writeDevicename(void *data, uint16_t size){

	uint8_t deviceNameSize = (uint8_t)(size-1);
	uint8_t deviceName[10];

	for(int i=1; i<size;i++){
		deviceName[i-1] = ((reinterpret_cast <uint8_t *>(data)))[i];
	}
	NRF_LOG_INFO("write DeviceName size: %i", deviceNameSize);
	attributeStorage.writeDeviceNameSize(deviceNameSize);
	attributeStorage.writeDeviceName(deviceName);
	app_sched_event_put(nullptr, 0, resetDevice);
}


void handleBleSettings(ble_nus_evt_rx_data_t receivedData){
NRF_LOG_INFO("In BLE Settings");

    switch(receivedData.p_data[0]){

    	case SENSOR_ADC_SETUP:
    		NRF_LOG_INFO("SENSOR_ADC_SETUP");
    		if(receivedData.p_data[1]==1){
    			app_sched_event_put(NULL, 1, ads1220_start_conv_scheduler_handler);
    		}else if(receivedData.p_data[1]==0){
    			app_sched_event_put(NULL, 1, ads1220_power_down_scheduler_handler);
    		}
    		break;

    	case SENSOR_MPU9250_SETUP:
    		NRF_LOG_INFO("SENSOR_MPU9250_SETUP");
    		if(receivedData.p_data[1]==1){
    			app_sched_event_put(NULL, 1, mpu9250_wake_up_scheduler_handler);
    		}else if(receivedData.p_data[1]==0){
    			app_sched_event_put(NULL, 1, mpu9250_sleep_scheduler_handler);
    		}
    		break;

    	case ATTRIBUTE_CALIBRATION_UNIT:
    		NRF_LOG_INFO("IN ATTRIBUTE_CALIBRATION_UNIT RECEIVED: %x ", receivedData.p_data[1]);
		    app_sched_event_put(&receivedData.p_data[1], 1, writeCalibrationUnit);
    		break;
    	case ATTRIBUTE_CALIBRATION_VALUE:
    	   app_sched_event_put(&receivedData.p_data[1], 4, writeCalibrationValue);
    	   break;
    	case ATTRIBUTE_IDENTIFIER:
    		app_sched_event_put(&receivedData.p_data[1], 2, readIdentifier);
    	break;
    	case ATTRIBUTE_DEVICENAME:
    		app_sched_event_put(receivedData.p_data, receivedData.length, writeDevicename);
        	break;
    	case ATTRIBUTE_RESET_DEVICE:
    		NRF_LOG_INFO("In reset Device:");
    	    app_sched_event_put(receivedData.p_data, receivedData.length, resetDevice);
    	break;

    }
}
/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 *
 * @todo switching mpu9250 state always wakes up ads1220, Maybe due to enabling all timers in mpu9250_sleep_scheduler_handler
 */
/**@snippet [Handling the data received over BLE] */
static void core_ble_data_handler(ble_nus_evt_t * p_evt)
{
    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
    	NRF_LOG_INFO("RECEIVED_DATA");
        NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);
        //NRF_LOG_RAW_INFO("\r\n Data: %i val: %i",p_evt->params.rx_data.p_data[0], p_evt->params.rx_data.p_data[1]);
        handleBleSettings(p_evt->params.rx_data);

    }else if(p_evt->type == BLE_NUS_EVT_COMM_STARTED){
    	startNotification = true;
    	//NRF_LOG_RAW_INFO("\r\nIs notification enabled %i", p_evt->p_link_ctx->is_notification_enabled);

    }else if(p_evt->type == BLE_NUS_EVT_COMM_STOPPED){
    	//NRF_LOG_RAW_INFO("\r\nIs notification enabled %i",p_evt->p_link_ctx->is_notification_enabled);
    	startNotification = false;
    }
}
/**@snippet [Handling the data received over BLE] */

void checkDeviceName(uint8_t deviceName[10]){
	if(deviceName[0]==0xff){
		// initialize with name core
		deviceName[0] = 0x63;
		deviceName[1] = 0x6F;
		deviceName[2] = 0x72;
		deviceName[3] = 0x65;
		attributeStorage.writeDeviceName(deviceName);
	}
}

void checkSizeOfDeviceName(uint16_t *sizeOfDeviceName){
	if(*sizeOfDeviceName==0xff){
		*sizeOfDeviceName = 4;
		attributeStorage.writeDeviceNameSize(*sizeOfDeviceName);
	}
}

void setAdvertisingAttributes(){


    attributeStorage.readDeviceName(deviceName);
    sizeOfDeviceName = attributeStorage.readDeviceNameSize();

    checkDeviceName(deviceName);
    checkSizeOfDeviceName(&sizeOfDeviceName);


    NRF_LOG_INFO("Size of device Name %i", sizeOfDeviceName);
    ble.ble_init_core(core_ble_data_handler, deviceName, sizeOfDeviceName);

    uint8_t adcValueZeros[3];
    ble.advertisementChangeAdcValue(adcValueZeros);

    start_timer_and_ppi();
    ble.advertisementChangeIdentifier(advertiseIdentifier);

    advCalibrationUnit = attributeStorage.readCalibrationUnit();
    NRF_LOG_INFO("Calibration Unit: %x", advCalibrationUnit);
    ble.advertisementChangeCalibrationUnit(advCalibrationUnit);

    NRF_LOG_INFO("DeviceName size: %x", attributeStorage.readDeviceNameSize());

    uint8_t floatBytes[4];
    NRF_LOG_INFO("Calibration Value: %i", (int)(attributeStorage.readCalibrationValue()));
    attributeStorage.float2Bytes(attributeStorage.readCalibrationValue(), floatBytes);
    ble.advertisementChangeCalibrationValue(floatBytes);
    NRF_LOG_INFO("Bytes %x %x %x %x", floatBytes[0], floatBytes[1], floatBytes[2], floatBytes[3]);
}


int main(void)
{
	UNUSED(flash_calib_value);
	//UNUSED(m_ppi_channel);
    //bool erase_bonds;

    // Initialize.
	norstd.Nordic_log_init();
	attributeStorage.nrf_fstorage_core_init();
    gpiote_init();
    sensors_setup();
    scheduler_init();
    timer_interrupt_init();
    norstd.Nordic_power_management_init();

    setAdvertisingAttributes();


    // Enter main loop.
    for (;;)
    {
    	app_sched_execute();
    	norstd.Nordic_idle_state_handle();
    }
}


/**
 * @}
 */
