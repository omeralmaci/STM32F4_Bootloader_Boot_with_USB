/*
 ******************************************************************************
 * @projectname    : Demo_Project_Parser
 * @file           : boot.h
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 *
 * @brief          : Header for boot.c file.
 * 					 Contains bootloader related functions, address definitions,
 * 					 and flash sector information for STM32F407VG.
 *
 * @description    : Bootloader core functionalities header. Handles application jump
 * 					 and flash memory operations (erase/write). Defines memory layout.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2024 ÖMER FARUK ALMACI.
 * All rights reserved.</center></h2>
 *
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_BOOT_H_
#define INC_BOOT_H_
/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"
/* External variables --------------------------------------------------------*/
extern uint8_t buffer_rx[30]; // Note: Consider a more descriptive name like usb_rx_buffer if used globally for USB RX
/* External functions --------------------------------------------------------*/
extern void address_selection(void); // Consider adding a @brief comment explaining its purpose if complex
extern void jump_to_user_app(void);
extern uint8_t mem_write(uint8_t *mem_value, uint32_t mem_address, uint32_t len);
extern uint8_t flash_erase(uint8_t sector_number , uint8_t number_of_sector);
/* Macros and Defines --------------------------------------------------------*/

/**
 * @defgroup F4_Flash_Sectors STM32F407VG Flash Sector Definitions
 * @brief STM32F407VG Flash Memory Organization.
 * Defines start addresses and typical sizes for each sector.
 * @{
 */
#define F4_SECTOR_0  (0x08000000)   /**< Sector 0 | SIZE: 16 Kbytes */
#define F4_SECTOR_1  (0x08004000)   /**< Sector 1 | SIZE: 16 Kbytes */
#define F4_SECTOR_2  (0x08008000)   /**< Sector 2 | SIZE: 16 Kbytes */
#define F4_SECTOR_3  (0x0800C000)   /**< Sector 3 | SIZE: 16 Kbytes */
#define F4_SECTOR_4  (0x08010000)   /**< Sector 4 | SIZE: 64 Kbytes */
#define F4_SECTOR_5  (0x08020000)   /**< Sector 5 | SIZE: 128 Kbytes */
#define F4_SECTOR_6  (0x08040000)   /**< Sector 6 | SIZE: 128 Kbytes */
#define F4_SECTOR_7  (0x08060000)   /**< Sector 7 | SIZE: 128 Kbytes */
#define F4_SECTOR_8  (0x08080000)   /**< Sector 8 | SIZE: 128 Kbytes */
#define F4_SECTOR_9  (0x080A0000)   /**< Sector 9 | SIZE: 128 Kbytes */
#define F4_SECTOR_10 (0x08C00000)   /**< Sector 10 | SIZE: 128 Kbytes */
#define F4_SECTOR_11 (0x08E00000)   /**< Sector 11 | SIZE: 128 Kbytes */
/** @} */ // End of F4_Flash_Sectors group

/**
 * @def APP_START_BASE_ADDRESS
 * @brief Base address where the user application code starts in Flash memory.
 * The bootloader will jump to this address to start the application.
 * Ensure this address corresponds to a valid sector start address.
 */
#define APP_START_BASE_ADDRESS (F4_SECTOR_2)    /**< User Application Start Address (Sector 2) */

#define INVALID_SECTOR 0x04                     /**< Error code returned for invalid flash sector operations */

/**
 * @defgroup MCU_Selection Target MCU Definition
 * @brief Defines the target STM32F4 series MCU to configure flash properties.
 * Based on the defined MCU, the total number of flash sectors is set.
 * @{
 */
#define STM32F407VGTx       /**< Define for STM32F407VG with 1Mbyte Flash Memory */
//#define STM32F407VETx     /**< Define for STM32F407VE with 512Kbytes Flash Memory */

#ifdef STM32F407VGTx
	#define TOTAL_SECTORS (11)    /**< Total number of sectors for 1Mbyte Flash (0-11) */
#elif defined(STM32F407VETx)
	#define TOTAL_SECTORS (7)     /**< Total number of sectors for 512Kbyte Flash (0-7) */
#else
	#define TOTAL_SECTORS (7)     /**< Default Total number of sectors */
#endif
/** @} */ // End of MCU_Selection group


#endif /* INC_BOOT_H_ */

/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
