# embeddedBridge_HW
THIS PROJECT IS UNDER CONSTRUCTION, IT WILL NOT WORK AS INTENDED!

## Introduction
The embeddedBridge provides an easy way to test your own embedded devices. It comes with an great amount of features with makes it compatible to a lot devices.

This is the Hardware repository. If you are looking fore the Software repository click [here](https://github.com/Asyny/embeddedBridge).

### Features
- based on Raspberry Pi Pico
- 2x variable powersuppies [XL6009]
  - voltage and current measurement [INA219]
- 4x switched output for powersuppies
  - 2x with voltage and current measurement [INA219]
- 1x battery simulation
  - source and sink current [MCP4725]
  - voltage and current measurement [INA219]
- 4x anlog inputs [ADS1115] (16-bit ADC)
- 4x analog ouput [MCP4728] (12-bit DAC)
  - amplified for a max of +24V
- 1x DDS output [AD9833]
  - amplified for a max of +-12V or +24V
  - variable offset [MCP4725]
- 2x CAN [MCP2515]
- 4x digital I/O with level shift
- 2x DPDT Relais
- 2x UART
- 1x ESP8266 header
- 1x SD Card
- 1x 32K EEPROM

## Production
The [Releases](https://github.com/Asyny/embeddedBridge_HW/releases) contain Gerber and Assembly files for [JLCPCB](https://jlcpcb.com/).
An HTML BOM which makes manual assembly easy, is included too.

## Contributing
If you want to contribute, please fork the repo and create a pull request. Thanks for your participation!