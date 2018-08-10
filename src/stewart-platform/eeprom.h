#ifndef FLASH_H
#define FLASH_H

uint32_t EEPROM_crc(int addr);

void EEPROM_updateVersion();
void EEPROM_updateCycles();

void saveConfig(StewartPlatform &platform);
void loadConfig(StewartPlatform &platform);

#endif
