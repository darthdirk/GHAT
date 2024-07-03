/*
 * tft.h
 *
 *  Created on: Jun 17, 2024
 *      Author: greg
 */

#ifndef INC_TFT_H_
#define INC_TFT_H_

#include "main.h"
#include <stdint.h>

void tft_init(void);
void tft_write_command(uint8_t cmd);
void tft_write_data(uint8_t data);
void tft_backlight_on(void);
void tft_backlight_off(void);



#endif /* INC_TFT_H_ */
