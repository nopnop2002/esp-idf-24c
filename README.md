# esp-idf-24c
Two-Wire Serial EEPROM Driver for esp-idf.   

# Software requirements   
ESP-IDF V4.4/V5.0.   
ESP-IDF V5 is required when using ESP32-C2.   

# Installation   

```
git clone https://github.com/nopnop2002/esp-idf-24c
cd esp-idf-24c
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3}
idf.py menuconfig
idf.py flash
```


# Configuration   
You have to set this config value with menuconfig.   
- CONFIG_MODEL   
- CONFIG_SCL_GPIO   
- CONFIG_SDA_GPIO   
- CONFIG_I2C_ADDRESS   


![config-1](https://user-images.githubusercontent.com/6020549/79178157-6f166700-7e3f-11ea-9e1a-c8beab040df5.jpg)
![config-2](https://user-images.githubusercontent.com/6020549/79178160-70e02a80-7e3f-11ea-92c2-1c671a090f7b.jpg)
![config-3](https://user-images.githubusercontent.com/6020549/79178166-73428480-7e3f-11ea-8266-ceac32c4e09b.jpg)


# Memory size

|Device|Number of Bits|Address range|i2c address|
|:---|:---|:---|:---|
|24C02|2K|0x00-0xFF|0x50-0x57|
|24C04|4K|0x00-0x1FF|0x50/0x52/0x54/0x56(*1)|
|24C08|8K|0x00-0x3FF|0x50/0x54(*2)|
|24C16|16K|0x00-0x7FF|0x50(*3)|
|24C32|32K|0x00-0xFFF|0x50-0x57|
|24C64|64K|0x00-0x1FFF|0x50-0x57|
|24C128|128K|0x00-0x3FFF|0x50-0x53|
|24C256|256K|0x00-0x7FFF|0x50-0x53|
|24C512|512K|0x00-0xFFFF|0x50-0x53|

(*1) Two consecutive i2c entries are used in the library.   
(*2) Four consecutive i2c entries are used in the library.   
(*3) Eight consecutive i2c entries are used in the library.   


# API
```
// Open device
//   i2c_port:I2C_NUM_0/I2C_NUM_1
esp_err_t InitRom(EEPROM_t * dev, i2c_port_t i2c_port);

// Get EEPROM maximum address
uint16_t MaxAddress(EEPROM_t * dev);

// Read data from EEPROM
//   data_addr:read address
//   data:read data buffer
esp_err_t ReadRom(EEPROM_t * dev, uint16_t data_addr, uint8_t * data);

// Write data to EEPROM
//   data_addr:write address
//   data:write data buffer
esp_err_t WriteRom(EEPROM_t * dev, uint16_t data_addr, uint8_t data);
```


# Wireing  

|24Cxx||ESP32|ESP32-S2/S3|ESP32-C2/C3||
|:-:|:-:|:-:|:-:|:-:|:-:|
|A0|--|GND|GND|GND|(*1)|
|A1|--|GND|GND|GND|(*1)|
|A2|--|GND|GND|GND|(*1)|
|GND|--|GND|GND|GND||
|SDA|--|GPIO21|GPIO11|GPIO5|(*2)|
|SCL|--|GPIO22|GPIO12|GPIO6|(*2)|
|WP|--|GND|GND|GND||
|VCC|--|3.3V|3.3V|3.3V||

(*1) I2C Address selection. See data sheet.   
(*2) You can change any GPIO using menuconfig.   

Typical circuit.   
When A0 A1 A2 is GND, i2c address is 0x50.  
![ESP32-at24](https://user-images.githubusercontent.com/6020549/203323623-46874bef-55e8-4b45-9d64-e1f55d6b7281.jpg)

# Serial Console   

- 24C04   
![24c04](https://user-images.githubusercontent.com/6020549/202889995-937bfd55-f6c7-4c9f-ad44-1d43cd60778a.jpg)

- 24C16   
![24c16](https://user-images.githubusercontent.com/6020549/202889996-2363c924-4f9e-4c81-bdd7-72c59f6d3f02.jpg)

- 24C32   
![24c32](https://user-images.githubusercontent.com/6020549/202890000-16089016-a1ce-48f4-b21c-1604f868329c.jpg)

- 24C64   
![24c64](https://user-images.githubusercontent.com/6020549/202890002-b9f3723d-3e51-4b91-9c28-50cedfc5072f.jpg)

- 24C128   
![24c128](https://user-images.githubusercontent.com/6020549/202890003-40b67add-18ae-424f-84e2-a0c083e33550.jpg)

- 24C256   
![24c256](https://user-images.githubusercontent.com/6020549/202890004-51accadc-c4f0-4d1c-9260-b84bb1449bba.jpg)

- 24C512   
![24c512](https://user-images.githubusercontent.com/6020549/202890005-1c7b9ac6-fcdb-4063-9350-150e9dfa9b8e.jpg)

