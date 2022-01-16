#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include <opentherm.h>
#include <DS18B20.h>

#define slave_id 10
#define baund 9600

#define BOILER_IN 3
#define BOILER_OUT 5
#define WIRE_PIN 7
#define RS485_PIN 6
#define OUTPU_PIN_1 A3
#define OUTPU_PIN_2 A4
#define OUTPU_PIN_3 A5

OpenthermData message;
RS485Class rs485{Serial, 1, RS485_PIN, -1};
ModbusRTUServerClass modbus{rs485};
DS18B20 ds(WIRE_PIN);

void error() {
  pinMode(1, OUTPUT);
  while (1) {
    digitalWrite(1, HIGH);
    delay(500);
    digitalWrite(1, LOW);
    delay(500);  
  }
}

void setup() {
  Serial.begin(baund);

  pinMode(OUTPU_PIN_1, OUTPUT);
  pinMode(OUTPU_PIN_2, OUTPUT);
  pinMode(OUTPU_PIN_3, OUTPUT);

  while (!Serial) ;
  if (!modbus.begin(slave_id, baund))
  {
    error();
  }
  modbus.configureCoils(0, 11);
  modbus.configureInputRegisters(0, 6);
}

void loop() {
  modbus.poll();
  
  if (modbus.coilRead(10))
  {
    modbus.coilWrite(10, 0);
    if (ds.selectNext() || ds.selectNext())
    {
      byte addr[8];
      ds.getAddress(addr);
      float temp{ds.getTempC()};
      modbus.writeInputRegisters(0, (uint16_t*)addr, 4);
      modbus.writeInputRegisters(4, (uint16_t*)(&temp), 2);
    }
  }

  digitalWrite(OUTPU_PIN_1, modbus.coilRead(0));
  digitalWrite(OUTPU_PIN_2, modbus.coilRead(1));
  digitalWrite(OUTPU_PIN_3, modbus.coilRead(2));
}
