/*
 * tft.c
 *
 *  Created on: Jun 17, 2024
 *      Author: greg
 */

#include "tft.h"
#include "main.h"

#include <assert.h>

// Define TFT control pins
#define TFT_CS_PIN        CS_Pin
#define TFT_CS_GPIO_PORT  CS_GPIO_Port
#define TFT_RST_PIN       RESET_Pin
#define TFT_RST_GPIO_PORT RESET_GPIO_Port
#define TFT_DC_PIN        D_C_Pin
#define TFT_DC_GPIO_PORT  D_C_GPIO_Port
#define TFT_BL_PIN        BLK_Pin
#define TFT_BL_GPIO_PORT  BLK_GPIO_Port


static void tft_reset(void);
static void tft_select(void);
static void tft_deselect(void);
static void tft_set_command_mode(void);
static void tft_set_data_mode(void);


void lcdInit(const lcdInitCmdT *initCmds) {
	const lcdInitCmdT *cmd;

    tft_reset();
	tft_select();
	// Iterate through the init commands as long as databytes does not reach the end of data
    for (cmd = initCmds; cmd->databytes != 0xFF; cmd++) {
    	lcdCmd(cmd->cmd);
    	// Check to see if any of the lower 7 bits of data are set. the lower 7 bytes rep data bytes to be sent.
    	if (cmd->databytes & 0x7F) {
    		lcdCmd(cmd->databytes & 0x7F);
    	}
    	// check to see if the MSB(bit7) of databytes is set that would indicate if there should be a delay after the cmd is sent
    	if (cmd->databytes & 0x80) {
    		HAL_Delay(15/100);
    	}
    }
    tft_backlight_on();
    tft_deselect();
}

HAL_StatusTypeDef lcdCmd(uint8_t cmd) {
    tft_set_command_mode();
    tft_select();
    HAL_StatusTypeDef status = HAL_SPI_Transmit((get_spi1_handle()), &cmd, sizeof(cmd), HAL_MAX_DELAY);
    tft_deselect();
    if (status != HAL_OK) {
    	return HAL_ERROR;
    }
    return HAL_OK;
    tft_set_data_mode();
}

HAL_StatusTypeDef lcdData(uint8_t data, int len) {
    tft_set_data_mode();
    HAL_StatusTypeDef status = HAL_SPI_Transmit((get_spi1_handle()), &data, sizeof(data), HAL_MAX_DELAY);
    if (status != HAL_OK) {
    	return HAL_ERROR;
    }
    return HAL_OK;
}

void tft_backlight_on(void) {
    HAL_GPIO_WritePin(TFT_BL_GPIO_PORT, TFT_BL_PIN, GPIO_PIN_SET);
}

void tft_backlight_off(void) {
    HAL_GPIO_WritePin(TFT_BL_GPIO_PORT, TFT_BL_PIN, GPIO_PIN_RESET);
}

static void tft_reset(void) {
    HAL_GPIO_WritePin(TFT_RST_GPIO_PORT, TFT_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(TFT_RST_GPIO_PORT, TFT_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(10);
}

static void tft_select(void) {
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_RESET);
}

static void tft_deselect(void) {
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

static void tft_set_command_mode(void) {
    HAL_GPIO_WritePin(TFT_DC_GPIO_PORT, TFT_DC_PIN, GPIO_PIN_RESET);
}

static void tft_set_data_mode(void) {
    HAL_GPIO_WritePin(TFT_DC_GPIO_PORT, TFT_DC_PIN, GPIO_PIN_SET);
}
