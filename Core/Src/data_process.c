/*
 ******************************************************************************
 * @projectname    : Demo_Project_Parser_Slave
 * @filename       : data_process.c
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 * 
 * @brief          : Message Processing and Command Execution Module
 * @description    : This module handles the processing of received messages,
 *                   executes commands based on message content, and generates
 *                   appropriate responses. It implements the command execution
 *                   layer for the bootloader communication protocol.
 ******************************************************************************
 * @attention
 * 
 * <h2><center>&copy; Copyright (c) 2024 Omer Faruk ALMACI.
 * All rights reserved.</center></h2>
 * 
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "usb_handler.h" // For send_message
#include "data_process.h"
/* External Functions --------------------------------------------------------*/
extern uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);
/* Variables -----------------------------------------------------------------*/
uint8_t buff_tx[15];
/* Prototypes ----------------------------------------------------------------*/
void response_message(void);
uint8_t process_data(void);
void handle_error(BL_Error_Handler_e err);
uint8_t read_process_data(uint32_t cmd_adress);
uint8_t write_process_data(uint32_t unit_adress);
/* Functions -----------------------------------------------------------------*/

/**
 * @fn void response_message(void)
 * @brief If the command sent by the master is applied correctly and without errors,
 *        it sends back the message itself as a response.
 */
void response_message(void) {

    buff_tx[0] = BOOTLOADER_RESP_START_BYTE;

    buff_tx[1] = m_message.command_number.b[0];
    buff_tx[2] = m_message.command_number.b[1];
    buff_tx[3] = m_message.target;
    buff_tx[4] = m_message.address.b[0];
    buff_tx[5] = m_message.address.b[1];
    buff_tx[6] = m_message.address.b[2];
    buff_tx[7] = m_message.address.b[3];
    m_message.command_type = CMD_TYPE_RESPONSE;
    buff_tx[8] = (uint8_t) m_message.command_type;
    buff_tx[9] = (uint8_t) m_message.data_type;
    buff_tx[10] = m_message.data.b[0];
    buff_tx[11] = m_message.data.b[1];
    buff_tx[12] = m_message.data.b[2];
    buff_tx[13] = m_message.data.b[3];

    buff_tx[14] = BOOTLOADER_RESP_END_BYTE;

    CDC_Transmit_FS(buff_tx, 15);

    m_message.command_number.u16 = 0;
    m_message.target = 0;
    m_message.address.u32 = 0;
    m_message.command_type = CMD_TYPE_UNKNOWN;
    m_message.data_type = DATA_TYPE_UNKNOWN;
    m_message.data.u32 = 0;
}

/**
 * @fn void handle_error(BL_Error_Handler_e)
 * @brief Transmits errors that occur during parsing or processing of the command sent by the master.
 *
 * @param err -> contains the relevant error number.
 */
void handle_error(BL_Error_Handler_e err) {
    buff_tx[0] = BOOTLOADER_RESP_START_BYTE;

    buff_tx[1] = m_message.command_number.b[0];
    buff_tx[2] = m_message.command_number.b[1];
    buff_tx[3] = m_message.target;
    m_message.address.u32 = TARGET_INVALID;
    buff_tx[4] = m_message.address.b[0];
    buff_tx[5] = m_message.address.b[1];
    buff_tx[6] = m_message.address.b[2];
    buff_tx[7] = m_message.address.b[3];
    m_message.command_type = CMD_TYPE_RESPONSE;
    buff_tx[8] = (uint8_t) m_message.command_type;
    m_message.data_type = DATA_TYPE_U8;
    buff_tx[9] = (uint8_t) m_message.data_type;
    buff_tx[10] = err;
    buff_tx[11] = 0;
    buff_tx[12] = 0;
    buff_tx[13] = 0;

    buff_tx[14] = BOOTLOADER_RESP_END_BYTE;

    CDC_Transmit_FS(buff_tx, 15);

    m_message.command_number.u16 = 0;
    m_message.target = 0;
    m_message.address.u32 = 0;
    m_message.command_type = CMD_TYPE_UNKNOWN;
    m_message.data_type = DATA_TYPE_UNKNOWN;
    m_message.data.u32 = 0;

    m_device.error_counter += 1; // Stores error count in memory.
}

/**
 * @brief Processes the received and parsed message (m_message).
 * @pre   m_device.message_state == MESSAGE_OK.
 * m_message contains the valid data parsed by parser.c.
 * @post  Executes the command specified in m_message (erase, write, reset, etc.).
 * Sends a response back to the host via USB.
 * Updates m_device.last_error if an error occurs during processing.
 * @param None (uses global m_message and m_device)
 * @retval None
 */
uint8_t process_data(void) {
    switch (m_message.command_type) {
        case CMD_TYPE_READ:
            return read_process_data(m_message.target);
            break;
        case CMD_TYPE_WRITE:
            return write_process_data(m_message.target);
            break;
        default:
            return BL_ERR_INVALID_CMD_TYPE;
            break;
    }
}

/**
 * @fn uint8_t read_process_data(uint32_t)
 * @brief Handles the processing of the (read) command sent by the master.
 * @pre Waits for the (read) command
 * @post After the (read) command arrives, calls the functions of the relevant units.
 * @param unit_address -> contains the address of the relevant unit.
 * @return BL_Error_Handler_e
 */
uint8_t read_process_data(uint32_t unit_adress) {
    switch (unit_adress) {
        case TARGET_FLASH_ERASE :
            return BL_OK;
        case TARGET_MEM_WRITE:
            return BL_OK;
        case TARGET_CHIP_RESET:
            return BL_OK;
        case TARGET_JUMP_APP:
            return BL_OK;
        case TARGET_GET_STATUS:
            return BL_OK;
        case UNIT_ADDRESS_6:
            return BL_OK;
        case UNIT_ADDRESS_7:
            return BL_OK;
        default:
            return BL_ERR_INVALID_TARGET;
    }
    return BL_ERR_INVALID_TARGET;
}

/**
 * @fn uint8_t write_process_data(uint32_t)
 * @brief Handles the processing of the (write) command sent by the master.
 *
 * @pre Waits for the (write) command
 * @post After the (write) command arrives, calls the functions of the relevant units.
 * @param unit_address -> contains the address of the relevant unit.
 * @return BL_Error_Handler_e
 */
uint8_t write_process_data(uint32_t unit_adress) {

    switch (unit_adress) {
        case TARGET_FLASH_ERASE :
            return flash_erase(m_message.address.b[0], m_message.data.b[0]);
        case TARGET_MEM_WRITE:
            return mem_write(m_message.data.b, m_message.address.u32, 4);
        case TARGET_JUMP_APP:
            response_message();
            HAL_Delay(100);
            jump_to_user_app();
            return BL_OK; // code should not come here
        case TARGET_CHIP_RESET:
            return BL_OK;
        case TARGET_GET_STATUS:
            return BL_OK;
        case UNIT_ADDRESS_6:
            return BL_OK;
        case UNIT_ADDRESS_7:
            return BL_OK;
        default:
            return BL_ERR_INVALID_TARGET;
    }
    return BL_ERR_INVALID_TARGET;
}

/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
