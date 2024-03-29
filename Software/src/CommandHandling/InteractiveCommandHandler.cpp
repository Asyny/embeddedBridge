#include "CommandHandling/InteractiveCommandHandler.h"

#include "hardware.h"

namespace SerialMenu {

    using namespace Menu;

    #define MAX_DEPTH 10

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

    TOGGLE(DataModel::vcc.enable, vccEnMenu, "output ", doNothing, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::vdc.enable, vdcEnMenu, "output ", doNothing, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    char* const allowedDigits PROGMEM = strdup("0123456789");
    char* const voltageValidator[] PROGMEM = { allowedDigits, allowedDigits, allowedDigits, strdup("."),
                                               allowedDigits, allowedDigits, allowedDigits, strdup("V")};
    char* const currentValidator[] PROGMEM = { allowedDigits, allowedDigits, allowedDigits, strdup("."),
                                               allowedDigits, allowedDigits, allowedDigits, strdup("m"), strdup("A")};
    char* const powerValidator[] PROGMEM =   { allowedDigits, allowedDigits, allowedDigits, strdup("."),
                                               allowedDigits, allowedDigits, allowedDigits, strdup("m"), strdup("W")};
    MENU(vccMenu, "VCC", doNothing, noEvent, noStyle
        ,SUBMENU(vccEnMenu)
        ,FIELD(DataModel::vcc.value, "set", "V", 0.0, VCC_VDC_MAX_VOLTAGE, 1.0, 0.1, doNothing, exitEvent, noStyle)
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

    SELECT(DataModel::dout1.value, dout1Menu, "Dout1", doNothing, exitEvent, noStyle
        ,VALUE("VDC", 2, doNothing, noEvent)
        ,VALUE("VCC", 1, doNothing, noEvent)
        ,VALUE("Off", 0, doNothing, noEvent)
    );

    SELECT(DataModel::dout2.value, dout2Menu, "Dout2", doNothing, exitEvent, noStyle
        ,VALUE("VDC", 2, doNothing, noEvent)
        ,VALUE("VCC", 1, doNothing, noEvent)
        ,VALUE("Off", 0, doNothing, noEvent)
    );

    SELECT(DataModel::dout3.value, dout3Menu, "Dout3", doNothing, exitEvent, noStyle
        ,VALUE("VDC", 2, doNothing, noEvent)
        ,VALUE("VCC", 1, doNothing, noEvent)
        ,VALUE("Off", 0, doNothing, noEvent)
    );

    SELECT(DataModel::dout4.value, dout4Menu, "Dout4", doNothing, exitEvent, noStyle
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

    SELECT(DataModel::fout.mode, foutModeMenu, "Mode", doNothing, exitEvent, noStyle
        ,VALUE("Off", 0, doNothing, noEvent)
        ,VALUE("Sine", 1, doNothing, noEvent)
        ,VALUE("Square1", 2, doNothing, noEvent)
        ,VALUE("Square2", 3, doNothing, noEvent)
        ,VALUE("Triangle", 4, doNothing, noEvent)
    );

    SELECT(DataModel::fout.multiplier, foutFreqMultipMenu, "Multiplier", doNothing, exitEvent, noStyle
        ,VALUE("Hz",  0, doNothing, noEvent)
        ,VALUE("kHz", 1, doNothing, noEvent)
        ,VALUE("MHz", 2, doNothing, noEvent)
    );

    MENU(foutMenu, "FOUT", doNothing, noEvent, noStyle
        ,SUBMENU(foutModeMenu)
        ,EDIT("Frequency", DataModel::fout.frequency, alphaNumMask, doNothing, exitEvent, noStyle)
        ,SUBMENU(foutFreqMultipMenu)
        ,FIELD(DataModel::fout.offset, "offset", "V", 0.0,12.0,1.0,0.1, doNothing, exitEvent, noStyle)
        ,EXIT("Back")
    );

    MENU(batMenu, "Battery sim", doNothing, noEvent, noStyle
        ,FIELD(DataModel::bat.value, "set", "V", 0.0,3.6,1.0,0.1, doNothing, exitEvent, noStyle)
        ,EXIT("Back")
    );

    TOGGLE(DataModel::lshift.enable1, lshiftEn1Menu, "1 ", doNothing, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::lshift.enable2, lshiftEn2Menu, "2 ", doNothing, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::lshift.enable3, lshiftEn3Menu, "3 ", doNothing, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::lshift.enable4, lshiftEn4Menu, "4 ", doNothing, enterEvent, wrapStyle
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

    TOGGLE(DataModel::relais.enable1, relaisEn1Menu, "1 ", doNothing, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    TOGGLE(DataModel::relais.enable2, relaisEn2Menu, "2 ", doNothing, enterEvent, wrapStyle
        ,VALUE("on", HIGH, doNothing, noEvent)
        ,VALUE("off", LOW, doNothing, noEvent)
    );

    MENU(relaisMenu, "Relais", doNothing, noEvent, noStyle
        ,SUBMENU(relaisEn1Menu)
        ,SUBMENU(relaisEn2Menu)
        ,EXIT("Back")
    );

    SELECT(DataModel::ain.gain, ainGainMenu, "gain", doNothing, exitEvent, noStyle
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

    SELECT(DataModel::newMode, commandInputModeMenu, "SCPI Mode", doNothing, exitEvent, noStyle
        ,VALUE("Yes",  DataModel::CommandHandlerInputMode::SCPI, doNothing, noEvent)
        ,VALUE("Idle", DataModel::CommandHandlerInputMode::IDLE, doNothing, noEvent)
        ,VALUE("No", DataModel::CommandHandlerInputMode::INTERACTIVE, doNothing, noEvent)
    );

    MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
        ,SUBMENU(doutMenu)
        ,SUBMENU(lshiftMenu)
        ,SUBMENU(ainMenu) 
        ,SUBMENU(foutMenu)
        ,SUBMENU(batMenu)
        ,SUBMENU(relaisMenu) 
        ,SUBMENU(commandInputModeMenu)
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


void SerialMenu::InteractiveCommandHandler::doSetup(void) {
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


void SerialMenu::InteractiveCommandHandler::doProcess(void) {
    nav.poll();
}