/*
 * crc.c
 *
 *  Created on: Dec 8, 2025
 *      Author: Mert
 */


#include "crc.h"

/*
 * CRC-8 Hesaplama adımları:
 *
 * 1. 8 bitlik mesaj alınır
 * 2. Mesajın sonuna 8 adet 0 eklenir (padding)
 * 3. Mesajın ve bölücü polinomun ilk '1'lerini aynı hizaya getir.
 * 4. Hizalanan polinom ve mesaj byte'larının her biti için XOR işlemi yap.
 * 5. Her bitin XOR sonucu ayrı birer sonuç olarak çıkar ve toplam sonuç byte'ında her biri bir biti temsil eder.
 */


uint8_t crc_calc_crc8(uint8_t* message, uint16_t length)
{
	uint8_t crc 	 = 0;					// CRC value initialization
	uint8_t divisor  = 0x07;				// Dividor initialization

	for(uint16_t B=0; B<length; B++)		// Bytes of message
	{
		crc ^= message[B];

		for(uint8_t b=0; b<8; b++)			// bits of message's byte
		{
			if(crc & 0x80)					// Controlling first bit of dividend if it is 1 or not
			{
				crc = (crc << 1) ^ divisor;	// Shifting by 1 and XOR operation with divisor
			}
			else
			{
				crc <<= 1;					// CRC value (message) shifting by 1 if firts bit is not 1
			}
		}
	}

	return crc;
}
