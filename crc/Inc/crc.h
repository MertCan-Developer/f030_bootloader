/*
 * crc.h
 *
 *  Created on: Dec 8, 2025
 *      Author: Mert
 */

#ifndef INC_CRC_H_
#define INC_CRC_H_

#include "main.h"

extern uint8_t num_of_chunks;
extern uint8_t chunk_count;
extern uint8_t num_of_chunks;

struct crc_s
{
	uint8_t  width;			// 	Bit number of data (8 bit, 16 bit, 32 bit)
	uint32_t init;			// 	Initial value of CRC register (CRC8->0x00, CRC16->0x0000, CRC32->0xFFFFFFFF)
	uint32_t xor_out;		// 	Xor'ing final crc value before out
	uint8_t  reflect_in;	// 	Reflecting data if needed to get data LSB-first format
	uint8_t  reflect_out;	// 	Reflecting data if needed to send data LSB-first
	uint8_t  cmd[5];		// 	Command to be calculated of CRC-8
	uint8_t  firmware[260];	//	Firmware array to be calculated of CRC16
	uint32_t length;		// 	Length of message will be calculated CRC8 value
};

// CRC8 Calculation function prototype
uint8_t crc_calc_crc8(struct crc_s* crc8);

// CRC16 Calculation function prototype
uint16_t crc_calc_crc16(struct crc_s* crc16);

#endif /* INC_CRC_H_ */
