/*
 ******************************************************************************
 * @filename       : boot.c
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 *
 * @brief          : Bootloader Core Functions Implementation
 * @description    : This file contains the core bootloader functions including
 *                   startup address selection, application jump functionality,
 *                   flash memory operations (write and erase). Provides
 *                   initialization sequence and startup flow control.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2024 Omer Faruk ALMACI.
 * All rights reserved.</center></h2>
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "boot.h"
#include "main.h" // For HAL_Delay, HAL types
/* Variables -----------------------------------------------------------------*/
uint8_t buffer_rx[30];
/* Prototypes ----------------------------------------------------------------*/
void jump_to_user_app(void);
void address_selection(void);
/* Functions -----------------------------------------------------------------*/

/**
 * @fn void address_selection(void)
 * @brief When the program starts, if the defined Button state is closed circuit (i.e., pressed),
 * 			it remains in the bootloader area; otherwise, it jumps to the application area.
 *
 * @pre program is run for the first time or MCU is reset.
 * @post stays in the bootloader area if the button is pressed, otherwise switches to the application area.
 */
void address_selection(void) {
	if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin)) {
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
	} else {
		jump_to_user_app();
	}
}

/**
 * @brief Jumps to the user application code located at APP_START_BASE_ADDRESS.
 * @pre   User application must be correctly flashed starting at APP_START_BASE_ADDRESS.
 * The first word at APP_START_BASE_ADDRESS must be the initial Main Stack Pointer (MSP) value.
 * The second word must be the address of the application's Reset Handler.
 * @post  MCU execution context is transferred to the user application. This function does not return.
 * @param None
 * @retval None
 */
void jump_to_user_app(void) {
	void (*app_reset_handler)(void);

	uint32_t msp_value = *(volatile uint32_t*) APP_START_BASE_ADDRESS;
	__set_MSP(msp_value);
	SCB->VTOR = APP_START_BASE_ADDRESS;

	uint32_t resethandler_address =
			*(volatile uint32_t*) (APP_START_BASE_ADDRESS + 4);
	app_reset_handler = (void*) resethandler_address;
	app_reset_handler();

    // Code should never reach here if the jump is successful.
	while (1) {
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);

		HAL_Delay(100);
	}
}

/**
 * @brief Writes data to Flash memory.
 * @pre   Target flash area must be erased before writing.
 * Flash memory must be unlocked.
 * @post  Data is written to the specified flash address. Flash memory is locked afterwards.
 * @param mem_value: Pointer to the data buffer to be written.
 * @param mem_address: Starting address in Flash memory to write to.
 * @param len: Number of bytes to write.
 * @retval HAL_OK (0) if successful, HAL_ERROR or other HAL status on failure.
 */
uint8_t mem_write(uint8_t *mem_value, uint32_t mem_address, uint32_t len) {
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // LED turns on before the write operation
	uint8_t status = HAL_OK;

	HAL_FLASH_Unlock(); // Unlock the Flash memory

	for (uint32_t i = 0; i < len; i++) {
		 // Note: Consider using larger program types (HalfWord, Word, DoubleWord)
		 //       if possible and if data/address alignment allows, for faster writing.
		 //       Requires checking FLASH_Program_Parallelism in datasheet.
		 //       Current implementation uses FLASH_TYPEPROGRAM_BYTE for simplicity.
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, mem_address + i, mem_value[i]); // The program data sent is written byte by byte to the relevant address
	}

	HAL_FLASH_Lock(); // Lock the Flash memory regardless of write success/failure
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // LED turns off after the write operation

	return status;
}

/**
 * @brief Erases a specified number of flash sectors starting from a given sector number.
 * @pre   Flash memory should be unlocked before calling this function.
 * Interrupts might need to be disabled during critical flash operations.
 * @post  Specified flash sectors are erased. Flash memory is locked afterwards.
 * @param sector_number: The starting sector number to erase (0 to TOTAL_SECTORS - 1).
 * @param number_of_sector: The total number of sectors to erase.
 * @retval HAL_OK (0) if successful, HAL_ERROR or HAL_BUSY/HAL_TIMEOUT otherwise.
 * Returns INVALID_SECTOR (defined in boot.h) if sector parameters are invalid.
 */
uint8_t flash_erase(uint8_t sector_number, uint8_t number_of_sector) {
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); // LED turns on before the erase operation
    FLASH_EraseInitTypeDef EraseInitStruct; // Structure for erase configuration
    uint32_t sectorError = 0;               // Variable to store potential error during erase
    HAL_StatusTypeDef status = HAL_OK;      // Variable to store HAL function return status

    // Validate sector number and count
	if (number_of_sector > TOTAL_SECTORS )
        return INVALID_SECTOR; // Invalid parameters

	if ((sector_number == 0xff) || (sector_number < 8)) {
		if (sector_number == (uint8_t) 0xff) {
			EraseInitStruct.TypeErase = FLASH_TYPEERASE_MASSERASE;
		} else {
			uint8_t remaining_sector = 8 - sector_number;
			if (number_of_sector > remaining_sector) {
				number_of_sector = remaining_sector;
			}
		    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS; // Erase type is sectors
		    EraseInitStruct.Sector        = sector_number;         // Starting sector number
		    EraseInitStruct.NbSectors     = number_of_sector;      // Number of sectors to erase
		}
		EraseInitStruct.Banks = FLASH_BANK_1;

		HAL_FLASH_Unlock(); // Unlock the Flash memory for erase/write operations
	    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3; // Voltage range for STM32F407 (2.7V to 3.6V)
		// Perform the erase operation
		// Note: HAL_FLASHEx_Erase blocks until the operation is complete.
		status = (uint8_t) HAL_FLASHEx_Erase(&EraseInitStruct, &sectorError);
		HAL_FLASH_Lock(); // Lock the Flash memory

		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); // LED turns off after the erase operation

		return status;
	}

    return INVALID_SECTOR; // Invalid parameters
}

/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
