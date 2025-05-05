/*
 ******************************************************************************
 * @projectname    : Demo_Project_Parser
 * @filename       : usb_handler.c
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 * 
 * @brief          : USB Communication Handler Module
 * @description    : This module manages USB communication status monitoring,
 *                   providing functions to track connection state and
 *                   communication timeouts.
 ******************************************************************************
 * @attention
 * 
 * <h2><center>&copy; Copyright (c) 2024 Omer Faruk ALMACI.
 * All rights reserved.</center></h2>
 * 
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <usb_handler.h>
/* Variables -----------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @fn void status_control(void)
 * @brief Controls the device communication status.
 */
void status_control(void)
{
	if(m_device.comm_state.last_rx_time >= 1000)
	{
		m_device.comm_state.status = COMM_OFFLINE;
		m_device.comm_state.last_rx_time = 1000;
	}
	else
	{
		m_device.comm_state.last_rx_time++;
		m_device.comm_state.status = COMM_ONLINE;
	}
}

/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
