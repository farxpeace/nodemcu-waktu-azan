

### Set Up the Hardware:
#### Components Needed
1.  NodeMCU (ESP8266)
2.  DFPlayer Mini MP3 module
3.  MicroSD card (for DFPlayer Mini)
6.  Speaker (Prefer use AUX connection)
#### Connect the DFPlayer Mini MP3 module to the NodeMCU:

1. VCC to 3.3V or 5V
2. GND to GND
3. TX of NodeMCU to RX of DFPlayer Mini
4. RX of NodeMCU to TX of DFPlayer Mini (if using software serial)
5. Connect a speaker to the SPK_1 and SPK_2 terminals of the DFPlayer Mini.


#### SD Card files structure
##### All mp3 files must be put inside folder "mp3"
![structure](https://i.imgur.com/vEYMFWv.png)