#include <Arduino_DebugUtils.h>

#include "CommandHandling/ScpiCommandHandler.h"

#include "hardware.h"

ScpiCommandHandler::ScpiCommandHandler(void) : CommandHandler() {

}


ScpiCommandHandler::~ScpiCommandHandler(void) {
    
}


void ScpiCommandHandler::doSetup(void) {
    Debug.print(DBG_INFO, "\n>>> ScpiCommandHandler::setup");
    // first read from serial interface always contains "\374" at the beginning
    // clearing this content by reading serial interface
    String input = MENU_SERIAL.readString();
}


void ScpiCommandHandler::doProcess(void) {
    if (MENU_SERIAL.available() > 0)
    {
        commandLine += char(MENU_SERIAL.read());
        uint32_t length = commandLine.length();

        if ((commandLine[length - 2] == '\r') &&
            (commandLine[length - 1] == '\n')) {
            commandLine[length - 2] = '\0';
            commandLine[length - 1] = '\0';
            commandProcessing();
            commandLine = "";
        }
    }
}


void ScpiCommandHandler::commandProcessing(void)
{
    if (commandLine.equals("SYStem:COMmand:INput MENU")) {
        DataModel::newMode = DataModel::CommandHandlerInputMode::INTERACTIVE;
    }

    bool done = relaisCommandProcessing();
    if (!done) {
        done = levelShiftCommandProcessing();
    }
    if (!done) {
        done = adcCommandProcessing();
    }
    if (!done) {
        done = batSimCommandProcessing();
    }
    if (!done) {
        done = fOutCommandProcessing();
    }
    if (!done) {
        done = dOutCommandProcessing();
    }
}


bool ScpiCommandHandler::relaisCommandProcessing(void) {
    bool done = false;

    if (commandLine.equals("RELais:A:ACTivate")) {
        DataModel::relais.enable1 = true;
        done = true;
    }
    else if (commandLine.equals("RELais:A:DEACTivate")) {
        DataModel::relais.enable1 = false;
        done = true;
    }
    else if (commandLine.equals("RELais:A?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::relais.enable1);
        done = true;
    }
    else if (commandLine.equals("RELais:B:ACTivate")) {
        DataModel::relais.enable2 = true;
        done = true;
    }
    else if (commandLine.equals("RELais:B:DEACTivate")) {
        DataModel::relais.enable2 = false;
        done = true;
    }
    else if (commandLine.equals("RELais:B?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::relais.enable2);
        done = true;
    }

    return done;
}


bool ScpiCommandHandler::levelShiftCommandProcessing(void) {
    bool done = false;
    
    if (commandLine.equals("LEVelSHift:A:ACTivate")) {
        DataModel::lshift.enable1 = true;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:A:DEACTivate")) {
        DataModel::lshift.enable1 = false;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:A?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable1);
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:B:ACTivate")) {
        DataModel::lshift.enable2 = true;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:B:DEACTivate")) {
        DataModel::lshift.enable2 = false;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:B?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable2);
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:C:ACTivate")) {
        DataModel::lshift.enable3 = true;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:C:DEACTivate")) {
        DataModel::lshift.enable3 = false;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:C?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable3);
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:D:ACTivate")) {
        DataModel::lshift.enable4 = true;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:D:DEACTivate")) {
        DataModel::lshift.enable4 = false;
        done = true;
    }
    else if (commandLine.equals("LEVelSHift:D?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable4);
        done = true;
    }

    return done;
}


