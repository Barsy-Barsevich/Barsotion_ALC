#include "TWIEX_Barsotion.h"
#define MPU6050_ADDR  0x68
#define MPU6050_ID    0x75


void setup() {
  //Инициализация Serial
  Serial.begin(9600);
  Serial.println(F("Start"));
  //Инициализация библиотеки
  twi.begin();
}

void loop() {
  //Простое чтение
  uint8_t data;
  bool rslt = twi.readRegister(MPU6050_ADDR, MPU6050_ID, &data);
  if (rslt) {
    Serial.print(F("MPU6050 id = "));
    Serial.println(data);
  }
  else {
    //Принудительное завершение транзакции передачи данных по шине TWI
    twi.end();
    Serial.println(F("Error!"));
  }
  delay(1000);
}
