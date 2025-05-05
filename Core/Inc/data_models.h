/*
 ******************************************************************************
 * @projectname    : Demo_Project_Parser
 * @file           : data_models.h
 * @author         : Omer Faruk ALMACI
 * @date           : Jan 5, 2024
 * 
 *
 * @brief          : Data Models and Protocol Definitions
 *
 * @description    : Defines data structures, enums, and constants for the
 *                   bootloader communication protocol via USB CDC.
 *                   This file defines the core communication protocol structure
 *                   used between the host (PC) and the STM32 bootloader device.
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
#ifndef INC_DATA_MODELS_H_
#define INC_DATA_MODELS_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h" // Provides HAL types if needed
#include "stdint.h" // Ensures standard integer types are available
/* Typedefs ------------------------------------------------------------------*/



// --- Communication Protocol Constants ---

/** @brief Start byte identifying the beginning of a command message from the host. */
#define HOST_CMD_START_BYTE     (0xA2) // Start of Frame (SOF) for host->device messages

/** @brief Start byte identifying the beginning of a response message from the bootloader. */
#define BOOTLOADER_RESP_START_BYTE (0xA3) // Start of Frame (SOF) for device->host responses

/** @brief End byte identifying the finishing of a response message from the bootloader. */
#define BOOTLOADER_RESP_END_BYTE (0x25) // End of Frame (EOF) for device->host responses



/**
 * @enum Device_Command_Target_e
 * @brief Defines the target component or function for a host command.
 * Used in the 'target' field of the message structure.
 */
typedef enum
{
	TARGET_FLASH_ERASE = 0x01,/**< Command targets the Flash Erase function */
	TARGET_MEM_WRITE   = 0x02,/**< Command targets the Memory Write function */
	TARGET_JUMP_APP    = 0x03,/**< Command targets the Jump to Application function */
	TARGET_CHIP_RESET  = 0x04,/**< Command targets the Chip Reset function */
	TARGET_GET_STATUS  = 0x05,/**< Command targets retrieving device status (Example) */
	// Add other specific targets/commands as needed
	UNIT_ADDRESS_6 = 0x06,/**< UNIT_ADDRESS_6 */
	UNIT_ADDRESS_7 = 0x08,/**< UNIT_ADDRESS_7 */
	TARGET_INVALID     = 0xFF /**< Indicates an invalid or unsupported target */
} Device_Command_Target_e;

/**
 * @enum BL_Message_State_e
 * @brief Represents the current state of incoming message processing by the parser.
 */
typedef enum
{
	WAIT_FOR_MESSAGE,/**< Parser is waiting for a new message (e.g., waiting for HOST_CMD_START_BYTE) */
	MESSAGE_OK       /**< A complete and potentially valid message has been received and parsed */
}BL_Message_State_e;


/**
 * @enum BL_Error_Handler_e
 * @brief Defines error codes related to message parsing and command processing.
 */
typedef enum
{
	BL_OK = 0,              /**< No error occurred */
	BL_ERR_INVALID_START,   /**< Received message did not start with HOST_CMD_START_BYTE */
	BL_ERR_INVALID_END,     /**< Received message did not end with expected format/byte (if applicable) */
	BL_ERR_INVALID_TARGET,  /**< The command target (target) is not recognized or supported */
	BL_ERR_INVALID_ADDRESS, /**< The provided memory/command address is invalid or out of range */
	BL_ERR_INVALID_CMD_TYPE,/**< The command type (read/write/response) is invalid for the context */
	BL_ERR_INVALID_DATA_TYPE,/**< The data type specified is not recognized or supported */
	BL_ERR_INVALID_DATA_SIZE,/**< The size of the data payload is incorrect for the command/data type */
	BL_ERR_INVALID_FORMAT,  /**< General message format error (e.g., field missing, incorrect length) */
	BL_ERR_FLASH_ERASE,     /**< Error during flash erase operation */
	BL_ERR_FLASH_WRITE,     /**< Error during flash write operation */
	BL_ERR_TIMEOUT          /**< Communication timeout occurred */
	// Add other specific error codes as needed
}BL_Error_Handler_e;

/**
 * @enum BL_Device_Comm_Status_e
 * @brief Represents the communication status with the host.
 */
typedef enum
{
	COMM_ONLINE,/**< Communication channel is active (e.g., recent message received) */
	COMM_OFFLINE/**< Communication channel is inactive (e.g., timeout occurred) */
} BL_Device_Comm_Status_e;

/**
 * @enum BL_Command_Type_e
 * @brief Specifies the type or action of a command message.
 */
typedef enum
{
	CMD_TYPE_UNKNOWN = 0,/**< Unknown or default command type */
	CMD_TYPE_READ,       /**< Request to read data from the device */
	CMD_TYPE_WRITE,      /**< Command to write data to the device */
	CMD_TYPE_RESPONSE    /**< Message is a response from the device to the host */
} BL_Command_Type_e;

/**
 * @enum BL_Data_Type_e
 * @brief Specifies the data type of the payload in a message.
 */
