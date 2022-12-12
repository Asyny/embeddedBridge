#include "CommandHandling/InteractiveCommandHandler.h"

#include "hardware.h"
#include "HAL.h"

namespace SerialMenu {

    using namespace Menu;

    #define MAX_DEPTH 10
    #define DEBUG_SERIAL Serial1
    #define DEBUG_LEVEL DBG_VERBOSE	// DBG_NONE, DBG_ERROR, DBG_WARNING, DBG_INFO, DBG_DEBUG, DBG_VERBOSE

    
    const navCodesDef myCodes = {
        {noCmd,(char)0xff},
        {escCmd, 'a'},
        {enterCmd, 'd'},
        {downCmd, 'w'},
        {upCmd, 's'},
        {leftCmd, 'a'},    // doesn't do anything anyway...
        {rightCmd, 'd'},   // doesn't do anything anyway...
        {idxCmd, '?'},     // doesn't do anything anyway...
        {scrlUpCmd, 0x35}, // doesn't do anything anyway...
        {scrlUpCmd, 0x36}  // doesn't do anything anyway...
    };

    const char* constMEM alphaNum MEMMODE="0123456789";
    const char* constMEM alphaNumMask[] MEMMODE={alphaNum};

    static void relaisEvent() {
        HAL::getInstance()->setRelais1(DataModel::relais.enable1);
        HAL::getInstance()->setRelais2(DataModel::relais.enable2);
    }

    static void ainGainEvent() {
        switch(DataModel::ain.gain) {
            case 0: MENU_SERIAL.println("auto gain not implemented yet!"); break; // ToDo: add auto gain
            case 1: HAL::getInstance()->setAinGain(GAIN_TWOTHIRDS); break;
            case 2: HAL::getInstance()->setAinGain(GAIN_ONE); break;
            case 3: HAL::getInstance()->setAinGain(GAIN_TWO); break;
            case 4: HAL::getInstance()->setAinGain(GAIN_FOUR); break;
            case 5: HAL::getInstance()->setAinGain(GAIN_EIGHT); break;
            case 6: HAL::getInstance()->setAinGain(GAIN_SIXTEEN); break;
            default: MENU_SERIAL.print("GAIN ERROR "); MENU_SERIAL.println(DataModel::ain.gain); return;
        }
        Debug.print(DBG_INFO, "\n>>> gain set to = %d", DataModel::ain.gain);
    }

    static void setPsu() {
        HAL::getInstance()->configurePowerSupply(HAL::PsuType::VCC_RAIL, DataModel::vcc.value);
        HAL::getInstance()->configurePowerSupply(HAL::PsuType::VDC_RAIL, DataModel::vdc.value);
    }

    static void psuEvent() {
        HAL::getInstance()->setVccOutput(DataModel::vcc.enable);
        HAL::getInstance()->setVdcOutput(DataModel::vdc.enable);
    }

    static void dout1Event() { HAL::getInstance()->setDigitalOut(1, static_cast<HAL::DigOutMode>(DataModel::dout1.value)); }
    static void dout2Event() { HAL::getInstance()->setDigitalOut(2, static_cast<HAL::DigOutMode>(DataModel::dout2.value)); }
    static void dout3Event() { HAL::getInstance()->setDigitalOut(3, static_cast<HAL::DigOutMode>(DataModel::dout3.value)); }
    static void dout4Event() { HAL::getInstance()->setDigitalOut(4, static_cast<HAL::DigOutMode>(DataModel::dout4.value)); }

    static void foutEvent() {
        HAL* hal = HAL::getInstance();
        hal->setFreqOutMode(hal->getFreqOutModeMapping(DataModel::fout.mode));
        hal->setFreqOutFrequency(atoi(DataModel::fout.frequency), DataModel::fout.multiplier);
    }

    static void foutOffsetEvent() {
        HAL::getInstance()->setFreqOutOffset(DataModel::fout.offset);
    }

    static void batEvent() {
        // HAL::getInstance()->setBatSimVoltage(bat.value);
    }

    static void lshiftEvent() {
        HAL::getInstance()->setLevelShift1(DataModel::lshift.enable1);
        HAL::getInstance()->setLevelShift2(DataModel::lshift.enable2);
        HAL::getInstance()->setLevelShift3(DataModel::lshift.enable3);
        HAL::getInstance()->setLevelShift4(DataModel::lshift.enable4);
    }

