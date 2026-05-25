#ifndef __FLASH_INTERCE_EXT_RTOS_NVS_H__
#define __FLASH_INTERCE_EXT_RTOS_NVS_H__

void flash_init(void);
bool flash_open(void);
void flash_close(void);
bool hasExternalFlash(void);
uint8_t readFlashPg(uint8_t page, uint32_t offset, uint8_t *pBuf, uint16_t len);
uint8_t readFlash(uint_least32_t addr, uint8_t *pBuf, size_t len);
uint8_t writeFlashPg(uint8_t page, uint32_t offset, uint8_t *pBuf, uint16_t len);
uint8_t writeFlash(uint_least32_t addr, uint8_t *pBuf, size_t len);
uint8_t eraseFlashPg(uint8_t page);

#endif
