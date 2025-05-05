/*
 ******************************************************************************
 * @projectname    : Demo_Project_Parser
 * @file           : data_process.h
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 *
 * @brief          : Data Processing Function Declarations
 *
 * @description    : Contains external function declarations for message processing,
 *                   command execution, and error handling functions.
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
#ifndef INC_DATA_PROCESS_H_
#define INC_DATA_PROCESS_H_
/* External Functions --------------------------------------------------------*/
extern void response_message(void);
extern uint8_t process_data(void);
extern void handle_error(BL_Error_Handler_e err);
extern uint8_t read_process_data(uint32_t cmd_adress);
extern uint8_t write_process_data(uint32_t unit_adress);
extern uint8_t f_value_func(uint8_t cmd_type, uint8_t data_type, BL_Data_u data);


#endif /* INC_DATA_PROCESS_H_ */

/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
