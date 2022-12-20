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
    String input = MENU_SERIAL.readString();
    if (!input.endsWith("\r\n")) {
        commandLine += input;
    }
    else {
        commandLine += input;
        commandLine.replace("\r\n", "");
        commandProcessing();
        commandLine = "";
    }
}


void ScpiCommandHandler::commandProcessing(void)
{
    if (commandLine.equals("SYStem:COMmand:INput MENU")) {
        DataModel::newMode = DataModel::CommandHandlerInputMode::INTERACTIVE;
    }

    relaisCommandProcessing();
    levelShiftCommandProcessing();
    adcCommandProcessing();
    batSimCommandProcessing();
    fOutCommandProcessing();
    dOutCommandProcessing();
}


void ScpiCommandHandler::relaisCommandProcessing(void) {
    if (commandLine.equals("RELais:A:ACTivate")) {
        DataModel::relais.enable1 = true;
    }
    if (commandLine.equals("RELais:A:DEACTivate")) {
        DataModel::relais.enable1 = false;
    }
    if (commandLine.equals("RELais:A?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::relais.enable1);
    }
    if (commandLine.equals("RELais:B:ACTivate")) {
        DataModel::relais.enable2 = true;
    }
    if (commandLine.equals("RELais:B:DEACTivate")) {
        DataModel::relais.enable2 = false;
    }
    if (commandLine.equals("RELais:B?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::relais.enable2);
    }
}


void ScpiCommandHandler::levelShiftCommandProcessing(void) {
    if (commandLine.equals("LEVelSHift:A:ACTivate")) {
        DataModel::lshift.enable1 = true;
    }
    if (commandLine.equals("LEVelSHift:A:DEACTivate")) {
        DataModel::lshift.enable1 = false;
    }
    if (commandLine.equals("LEVelSHift:A?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable1);
    }
    if (commandLine.equals("LEVelSHift:B:ACTivate")) {
        DataModel::lshift.enable2 = true;
    }
    if (commandLine.equals("LEVelSHift:B:DEACTivate")) {
        DataModel::lshift.enable2 = false;
    }
    if (commandLine.equals("LEVelSHift:B?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable2);
    }
    if (commandLine.equals("LEVelSHift:C:ACTivate")) {
        DataModel::lshift.enable3 = true;
    }
    if (commandLine.equals("LEVelSHift:C:DEACTivate")) {
        DataModel::lshift.enable3 = false;
    }
    if (commandLine.equals("LEVelSHift:C?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable3);
    }
    if (commandLine.equals("LEVelSHift:D:ACTivate")) {
        DataModel::lshift.enable4 = true;
    }
    if (commandLine.equals("LEVelSHift:D:DEACTivate")) {
        DataModel::lshift.enable4 = false;
    }
    if (commandLine.equals("LEVelSHift:D?")) {
        MENU_SERIAL.printf("%d\r\n", DataModel::lshift.enable4);
    }
}


void ScpiCommandHandler::adcCommandProcessing(void) {
    if (commandLine.equals("AnalogIN:A?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc1_value);
    }
    if (commandLine.equals("AnalogIN:B?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc2_value);
    }
    if (commandLine.equals("AnalogIN:C?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc3_value);
    }
    if (commandLine.equals("AnalogIN:D?")) {
        MENU_SERIAL.printf("%s\r\n", DataModel::ain.adc4_value);
    }
    if (commandLine.equals("AnalogIN:GAIN?")) {
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
    } 
    // TODO: Check better parameter format. Using string representing numbers an integers itself
    // may confuse the user! 
    if (commandLine.startsWith("AnalogIN:GAIN ")) {
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
    }
}


void ScpiCommandHandler::batSimCommandProcessing(void) {

}


void ScpiCommandHandler::fOutCommandProcessing(void) {

}


void ScpiCommandHandler::dOutCommandProcessing(void) {

}