    TOGGLE(DataModel::vcc.enable, vccEnMenu, "output ", psuEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::vdc.enable, vdcEnMenu, "output ", psuEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    char* const allowedDigits PROGMEM="0123456789";
    char* const voltageValidator[] PROGMEM = { allowedDigits, allowedDigits, allowedDigits, ".",
                                               allowedDigits, allowedDigits, allowedDigits, "V"};
    char* const currentValidator[] PROGMEM = { allowedDigits, allowedDigits, allowedDigits, ".",
                                               allowedDigits, allowedDigits, allowedDigits, "m", "A"};
    char* const powerValidator[] PROGMEM =   { allowedDigits, allowedDigits, allowedDigits, ".",
                                               allowedDigits, allowedDigits, allowedDigits, "m", "W"};
    MENU(vccMenu, "VCC", doNothing, noEvent, noStyle
        ,SUBMENU(vccEnMenu)
        ,FIELD(DataModel::vcc.value, "set", "V", 0.0, VCC_VDC_MAX_VOLTAGE, 1.0, 0.1, setPsu, exitEvent, noStyle)
        ,EDIT("voltage", DataModel::vcc.ina.voltage, voltageValidator, doNothing, noEvent, noStyle)
        ,EDIT("current", DataModel::vcc.ina.current, currentValidator, doNothing, noEvent, noStyle)
        ,EDIT("power", DataModel::vcc.ina.power, powerValidator, doNothing, noEvent, noStyle)
    );

    /*MENU(vdcMenu, "VDC", doNothing, noEvent, noStyle
        ,SUBMENU(vdcEnMenu)
        ,FIELD(vdc.value, "set", "V", 0.0,VCC_VDC_MAX_VOLTAGE,1.0,0.1, setPsu, exitEvent, noStyle)
        ,EDIT("voltage", vdc.ina.voltage, empty, doNothing, noEvent, noStyle)
        ,EDIT("current", vdc.ina.current, empty, doNothing, noEvent, noStyle)
        ,EDIT("power", vdc.ina.power, empty, doNothing, noEvent, noStyle)
    );*/

    SELECT(DataModel::dout1.value, dout1Menu, "Dout1", dout1Event, exitEvent, noStyle
        ,VALUE("VDC", 2, doNothing, noEvent)
        ,VALUE("VCC", 1, doNothing, noEvent)
        ,VALUE("Off", 0, doNothing, noEvent)
    );

    SELECT(DataModel::dout2.value, dout2Menu, "Dout2", dout2Event, exitEvent, noStyle
        ,VALUE("VDC", 2, doNothing, noEvent)
        ,VALUE("VCC", 1, doNothing, noEvent)
        ,VALUE("Off", 0, doNothing, noEvent)
    );

    SELECT(DataModel::dout3.value, dout3Menu, "Dout3", dout3Event, exitEvent, noStyle
        ,VALUE("VDC", 2, doNothing, noEvent)
        ,VALUE("VCC", 1, doNothing, noEvent)
        ,VALUE("Off", 0, doNothing, noEvent)
    );

    SELECT(DataModel::dout4.value, dout4Menu, "Dout4", dout4Event, exitEvent, noStyle
        ,VALUE("VDC", 2, doNothing, noEvent)
        ,VALUE("VCC", 1, doNothing, noEvent)
        ,VALUE("Off", 0, doNothing, noEvent)
    );

    MENU(doutMenu, "DOUT", doNothing, noEvent, noStyle
        ,SUBMENU(vccMenu)
        ,EDIT("vcc voltage", DataModel::vcc.ina.voltage, voltageValidator, doNothing, noEvent, noStyle)
        //	,SUBMENU(vdcMenu)
        //	,EDIT("vdc voltage", vdc.ina.voltage, empty, doNothing, noEvent, noStyle)
        ,SUBMENU(dout1Menu)
        ,SUBMENU(dout2Menu)
        ,SUBMENU(dout3Menu)
        ,SUBMENU(dout4Menu)
        ,EXIT("Back")
    );

    SELECT(DataModel::fout.mode, foutModeMenu, "Mode", foutEvent, exitEvent, noStyle
        ,VALUE("Off", 0, doNothing, noEvent)
        ,VALUE("Sine", 1, doNothing, noEvent)
        ,VALUE("Square1", 2, doNothing, noEvent)
        ,VALUE("Square2", 3, doNothing, noEvent)
        ,VALUE("Triangle", 4, doNothing, noEvent)
    );

    SELECT(DataModel::fout.multiplier, foutFreqMultipMenu, "Multiplier", foutEvent, exitEvent, noStyle
        ,VALUE("Hz",  0, doNothing, noEvent)
        ,VALUE("kHz", 1, doNothing, noEvent)
        ,VALUE("MHz", 2, doNothing, noEvent)
    );

    MENU(foutMenu, "FOUT", doNothing, noEvent, noStyle
        ,SUBMENU(foutModeMenu)
        ,EDIT("Frequency", DataModel::fout.frequency, alphaNumMask, foutEvent, exitEvent, noStyle)
        ,SUBMENU(foutFreqMultipMenu)
        ,FIELD(DataModel::fout.offset, "offset", "V", 0.0,12.0,1.0,0.1, foutOffsetEvent, exitEvent, noStyle)
        ,EXIT("Back")
    );

    MENU(batMenu, "Battery sim", doNothing, noEvent, noStyle
        ,FIELD(DataModel::bat.value, "set", "V", 0.0,3.6,1.0,0.1, batEvent, exitEvent, noStyle)
        ,EXIT("Back")
    );

    TOGGLE(DataModel::lshift.enable1, lshiftEn1Menu, "1 ", lshiftEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::lshift.enable2, lshiftEn2Menu, "2 ", lshiftEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::lshift.enable3, lshiftEn3Menu, "3 ", lshiftEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::lshift.enable4, lshiftEn4Menu, "4 ", lshiftEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    MENU(lshiftMenu, "Level Shift", doNothing, noEvent, noStyle
        ,SUBMENU(lshiftEn1Menu)
        ,SUBMENU(lshiftEn2Menu)
        ,SUBMENU(lshiftEn3Menu)
        ,SUBMENU(lshiftEn4Menu)
        ,EXIT("Back")
    );

    TOGGLE(DataModel::relais.enable1, relaisEn1Menu, "1 ", relaisEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::relais.enable2, relaisEn2Menu, "2 ", relaisEvent, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    MENU(relaisMenu, "Relais", doNothing, noEvent, noStyle
        ,SUBMENU(relaisEn1Menu)
        ,SUBMENU(relaisEn2Menu)
        ,EXIT("Back")
    );

    SELECT(DataModel::ain.gain, ainGainMenu, "gain", ainGainEvent, exitEvent, noStyle
        ,VALUE("auto", 0, doNothing, noEvent)
        ,VALUE("2/3", 1, doNothing, noEvent)
        ,VALUE("1", 2, doNothing, noEvent)
        ,VALUE("2", 3, doNothing, noEvent)
        ,VALUE("4", 4, doNothing, noEvent)
        ,VALUE("8", 5, doNothing, noEvent)
        ,VALUE("16", 6, doNothing, noEvent)
    );

    MENU(ainMenu, "AIN", doNothing, noEvent, noStyle
        ,SUBMENU(ainGainMenu)
        ,EDIT("AIN1", DataModel::ain.adc1_value, voltageValidator, doNothing, noEvent, noStyle)
        ,EDIT("AIN2", DataModel::ain.adc2_value, voltageValidator, doNothing, noEvent, noStyle)
        ,EDIT("AIN3", DataModel::ain.adc3_value, voltageValidator, doNothing, noEvent, noStyle)
        ,EDIT("AIN4", DataModel::ain.adc4_value, voltageValidator, doNothing, noEvent, noStyle)
        ,EXIT("Back")
    );

    MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
        ,SUBMENU(doutMenu)
        ,SUBMENU(lshiftMenu)
        ,SUBMENU(ainMenu) 
        ,SUBMENU(foutMenu)
        ,SUBMENU(batMenu)
        ,SUBMENU(relaisMenu) 
        // ToDo: calibration for AIN
        ,EXIT("EXIT")
    );

    config myOptions('>', '-', myCodes, true);
    serialIn serial(MENU_SERIAL);
    MENU_OUTPUTS(out,MAX_DEPTH,SERIAL_OUT(MENU_SERIAL),NONE);
    NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);
 }


SerialMenu::InteractiveCommandHandler::InteractiveCommandHandler(void) : CommandHandler()
{
}


SerialMenu::InteractiveCommandHandler::~InteractiveCommandHandler(void) {
    
}


void SerialMenu::InteractiveCommandHandler::setup(void) {
    options = &myOptions;

    ainMenu[1].disable();
    ainMenu[2].disable();
    ainMenu[3].disable();
    ainMenu[4].disable();

    doutMenu[1].disable();	// shows current output voltage of vcc
	//doutMenu[3].disable();	// shows current output voltage of vdc

    vccMenu[2].disable();	// shows current output voltage of vcc
	vccMenu[3].disable();	// shows current output current of vcc
	vccMenu[4].disable();	// shows current output power of vcc

    // vdcMenu[2].disable();	// shows current output voltage of vdc
	// vdcMenu[3].disable();	// shows current output current of vdc
	// vdcMenu[4].disable();	// shows current output power of vdc
}


void SerialMenu::InteractiveCommandHandler::process(void) {
    nav.poll();
}