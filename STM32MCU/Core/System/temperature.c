
#include "temperature.h"
#include "main.h"

// RAM datasheet pg.17
// prefix all defines with 'TS' for 'temp sensor'
#define TS_RAW_DATA_IR_CHANNEL_1 0x04
#define TS_RAW_DATA_IR_CHANNEL_2 0x05
#define TS_AMBIENT_CHANNEL       0x06
#define TS_OBJ1_CHANNEL          0x07
#define TS_OBJ2_CHANNEL          0x08

//EEPROM datasheet page.14
#define EEPROM_ACCESS_BIT 		 5
#define TS_EE_ID1				 0x1C
#define TS_EE_ID2				 0x1D
#define TS_EE_ID3				 0x1E
#define	TS_EE_ID4				 0x1F

#define TS_SLAVE_ADDRESS (0x5a << 1)

static float temperature_c;
static float temperature_f;
static uint32_t part_id;

static bool readPartId(uint8_t channel, uint32_t * part_number);
static bool readTemperature(uint8_t channel, float * degrees_c, float * degrees_f);


void temperatureInit(void) {

}

void temperatureStateMachine(void) {
	readTemperature(TS_OBJ1_CHANNEL, &temperature_c, &temperature_f);
	readPartId(TS_EE_ID1, &part_id);


}

static bool readPartId(uint8_t channel, uint32_t * part_number){
	uint8_t part_id_buffer[4];
	*part_number = 0;

	bool success = (HAL_I2C_Mem_Read(get_i2c1_handle(), TS_SLAVE_ADDRESS, (channel | (1 << EEPROM_ACCESS_BIT)),
			        I2C_MEMADD_SIZE_8BIT, part_id_buffer, sizeof(part_id_buffer), 100) == HAL_OK);

	if (true == success) {
		uint32_t part_id = ((part_id_buffer[3] << 24) | (part_id_buffer[2] << 16) |
							(part_id_buffer[1] << 8) | (part_id_buffer[0]));
		*part_number = part_id;
	}

	return success;
}

static bool readTemperature(uint8_t channel, float * degrees_c, float * degrees_f){
	uint8_t raw_temp_buffer[2];

    bool success = (HAL_I2C_Mem_Read(get_i2c1_handle(), TS_SLAVE_ADDRESS, channel,
			        I2C_MEMADD_SIZE_8BIT, raw_temp_buffer, sizeof(raw_temp_buffer), 100) == HAL_OK);

	if (true == success) {
        uint16_t raw_temp = ((raw_temp_buffer[1] << 8) | raw_temp_buffer[0]);

        // convert raw_temp to real temp here (page 30 of data sheet)
        if (TS_AMBIENT_CHANNEL == channel) {
            *degrees_c = ((float)raw_temp * 0.02f - 273.15f);
            *degrees_f = ((*degrees_c) * 1.8 + 32.0);
        } else if (TS_OBJ1_CHANNEL == channel) {
            *degrees_c = ((float)raw_temp * 0.02f - 273.15f);
            *degrees_f = ((*degrees_c) * 1.8 + 32.0);
        } else if (TS_OBJ2_CHANNEL == channel) {
            *degrees_c = ((float)raw_temp * 0.02f - 273.15f);
            *degrees_f = ((*degrees_c) * 1.8 + 32.0);
        } else {
            success = false;
        }
	}
	// leave these for now just in case we want to try SMBus again
	//HAL_SMBUS_Master_Transmit_IT(&hsmbus1, TEMP_SENSOR_SLAVE_ADDRESS, txData, sizeof(txData), SMBUS_OTHER_AND_LAST_FRAME_NO_PEC);
	//HAL_SMBUS_Master_Receive_IT(&hsmbus1, TEMP_SENSOR_SLAVE_ADDRESS, tempData, 2, SMBUS_OTHER_AND_LAST_FRAME_NO_PEC);

	return success;
}
