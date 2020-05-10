# esp-idf-at24c
Two-Wire Serial EEPROM Access Library for esp-idf.   

# Configure
You have to set this config value with menuconfig.   
- CONFIG_MODEL   
- CONFIG_SCL_GPIO   
- CONFIG_SDA_GPIO   
- CONFIG_I2C_ADDRESS   

```
git clone https://github.com/nopnop2002/esp-idf-at24c
cd esp-idf-at24c
make menuconfig
make flash
```

![config-1](https://user-images.githubusercontent.com/6020549/79178157-6f166700-7e3f-11ea-9e1a-c8beab040df5.jpg)

![config-2](https://user-images.githubusercontent.com/6020549/79178160-70e02a80-7e3f-11ea-92c2-1c671a090f7b.jpg)

![config-3](https://user-images.githubusercontent.com/6020549/79178166-73428480-7e3f-11ea-8266-ceac32c4e09b.jpg)

---

# Memory size

|Device|Number of Bits|Number of 8-bit Bytes|
|:---|:---|:---|
|24C02|2K|256|

---

# API
```
// Open device
esp_err_t i2c_master_driver_initialize(EEPROM_t * dev, uint16_t model, i2c_port_t i2c_port, int chip_addr, int i2c_gpio_sda, int i2c_gpio_scl);

// Get EEPROM memory size
uint16_t getMaxAddress(EEPROM_t * dev);

// Read data from EEPROM
esp_err_t ReadRom(EEPROM_t * dev, uint16_t data_addr, uint8_t * data);

// Write data to EEPROM
esp_err_t WriteRom(EEPROM_t * dev, uint16_t data_addr, uint8_t data);
```

---

# Wireing  

|AT24Cxx||ESP32|
|:-:|:-:|:-:|
|A0|--|GND(*1)|
|A1|--|GND(*1)|
|A2|--|GND(*1)|
|GND|--|GND|
|SDA|--|GPIO21(*2)|
|SCL|--|GPIO22(*2)|
|WP|--|GND|
|VCC|--|3.3V|

(*1) I2C Address Inputs. See data sheet.   
(*2) PullUp required. You can change any GPIO using menuconfig.   

Typical circuit.   
![ESP32-at24](https://user-images.githubusercontent.com/6020549/79180746-b0117a00-7e45-11ea-8b68-2b19e5a3821f.jpg)

---

# Serial Console   

- 24C04   
![24c04](https://user-images.githubusercontent.com/6020549/79179697-3082ab80-7e43-11ea-9209-9246f335efde.jpg)

- 24C16   
![24c16](https://user-images.githubusercontent.com/6020549/79179706-337d9c00-7e43-11ea-8d0f-4722073b7b56.jpg)

- 24C32   
![24c32](https://user-images.githubusercontent.com/6020549/79179713-37112300-7e43-11ea-93d5-afd1c70b28d3.jpg)

- 24C64   
![24c64](https://user-images.githubusercontent.com/6020549/79179721-3bd5d700-7e43-11ea-8425-df9ec47ae52d.jpg)

- 24C128   
![24c128](https://user-images.githubusercontent.com/6020549/79179731-4001f480-7e43-11ea-9296-c62ea59c204c.jpg)

- 24C256   
![24c256](https://user-images.githubusercontent.com/6020549/79179735-43957b80-7e43-11ea-92cd-a667ad20d3ed.jpg)


