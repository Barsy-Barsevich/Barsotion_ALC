#include "TWIEX_Barsotion.h"
#define MPU6050_ADDR   0x68
#define MPU6050_ID     0x75

/*
 * Данная программа демонстрирует работу с модулем MPU6050 по прерываниям.
 * Программа считывает ID модуля (должен быть 0x68) и печатает его в порт
 */

//Глобальная переменная mode определяет ступень TWI-последовательности
volatile uint8_t mode;

//TWI-функции
void TWI_00() {twi.start();}
void TWI_01() {twi.write(MPU6050_ADDR<<1);}
void TWI_02() {twi.write(MPU6050_ID);}
void TWI_03() {twi.start();}
void TWI_04() {twi.write((MPU6050_ADDR<<1)|1);}
void TWI_05() {twi.request(false);}
void TWI_06() {
  //Не рекомендуется задерживать TWI-последовательность во время исполнения
  //более чем на время ошибки во избежание ошибок 
  uint8_t data = twi.read();
  twi.end();
  Serial.print(F("MPU6050 id = "));
  Serial.println(data);
}

//Массив указателей на TWI-функции
void (*twi_queue[7])();


void setup() {
  //Инициализация
  twi_queue[0] = TWI_00;
  twi_queue[1] = TWI_01;
  twi_queue[2] = TWI_02;
  twi_queue[3] = TWI_03;
  twi_queue[4] = TWI_04;
  twi_queue[5] = TWI_05;
  twi_queue[6] = TWI_06;
  //Serial инициализация
  Serial.begin(9600);
  Serial.println(F("Start!"));
  //Инициализация библиотеки
  twi.begin();
  //Установка времени ошибки в микросекундах
  //(минимальное значение - 28мкс, максимальное - 252мкс, по умолчанию - 28мкс)
  twi.setErrorTime(28);
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
  //Вызов TWI-функции под номером mode из массива
  twi_queue[mode]();
  //Инкремент mode
  mode += 1;
}
