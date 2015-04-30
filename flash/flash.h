
#ifndef __FLASH_H__
#define __FLASH_H__

#define USER_FLASH_START_ADDR    ((uint32_t)0x08000000) 

#define USER_FLASH_END_ADDR      0x08100000

#define USER_FLASH_SZ       (USER_FLASH_END_ADDR - USER_FLASH_START_ADDR)

#define USER_FLASH_MID_ADDR (USER_FLASH_START_ADDR + USER_FLASH_SZ/2)

#define USER_FLASH_SECTOR_SZ    0x20000

#define FLASH_TIMEOUT_VALUE       ((uint32_t)50000)

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

uint32_t    flash_fsz0(void);
uint32_t    flash_fsz1(void);
uint32_t    flash_write(uint32_t addr, uint32_t data);
uint32_t    flash_write_array(uint32_t addr, uint8_t *data, uint16_t sz);
uint32_t    flash_erase1(void);
uint32_t    flash_crc0(uint32_t sz);
uint32_t    flash_crc1(uint32_t sz);
void        flash_copy10(void);
void        flash_erase_img1(void);

#endif /* __FLASH_H__ */

