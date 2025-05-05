# STM32F407 USB CDC Bootloader

This project implements a USB CDC (Communication Device Class) bootloader for the STM32F407VG microcontroller. It enables firmware updates over USB by emulating a Virtual COM Port.

## Features

* **Firmware Update via USB**: Allows flashing new application firmware through a USB connection.
* **USB CDC Protocol**: Communicates using the standard CDC protocol, appearing as a Virtual COM Port to the host computer.
* **Basic Application Validation**: Performs a simple check by reading the Stack Pointer (SP) and Reset Handler address from the application's vector table before jumping to the main application.
* **Command-Based Protocol**: Implements a flexible protocol (defined in `Core/Inc/data_models.h`) for bootloader operations.
* **Multi-LED Status Indication**: Provides visual feedback for various operations (USB status, errors, activity).
* **Sector-Based Flash Management**: Manages flash memory efficiently (Sector erase/write).
* **Bootloader Mode Entry**: Can enter bootloader mode by pressing a button during reset, or automatically if no valid application is found.
* **Error Reporting**: Includes error handling and reports error codes back to the host (see `BL_Error_Handler_e` in `Core/Inc/data_models.h`).

## Hardware Requirements

* STM32F407VG microcontroller (e.g., on an STM32F4 Discovery board).
* USB Full-Speed connection to a host computer.
* 4 LEDs connected to GPIOD pins (LED1-LED4, defined in `Core/Inc/main.h`).
* A push button connected to `BUTTON_Pin` (PA0, defined in `Core/Inc/main.h`) for manual bootloader entry.
* External 8MHz crystal (HSE) for reliable USB operation.

## Memory Layout

The flash memory is partitioned as follows:

| Region            | Start Address | Size                    | Flash Sectors       | Description                                    |
| :---------------- | :------------ | :---------------------- | :------------------ | :--------------------------------------------- |
| Bootloader        | `0x08000000`  | 32 KBytes               | 0, 1                | The code for this bootloader application.      |
| User Application  | `0x08008000`  | Remaining Flash (~992 KB) | 2, 3, 4, ..., 11    | Area where the main application code resides. |

**Note:** The bootloader attempts to jump to the main application located at `0x08008000` (`APP_START_BASE_ADDRESS`).

## Communication Protocol

The bootloader communicates over the USB Virtual COM Port using a custom protocol defined in `Core/Inc/data_models.h`.

### Message Structure

| Byte(s) | Field             | Description                               | Values               |
|---------|-------------------|-------------------------------------------|---------------------|
| 0       | Start Byte        | Command/Response identifier               | Host: 0xA2, Bootloader: 0xA3 |
| 1-2     | Command Number    | Sequence number                           | uint16_t            |
| 3       | Target            | Target component                          | See Device_Command_Target_e |
| 4-7     | Address           | Memory address                            | uint32_t            |
| 8       | Command Type      | Operation type                            | See BL_Command_Type_e |
| 9       | Data Type         | Data format                               | See BL_Data_Type_e  |
| 10-13   | Data              | Command data                              | 4 bytes             |
| 14      | End Byte          | Message terminator                        | 0x25                |

**Note:** Total message length is 15 bytes (`MESSAGE_LENGTH` in `parser.c`).

### Supported Commands (Device_Command_Target_e)

| Target ID | Name               | Description                               | Command Type |
|-----------|-------------------|-------------------------------------------|--------------|
| 0x01      | TARGET_FLASH_ERASE| Erases specified flash sectors            | WRITE        |
| 0x02      | TARGET_MEM_WRITE  | Writes data to specified flash address    | WRITE        |
| 0x03      | TARGET_JUMP_APP   | Jumps to main application                 | WRITE        |
| 0x04      | TARGET_CHIP_RESET | Performs software reset                   | WRITE        |
| 0x05      | TARGET_GET_STATUS | Gets bootloader status                    | READ         |

**Note:** Currently, READ commands return `BL_OK` without actual implementation.

