/*
 * bl_cm0.h
 *
 *  Created on: Dec 4, 2025
 *      Author: Mert
 */

#ifndef INC_BL_CM0_H_
#define INC_BL_CM0_H_

#include "cm_boot.h"
#include "crc.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "main.h"

/*
 * 	##########   DATA FRAME OF SENDING COMMAND TO BOOTLOADER   ##########
 *  _______ ___________ _________ ______ ______
 * |       |           |         |      |      |
 * | Start | Indicator |         |      | Stop |
 * | byte  |    Byte   | Command | CRC8 | byte |
   |       |           |         |      |      |
 * |_______|___________|_________|______|______|
 *
 * 	After sending command, target device should send ACK/NACK according to CRC16 comparison to the host device.
 *	If the command doesn't take any payload (eg. BL_CMD_GET_CHIP_ID or BL_CMD_JMP_TO_APP) then '-' should be
 *	written to payload section.
 *
 * Indicator byte is an indicator for data frame. It indicates that frame which sent is firmware or command frame.
 * According to that frame code executes command or updates firmware.
 *
 *	If you want to use writing command you have to send two bytes of payload. First byte for the data, second
 *	byte for the address.
 *
 */


/*
 * 	##########   DATA FRAME OF SENDING FIRMWARE TO BOOTLOADER FOR WRITING IT TO THE FLASH AREA   ##########
 *  _______ ___________ ________ _________ _________ _________ _________ _________ _______
 * |       |           |        |         |         |         |         |         |       |
 * | Start | Indicator | Number | 		  |         |         |         |         | Stop  |
 * | byte  |   Byte    |   of   | Chunk-1 | Chunk-2 |   ...   | CRC16_H | CRC16_L | byte  |
   |       |           | Chunks |         |         |         |         |         |       |
 * |_______|___________|________|_________|_________|_________|_________|_________|_______|
 *
 * There should be 259 bytes array to store data. The reason to be 259-bytes is last chunk.
 * At the last tranmission operation CRC16_H, CRC16_L and Stop byte will be sent with 256-bytes chunk.
 * That's why the size of buffer should be 259 bytes. So at the beginning there will be 258-bytes in the
 * 259-bytes of array and at the end 259-bytes of array will be completely full. But 259-bytes of array
 * will not be completely full while sending the chunks between. There will be just 256-bytes of data in
 * the array. That's why array's first and last bytes should be controlled.
 */

extern UART_HandleTypeDef huart1;
#define _USART1		&huart1

#define FLASH_SECTOR5_ADDR			0x08006000U
#define APP_PAGE_COUNT				20
#define SRAM_BASE_u      			0x20000000
#define VECT_TABLE_SIZE 			0xC0  						// 48 vectors * 4 bytes

extern uint8_t command;

/********** Bootloader command or data indication flags **********/
#define BL_CMD			0xCC
#define BL_DATA			0xDD


/********** Correction Flags for Every Part of Firmware Which Came From Host **********/
#define BL_CRCTN_TRUE	0xAA	// The part of firmware is not corrupted
#define BL_CRCTN_FALSE	0xFF	// The part of firmware is corrupted

/********** Bootloader Command Definitions **********/
#define BL_CMD_GET_CHIP_ID	0x00	// Command for reading chip ID
#define BL_CMD_ERASE_MEM	0x01	// Command for erasing 1 byte from specified memory address
#define BL_CMD_GET_CRC16	0x02	// Command for getting CRC16 value of sent firmware
#define BL_CMD_JMP_TO_APP	0x03	// Command for jumping to application code

// Function for printing messages to terminal via USART
void bl_usart_printf(char *format,...);

// Function for jumping bootloader to application code
void bl_jump_to_app(void);

// Function for write firmware to flash area
void bl_write_mem_256_bytes(uint8_t* data, uint32_t addr);

// Function for erase 20kB application code area in flash
void bl_erase_20kB_mem(void);

// Function for read firmware from flash area
void bl_read_mem_256_bytes(uint8_t* data, uint32_t addr);

// Function for executing commands
void bl_execute_cmd(uint8_t cmd);

// Function for parsing data frame
void bl_data_frame_parser(uint8_t* data_frame);

#endif /* INC_BL_CM0_H_ */
