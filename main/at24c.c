/*
Driver for reading and writing data to 24Cxx external I2C EEPROMs.
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

#include "at24c.h"

#define tag "at24c"

esp_err_t i2c_master_driver_initialize(EEPROM_t * dev, uint16_t bits, i2c_port_t i2c_port, int chip_addr, int i2c_gpio_sda, int i2c_gpio_scl)
{
	dev->_i2c_port = i2c_port;
	dev->_chip_addr = chip_addr;
	dev->_bits = bits;
	dev->_bytes = 128 * bits;

	esp_err_t ret;
	i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_gpio_sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = i2c_gpio_scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQUENCY
	};
	ret = i2c_param_config(i2c_port, &conf);
	ESP_LOGD(tag, "i2c_param_config=%d", ret);
	if (ret != ESP_OK) return ret;
	ret = i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);
	ESP_LOGD(tag, "i2c_driver_install=%d", ret);
	return ret;
}

uint16_t getMaxAddress(EEPROM_t * dev) {
	return dev->_bytes;
}



static esp_err_t ReadReg8(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint8_t data_addr, uint8_t * data)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, chip_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, chip_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, data, NACK_VAL);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

static esp_err_t WriteReg8(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint8_t data_addr, uint8_t data)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, chip_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	usleep(1000*2);
	return ret;
}

static esp_err_t ReadReg16(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint16_t data_addr, uint8_t * data)
{
	uint8_t high_addr = (data_addr >> 8) & 0xff;
	uint8_t low_addr = data_addr & 0xff;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, chip_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, high_addr, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, low_addr, ACK_CHECK_EN);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, chip_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, data, NACK_VAL);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

static esp_err_t WriteReg16(EEPROM_t * dev, i2c_port_t i2c_port, int chip_addr, uint16_t data_addr, uint8_t data)
{
	uint8_t high_addr = (data_addr >> 8) & 0xff;
	uint8_t low_addr = data_addr & 0xff;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, chip_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, high_addr, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, low_addr, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	usleep(1000*2);
	return ret;
}

esp_err_t ReadRom(EEPROM_t * dev, uint16_t data_addr, uint8_t * data)
{
	if (data_addr > dev->_bytes) return 0;

	if (dev->_bits < 32) {
		int blockNumber = data_addr / 256;
		uint16_t _data_addr = data_addr - (blockNumber * 256);
		int _chip_addr = dev->_chip_addr + blockNumber;
		ESP_LOGD(tag, "ReadRom _chip_addr=%x _data_addr=%d", _chip_addr, _data_addr);
		return ReadReg8(dev, dev->_i2c_port, _chip_addr, _data_addr, data);
	} else {
		int _chip_addr = dev->_chip_addr;
		return ReadReg16(dev, dev->_i2c_port, _chip_addr, data_addr, data);
	}
}

esp_err_t WriteRom(EEPROM_t * dev, uint16_t data_addr, uint8_t data)
{
	if (data_addr > dev->_bytes) return 0;

	if (dev->_bits < 32) {
		int blockNumber = data_addr / 256;
		uint16_t _data_addr = data_addr - (blockNumber * 256);
		int _chip_addr = dev->_chip_addr + blockNumber;
		ESP_LOGD(tag, "WriteRom _chip_addr=%x _data_addr=%d", _chip_addr, _data_addr);
		return WriteReg8(dev, dev->_i2c_port, _chip_addr, _data_addr, data);
	} else {
		int _chip_addr = dev->_chip_addr;
		return WriteReg16(dev, dev->_i2c_port, _chip_addr, data_addr, data); 
	}
}
