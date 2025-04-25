# PIUIO-PICO (FootPCB Edition)

A **Pump It Up IO board (PIUIO) clone** based on the **Raspberry Pi Pico** microcontroller (inspired by [48productions](https://github.com/48productions)). The key improvement is compatibility with **FootPCB**, including **individual sensor recognition!** :D  

---

## ⚙️ Hardware Requirements  
You will need:  
- **Raspberry Pi Pico** (RP2040 tested)  
- **2× FootPCB** (for 2 players; only 1 needed for single-player)  
- **Foot sensors** (for buttons)  
- **12V switching power supply**  
- **PIUIO cables** (to connect Pico → FootPCB)  
- *(More items may be added later...)*  

---

## 🔌 FootPCB and RP2040 Pin Configuration  
Modified pins in [`piuio_config.h`](./piuio_config.h) *(schematic coming soon)*.  

### 📌 FootPCB Connectors  
#### **Top Side (3 connectors)**  
1. **Power Supply**  
   - `12V`, `12V` → Connect to `V+`  
   - `GND`, `GND`, `GND` → Connect to `V-`  
   - `FG` → Earth Ground (symbol: ⏚)  

2. **Button Output (to PIUIO)**  
   - Signals: `OUTUP_P1`, `OUTDOWN_P1`, `OUTLEFT_P1`, `OUTRIGHT_P1`, `OUTCEN_P1`  
   - *Note*: Mapped as `UpLeft`, `UpRight`, `Center`, `DownLeft`, `DownRight`. Confusing? Maybe, but it works! 😆  

3. **Light Input (from PIUIO)**  
   - Pins: `INX0`, `INX1`, `LTUP_1P`, `LTDOWN_1P`, `LTLEFT_1P`, `LTRIGHT_1P`, `LTCEN_1P`, `FG`, `GND`, `VCC`  

   **`INX0`/`INX1` MUX Logic**  
   | `INX0` | `INX1` | Active Sensor |  
   |--------|--------|---------------|  
   |   0    |   0    | Sensor 1      |  
   |   0    |   1    | Sensor 2      |  
   |   1    |   0    | Sensor 3      |  
   |   1    |   1    | Sensor 4      |  
   *(Magic? No—just multiplexing!)*  

   - `LT*_1P` pins control LEDs.  
   - `FG`/`GND` → Pico GND, `VCC` → `3V3_OUT`.  

#### **Bottom Side (5 connectors)**  
- Button/LED pins:  
  `Sensor 1`, `Sensor 2`, `Sensor 3`, `Sensor 4`, `GND`, `Led In`, `GND Led`  

---

## 🛠️ Schematic Preview  
*(Coming soon!)*  

---

## 💾 Firmware Setup
[See the Pi Pico SDK repo](https://github.com/raspberrypi/pico-sdk) for instructions on preparing the build environment. All the required CMake files should already be present and ready for compilation.

General overview:
 - Add the SDK via your method of choice (see the pico-sdk repo)
 - Run `git submodule update --init` inside the SDK directory to set up the other required libraries (namely tinyusb)
 - Create the "build" folder inside the "piuio-pico" folder.
 - Inside the build folder, run `cmake ..`, then run `make`. You'll get a .UF2 file in the build folder that you can upload to the Pico!
 - With the Pico unplugged from the PC, hold the "BOOTSEL" button on the Pico, then plug in the Pico.
 - The Pico will now show up as a "flash drive" in your OS. Copy the .UF2 file onto the Pico, and the code will be uploaded!

## 🚨 PIU Online Notice
Using a hand controller with games connected to the official Pump it Up online service is considered cheating by Andamiro and may lead to actions being taken against your account.
As such, this use case is not officially endorsed by piuio-pico.

## 🌟 Credits  
- Original PIUIO concept by [48productions](https://github.com/48productions).  
- FootPCB adaptation by **you**!

- This project is based off of the [tinyusb device USB examples](https://github.com/hathach/tinyusb/tree/master/examples/device) (specifically webusb_serial and hid_generic_input)

- Protocol information from the [PIUIO_Arduino](https://github.com/ckdur/PIUIO_Arduino/) and [piuio_clone](https://github.com/racerxdl/piuio_clone/) repositories.
