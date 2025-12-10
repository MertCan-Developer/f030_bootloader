/*
 * crc.c
 *
 *  Created on: Dec 8, 2025
 *      Author: Mert
 */


#include "crc.h"

extern uint8_t DataFrame[260];

// CRC8 Calculation function
uint8_t crc_calc_crc8(struct crc_s* crc8)
{
	uint8_t crc_reg = 0x00;				// CRC value initialization
	uint8_t poly  	= 0x07;				// Generator polynom initialization
	uint8_t* src;

	if(DataFrame[1] == 0xCC)			// Is data frame command frame?
	{
		src = crc8->cmd;
	}
	else if(DataFrame[1] == 0xDD)		// Is data frame firmware frame?
	{									// If it's true then code enters to Error Handler function.
		Error_Handler();				// Because in command frame there is no CRC16 calculation.
	}


	for(uint16_t B=0; B<crc8->length; B++)			// Bytes of message
	{
		crc_reg ^= src[B];							// Bytes of messages

		for(uint8_t b=0; b<8; b++)					// bits of message's byte
		{
			if(crc_reg & 0x80)						// Controlling first bit of dividend if it is 1 or not
			{
				crc_reg = (crc_reg << 1) ^ poly;	// Shifting by 1 and XOR operation with divisor
			}
			else
			{
				crc_reg <<= 1;						// CRC value (message) shifting by 1 if firts bit is not 1
			}
		}
	}

	return crc_reg;
}

// CRC16 Calculation function
uint16_t crc_calc_crc16(struct crc_s* crc16)
{
	uint16_t crc_reg = 0xFFFF;			// Initial value
	uint16_t poly	 = 0x1021;			// CRC16 polynom
	uint8_t* src;

	if(DataFrame[1] == 0xDD)			// Is the data frame the firmware frame?
	{
		src = DataFrame;
	}
	else if(DataFrame[1] == 0xCC)		// Is data frame command frame?
	{									// If it's true then code enters to Error Handler function.
		Error_Handler();				// Because in firmware frame there is no CRC8 calculation.
	}

	// Calculating CRC16 value for "START BYTE + Inidicator Byte + Num of Chunks Byte + First Chunk"
	if(chunk_count == 0)
	{
		for(uint8_t B=0; B<3; B++)
		{
			crc_reg ^= src[B];

			for(uint8_t b=0; b<8; b++)
			{
				if(crc_reg & 0x8000)
				{
					crc_reg = (crc_reg << 1) ^ poly;
				}
				else
				{
					crc_reg <<= 1;
				}
			}
		}
		chunk_count++;
	}

	// Calculating CRC16 for only chunk section
	if((chunk_count > 0) && (chunk_count < num_of_chunks))
	{
		for(uint8_t B=3; B<(crc16->length - 3); B++)
		{
			crc_reg ^= src[B];

			for(uint8_t b=0; b<8; b++)
			{
				if(crc_reg & 0x8000)
				{
					crc_reg = (crc_reg << 1) ^ poly;
				}
				else
				{
					crc_reg <<= 1;
				}
			}
		}
		chunk_count++;
	}

	return crc_reg;
}
