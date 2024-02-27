#pragma once

#include <Arduino.h>
#include "GCodeCommand.h"
#include "GCode.h"

// Creates a GCode command object
class GCodeHandler {
private:
    HardwareSerial* serial;
    long baudrate;

public:
    void SetSerialInterface(HardwareSerial& serial, long baudrate);
    void Initialize();
    GCodeCommand ReadCommand();
    void SendOK();
};

void GCodeHandler::SetSerialInterface(HardwareSerial& serial, long baudrate){
    this->serial = &serial;
    this->baudrate = baudrate;
}


void GCodeHandler::Initialize(){
    this->serial->begin(baudrate);
}

GCodeCommand GCodeHandler::ReadCommand(){
    GCodeCommand cmd;
    String line;
    
    // Extract the command type and number
    int firstSpaceIndex = line.indexOf(' ');
    if (firstSpaceIndex != -1) {
        String commandPart = line.substring(0, firstSpaceIndex);
        if (commandPart.length() > 1) {
            cmd.commandType = commandPart.charAt(0);
            cmd.commandNumber = commandPart.substring(1).toInt();
        }

        // Process parameters
        String parameters = line.substring(firstSpaceIndex + 1);
        int nextSpaceIndex;
        while ((nextSpaceIndex = parameters.indexOf(' ')) != -1) {
            if (cmd.parametersUsed >= GCodeCommand::parameterCount) {
                break; // Ensure we don't exceed the array bounds
            }
            
            String parameter = parameters.substring(0, nextSpaceIndex);
            if (parameter.length() > 1) {
                cmd.characters[cmd.parametersUsed] = parameter.charAt(0); // Store parameter character
                cmd.values[cmd.parametersUsed] = parameter.substring(1).toFloat(); // Convert and store value
                cmd.parametersUsed++; // Increment the count of parameters used
            }
            parameters = parameters.substring(nextSpaceIndex + 1);
        }

        // Catch any last parameter not followed by a space
        if (parameters.length() > 1 && cmd.parametersUsed < GCodeCommand::parameterCount) {
            cmd.characters[cmd.parametersUsed] = parameters.charAt(0);
            cmd.values[cmd.parametersUsed] = parameters.substring(1).toFloat();
            cmd.parametersUsed++; // Increment here as well
        }
    }

    return cmd;
}

void GCodeHandler::SendOK(){
    serial->println("ok");
}