bool ScpiCommandHandler::adcCommandProcessing(void) {
    bool done = false;
    
    if (commandLine.equals("AnalogIN:A?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc1_value);
        done = true;
    }
    else if (commandLine.equals("AnalogIN:B?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc2_value);
        done = true;
    }
    else if (commandLine.equals("AnalogIN:C?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc3_value);
        done = true;
    }
    else if (commandLine.equals("AnalogIN:D?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc4_value);
        done = true;
    }
    else if (commandLine.equals("AnalogIN:GAIN?")) {
        switch (DataModel::ain.gain) {
            case 0:
                MENU_SERIAL.print("auto\r\n");
                break;
            
            case 1:
                MENU_SERIAL.print("2/3\r\n");
                break;

            case 2:
                MENU_SERIAL.print("1\r\n");
                break;

            case 3:
                MENU_SERIAL.print("2\r\n");
                break;

            case 4:
                MENU_SERIAL.print("4\r\n");
                break;

            case 5:
                MENU_SERIAL.print("8\r\n");
                break;

            case 6:
                MENU_SERIAL.print("16\r\n");
                break;
            
            default:
                MENU_SERIAL.print("error\r\n");
                break;
        }
        done = true;
    } 
    // TODO: Check better parameter format. Using string representing numbers an integers itself
    // may confuse the user! 
    else if (commandLine.startsWith("AnalogIN:GAIN ")) {
        String parameter = commandLine;
        parameter.replace("AnalogIN:GAIN ", "");
        if (parameter.length() != 0) {
            uint8_t numericAdcGain = parameter.toInt();
            Debug.print(DBG_INFO, "\n>>> ADC gain received via SCPI interface is: %d", numericAdcGain);
            DataModel::ain.gain = numericAdcGain;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }

    return done;
}


bool ScpiCommandHandler::batSimCommandProcessing(void) {
    bool done = false;
    
    if (commandLine.equals("BATterySIMulation:VOLTage?")) {
        MENU_SERIAL.printf("%f\r\n", DataModel::bat.value);
        done = true;
    } 
    else if (commandLine.equals("BATterySIMulation:INA:VOLTage?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::bat.ina.voltage);
        done = true;
    }
    else if (commandLine.equals("BATterySIMulation:INA:CURrent?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::bat.ina.current);
        done = true;
    }
    else if (commandLine.equals("BATterySIMulation:INA:POWer?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::bat.ina.power);
        done = true;
    }
    else if (commandLine.startsWith("BATterySIMulation:VOLTage ")) {
        String parameter = commandLine;
        parameter.replace("BATterySIMulation:VOLTage ", "");
        if (parameter.length() != 0) {
            float_t batteryVoltage = parameter.toFloat();
            Debug.print(DBG_INFO, "\n>>> Battery nominal voltage received via SCPI interface is: %f", batteryVoltage);
            DataModel::bat.value = batteryVoltage;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }

    return done;
}


bool ScpiCommandHandler::fOutCommandProcessing(void) {
    bool done = false;
    
    if (commandLine.equals("FREQuencyOUTput:FREQuency?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::fout.frequency);
        done = true;
    } 
    else if (commandLine.equals("FREQuencyOUTput:MODE?")) {
        switch (DataModel::fout.mode) {
            case 0:
                MENU_SERIAL.print("off\r\n");
                break;
            
            case 1:
                MENU_SERIAL.print("sine\r\n");
                break;

            case 2:
                MENU_SERIAL.print("square1\r\n");
                break;

            case 3:
                MENU_SERIAL.print("square2\r\n");
                break;

            case 4:
                MENU_SERIAL.print("Triangle\r\n");
                break;
            
            default:
                MENU_SERIAL.print("error\r\n");
                break;
        }
        done = true;
    } 
    else if (commandLine.equals("FREQuencyOUTput:MUTLiplier?")) {
        switch (DataModel::fout.multiplier) {
            case 0:
                MENU_SERIAL.print("Hz\r\n");
                break;
            
            case 1:
                MENU_SERIAL.print("kHz\r\n");
                break;

            case 2:
                MENU_SERIAL.print("MHz\r\n");
                break;

            default:
                MENU_SERIAL.print("error\r\n");
                break;
        }
        MENU_SERIAL.printf("%s\r\n", DataModel::fout.multiplier);
        done = true;
    } 
    else if (commandLine.equals("FREQuencyOUTput:OFFSET?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::fout.offset);
        done = true;
    }
    else if (commandLine.startsWith("FREQuencyOUTput:FREQuency ")) {
        String parameter = commandLine;
        parameter.replace("FREQuencyOUTput:FREQuency ", "");
        if (parameter.length() != 0) {
            Debug.print(DBG_INFO, "\n>>> Frequency received via SCPI interface is: %s", parameter);
            strncpy(DataModel::fout.frequency, parameter.c_str(), 
                parameter.length() < sizeof(DataModel::fout.frequency) ? parameter.length() : 
                    sizeof(DataModel::fout.frequency));
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("FREQuencyOUTput:MODE ")) {
        String parameter = commandLine;
        parameter.replace("FREQuencyOUTput:MODE ", "");
        if (parameter.length() != 0) {
            uint8_t mode = parameter.toInt();
            Debug.print(DBG_INFO, "\n>>> Frequency mode received via SCPI interface is: %d", mode);
            DataModel::fout.mode = mode;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("FREQuencyOUTput:MUTLiplier ")) {
        String parameter = commandLine;
        parameter.replace("FREQuencyOUTput:MUTLiplier ", "");
        if (parameter.length() != 0) {
            uint8_t multiplier = parameter.toInt();
            Debug.print(DBG_INFO, "\n>>> Frequency multiplier received via SCPI interface is: %d", multiplier);
            DataModel::fout.multiplier = multiplier;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("FREQuencyOUTput:OFFSET ")) {
        String parameter = commandLine;
        parameter.replace("FREQuencyOUTput:OFFSET ", "");
        if (parameter.length() != 0) {
            float_t offset = parameter.toFloat();
            Debug.print(DBG_INFO, "\n>>> Frequency offset received via SCPI interface is: %f", offset);
            DataModel::fout.offset = offset;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }

    return done;
}


bool ScpiCommandHandler::dOutCommandProcessing(void) {
    bool done = false;

    if (commandLine.equals("DIGitalOUTput:VCC?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::vcc.enable);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VCC:VOLTage?")) {
        MENU_SERIAL.printf("%f\r\n", DataModel::vcc.value);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VCC:INA:VOLTage?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::vcc.ina.voltage);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VCC:INA:CURrent?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::vcc.ina.current);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VCC:INA:POWer?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::vcc.ina.power);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VDC?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::vcc.enable);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VDC:VOLTage?")) {
        MENU_SERIAL.printf("%f\r\n", DataModel::vdc.value);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VDC:INA:VOLTage?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::vdc.ina.voltage);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VDC:INA:CURrent?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::vdc.ina.current);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VDC:INA:POWer?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::vdc.ina.power);
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:A:Source?")) {
        switch (DataModel::dout1.value) {
            case 0:
                MENU_SERIAL.print("off\r\n");
                break;
            
            case 1:
                MENU_SERIAL.print("vcc\r\n");
                break;

            case 2:
                MENU_SERIAL.print("vdd\r\n");
                break;

            default:
                MENU_SERIAL.print("error\r\n");
                break;
        }
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:B:Source?")) {
        switch (DataModel::dout1.value) {
            case 0:
                MENU_SERIAL.print("off\r\n");
                break;
            
            case 1:
                MENU_SERIAL.print("vcc\r\n");
                break;

            case 2:
                MENU_SERIAL.print("vdd\r\n");
                break;

            default:
                MENU_SERIAL.print("error\r\n");
                break;
        }
        done = true;
    } 
    else if (commandLine.equals("DIGitalOUTput:C:Source?")) {
        switch (DataModel::dout1.value) {
            case 0:
                MENU_SERIAL.print("off\r\n");
                break;
            
            case 1:
                MENU_SERIAL.print("vcc\r\n");
                break;

            case 2:
                MENU_SERIAL.print("vdd\r\n");
                break;

            default:
                MENU_SERIAL.print("error\r\n");
                break;
        }
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:D:Source?")) {
        switch (DataModel::dout1.value) {
            case 0:
                MENU_SERIAL.print("off\r\n");
                break;
            
            case 1:
                MENU_SERIAL.print("vcc\r\n");
                break;

            case 2:
                MENU_SERIAL.print("vdd\r\n");
                break;

            default:
                MENU_SERIAL.print("error\r\n");
                break;
        }
        done = true;
    } 
    else if (commandLine.equals("DIGitalOUTput:VCC:ACTivate")) {
        DataModel::vcc.enable = true;
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VCC:DEACTivate")) {
        DataModel::vcc.enable = false;
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VDC:ACTivate")) {
        DataModel::vdc.enable = true;
        done = true;
    }
    else if (commandLine.equals("DIGitalOUTput:VDC:DEACTivate")) {
        DataModel::vdc.enable = false;
        done = true;
    }
    else if (commandLine.startsWith("DIGitalOUTput:A:Source ")) {
        String parameter = commandLine;
        parameter.replace("DIGitalOUTput:A:Source ", "");
        if (parameter.length() != 0) {
            uint8_t source = parameter.toInt();
            Debug.print(DBG_INFO, "\n>>> Digital output 1 source received via SCPI interface is: %d", source);
            DataModel::dout1.value = source;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("DIGitalOUTput:B:Source ")) {
        String parameter = commandLine;
        parameter.replace("DIGitalOUTput:B:Source ", "");
        if (parameter.length() != 0) {
            uint8_t source = parameter.toInt();
            Debug.print(DBG_INFO, "\n>>> Digital output 2 source received via SCPI interface is: %d", source);
            DataModel::dout2.value = source;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("DIGitalOUTput:C:Source ")) {
        String parameter = commandLine;
        parameter.replace("DIGitalOUTput:C:Source ", "");
        if (parameter.length() != 0) {
            uint8_t source = parameter.toInt();
            Debug.print(DBG_INFO, "\n>>> Digital output 3 source received via SCPI interface is: %d", source);
            DataModel::dout3.value = source;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("DIGitalOUTput:D:Source ")) {
        String parameter = commandLine;
        parameter.replace("DIGitalOUTput:D:Source ", "");
        if (parameter.length() != 0) {
            uint8_t source = parameter.toInt();
            Debug.print(DBG_INFO, "\n>>> Digital output 4 source received via SCPI interface is: %d", source);
            DataModel::dout4.value = source;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("DIGitalOUTput:VCC:VOLTage ")) {
        String parameter = commandLine;
        parameter.replace("DIGitalOUTput:VCC:VOLTage ", "");
        if (parameter.length() != 0) {
            float_t voltage = parameter.toFloat();
            Debug.print(DBG_INFO, "\n>>> VCC voltage received via SCPI interface is: %f", voltage);
            DataModel::vcc.value = voltage;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }
    else if (commandLine.startsWith("DIGitalOUTput:VDC:VOLTage ")) {
        String parameter = commandLine;
        parameter.replace("DIGitalOUTput:VDC:VOLTage ", "");
        if (parameter.length() != 0) {
            float_t voltage = parameter.toFloat();
            Debug.print(DBG_INFO, "\n>>> VDC voltage received via SCPI interface is: %f", voltage);
            DataModel::vdc.value = voltage;
        }
        else {
            Debug.print(DBG_ERROR, "\n>>> Empty SCPI parameter");
        }
        done = true;
    }

    return done;
}