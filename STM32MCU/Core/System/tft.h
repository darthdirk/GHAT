/*
 * tft.h
 * driver for ST7789 CFAF240240A3-013TN-E1 TFT display
 *  Created on: Jun 17, 2024
 *      Author: greg
 */

#ifndef INC_TFT_H_
#define INC_TFT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#include "main.h"

// Defines for the ST7789 registers
// REF ST7789H2 Page 157
#define ST7789_SWRESET	(0x01) // software reset
#define ST7789_RDDSDR	(0x0F) // read display self-diagnostic
#define ST7789_SLPIN  	(0x10) // sleep in
#define ST7789_SLPOUT	(0x11) // sleep out
#define ST7789_NORON	(0x13) // normal display mode on
#define ST7789_INVOFF	(0x20) // inversion off
#define ST7789_INVON	(0x21) // inversion on
#define ST7789_DISPOFF	(0x28) // display off
#define ST7789_DISPON	(0x29) // display on
#define ST7789_CASET	(0x2A) // column address set
#define ST7789_RASET	(0x2B) // row address set
#define ST7789_RAMWR	(0x2C) // memory write
#define ST7789_RAMRD	(0x2E) // memory read
#define ST7789_PTLAR	(0x30) // partial start end address set
#define ST7789_VSCRDEF	(0x33) // vertical scrolling definition
#define ST7789_TEOFF	(0x34) // tearing effect line off
#define ST7789_TEON		(0x35) // tearing effect line on
#define ST7789_MADCTL	(0x36) // memory data access control
#define ST7789_VSCRSADD	(0x37) // vertical scrolling start address
#define ST7789_IDMOFF	(0x38) // idle mode off
#define ST7789_IDMON	(0x39) // idle mode on
#define ST7789_COLMOD	(0x3A) // interface pixel format
#define ST7789_RAMWRC	(0x3C) // memory write continue
#define ST7789_RAMRDC	(0x3E) // memory read continue
#define ST7789_PORCTRL	(0xB2) // porch control
#define ST7789_GCTRL	(0xB7) // gate control
#define ST7789_VCOMS	(0xBB) // vcom setting
#define ST7789_POWSAVE	(0xBC) // power saving mode
#define ST7789_SLPOFFSAVE (0xBD) // display off power save
#define ST7789_LCMCTRL	(0xC0) // LCD control
#define ST7789_VDVVRHEN	(0xC2) // vdv and vrh command enable
#define ST7789_VRHS		(0xC3) // VRH set
#define ST7789_VDVSET	(0xC4) // VDV set
#define ST7789_FRCTR2	(0xC6) // frame rate control in normal mode
#define ST7789_PWCTRL1	(0xD0) // power control 1
#define ST7789_PVGAMCTRL (0xE0) // positive voltage gamma control
#define ST7789_NVGAMCTRL (0xE1) // negative voltage gamma control

/*
The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct {
 uint8_t cmd;
 uint8_t data[16];
 uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcdInitCmdT;


typedef enum {
 LCD_TYPE_ILI = 1,
 LCD_TYPE_ST,
 LCD_TYPE_MAX,
} typeLcdT;

void lcdInit(const lcdInitCmdT *initCmds);


HAL_StatusTypeDef lcdCmd(uint8_t cmd);


HAL_StatusTypeDef lcdData(uint8_t data, int len);



uint32_t lcdGetId(SPI_HandleTypeDef *hspi1);

void startAtPage(SPI_HandleTypeDef *hspi1(), int ypos);

void sendPackage(SPI_HandleTypeDef *hspi1(), uint16_t *linedata, uint32_t size);
/* To send a set of lines we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
 * before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
 * because the D/C line needs to be toggled in the middle.)
 * This routine queues these commands up as interrupt transactions so they get
 * sent faster (compared to calling spi_device_transmit several times), and at
 * the mean while the lines for next transactions can get calculated.
 */
void sendLines(SPI_HandleTypeDef *hspi1(), int ypos, uint16_t *linedata);

void sendLineFinish(SPI_HandleTypeDef *hspi1());

