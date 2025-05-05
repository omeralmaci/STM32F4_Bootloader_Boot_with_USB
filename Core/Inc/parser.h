/*
 ******************************************************************************
 * @projectname    : Demo_Project_Parser
 * @file           : parser.h
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 * 
 * @brief          : Header file for the bootloader command message parser.
 *
 * @description    : This module is responsible for receiving raw bytes (usually from USB),
 *                   parsing them according to the defined protocol (see data_models.h),
 *                   validating the message (CRC, length), and triggering data processing.
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
#ifndef INC_PARSER_H_
#define INC_PARSER_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>
#include "data_models.h" // Include protocol definitions
/* Function Prototypes -------------------------------------------------------*/
void parse_message(uint8_t* raw_buff,uint8_t len);
void pre_process_message(uint8_t* buff);

#endif /* INC_PARSER_H_ */

/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
