/*
 * Пример использования библиотеки программного SPI - будем передавать значения
 * в порт и тут же считывать значения из порта.
 * 
 * Для отладки рекомендуется соединить пины PB0 и PB1 вместе, так мы сможем
 * в сериал мониторе убедиться, что при соединении MISO и MOSI мы что отправили,
 * то и получим
 */



#define _PORT_MOSI_   PORTB
#define _PORT_MISO_   PORTB
#define _PORT_SCK_    PORTB
#define _PORT_SS_     PORTB
#define _BIT_MOSI_    0
#define _BIT_MISO_    1
#define _BIT_SCK_     2
#define _BIT_SS_      3

#include "SPIEX_Barsotion.h"
SPIASM sspi;


//В этот массив можно предзагрузить какие-то интересные значения, но мне лень
uint8_t arr[255];

void setup() {
  //Активация программного SPI. Также существует функция deactivate() для деактивации
  sspi.activate();
  //Инициализация сериал
  Serial.begin(9600);
  Serial.println("Start");
  
  for (uint8_t i=0; i<255; i++) {
    //Печать номера элемента массива
    Serial.print(i);
    Serial.print("\t");
    //Печать самого элемента массива (то, что отправляем)
    Serial.print(arr[i]);
    Serial.print("\t");
    //Отправляем
    sspi.down();
    uint8_t b = sspi.ex(arr[i]);
    sspi.up();
    //Печатаем принятое
    Serial.print(b);
    Serial.print("\n");
  }
  sspi.deactivate();
}

void loop() {}
