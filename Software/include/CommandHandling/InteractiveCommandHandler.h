#ifndef INTERACTIVE_COMMAND_HANDLER_H
#define INTERACTIVE_COMMAND_HANDLER_H

#include "CommandHandler.h"

#include <Arduino_DebugUtils.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

#include "hardware.h"
#include "HAL.h"


namespace SerialMenu {

    using namespace Menu;

    #define MAX_DEPTH 10
    #define DEBUG_SERIAL Serial1
    #define DEBUG_LEVEL DBG_VERBOSE	// DBG_NONE, DBG_ERROR, DBG_WARNING, DBG_INFO, DBG_DEBUG, DBG_VERBOSE

    
    static const navCodesDef myCodes = {
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

    static config myOptions('>', '-', myCodes, true);
    static serialIn serial(MENU_SERIAL);
    static navRoot* navInstance;

    static struct relaisSettings {
        bool enable1 = false;
        bool enable2 = false;
    } relais;

    struct ainSettingsSpecific {
        int16_t adc_value;
        char adc_value_string[8];	// xx.xxxV\n
    };

    static struct ainSettings {
        uint8_t gain = 0;
        uint8_t mode = 0;
        struct ainSettingsSpecific ain1;
        struct ainSettingsSpecific ain2;
        struct ainSettingsSpecific ain3;
        struct ainSettingsSpecific ain4;
    } ain;

    static const char *constMEM empty[] MEMMODE = {};

    struct inaValues {
        char* voltage = new char[7];	// xx.xxV\n
        char* current = new char[10];	// xxxx.xxmA\n
        char* power = new char[10];		// xxxx.xxmW\n
    };

    struct vccSettings {
        struct inaValues ina;
        float value = 0;
        bool enable = false;
    };
    static struct vccSettings vcc;
    static struct vccSettings vdc;

    // ToDo: add INA219
    struct doutSettings {
        uint8_t value = 0;
    };

    static struct doutSettings dout1;
    static struct doutSettings dout2;
    static struct doutSettings dout3;
    static struct doutSettings dout4;

    static struct foutSettings {
        char frequency[50]="1000";
        float offset = 0.0;
        uint8_t mode = 0;
        uint8_t multiplier = 0;
    } fout;

    static const char* constMEM alphaNum MEMMODE="0123456789";
    static const char* constMEM alphaNumMask[] MEMMODE={alphaNum};

    static struct batSettings {
        float value = 0.0;
        struct inaValues ina;
    } bat;

    static struct lshiftSettings {
        bool enable1 = false;
        bool enable2 = false;
        bool enable3 = false;
        bool enable4 = false;
    } lshift;

    static void relaisEvent() {
        HAL::getInstance()->setRelais1(relais.enable1);
        HAL::getInstance()->setRelais2(relais.enable2);
    }

    static void ainGainEvent() {
        switch(ain.gain) {
            case 0: MENU_SERIAL.println("auto gain not implemented yet!"); break; // ToDo: add auto gain
            case 1: HAL::getInstance()->setAinGain(GAIN_TWOTHIRDS); break;
            case 2: HAL::getInstance()->setAinGain(GAIN_ONE); break;
            case 3: HAL::getInstance()->setAinGain(GAIN_TWO); break;
            case 4: HAL::getInstance()->setAinGain(GAIN_FOUR); break;
            case 5: HAL::getInstance()->setAinGain(GAIN_EIGHT); break;
            case 6: HAL::getInstance()->setAinGain(GAIN_SIXTEEN); break;
            default: MENU_SERIAL.print("GAIN ERROR "); MENU_SERIAL.println(ain.gain); return;
        }
        Debug.print(DBG_INFO, "\n>>> gain set to = %d", ain.gain);
    }

    static void setPsu() {
        HAL::getInstance()->configurePowerSupply(HAL::PsuType::VCC_RAIL, vcc.value);
        HAL::getInstance()->configurePowerSupply(HAL::PsuType::VDC_RAIL, vdc.value);
    }

    static void psuEvent() {
        HAL::getInstance()->setVccOutput(vcc.enable);
        HAL::getInstance()->setVdcOutput(vdc.enable);
    }

    static void dout1Event() { HAL::getInstance()->setDigitalOut(1, static_cast<HAL::DigOutMode>(dout1.value)); }
    static void dout2Event() { HAL::getInstance()->setDigitalOut(2, static_cast<HAL::DigOutMode>(dout2.value)); }
    static void dout3Event() { HAL::getInstance()->setDigitalOut(3, static_cast<HAL::DigOutMode>(dout3.value)); }
    static void dout4Event() { HAL::getInstance()->setDigitalOut(4, static_cast<HAL::DigOutMode>(dout4.value)); }

    static void foutEvent() {
        HAL* hal = HAL::getInstance();
        hal->setFreqOutMode(hal->getFreqOutModeMapping(fout.mode));
        hal->setFreqOutFrequency(atoi(fout.frequency), fout.multiplier);
    }

    static void foutOffsetEvent() {
        HAL::getInstance()->setFreqOutOffset(fout.offset);
    }

    static void batEvent() {
        // HAL::getInstance()->setBatSimVoltage(bat.value);
    }

    static void lshiftEvent() {
        HAL::getInstance()->setLevelShift1(lshift.enable1);
        HAL::getInstance()->setLevelShift2(lshift.enable2);
        HAL::getInstance()->setLevelShift3(lshift.enable3);
        HAL::getInstance()->setLevelShift4(lshift.enable4);
    }

    static void initialize() {

        TOGGLE(vcc.enable, vccEnMenu, "output ", psuEvent, enterEvent, wrapStyle
            ,VALUE("on", HIGH, doNothing, noEvent)
            ,VALUE("off", LOW, doNothing, noEvent)
        );

        TOGGLE(vdc.enable, vdcEnMenu, "output ", psuEvent, enterEvent, wrapStyle
            ,VALUE("on", HIGH, doNothing, noEvent)
            ,VALUE("off", LOW, doNothing, noEvent)
        );

        MENU(vccMenu, "VCC", doNothing, noEvent, noStyle
            ,SUBMENU(vccEnMenu)
            ,FIELD(vcc.value, "set", "V", 0.0,VCC_VDC_MAX_VOLTAGE,1.0,0.1, setPsu, exitEvent, noStyle)
            ,EDIT("voltage", vcc.ina.voltage, empty, doNothing, noEvent, noStyle)
            ,EDIT("current", vcc.ina.current, empty, doNothing, noEvent, noStyle)
            ,EDIT("power", vcc.ina.power, empty, doNothing, noEvent, noStyle)
        );

        vccMenu[2].disable();	// shows current output voltage of vcc
	    vccMenu[3].disable();	// shows current output current of vcc
	    vccMenu[4].disable();	// shows current output power of vcc

        /*MENU(vdcMenu, "VDC", doNothing, noEvent, noStyle
            ,SUBMENU(vdcEnMenu)
            ,FIELD(vdc.value, "set", "V", 0.0,VCC_VDC_MAX_VOLTAGE,1.0,0.1, setPsu, exitEvent, noStyle)
            ,EDIT("voltage", vdc.ina.voltage, empty, doNothing, noEvent, noStyle)
            ,EDIT("current", vdc.ina.current, empty, doNothing, noEvent, noStyle)
            ,EDIT("power", vdc.ina.power, empty, doNothing, noEvent, noStyle)
        );*/

        SELECT(dout1.value, dout1Menu, "Dout1", dout1Event, exitEvent, noStyle
            ,VALUE("VDC", 2, doNothing, noEvent)
            ,VALUE("VCC", 1, doNothing, noEvent)
            ,VALUE("Off", 0, doNothing, noEvent)
        );

        SELECT(dout2.value, dout2Menu, "Dout2", dout2Event, exitEvent, noStyle
            ,VALUE("VDC", 2, doNothing, noEvent)
            ,VALUE("VCC", 1, doNothing, noEvent)
            ,VALUE("Off", 0, doNothing, noEvent)
        );

        SELECT(dout3.value, dout3Menu, "Dout3", dout3Event, exitEvent, noStyle
            ,VALUE("VDC", 2, doNothing, noEvent)
            ,VALUE("VCC", 1, doNothing, noEvent)
            ,VALUE("Off", 0, doNothing, noEvent)
        );

        SELECT(dout4.value, dout4Menu, "Dout4", dout4Event, exitEvent, noStyle
            ,VALUE("VDC", 2, doNothing, noEvent)
            ,VALUE("VCC", 1, doNothing, noEvent)
            ,VALUE("Off", 0, doNothing, noEvent)
        );

        MENU(doutMenu, "DOUT", doNothing, noEvent, noStyle
            ,SUBMENU(vccMenu)
            ,EDIT("vcc voltage", vcc.ina.voltage, empty, doNothing, noEvent, noStyle)
        //	,SUBMENU(vdcMenu)
        //	,EDIT("vdc voltage", vdc.ina.voltage, empty, doNothing, noEvent, noStyle)
            ,SUBMENU(dout1Menu)
            ,SUBMENU(dout2Menu)
            ,SUBMENU(dout3Menu)
            ,SUBMENU(dout4Menu)
            ,EXIT("Back")
        );

        doutMenu[1].disable();	// shows current output voltage of vcc
	    //doutMenu[3].disable();	// shows current output voltage of vdc

        SELECT(fout.mode, foutModeMenu, "Mode", foutEvent, exitEvent, noStyle
            ,VALUE("Off", 0, doNothing, noEvent)
            ,VALUE("Sine", 1, doNothing, noEvent)
            ,VALUE("Square1", 2, doNothing, noEvent)
            ,VALUE("Square2", 3, doNothing, noEvent)
            ,VALUE("Triangle", 4, doNothing, noEvent)
        );

        SELECT(fout.multiplier, foutFreqMultipMenu, "Multiplier", foutEvent, exitEvent, noStyle
            ,VALUE("Hz",  0, doNothing, noEvent)
            ,VALUE("kHz", 1, doNothing, noEvent)
            ,VALUE("MHz", 2, doNothing, noEvent)
        );

        MENU(foutMenu, "FOUT", doNothing, noEvent, noStyle
            ,SUBMENU(foutModeMenu)
            ,EDIT("Frequency", fout.frequency, alphaNumMask, foutEvent, exitEvent, noStyle)
            ,SUBMENU(foutFreqMultipMenu)
            ,FIELD(fout.offset, "offset", "V", 0.0,12.0,1.0,0.1, foutOffsetEvent, exitEvent, noStyle)
            ,EXIT("Back")
        );

        MENU(batMenu, "Battery sim", doNothing, noEvent, noStyle
            ,FIELD(bat.value, "set", "V", 0.0,3.6,1.0,0.1, batEvent, exitEvent, noStyle)
            ,EXIT("Back")
        );

        TOGGLE(lshift.enable1, lshiftEn1Menu, "1 ", lshiftEvent, enterEvent, wrapStyle
            ,VALUE("on", HIGH, doNothing, noEvent)
            ,VALUE("off", LOW, doNothing, noEvent)
        );

        TOGGLE(lshift.enable2, lshiftEn2Menu, "2 ", lshiftEvent, enterEvent, wrapStyle
            ,VALUE("on", HIGH, doNothing, noEvent)
            ,VALUE("off", LOW, doNothing, noEvent)
        );

        TOGGLE(lshift.enable3, lshiftEn3Menu, "3 ", lshiftEvent, enterEvent, wrapStyle
            ,VALUE("on", HIGH, doNothing, noEvent)
            ,VALUE("off", LOW, doNothing, noEvent)
        );

        TOGGLE(lshift.enable4, lshiftEn4Menu, "4 ", lshiftEvent, enterEvent, wrapStyle
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

        TOGGLE(relais.enable1, relaisEn1Menu, "1 ", relaisEvent, enterEvent, wrapStyle
            ,VALUE("on", HIGH, doNothing, noEvent)
            ,VALUE("off", LOW, doNothing, noEvent)
        );

        TOGGLE(relais.enable2, relaisEn2Menu, "2 ", relaisEvent, enterEvent, wrapStyle
            ,VALUE("on", HIGH, doNothing, noEvent)
            ,VALUE("off", LOW, doNothing, noEvent)
        );

        MENU(relaisMenu, "Relais", doNothing, noEvent, noStyle
            ,SUBMENU(relaisEn1Menu)
            ,SUBMENU(relaisEn2Menu)
            ,EXIT("Back")
        );

        SELECT(ain.gain, ainGainMenu, "gain", ainGainEvent, exitEvent, noStyle
            ,VALUE("auto", 0, doNothing, noEvent)
            ,VALUE("2/3", 1, doNothing, noEvent)
            ,VALUE("1", 2, doNothing, noEvent)
            ,VALUE("2", 3, doNothing, noEvent)
            ,VALUE("4", 4, doNothing, noEvent)
            ,VALUE("8", 5, doNothing, noEvent)
            ,VALUE("16", 6, doNothing, noEvent)
        );

        const char *constMEM empty[] MEMMODE = {};
        MENU(ainMenu, "AIN", doNothing, noEvent, noStyle
            ,SUBMENU(ainGainMenu)
            ,EDIT("AIN1", ain.ain1.adc_value_string, empty, doNothing, noEvent, noStyle)
            ,EDIT("AIN2", ain.ain2.adc_value_string, empty, doNothing, noEvent, noStyle)
            ,EDIT("AIN3", ain.ain3.adc_value_string, empty, doNothing, noEvent, noStyle)
            ,EDIT("AIN4", ain.ain4.adc_value_string, empty, doNothing, noEvent, noStyle)
            ,EXIT("Back")
        );

        ainMenu[1].disable();
        ainMenu[2].disable();
        ainMenu[3].disable();
        ainMenu[4].disable();

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

        // Expanded MENU_OUTPUTS + NAVROOT macro
        XFOR_EACH(CALL, WITH(VAR, out, MAX_DEPTH), ON(SERIAL_OUT, 10006, MENU_SERIAL), NONE) 
        static menuOut* constMEM out_outPtrs[] MEMMODE = 
            { XFOR_EACH(CALL, WITH(REF, out, MAX_DEPTH), ON(SERIAL_OUT, 10006, MENU_SERIAL), NONE) };
        static outputsList out(out_outPtrs, sizeof(out_outPtrs) / sizeof(menuOut*));
        static navNode nav_path[MAX_DEPTH]; 
        static navRoot nav(mainMenu, nav_path, MAX_DEPTH, serial, out);
        navInstance = &nav; 
    }


    class InteractiveCommandHandler : public CommandHandler {

    public:
        InteractiveCommandHandler(void); 
        virtual ~InteractiveCommandHandler(void);

        virtual void setup(void) override;
        virtual void process(void) override;
    };
}

#endif // INTERACTIVE_COMMAND_HANDLER_H