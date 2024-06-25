#include "Implementation.h"

void Implementation::SetSerialInterface(HardwareSerial* serial, long baudrate){
    SerialHandler::SetSerialInterface(Serial, 38400);
    SerialHandler::Initialize();
}

void Implementation::SetSerialInterface(usb_serial_class* serial, long baudrate){
    SerialHandler::SetSerialInterface(Serial, 38400);
    SerialHandler::Initialize();
}

bool Implementation::IsCommandAvailable(){
    return SerialHandler::CommandAvailable();
}

void Implementation::ExecuteCommand(){
    GCodeCommand cmd = SerialHandler::ReadCommand();

    gCode.ExecuteGCode(&cmd);

    SerialHandler::SendOK();

    SerialHandler::SendCommandAsk();
}