### Data Types (BL_Data_Type_e)

| Type ID | Name            | Size     | Description                   |
|---------|-----------------|----------|-------------------------------|
| 0x00    | DATA_TYPE_UNKNOWN| -        | Default type                  |
| 0x01    | DATA_TYPE_CHAR  | 1 byte   | Character                     |
| 0x02    | DATA_TYPE_U8    | 1 byte   | Unsigned 8-bit                |
| 0x03    | DATA_TYPE_I16   | 2 bytes  | Signed 16-bit                 |
| 0x04    | DATA_TYPE_U16   | 2 bytes  | Unsigned 16-bit               |
| 0x05    | DATA_TYPE_I32   | 4 bytes  | Signed 32-bit                 |
| 0x06    | DATA_TYPE_U32   | 4 bytes  | Unsigned 32-bit               |
| 0x07    | DATA_TYPE_FLOAT | 4 bytes  | Floating point                |

### Error Codes (BL_Error_Handler_e)

| Error Code | Name                      | Description                           |
|------------|---------------------------|---------------------------------------|
| 0x00       | BL_OK                     | No error                              |
| 0x01       | BL_ERR_INVALID_START      | Invalid start byte                    |
| 0x02       | BL_ERR_INVALID_END        | Invalid end byte                      |
| 0x03       | BL_ERR_INVALID_TARGET     | Invalid target                        |
| 0x04       | BL_ERR_INVALID_ADDRESS    | Invalid address                       |
| 0x05       | BL_ERR_INVALID_CMD_TYPE   | Invalid command type                  |
| 0x06       | BL_ERR_INVALID_DATA_TYPE  | Invalid data type                     |
| 0x07       | BL_ERR_INVALID_DATA_SIZE  | Invalid data size                     |
| 0x08       | BL_ERR_INVALID_FORMAT     | Invalid format                        |
| 0x09       | BL_ERR_FLASH_ERASE        | Flash erase error                     |
| 0x0A       | BL_ERR_FLASH_WRITE        | Flash write error                     |
| 0x0B       | BL_ERR_TIMEOUT            | Communication timeout                 |

### Example Command Sequence

For firmware update, the typical command sequence is:

1. **Erase Flash Sectors**:
   ```
   Host: [0xA2][0x00][0x01][0x01][0x05][0x00][0x00][0x00][0x02][0x02][0x02][0x00][0x00][0x00][0x25]
   (Erase 2 sectors starting from sector 5)
   ```

2. **Write Data**:
   ```
   Host: [0xA2][0x00][0x02][0x02][0x08][0x00][0x08][0x00][0x02][0x06][Data 4 bytes][0x25]
   (Write 4 bytes to address 0x08080000)
   ```

3. **Jump to Application**:
   ```
   Host: [0xA2][0x00][0x03][0x03][0x00][0x00][0x00][0x00][0x02][0x00][0x00][0x00][0x00][0x00][0x25]
   ```

## LED Status Indicators

| LED   | Function                                         | Behavior                              |
|-------|--------------------------------------------------|--------------------------------------|
| LED1  | Bootloader heartbeat                             | Toggles every 500ms                  |
| LED2  | Flash operation indicator                        | ON during erase/write operations     |
| LED3  | Button press indicator                           | ON when button is pressed            |
| LED4  | Communication status                             | Toggles every 50ms when offline, OFF when online                |

## Getting Started

1.  **Clone/Download the Project.**
2.  **Open in IDE:** Open the project in a compatible IDE like STM32CubeIDE.
3.  **Build:** Compile the project (`Project -> Build All`).
4.  **Flash Bootloader:** Program the compiled bootloader binary (`.hex` or `.bin` from the `Debug` folder) to the beginning of the STM32F407VG's flash memory (`0x08000000`) using a programmer like ST-LINK.
5.  **Connect USB:** Connect the microcontroller's USB port to the host computer. The device should enumerate as a Virtual COM Port.
6.  **Enter Bootloader Mode:** Use one of the following methods:
    * Press and hold the button connected to `BUTTON_Pin` while resetting the microcontroller.
    * If the main application area is empty or invalid (and the `address_selection` function checks for this), the bootloader might start automatically.
