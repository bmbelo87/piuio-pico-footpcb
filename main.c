#include "bsp/board.h"
#include "device/usbd.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "piuio_structs.h"
#include "piuio_config.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define UART_TX_PIN 12
#define UART_RX_PIN 13
#define BAUD_RATE 115200

const uint8_t pos[] = { 0, 1, 2, 3, 4 }; // don't touch this

// PIUIO input and output data
uint8_t inputData[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
struct lampArray lamp = {};

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request) {
    // nothing to with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;

    // Request 0xAE = IO Time
    if (request->bRequest == 0xAE) {
        switch (request->bmRequestType) {
            case 0x40:
                return tud_control_xfer(rhport, request, (void *)&lamp.data, 8);
            case 0xC0:
                return tud_control_xfer(rhport, request, (void *)&inputData, 8);
            default:
                return false;
        }
    }

    return false;
}

void setup_uart() {
    uart_init(UART_ID, BAUD_RATE); // Configure UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // RX
}

void piuio_task(void) {
    // 1. Controle do MUX
    uint8_t mux_pos = lamp.data[PLAYER_1] & 0x03;
    gpio_put(pinNX[1], mux_pos & 1);
    gpio_put(pinNX[0], (mux_pos >> 1) & 1);

    uint8_t mux_posP2 = lamp.data[PLAYER_2] & 0x03;
    gpio_put(pinNXP2[1], mux_posP2 & 1);
    gpio_put(pinNXP2[0], (mux_posP2 >> 1) & 1);

    // 2. Espera para estabilização
    sleep_us(200); // anterior 200, 1000 apenas para testes.

    // 3. Reset dos dados de input
    inputData[PLAYER_1] = 0xFF;
    inputData[PLAYER_2] = 0xFF;
    inputData[CABINET] = 0xFF;

    // 4. Leitura dos sensores
    for (int btn = 0; btn < 5; btn++) {
        bool pressed = !gpio_get(pinSwitch[btn]);
        if (mux_pos == 0 && pressed) inputData[PLAYER_1] &= ~(1 << btn);
        else if (mux_pos == 3 && pressed) inputData[PLAYER_1] &= ~(1 << btn);
        else if (mux_pos == 2 && pressed) inputData[PLAYER_1] &= ~(1 << btn);
        else if (mux_pos == 1 && pressed) inputData[PLAYER_1] &= ~(1 << btn);

        pressed = !gpio_get(pinSwitch[btn + 5]);
        if (mux_posP2 == 0 && pressed) inputData[PLAYER_2] &= ~(1 << btn);
        else if (mux_posP2 == 3 && pressed) inputData[PLAYER_2] &= ~(1 << btn);
        else if (mux_posP2 == 2 && pressed) inputData[PLAYER_2] &= ~(1 << btn);
        else if (mux_posP2 == 1 && pressed) inputData[PLAYER_2] &= ~(1 << btn);
    }

    // 5. Recebe os dados de Coin, Test e Service via UART0
    if (uart_is_readable(UART_ID)) {
        uint8_t received = uart_getc(UART_ID);

        // Acende LED da Pico A quando receber botão pressionado via UART
        bool any_pressed = false;

        if (!(received & (1 << 1))) {
            inputData[CABINET] &= ~(1 << 1);
            any_pressed = true;
            sleep_us(2);
        }
        
        if (!(received & (1 << 2))) {
            inputData[CABINET] &= ~(1 << 2);
            any_pressed = true;
            sleep_us(2);
        }
        
        if (!(received & (1 << 6))) {
            inputData[CABINET] &= ~(1 << 6);
            any_pressed = true;
            sleep_us(2);
        }

        if (!(received & (1 << 7))) {
            inputData[CABINET] &= ~(1 << 7);
            any_pressed = true;
            sleep_us(2);
        }
    
        gpio_put(25, any_pressed ? 1 : 0);
    }

    // 6. Controle de LEDs
    for (int i = 0; i < 5; i++) {
        gpio_put(pinLED[i], lamp.data[PLAYER_1] & (1 << (pos[i] + 2)) ? 0 : 1);
        gpio_put(pinLED[i + 5], lamp.data[PLAYER_2] & (1 << (pos[i] + 2)) ? 0 : 1);
    }

    // 7. Enviar sinais de luz via UART
    uint8_t cab_lights = 0;
    cab_lights |= (lamp.r2_halo      << 0);
    cab_lights |= (lamp.r1_halo      << 1);
    cab_lights |= (lamp.l2_halo      << 2);
    cab_lights |= (lamp.l1_halo      << 3);
    cab_lights |= (lamp.bass_light   << 4);
    uart_putc(UART_ID, cab_lights);

    // uint8_t neon_lights = 0;
    // neon_lights 
    // uart_putc(UART_ID, neon_lights);
}

int main(void) {
    board_init();

    setup_uart();

    // Setup Inputs
    for (int i = 0; i < 10; i++) {
        gpio_init(pinSwitch[i]);
        gpio_set_dir(pinSwitch[i], false);
        gpio_pull_up(pinSwitch[i]);
    }

    // Setup LEDs
    for (int i = 0; i < 10; i++) {
        gpio_init(pinLED[i]);
        gpio_set_dir(pinLED[i], true);
    }

    // Setup MUX control pins
    for (int i = 0; i < 2; i++) {
        gpio_init(pinNX[i]);
        gpio_set_dir(pinNX[i], true);
    }

    for (int i = 0; i < 2; i++) {
        gpio_init(pinNXP2[i]);
        gpio_set_dir(pinNXP2[i], true);
    }

    tud_init(BOARD_TUD_RHPORT);

    while (true) {
        tud_task();
        piuio_task();
    }

    return 0;
}
