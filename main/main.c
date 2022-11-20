/*
Driver for reading and writing data to 24Cxx external I2C EEPROMs.
*/
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

#include "at24c.h"

#define tag "24cxx"

void dump(char * title, uint8_t *dt, int32_t n)
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
			printf("%05"PRIx32": ",addr);
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
	EEPROM_t dev;
	i2c_port_t i2c_port = I2C_NUM_0;
	InitRom(&dev, i2c_port);
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
