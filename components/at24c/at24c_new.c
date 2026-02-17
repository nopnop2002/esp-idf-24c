/*
 * Copyright (c) 2020 nopnop2002
 *
 * Driver for reading and writing data to 24Cxx external I2C EEPROMs.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

#include "at24c.h"

#define TAG "at24c"

#if CONFIG_24C02
#define EEPROM_SIZE 2
#endif

#if CONFIG_24C04
#define EEPROM_SIZE 4
#endif

#if CONFIG_24C08
#define EEPROM_SIZE 8
#endif

#if CONFIG_24C16
#define EEPROM_SIZE 16
#endif

#if CONFIG_24C32
#define EEPROM_SIZE 32
#endif

#if CONFIG_24C64
#define EEPROM_SIZE 64
#endif

#if CONFIG_24C128
#define EEPROM_SIZE 128
#endif

#if CONFIG_24C256
#define EEPROM_SIZE 256
#endif

#if CONFIG_24C512
#define EEPROM_SIZE 512
#endif

i2c_master_dev_handle_t dev_handle;

esp_err_t InitRom(EEPROM_t * dev, i2c_port_t i2c_port)
{
	ESP_LOGI(TAG, "Using new driver");
	ESP_LOGI(TAG, "EEPROM is 24C%.02d",EEPROM_SIZE);
	ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(TAG, "CONFIG_I2C_ADDRESS=0x%x",CONFIG_I2C_ADDRESS);

	dev->_i2c_port = i2c_port;
	dev->_chip_addr = CONFIG_I2C_ADDRESS;
	dev->_kbits = EEPROM_SIZE;
	dev->_address = (128 * EEPROM_SIZE) - 1; // 0-0xFFFF

	i2c_master_bus_config_t i2c_mst_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.i2c_port = i2c_port,
		.scl_io_num = CONFIG_SCL_GPIO,
		.sda_io_num = CONFIG_SDA_GPIO,
		.flags.enable_internal_pullup = true,
	};
	i2c_master_bus_handle_t bus_handle;
	esp_err_t ret;
	ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
	if (ret != ESP_OK) return ret;

	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = CONFIG_I2C_ADDRESS,
		.scl_speed_hz = I2C_FREQUENCY,
	};
	//i2c_master_dev_handle_t dev_handle;
	ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
	return ret;
}


static esp_err_t ReadReg8(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint8_t data_addr, uint8_t * data)
{
	esp_err_t ret = i2c_master_device_change_address(dev_handle, chip_addr, I2C_TICKS_TO_WAIT);
	if (ret != ESP_OK) return ret;

	uint8_t out_buf[1];
	int out_index = 0;
	out_buf[out_index++] = data_addr;
	ret = i2c_master_transmit_receive(dev_handle, out_buf, sizeof(out_buf), data, 1, I2C_TICKS_TO_WAIT);
	return ret;
}

static esp_err_t WriteReg8(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint8_t data_addr, uint8_t data)
{
	esp_err_t ret = i2c_master_device_change_address(dev_handle, chip_addr, I2C_TICKS_TO_WAIT);
	if (ret != ESP_OK) return ret;

	uint8_t out_buf[2];
	int out_index = 0;
	out_buf[out_index++] = data_addr;
	out_buf[out_index++] = data;
	ret = i2c_master_transmit(dev_handle, out_buf, 2, I2C_TICKS_TO_WAIT);
	usleep(1000*2);
	return ret;
}

static esp_err_t ReadReg16(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint16_t data_addr, uint8_t * data)
{
	uint8_t out_buf[2];
	int out_index = 0;
	uint8_t high_addr = (data_addr >> 8) & 0xff;
	uint8_t low_addr = data_addr & 0xff;
	out_buf[out_index++] = high_addr;
	out_buf[out_index++] = low_addr;
	esp_err_t ret = i2c_master_transmit_receive(dev_handle, out_buf, sizeof(out_buf), data, 1, I2C_TICKS_TO_WAIT);
	return ret;
}

static esp_err_t WriteReg16(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint16_t data_addr, uint8_t data)
{
	uint8_t out_buf[3];
	int out_index = 0;
	uint8_t high_addr = (data_addr >> 8) & 0xff;
	uint8_t low_addr = data_addr & 0xff;
	out_buf[out_index++] = high_addr;
	out_buf[out_index++] = low_addr;
	out_buf[out_index++] = data;
	esp_err_t ret = i2c_master_transmit(dev_handle, out_buf, 3, I2C_TICKS_TO_WAIT);
	usleep(1000*2);
	return ret;
}

esp_err_t ReadRom(EEPROM_t * dev, uint16_t data_addr, uint8_t * data)
{
	if (data_addr > dev->_address) return 0;

	if (dev->_kbits < 32) {
		int blockNumber = data_addr / 256;
		uint16_t _data_addr = data_addr - (blockNumber * 256);
		int _chip_addr = dev->_chip_addr + blockNumber;
		ESP_LOGD(TAG, "ReadRom _chip_addr=%x _data_addr=%d", _chip_addr, _data_addr);
		return ReadReg8(dev, dev->_i2c_port, _chip_addr, _data_addr, data);
	} else {
		int _chip_addr = dev->_chip_addr;
		return ReadReg16(dev, dev->_i2c_port, _chip_addr, data_addr, data);
	}
}

esp_err_t WriteRom(EEPROM_t * dev, uint16_t data_addr, uint8_t data)
{
	if (data_addr > dev->_address) return 0;

	if (dev->_kbits < 32) {
		int blockNumber = data_addr / 256;
		uint16_t _data_addr = data_addr - (blockNumber * 256);
		int _chip_addr = dev->_chip_addr + blockNumber;
		ESP_LOGD(TAG, "WriteRom _chip_addr=%x _data_addr=%d", _chip_addr, _data_addr);
		return WriteReg8(dev, dev->_i2c_port, _chip_addr, _data_addr, data);
	} else {
		int _chip_addr = dev->_chip_addr;
		return WriteReg16(dev, dev->_i2c_port, _chip_addr, data_addr, data); 
	}
}

uint16_t MaxAddress(EEPROM_t * dev) {
	return dev->_address;
}