//Simple routine to generate some patterns and send them to the LCD. Don't expect anything too
//impressive. Because the SPI driver handles transactions in the background, we can calculate the next line
//while the previous one is being sent.
// void display_pretty_colors(spi_device_handle_t spi);

// Clear the screen with a given rgb value
void clearScreen(SPI_HandleTypeDef *hspi1(), uint16_t rgb);


static const lcdInitCmdT stInitCmds[] = {

   // //{ST7789_SLPOUT,{0x00},0x80},
   // {ST7789_SLPIN, {0x00}, 0x80}, // Sleep In (Low power mode)

   //-----------------------------Display setting--------------------------------
   {ST7789_MADCTL, {0x60}, 1}, //Page 215
   //SPI_sendData(0x00); //DEFAULT
   //SPI_sendData(0x48); //TEST

   // Bit D7- Page Address Order
   // “0” = Top to Bottom (When MADCTL D7=”0”).
   // “1” = Bottom to Top (When MADCTL D7=”1”).
   // Bit D6- Column Address Order
   // “0” = Left to Right (When MADCTL D6=”0”).
   // “1” = Right to Left (When MADCTL D6=”1”).
   // Bit D5- Page/Column Order
   // “0” = Normal Mode (When MADCTL D5=”0”).
   // “1” = Reverse Mode (When MADCTL D5=”1”)
   // Note: Bits D7 to D5, alse refer to section 8.12 Address Control
   // Bit D4- Line Address Order
   // “0” = LCD Refresh Top to Bottom (When MADCTL D4=”0”)
   // “1” = LCD Refresh Bottom to Top (When MADCTL D4=”1”)
   // Bit D3- RGB/BGR Order
   // “0” = RGB (When MADCTL D3=”0”)
   // “1” = BGR (When MADCTL D3=”1”)
   // Bit D2- Display Data Latch Data Order
   // “0” = LCD Refresh Left to Right (When MADCTL D2=”0”)
   // “1” = LCD Refresh Right to Left (When MADCTL D2=”1”)
   //Address control
   {ST7789_COLMOD, {0x55}, 1}, //Interface pixel format Pg 224


//{ST7789_INVOFF,{0x00},0},
 {ST7789_INVON, {0x00}, 0},
 {ST7789_CASET, {0x00, 0x00, 0x00, 0xEF}, 4},

 {ST7789_RASET, {0x00, 0x00, 0x00, 0xEF}, 4},

 //------------------------- Frame rate setting-------------------
 {ST7789_PORCTRL, {0x0C, 0x0C, 0x00, 0x33, 0x33}, 5},

 {ST7789_GCTRL, {0x35}, 1}, //Gate Control
                            //VGH:13.26/VGL:-10.43

 //----------------------- Power setting-------------------

 {ST7789_VCOMS, {0x1F}, 1}, //VCOM Setting
                            //VCOM = 0.875V

 {ST7789_LCMCTRL, {0x2C}, 1}, //LCM Control
                              //Power On Sequence default

 {ST7789_VDVVRHEN, {0x01}, 1}, //VDV and VRH Command Enable

 {ST7789_VRHS, {0x12}, 1}, //VRH Set
                           //4.45+( vcom+vcom offset+0.5vdv)

 {ST7789_VDVSET, {0x20}, 1}, //VDV Set
                             //VDV = 0V

 {ST7789_FRCTR2, {0x0F}, 1}, //Frame Rate Control in Normal Mode
                             //60HZ

 {ST7789_PWCTRL1, {0xA4, 0xA1}, 2}, //Power Control 1
                                    //VDS=2.3V/AVCL = -4.8V /AVDD=6.8V

 //               --------------------
 //               --------------------
 //               Set Gamma     for BOE 1.3
 //               --------------------
 // Set_Gamma: //  Is this a goto?

 {ST7789_PVGAMCTRL, {0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39, 0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D}, 14},

 //--------------------
 {ST7789_NVGAMCTRL, {0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39, 0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31}, 14},
 /* Sleep Out */
 {ST7789_SLPOUT, {0}, 0x80},
 /* Display On */
 {ST7789_DISPON, {0}, 0x80},
 {0, {0}, 0xff}



};

#endif /* INC_TFT_H_ */
