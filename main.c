
#include "bsp/board.h"
#include "device/usbd.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "piuio_structs.h"
#include "piuio_config.h"

#ifdef ENABLE_WS2812_SUPPORT
#include "piuio_ws2812.h"
#endif

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

// Estados dos sensores (4 sensores por botão, 5 botões por jogador)
static uint8_t sensor_states[2][5] = {0}; // [player][button]
static bool leds_initialized = false;

void piuio_task(void) {

    // 1. Controle do MUX
    uint8_t mux_pos = lamp.data[PLAYER_1] & 0x03;
    gpio_put(pinNX[0], mux_pos & 1);
    gpio_put(pinNX[1], (mux_pos >> 1) & 1);

    uint8_t mux_posP2 = lamp.data[PLAYER_2] & 0x03;
    gpio_put(pinNXP2[0], mux_posP2 & 1);
    gpio_put(pinNXP2[1], (mux_posP2 >> 1) & 1);
    
    // 2. Espera para estabilização (aumentada para 50µs)
    sleep_us(200); 

    // 3. Reset dos dados de input
    inputData[PLAYER_1] = 0xFF;
    inputData[PLAYER_2] = 0xFF;
    inputData[CABINET] = 0xFF;

    // 4. Leitura precisa dos sensores com máscara estrita
    for(int btn = 0; btn < 5; btn++) {
        bool pressed = !gpio_get(pinSwitch[btn]);
        
        // Player 1 - Atualização estrita por MUX
        if(mux_pos == 0 && pressed) { // Sensor 1
            inputData[PLAYER_1] &= ~(1 << btn);
        }
        else if(mux_pos == 1 && pressed) { // Sensor 2
            inputData[PLAYER_1] &= ~(1 << btn);
        }
        else if(mux_pos == 2 && pressed) { // Sensor 3
            inputData[PLAYER_1] &= ~(1 << btn);
        }
        else if(mux_pos == 3 && pressed) { // Sensor 4
            inputData[PLAYER_1] &= ~(1 << btn);
        }

        // Player 2 - mesma lógica
        pressed = !gpio_get(pinSwitch[btn+5]);
        if(mux_posP2 == 0 && pressed) {
            inputData[PLAYER_2] &= ~(1 << btn);
        }
        else if(mux_posP2 == 1 && pressed) {
            inputData[PLAYER_2] &= ~(1 << btn);
        }
        else if(mux_posP2 == 2 && pressed) {
            inputData[PLAYER_2] &= ~(1 << btn);
        }
        else if(mux_posP2 == 3 && pressed) {
            inputData[PLAYER_2] &= ~(1 << btn);
        }
    }

    // Adicione para verificar o MUX
    gpio_put(pinLED[0], mux_pos & 1);       // LED indica bit 0 do MUX
    gpio_put(pinLED[1], (mux_pos >> 1) & 1); // LED indica bit 1 do MUX
    gpio_put(pinLED[5], mux_posP2 & 1);       // LED indica bit 0 do MUX
    gpio_put(pinLED[6], (mux_posP2 >> 1) & 1); // LED indica bit 1 do MUX
    


    // 5. Botões de cabinet (fora da FootPCB)
    if(!gpio_get(pinSwitch[10])) inputData[CABINET] &= ~(1 << 1); // Test
    if(!gpio_get(pinSwitch[11])) inputData[CABINET] &= ~(1 << 6); // Service

    // 6. Controle de LEDs (seu código original)
    for(int i = 0; i < 5; i++) {
        gpio_put(pinLED[i], lamp.data[PLAYER_1] & (1 << (pos[i] + 2)) ? 0 : 1);
        gpio_put(pinLED[i+5], lamp.data[PLAYER_2] & (1 << (pos[i] + 2)) ? 0 : 1);
    }

    gpio_put(25, lamp.bass_light);
}

int main(void) {
    board_init();

    // Init WS2812B
    #ifdef ENABLE_WS2812_SUPPORT
    ws2812_init(&lamp);
    #endif

    // Set up GPIO pins: Inputs first, then outputs
    for (int i = 0; i < 12; i++) {
        gpio_init(pinSwitch[i]);
        gpio_set_dir(pinSwitch[i], false);
        gpio_pull_up(pinSwitch[i]);
    }

    for (int i = 0; i < 10; i++) {
        gpio_init(pinLED[i]);
        gpio_set_dir(pinLED[i], true);
    }

    for (int i = 0; i < 2; i++) {
        gpio_init(pinNX[i]);
        gpio_set_dir(pinNX[i], true);
    }

    for (int i = 0; i < 2; i++) {
        gpio_init(pinNXP2[i]);
        gpio_set_dir(pinNXP2[i], true);
    }

    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);

    // Main loop
    while (true) {
        tud_task(); // tinyusb device task
        piuio_task();
    }

    return 0;
}