7.  **Use Host Tool:** Utilize a host application (a serial terminal or a custom tool) capable of communicating over the Virtual COM Port. Send commands according to the protocol to perform firmware updates (Erase, Write, Jump).

## Bootloader Entry Logic

Upon device reset, the `address_selection()` function in `Core/Src/boot.c` is executed. If `BUTTON_Pin` (PA0) is detected as HIGH (button pressed), the device remains in bootloader mode. Otherwise, it attempts to jump to the main application at `APP_START_BASE_ADDRESS` (`0x08008000`).

## Building

This project is configured for building with STM32CubeIDE and the GNU Arm Embedded Toolchain. Refer to the `Debug/makefile` for build details.

## Directory Structure (Key Files)

```
bootloader_boot/
├── Core/
│   ├── Inc/              # Header files
│   │   ├── boot.h
│   │   ├── data_models.h # Protocol and data structures
│   │   ├── data_process.h
│   │   ├── parser.h
│   │   ├── usb_handler.h
│   │   └── main.h        # Pin definitions, etc.
│   └── Src/              # Source code files
│       ├── boot.c        # Jump logic, flash operations
│       ├── data_process.c# Command processing
│       ├── parser.c      # Message parsing
│       ├── usb_handler.c # USB communication functions
│       └── main.c        # Main program, initializations, loop
├── Drivers/              # HAL and CMSIS drivers
│   ├── CMSIS/
│   └── STM32F4xx_HAL_Driver/
├── Middlewares/          # ST USB Library
│   └── ST/
│       └── STM32_USB_Device_Library/
├── USB_DEVICE/           # USB Device configuration
│   ├── App/
│   └── Target/
└── README.md             # This file
```

## Dependencies

* STM32F4xx HAL Library
* STM32 USB Device Library  
* CMSIS Core
* GNU Arm Embedded Toolchain

*(Check project settings for exact versions used)*

## License

* Bootloader Core Code: Copyright (c) 2024 Omer Faruk ALMACI (License unspecified unless provided elsewhere)
* STM32 HAL/USB Libraries: STMicroelectronics License (Refer to specific files)
* CMSIS: Typically Apache License 2.0 (Refer to specific files)

Consult the `LICENSE.txt` files within the respective directories for full licensing details.

## Troubleshooting

### Common Issues

1.  **Device not enumerated:**
    * Check the USB cable connection.
    * Verify the HSE crystal and clock configuration (`main.c` -> `SystemClock_Config`) are correct.
    * Ensure the USB clock is configured to 48MHz.
2.  **Cannot enter bootloader mode:**
    * Check the connection and functionality of the `BUTTON_Pin` hardware.
    * Ensure the button is pressed correctly *during* reset.
    * Verify that the main application (if present) is not interfering with bootloader entry.
3.  **Flash operation errors:**
    * Ensure correct flash sectors are specified for erase operations.
    * Verify data alignment for write operations.
    * Check that the flash is properly unlocked before operations.
4.  **Communication timeout:**
    * Verify baud rate settings (not applicable to USB CDC).
    * Check if data is being transmitted in the correct format.
    * Ensure proper message start/end bytes are used.

## Known Limitations

* READ commands are not fully implemented (returns `BL_OK` without functionality).
* No built-in application integrity checking (e.g., CRC verification).
* Flash write operations are performed byte-by-byte (could be optimized for faster programming).
* Communication timeout is fixed at 1000ms.

## Future Improvements

* Add application checksum verification.
* Implement actual READ command functionality.
* Support for larger data payloads.
* Add secure boot features.
* Optimize flash programming speed.


## Acknowledgements

Acknowledgements to **Kiran Nayak (@niekiran)**. The concepts and solutions presented in their Udemy course were instrumental in resolving specific challenges faced during the development of this project.