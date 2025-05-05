/*
 ******************************************************************************
 * @projectname    : Demo_Project_Parser
 * @filename       : parser.c
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 * 
 * @brief          : Message Parser Module
 * @description    : This module handles parsing and validation of incoming
 *                   messages from the host. It extracts message fields,
 *                   validates format, and prepares data for processing.
 ******************************************************************************
 * @attention
 * 
 * <h2><center>&copy; Copyright (c) 2024 Omer Faruk ALMACI.
 * All rights reserved.</center></h2>
 * 
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "parser.h"
/* Typedefs ------------------------------------------------------------------*/
BL_Device_t m_device;
BL_Message_Structure_t m_message;
/* Defines and Macros --------------------------------------------------------*/
#define MESSAGE_LENGTH (15)
#define NUM_VALID_COMMANDS_ADDRESS (7)
/* Functions -----------------------------------------------------------------*/

/**
 * @fn void parse_message(uint8_t*, uint8_t)
 * @brief Parses the message sent by the master and confirms its correctness
 *
 * @pre raw message is expected.
 * @post message structure is checked
 * @param raw_buff -> unprocessed message
 * @param len -> size of the unprocessed message
 */
void parse_message(uint8_t *raw_buff, uint8_t len)
{
	if(m_device.message_state == WAIT_FOR_MESSAGE)  // Waiting for message.
	{
		m_device.comm_state.last_rx_time = 0;                // Resets the device's communication control counter when any data communication occurs.
		if(raw_buff[0] != BOOTLOADER_RESP_START_BYTE && raw_buff[len - 1] == BOOTLOADER_RESP_END_BYTE)    // Does the message start with '£'? (Note: Check actual start byte)
		{
			m_device.last_error = BL_ERR_INVALID_START;
		}
		else if(raw_buff[0] == BOOTLOADER_RESP_START_BYTE && raw_buff[len - 1] != BOOTLOADER_RESP_END_BYTE)   // Does the message end with '%'? (Note: Check actual end byte)
		{
			m_device.last_error = BL_ERR_INVALID_END;
		}

		else if(len != MESSAGE_LENGTH) // Is the message size correct? // 15 Bytes
		{
			m_device.last_error = BL_ERR_INVALID_FORMAT;
		}
		else if(raw_buff[0] == BOOTLOADER_RESP_START_BYTE && raw_buff[len - 1] == BOOTLOADER_RESP_END_BYTE && (len == MESSAGE_LENGTH)) // If the start, end, and size of the message are correct.
		{
			pre_process_message(raw_buff);
			m_device.last_error = BL_OK; // No error, preprocessing function was called.
		}
		else
		{
			m_device.last_error = BL_ERR_INVALID_FORMAT; // If no condition is met, returns an unknown format error
		}
		m_device.message_state = MESSAGE_OK; // Message received, ready for processing in the main loop.
	}
}

/**
 * @fn void pre_process_message(uint8_t*)
 * @brief If the structure of the message sent by the master is correct, preprocessing is done on the relevant struct structures
 *
 * @pre Waits for the confirmation of messages sent by the Master.
 * @post The message whose structural correctness is confirmed is equated to the relevant struct structures
 * @param buff -> message whose structural correctness is confirmed
 */
void pre_process_message(uint8_t *buff)
{
	m_message.command_number.b[0] = buff[1];
	m_message.command_number.b[1] = buff[2];

	m_message.target = buff[3];

	m_message.address.b[0] = buff[4];
	m_message.address.b[1] = buff[5];
	m_message.address.b[2] = buff[6];
	m_message.address.b[3] = buff[7];

	m_message.command_type = buff[8]; // 0x00 -> Unknown | 0x01 -> Read | 0x02 -> Write

	m_message.data_type = buff[9];  // 0-> Unknown | 1 -> char | 2 -> u8 | 3 -> short | 4 -> u16 | 5 -> long | 6 -> u32 | 7 -> float

	m_message.data.b[0] = buff[10];
	m_message.data.b[1] = buff[11];
	m_message.data.b[2] = buff[12];
	m_message.data.b[3] = buff[13];

}


/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
