#pragma once

#include <Arduino.h>
#include "GCodeCommand.h"

// Creates a GCode command object
class SerialHandler {
private:
    static HardwareSerial* serialHS;
    static usb_serial_class* serialUSC;
    static long baudrate;
    static bool serialType;

public:
    static void Initialize();
    static bool IsReady();
    static int CommandAvailable();
    static GCodeCommand ReadCommand();
    static void SendOK();
    static void SendNotImplemented();
    static void SendCommandAsk();
    static void SendCharacter(char character);
    static void SendMessage(String message);
    static void SendMessageNLN(String message);
    static void SendMessageTab(String message);
    
    static void SetSerialInterface(HardwareSerial& serial, long baudrate);
    static void SetSerialInterface(usb_serial_class& serial, long baudrate);
    
	template <typename T>
    static void SendMessageValue(String message, T value);
};


HardwareSerial* SerialHandler::serialHS;
usb_serial_class* SerialHandler::serialUSC;
long SerialHandler::baudrate;
bool SerialHandler::serialType;

void SerialHandler::SetSerialInterface(HardwareSerial& serial, long baudrate){
    SerialHandler::serialHS = &serial;
    SerialHandler::baudrate = baudrate;
    SerialHandler::serialType = true;
}

void SerialHandler::SetSerialInterface(usb_serial_class& serial, long baudrate){
    SerialHandler::serialUSC = &serial;
    SerialHandler::baudrate = baudrate;
    SerialHandler::serialType = false;
}


void SerialHandler::Initialize(){
    if(serialType) SerialHandler::serialHS->begin(baudrate);
    else SerialHandler::serialUSC->begin(baudrate);
}

bool SerialHandler::IsReady(){
    if(serialType) return serialHS;
    else return serialUSC;
}

int SerialHandler::CommandAvailable(){
    if(serialType) return serialHS->available();
    else return serialUSC->available();
}

GCodeCommand SerialHandler::ReadCommand(){
    GCodeCommand cmd;
    String line;
    char incomingChar;

    while (true) {
        int available = serialType ? serialHS->available() : serialUSC->available();
        if(available > 0){
            incomingChar = serialType ? serialHS->read() : serialUSC->read();
            
            SendCharacter(incomingChar);
            
            if (incomingChar == '\n') break;

            if (incomingChar == '\b') {
                if (line.length() > 0) {
                    line.remove(line.length() - 1);
                }
            }
            else {
                line += incomingChar;
            }
            
        }
    }
    
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
    else{
        if (line.length() > 1) {
            cmd.commandType = line.charAt(0);
            cmd.commandNumber = line.substring(1).toInt();
        }
    }

    return cmd;
}

void SerialHandler::SendOK(){
    SendMessage("ok");
}

void SerialHandler::SendNotImplemented(){
    SendMessage("error");
}

void SerialHandler::SendCommandAsk(){
    SendMessageNLN("Command:");
}

void SerialHandler::SendCharacter(char character){
    if(serialType) serialHS->print(character);
    else serialUSC->print(character);
}

void SerialHandler::SendMessage(String message){
    if(serialType) serialHS->println(message);
    else serialUSC->println(message);
}

void SerialHandler::SendMessageNLN(String message){
    if(serialType) serialHS->print(message);
    else serialUSC->print(message);
}

void SerialHandler::SendMessageTab(String message){
    if(serialType) {
        serialHS->print(message); serialHS->print('\t');
    }
    else {
        serialUSC->print(message); serialUSC->print('\t');
    }
}

template <typename T>
void SerialHandler::SendMessageValue(String message, T value){
    if(serialType) {
        serialHS->print(message); serialHS->print(": ");
        serialHS->println(value);
    }
    else {
        serialUSC->print(message); serialUSC->print(": ");
        serialUSC->println(value);
    }
}
