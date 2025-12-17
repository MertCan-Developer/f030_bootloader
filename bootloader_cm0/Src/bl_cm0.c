/*
 * bl_cm0.c
 *
 *  Created on: Dec 4, 2025
 *      Author: Mert
 */

#include "bl_cm0.h"
#include "cm_boot.h"


__attribute__((section(".ram_isr_vector"))) struct cm0_vectable_ ram_vectable;
uint32_t APP_START_ADDR	 = 	FLASH_SECTOR5_ADDR;
uint32_t  flash_addr	 = 	(uint32_t)0x08006000U; 			// Variable for erasing and flashing flash memory

// Function for printing messages to terminal via USART
void bl_usart_printf(char *format,...)
{
	char str[80] = {0};														// Created array variable in type of "char"
	va_list	args;															// Creates an object of the type of va_list
	va_start(args, format);													// Begins to reading arguments which is given to this functions
	vsprintf(str, format, args);											// Writes the arguments from args to str array in the format of "format"
	HAL_UART_Transmit(_USART1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);	// Tranmits data
	va_end(args);															// Making end of reading of arguments

	/*
	 * Usage of this function:
	 *
	 * usart_printf("Hello World = %d", int_value);
	 */
}

// Function for jumping bootloader to application code
void bl_jump_to_app(void)
{

    uint32_t app_reset   = *((uint32_t *)(APP_START_ADDR + 4));
    void (*reset_handler)(void) = (void (*)(void))app_reset;

	bl_usart_printf("BL_DBG_MSG: Bootloader jump to user app\r\n");
	bl_usart_printf("BL_DBG_MSG: App reset handler addr: %#x\r\n\n\n\n", reset_handler);
	HAL_Delay(3000);

	disable_unpend_all_ints();
	vectable_setup(APP_START_ADDR);
	SYSCFG->CFGR1 = SYSCFG_CFGR1_MEM_MODE;	// map RAM at 0
	app_start(APP_START_ADDR);
}

// Function for erase 20kB application code area in flash
void bl_erase_35kB_mem(void)
{
	FLASH_EraseInitTypeDef flash = {0};				// Flash erasing object
	HAL_FLASH_Unlock();								// Unlocks the flash to erase

	// Erasing flash area of code
	flash.TypeErase  	= 	FLASH_TYPEERASE_PAGES;	// Erase type is page erase
	flash.PageAddress	= 	APP_START_ADDR;			// Erasing start address
	flash.NbPages		=	APP_PAGE_COUNT;			// Number of pages to be erased
	uint32_t PageError	= 0;						// Error flag definition
	HAL_FLASHEx_Erase(&flash, &PageError);

	HAL_FLASH_Lock();								// Locks the flash
	bl_usart_printf("### Erased 35kB application code pages. Page error code: 0x%08lX ###\r\n", PageError);
}

// Function for write firmware to flash area
void bl_write_mem_256_bytes(uint8_t* data, uint32_t addr)
{
	uint32_t word = 0;								// 32-bit variable to store 4 bytes


	HAL_FLASH_Unlock();
	// Flashing the flash area of code
	for(uint16_t i=0; i<256; i+=4)
	{
		/*
		 * Writing firmware to flash word by word
		 */
		word = data[i]				// LSB byte
			 | (data[i+1] << 8)		// Next byte
			 | (data[i+2] << 16)	// Next byte
			 | (data[i+3] << 24);	// MSB byte
		/*
		 * As total we have 4 bytes at once. So flashing 4 bytes to flash at once, there will be 64 iterations
		 * to write 256 bytes to flash memory.
		 */

		// Flashing flash memory
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, word) != HAL_OK)
		{
			Error_Handler();
		}

		// Increasing the address of memory to write next memory area
		addr += 4;
	}
	HAL_FLASH_Lock();								// Locking the flash
}

// Function for executing commands
void bl_execute_cmd(uint8_t cmd)
{
	switch(cmd)
	{
	case BL_CMD_JMP_TO_APP:	// Jumping application code
		bl_jump_to_app();
		break;
	case BL_CMD_ERASE_MEM:	// Erasing 20kB application code area
		bl_erase_35kB_mem();
		break;
	case BL_CMD_GET_CHIP_ID:
		// Read Chip ID section from flash and send to host
		break;
	case BL_CMD_GET_CRC16:
		// Calculate CRC16 value of last firmware and send to host
		break;
	default:
		Error_Handler();
		break;
	}
}

// Function for parsing data frame
void bl_data_frame_parser(uint8_t* data_frame)
{
	if(data_frame[0] != '$' || (data_frame[0] == *((uint8_t*)NULL)))
	{
		Error_Handler();
	}

	if(data_frame[1] == BL_IND_CMD)
	{
		command = data_frame[2];


		//CRC8 Calculation for the bytes before CRC8 section

		bl_execute_cmd(crc8.cmd[2]);

		if(data_frame[4] != '!')
		{
			Error_Handler();
		}
	}

}
