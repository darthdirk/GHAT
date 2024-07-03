/*
 * tft.c
 *
 *  Created on: Jun 17, 2024
 *      Author: greg
 */

#include "tft.h"
#include "main.h"

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

void tft_init(void) {
    tft_reset();

    tft_select();

    tft_write_command(0x01); // Software reset
    HAL_Delay(150);

    tft_write_command(0x11); // Sleep out
    HAL_Delay(500);

    tft_write_command(0x13); // Normal display mode on
    HAL_Delay(10);

    tft_write_command(0x29); // Display on
    HAL_Delay(10);

    tft_deselect();
}

void tft_write_command(uint8_t cmd) {
    tft_set_command_mode();
    HAL_SPI_Transmit((get_spi1_handle()), &cmd, 1, HAL_MAX_DELAY);
}

void tft_write_data(uint8_t data) {
    tft_set_data_mode();
    HAL_SPI_Transmit((get_spi1_handle()), &data, 1, HAL_MAX_DELAY);
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