typedef enum
{
	DATA_TYPE_UNKNOWN = 0,/**< Unknown or default data type */
	DATA_TYPE_CHAR,       /**< Single character (1 byte) */
	DATA_TYPE_U8,         /**< Unsigned 8-bit integer (1 byte) */
	DATA_TYPE_I16,        /**< Signed 16-bit integer (2 bytes) - Assuming 'SHORT' meant i16 */
	DATA_TYPE_U16,        /**< Unsigned 16-bit integer (2 bytes) */
	DATA_TYPE_I32,        /**< Signed 32-bit integer (4 bytes) */
	DATA_TYPE_U32,        /**< Unsigned 32-bit integer (4 bytes) */
	DATA_TYPE_FLOAT,      /**< Single-precision float (4 bytes) */
	DATA_TYPE_BYTE_ARRAY  /**< Raw byte array (length may vary or be implicit) */
} BL_Data_Type_e;


// --- Helper Unions for Data Type Conversion ---

/**
 * @union models_u16_u
 * @brief Union to easily convert between a uint16_t and a 2-byte array. Useful for serialization/deserialization.
 */
typedef union
{
	uint16_t u16;   /**< Access as a 16-bit unsigned integer */
	uint8_t b[2];   /**< Access as an array of 2 bytes */
} models_u16_u;

/**
 * @union models_u32_u
 * @brief Union to easily convert between a uint32_t and a 4-byte array. Useful for serialization/deserialization.
 */
typedef union
{
	uint32_t u32;   /**< Access as a 32-bit unsigned integer */
	uint8_t b[4];   /**< Access as an array of 4 bytes */
} models_u32_u;

/**
 * @union BL_Data_u
 * @brief Union to hold the data payload of a message. Allows accessing the data
 * according to the specified BL_Data_Type_e without casting.
 * @note Ensure the correct member is accessed based on the message's data_type field.
 * The size of this union is determined by the largest member (typically 4 bytes).
 */
typedef union
{
	char c;         /**< Access data as CHAR */
	uint8_t u8;     /**< Access data as UINT8_T */
	int16_t i16;    /**< Access data as INT16_T */
	uint16_t u16;   /**< Access data as UINT16_T */
	int32_t i32;    /**< Access data as INT32_T */
	uint32_t u32;   /**< Access data as UINT32_T */
	float f;        /**< Access data as FLOAT */

	uint8_t b[4];   /**< Access data as a 4-byte array (useful for raw bytes or types <= 4 bytes) */
} BL_Data_u;


// --- Device State Structures ---


/**
 * @struct BL_Device_Comm_State_t
 * @brief Holds the communication status and related counters.
 */
typedef struct
{
	BL_Device_Comm_Status_e status; /**< Current communication status (COMM_ONLINE or COMM_OFFLINE) */
	uint32_t last_rx_time;          /**< Timestamp (e.g., HAL_GetTick()) of the last message reception, used for timeout */
} BL_Device_Comm_State_t;

/**
 * @struct BL_Device_t
 * @brief Main structure holding the overall state of the bootloader device.
 */
typedef struct
{
	BL_Device_Comm_State_t comm_state;      /**< Device communication status with the host */
	BL_Message_State_e message_state;     /**< Current state of the incoming message parser */
	BL_Error_Handler_e last_error;        /**< Stores the code of the last error that occurred */
	uint32_t error_counter;               /**< Counter for the total number of errors detected */

} BL_Device_t;


// --- Message Structure ---

/**
 * @struct BL_Message_Structure_t
 * @brief Defines the structure of the communication message between host and device.
 * @note This structure is populated by the parser (`parser.c`) from the raw byte stream
 * received via USB CDC and used by the data processor (`data_process.c`).
 * A similar structure (potentially excluding some fields like command_number)
 * is used for sending responses back to the host.
 */
typedef struct
{
	// --- Header Fields ---
	// uint8_t start_byte;          // Implicitly handled by parser start state

	models_u16_u command_number;    /**< Unique sequence number for the command (optional, could be used for tracking) */
	Device_Command_Target_e target; /**< The target function/component for this command (e.g., erase, write, reset) */
	models_u32_u address;         /**< Memory address or parameter associated with the command (e.g., flash address to write/erase) - Replaces command_address */
	BL_Command_Type_e command_type; /**< Type of the command (READ, WRITE, RESPONSE) */
	BL_Data_Type_e data_type;       /**< Type of the data in the payload */
	uint16_t data_length;           /**< Length of the data payload in bytes (can be implicit for fixed-size types) - EKLENDİ */

	// --- Payload ---
	BL_Data_u data;                 /**< Data payload union (access member based on data_type) */

	// --- Footer/Checksum (in the features) ---
	// uint8_t checksum;            // Example: Add checksum for integrity
	// uint8_t end_byte;            // Implicitly handled by parser end state/logic
} BL_Message_Structure_t;


/* Extarnal Typedefs ---------------------------------------------------------*/
// Use extern cautiously. Consider passing pointers to functions instead of relying on globals.
extern BL_Device_t m_device;            /**< Global struct holding the device state */
extern BL_Message_Structure_t m_message; /**< Global struct holding the last parsed message */

#endif /* INC_DATA_MODELS_H_ */

/************************ (C) COPYRIGHT Omer Faruk ALMACI *****END OF FILE****/
