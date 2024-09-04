

### Set Up the Hardware:
#### Components Needed
1.  NodeMCU (ESP8266)
2.  DFPlayer Mini MP3 module
3.  MicroSD card (for both the DFPlayer Mini and NodeMCU SD card reader if using separate cards)
4.  SD card reader module
5.  Relay module (one relay per Adhan or one relay that is controlled based on the current Adhan)
6.  Speaker
7.  LEDs (Blue, Green, Red)
8.  Current-limiting resistors (220 ohms recommended)
9.  Connections (jumper wires, breadboard, etc.)
10.
#### Connect the DFPlayer Mini MP3 module to the NodeMCU:

1. VCC to 3.3V or 5V
2. GND to GND
3. TX of NodeMCU to RX of DFPlayer Mini
4. RX of NodeMCU to TX of DFPlayer Mini (if using software serial)
5. Connect a speaker to the SPK_1 and SPK_2 terminals of the DFPlayer Mini.

#### Connect the SD card reader module to the NodeMCU:
1. VCC to 3.3V or 5V
2. GND to GND
3. MISO to D6 (or another MISO pin)
4. MOSI to D7 (or another MOSI pin)
5. SCK to D5 (or another SCK pin)
6. CS to D8 (or another CS pin)




**LED Setup**

-   **Blue LED**: Indicates that the WiFi is connected and the clock is synchronized.
-   **Green LED**: Indicates that the NodeMCU is powered on and running.
-   **Red LED**: Indicates that the MP3 is currently playing.

**GPIO Pin Assignments**

-   **Blue LED (WiFi status)**: D0
-   **Green LED (NodeMCU power status)**: D1
-   **Red LED (MP3 playing status)**: D2

**Circuit Setup**

-   Connect each LED's anode (longer leg) to a corresponding GPIO pin (D0, D1, D2) via a 220-ohm resistor.
-   Connect the cathode (shorter leg) of each LED to the common ground.
