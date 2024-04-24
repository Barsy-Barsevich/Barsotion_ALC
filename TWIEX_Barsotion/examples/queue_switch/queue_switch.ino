#include "TWIEX_Barsotion.h"
#define MPU6050_ADDR    0x68
#define MPU6050_ID      0x75

/*
 * Данная программа демонстрирует работу с модулем MPU6050 по прерываниям.
 * Программа считывает ID модуля (должен быть 0x68) и печатает его в порт
 */

//Глобальная переменная mode определяет ступень TWI-последовательности
volatile uint8_t mode;


void setup() {
  //Serial initialization
  Serial.begin(9600);
  Serial.println(F("Start"));
  //Инициализация библиотеки
  twi.begin();
  //Установка времени ошибки
  twi.setErrorTime(32);
}

void loop() {
  //Старт TWI-последовательности
  mode = 1;
  twi.startQueue();
  //Фоновый процесс
  delay(1000);
}


//-<Обработчики прерываний>-----------------------------------------------------
ISR(TIMER2_COMPA_vect) {
  //Принудительное завершение транзакции передачи данных по шине TWI
  twi.end();
  Serial.println(F("Error!"));
}

ISR(TWI_vect) {
  switch (mode) {
    case 0:
      twi.start();
      break;
    case 1:
      twi.write(MPU6050_ADDR<<1);
      break;
    case 2:
      twi.write(MPU6050_ID);
      break;
    case 3:
      twi.start();
      break;
    case 4:
      twi.write((MPU6050_ADDR<<1)|1);
      break;
    case 5:
      twi.request(false);
      break;
    case 6:
      //Не рекомендуется задерживать TWI-последовательность во время исполнения
      //более чем на время ошибки во избежание ошибок 
      uint8_t data = twi.read();
      twi.end();
      Serial.print(F("MPU6050 id = "));
      Serial.println(data);
  }
  mode += 1;
}
