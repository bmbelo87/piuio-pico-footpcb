#ifndef PIUIO_PICO_PIUIO_CONFIG_H
#define PIUIO_PICO_PIUIO_CONFIG_H
#include "piuio_ws2812_helpers.h"

// Descomente essa parte se quiser usar fita LED WS2812 endereçavel.
// #define ENABLE_WS2812_SUPPORT
// #define WS2812_IS_RGBW false
// #define WS2812_PIN 12



// Para configurar fita led WS2812 endereçavel. 
static uint32_t ws2812_color[5] = {
        urgb_u32(0, 255, 0),    // Lower left
        urgb_u32(255, 0, 0),    // Upper left
        urgb_u32(0, 0, 255),    // Bass / neon
        urgb_u32(255, 0, 0),    // Upper right
        urgb_u32(0, 255, 0)     // Lower right
};

// Mapeamento dos pinos para cada botão
// Esse é o mapeamento padrão do piuio clone, é possível alterar caso queira mudar os pinos.
static const uint8_t pinSwitch[12] = {
        // Player 1
        0,     // P1 UL ↖️ OUTUP
        2,      // P1 UR ↗️ OUTDOWN
        4,     // P1 CN ⏹️ OUTLEFT
        6,     // P1 DL ↙️ OUTRIGHT
        8,      // P1 DR ↘️ OUTCEN
        // Player 2
        28,     // P2 UL ↖️
        26,      // P2 UR ↗️
        21,      // P2 CN ⏹️
        19,     // P2 DL ↙️
        17,      // P2 DR ↘️
        // Service/Test/Coins
        15,     // Service 🔼
        14      // Test ⏫
};

static const uint8_t pinNX[2] = {
        10, // INX0
        11  // INX1
};

static const uint8_t pinNXP2[2] = {
        12, // INX0
        13  // INX1

};

// Mapeamento dos pinos para cada LED dos botões (Quando aciona um sensor)
static const uint8_t pinLED[10] = {
        // Player 1 Led
        1,     // P1 UL ↖️
        3,      // P1 UR ↗️
        5,     // P1 CN ⏹️
        7,     // P1 DL ↙️
        9,      // P1 DR ↘️
        // Player 2 Led
        27,     // P2 UL ↖️
        22,      // P2 UR ↗️
        20,      // P2 CN ⏹️
        18,     // P2 DL ↙️
        16       // P2 DR ↘️
};


#endif //PIUIO_PICO_PIUIO_CONFIG_H
