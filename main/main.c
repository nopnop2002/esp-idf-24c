/*
Driver for reading and writing data to 24Cxx external I2C EEPROMs.
*/
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

#include "at24c.h"

#if 0
#define CONFIG_SDA_GPIO		21
#define CONFIG_SCL_GPIO		22
#define CONFIG_I2C_ADDRESS	0x50
#define CONFIG_24C02		1
//#define CONFIG_24C32		1
#endif

#if CONFIG_24C02
#define	EEPROM_SIZE			2
#endif

#if CONFIG_24C04
#define	EEPROM_SIZE			4
#endif

#if CONFIG_24C08
#define	EEPROM_SIZE			8
#endif

#if CONFIG_24C16
#define	EEPROM_SIZE			16
#endif

#if CONFIG_24C32
#define	EEPROM_SIZE			32
#endif

#if CONFIG_24C64
#define	EEPROM_SIZE			64
#endif

#if CONFIG_24C128
#define	EEPROM_SIZE			128
#endif

#if CONFIG_24C256
#define	EEPROM_SIZE			256
#endif

#if CONFIG_24C512
#define	EEPROM_SIZE			512
#endif

#define tag "24cxx"

void dump(char * title, uint8_t *dt, uint32_t n)
{
	uint16_t clm = 0;
	uint8_t data;
	uint8_t sum;
	uint8_t vsum[16];
	uint8_t total =0;
	uint32_t saddr =0;
	uint32_t eaddr =n-1;

	printf("-------------------- [%s] --------------------\n", title);
	uint16_t i;
	for (i=0;i<16;i++) vsum[i]=0;
	uint32_t addr;
	for (addr = saddr; addr <= eaddr; addr++) {
		data = dt[addr];
		if (clm == 0) {
			sum =0;
			printf("%05x: ",addr);
		}

		sum+=data;
		vsum[addr % 16]+=data;

		printf("%02x ",data);
		clm++;
		if (clm == 16) {
			printf("|%02x \n",sum);
			clm = 0;
		}
	}
	printf("-------------------- [%s] --------------------\n", title);
	printf("       ");
	for (i=0; i<16;i++) {
		total+=vsum[i];
		printf("%02x ",vsum[i]);
	}
	printf("|%02x \n\n",total);
}


void app_main()
{
	ESP_LOGI(tag, "EEPROM is 24C%.02d",EEPROM_SIZE);
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_I2C_ADDRESS=0x%x",CONFIG_I2C_ADDRESS);

	EEPROM_t dev;
	i2c_port_t i2c_port = I2C_NUM_0;
	i2c_master_driver_initialize(&dev, EEPROM_SIZE, i2c_port, CONFIG_I2C_ADDRESS, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO);
	uint16_t bytes = MaxAddress(&dev);

	esp_err_t ret;
	uint8_t buffer[256];
	char title[32];
	
	// write first block
	for (int i=0; i<256; i++) {
		uint16_t data_addr = i;
		uint8_t data = i;
		ret = WriteRom(&dev, data_addr, data);
		if (ret != ESP_OK) {
			ESP_LOGI(tag, "WriteRom[%d] fail %d", data_addr, ret);
			while(1) { vTaskDelay(1); }
		}
	}

	// write last block
	int offset = bytes - 256;
	for (int i=0; i<256; i++) {
		uint16_t data_addr = i + offset;
		uint8_t data = 255 - i;
		ret = WriteRom(&dev, data_addr, data);
		if (ret != ESP_OK) {
			ESP_LOGI(tag, "WriteRom[%d] fail %d", data_addr, ret);
			while(1) { vTaskDelay(1); }
		}
	}

#if 0
	// read first block
	for (int i=0; i<256; i++) {
		uint16_t data_addr = i;
		ret = ReadRom(&dev, data_addr, &buffer[i]);
		if (ret != ESP_OK) {
			ESP_LOGI(tag, "ReadRom fail %d", ret);
			while(1) { vTaskDelay(1); }
		}
	}
	sprintf(title, "address 0-255");
	dump(title, buffer, 256);
	vTaskDelay(100);
#endif

	// read last block
	for (int i=0; i<256; i++) {
		uint16_t data_addr = i + offset;
		ret = ReadRom(&dev, data_addr, &buffer[i]);
		if (ret != ESP_OK) {
			ESP_LOGI(tag, "ReadRom fail %d", ret);
			while(1) { vTaskDelay(1); }
		}
	}
	sprintf(title, "address %d-%d", offset, offset+255);
	dump(title, buffer, 256);
	vTaskDelay(100);

	// read first block
	for (int i=0; i<256; i++) {
		uint16_t data_addr = i;
		ret = ReadRom(&dev, data_addr, &buffer[i]);
		if (ret != ESP_OK) {
			ESP_LOGI(tag, "ReadRom fail %d", ret);
			while(1) { vTaskDelay(1); }
		}
	}
	sprintf(title, "address 0-255");
	dump(title, buffer, 256);
	vTaskDelay(100);

}
