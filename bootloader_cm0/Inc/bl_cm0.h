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
 *  _______ ___________ ________ _________ _________ _________ _______
 * |       |           |        |         |         |         |       |
 * | Start | Indicator | Number | 		  |         |         | Stop  |
 * | byte  |   Byte    |   of   | Chunk-x | CRC16_H | CRC16_L | byte  |
   |       |           | Chunks |(256Byte)|         |         |       |
 * |_______|___________|________|_________|_________|_________|_______|
 *
 * Data transmission will 262-bytes for the data frame which is above. Firmware will be send chunk by chunk
 * as number of chunks as with the data frame like above.
 * When the count of frame which is received from host device is equal to number of chunks then micro controller
 * realizes that the firmware transmission is ended and waits for the final CRC16 data frame tranmission to
 * compare the total CRC16 value which is received from host device with calculated one to verify the firmware.
 */

/*
 * 	##########   DATA FRAME OF TOTAL CRC16 VALUE  ##########
 *  _______ ___________ _________ _________ ______
 * |       |           |         |         |      |
 * | Start | Indicator |  Total	 |  Total  | Stop |
 * | byte  |   Byte    | CRC16_H | CRC16_L | byte |
   |       |           |         |         |      |
 * |_______|___________|_________|_________|______|
 *
 * This frame receiving from the host device after all firmware frames received with the individual CRC16 values.
 * This frame for receive and compare whole firmware's total CRC16 value.
 * When the target device received this frame, compares total CRC16 values with the CRC16 value which is calculated
 * in itself.
 */

extern UART_HandleTypeDef huart1;
#define _USART1		&huart1

#define FLASH_SECTOR5_ADDR			0x08006000U
#define APP_PAGE_COUNT				35
#define SRAM_BASE_u      			0x20000000
#define VECT_TABLE_SIZE 			0xC0  						// 48 vectors * 4 bytes

extern uint8_t command;
extern struct crc_s crc8;

/********** Bootloader indicator byte definitions **********/
#define BL_START		0x24	// Start byte
#define BL_IND_CMD			0xCC	// Command data frame indicator
#define BL_IND_DATA			0xDD	// Firmware data frame indicator
#define BL_IND_CRC			0xFF	// Final CRC data frame indicator

/********** Bootloader Command Definitions **********/
#define BL_CMD_GET_CHIP_ID	0x00	// Command for reading chip ID
#define BL_CMD_ERASE_MEM	0x01	// Command for erasing 1 byte from specified memory address
#define BL_CMD_GET_CRC16	0x02	// Command for getting CRC16 value of sent firmware
#define BL_CMD_JMP_TO_APP	0x03	// Command for jumping to application code
#define BL_CMD_GET_STATUS	0x04	// Command for getting status of bootlaoder
#define BL_CMD_RESTART		0x05	// Command for restarting device

// Function for printing messages to terminal via USART
void bl_usart_printf(char *format,...);

// Function for jumping bootloader to application code
void bl_jump_to_app(void);

// Function for write firmware to flash area
void bl_write_mem_256_bytes(uint8_t* data, uint32_t addr);

// Function for erase 20kB application code area in flash
void bl_erase_35kB_mem(void);

// Function for read firmware from flash area
void bl_read_mem_256_bytes(uint8_t* data, uint32_t addr);

// Function for executing commands
void bl_execute_cmd(uint8_t cmd);

// Function for parsing data frame
void bl_data_frame_parser(uint8_t* data_frame);

#endif /* INC_BL_CM0_H_ */
