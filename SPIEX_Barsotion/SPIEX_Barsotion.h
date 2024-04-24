// (E) Barsotion KY
// SPIEX_Barsotion v2.0 07.11.23
// Software SPI library by Barsy-Barsevich 


#ifndef SPIEX_Barsotion_lib
#define SPIEX_Barsotion_lib

//--<Объявления портов и битов>-------------------------------------------------
/*#define _PORT_MOSI_     PORTC
#define _PORT_MISO_     PORTC
#define _PORT_SCK_      PORTC
#define _PORT_SS_       PORTC
#define _BIT_MOSI_      3
#define _BIT_MISO_      1
#define _BIT_SCK_       2
#define _BIT_SS_        0*/

//--<Установка режима работы SPI>-----------------------------------------------
#define SPI_MODE_00
//#define SPI_MODE_01
//#define SPI_MODE_10
//#define SPI_MODE_11


class SPIASM {
    private:
        //uint8_t _BIT_MOSI_, _BIT_MISO_, _BIT_SCK_, _BIT_SS_;
        //int _PORT_MISO_, _PORT_MOSI_, _PORT_SCK_, _PORT_SS_;
    
    public:
        SPIELIB() {}
       

    public:
        // Управление линией \CS
        inline void down() { //ss_down
            asm volatile(
                "CBI %0,%1" "\n\t"
                :: "I" (_SFR_IO_ADDR(_PORT_SS_)) /* %0 */, "I" (_BIT_SS_) /* %1 */
            );
        }
        inline void up() { //ss_up
            asm volatile(
                "SBI %0,%1" "\n\t"
                :: "I" (_SFR_IO_ADDR(_PORT_SS_)) /* %0 */, "I" (_BIT_SS_) /* %1 */
            );
        }
        //Организация обмена данными по SPI
#ifdef SPI_MODE_00
        inline uint8_t ex(uint8_t data) { // Обмен по SPI
            uint8_t counter;
            uint8_t out;
            asm volatile(
                "LDI  %7,8"       "\n\t" //load '8' to counter
                "CBI  %5,%6"      "\n\t" //clear SCK pin
                "main_cycle_%=: "//     "\n\t"
                "BST  %8,7"       "\n\t" //1
                "CBI  %1,%2"      "\n\t" //2
                "BRTC metka_%="   "\n\t" //2/1
                "SBI  %1,%2"      "\n\t" //2
                "metka_%=:"
                "SBI  %5,%6"      "\n\t" //set SCK pin
                "LSL  %8"         "\n\t" //data reg left shift
                "SBIC %3-2,%4"    "\n\t" //skip if MISO pin clear
                "ORI  %8,1"       "\n\t" //set bit 0 if data reg
                "CBI  %5,%6"      "\n\t" //clear SCK pin
                "DEC  %7"         "\n\t" //decrement counter
                "BRNE main_cycle_%=" "\n\t" //branch if not zero
                "CBI  %1,%2"      "\n\t" //clear MOSI pin
                "MOV  %0,%8"      "\n\t"
                : "=r" (out)
                : "I" (_SFR_IO_ADDR(_PORT_MOSI_)) /*_%1_*/, "I" (_BIT_MOSI_) /* %2 */,
                  "I" (_SFR_IO_ADDR(_PORT_MISO_)) /* %3 */, "I" (_BIT_MISO_) /* %4 */, 
                  "I" (_SFR_IO_ADDR(_PORT_SCK_))  /* %5 */, "I" (_BIT_SCK_)  /* %6 */,
                  "r" (counter) /* %7 */, "d" (data) /* %8 */
            );
            return out;
        }
        uint8_t SPI_ex(uint8_t data) { // Обмен по SPI
    uint8_t out_data = 0;
    for (uint8_t i=0; i<8; i++) {
        //(1) Установка MOSI
        if (data & 0x80) PORTB |= (1<<_BIT_MOSI_);
        else             PORTB &= ~(1<<_BIT_MOSI_);
        data <<= 1;
        //_delay_us(1);
        //(2) Поднятие SCK
        PORTB |= (1<<_BIT_SCK_);
        //_delay_us(1);
        //(3) Чтение MISO
        out_data <<= 1;
        if (PINB & (1<<_BIT_MISO_)) out_data += 1;
        //_delay_us(1);
        //(4) Опускание SCK
        PORTB &= ~(1<<_BIT_SCK_);
        //_delay_us(1);
    }
    PORTB &= ~(1<<_BIT_MOSI_);
    return out_data;
}
#endif
#ifdef SPI_MODE_01
        uint8_t ex(uint8_t data) { // Обмен по SPI
            uint8_t counter;
            uint8_t out;
            asm volatile(
                "LDI  %7,8"       "\n\t" //load '8' to counter
                "CBI  %5,%6"      "\n\t" //clear SCK pin
                "main_cycle_%=: "//     "\n\t"
                "SBI  %5,%6"      "\n\t" //set SCK pin
                "SBRS %8,7"       "\n\t" //skip if 7th bit of data is set
                "SBI  %1,%2"      "\n\t" //set MOSI pin
                "SBRC %8,7"       "\n\t" //skip if 7th bit of data is clear
                "CBI  %1,%2"      "\n\t" //clear MOSI pin
                "CBI  %5,%6"      "\n\t" //clear SCK pin
                "SBIC %3-2,%4"    "\n\t" //skip if MISO pin clear
                "ORI  %8,1"       "\n\t" //set bit 0 if data reg
                "ROL  %8"         "\n\t" //data reg left shift
                "DEC  %7"         "\n\t" //decrement counter
                "BRNE main_cycle_%=" "\n\t" //branch if not zero
                "CBI  %1,%2"      "\n\t" //clear MOSI pin
                "MOV  %0,%8"      "\n\t"
                : "=r" (out)
                : "I" (_SFR_IO_ADDR(_PORT_MOSI_)) /*_%1_*/, "I" (_BIT_MOSI_) /* %2 */,
                  "I" (_SFR_IO_ADDR(_PORT_MISO_)) /* %3 */, "I" (_BIT_MISO_) /* %4 */, 
                  "I" (_SFR_IO_ADDR(_PORT_SCK_))  /* %5 */, "I" (_BIT_SCK_)  /* %6 */,
                  "r" (counter) /* %7 */, "d" (data) /* %8 */
            );
            return data;
        }
#endif
#ifdef SPI_MODE_10
        uint8_t ex(uint8_t data) { // Обмен по SPI
            uint8_t counter;
            uint8_t out;
            asm volatile(
                "LDI  %7,8"       "\n\t" //load '8' to counter
                "SBI  %5,%6"      "\n\t" //set SCK pin
                "main_cycle_%=: "//     "\n\t"
                "SBRS %8,7"       "\n\t" //skip if 7th bit of data is set
                "SBI  %1,%2"      "\n\t" //set MOSI pin
                "SBRC %8,7"       "\n\t" //skip if 7th bit of data is clear
                "CBI  %1,%2"      "\n\t" //clear MOSI pin
                "CBI  %5,%6"      "\n\t" //clear SCK pin
                "SBIC %3-2,%4"    "\n\t" //skip if MISO pin clear
                "ORI  %8,1"       "\n\t" //set bit 0 if data reg
                "ROL  %8"         "\n\t" //data reg left shift
                "SBI  %5,%6"      "\n\t" //set SCK pin
                "DEC  %7"         "\n\t" //decrement counter
                "BRNE main_cycle_%=" "\n\t" //branch if not zero
                "SBI  %1,%2"      "\n\t" //set MOSI pin
                "MOV  %0,%8"      "\n\t"
                : "=r" (out)
                : "I" (_SFR_IO_ADDR(_PORT_MOSI_)) /*_%1_*/, "I" (_BIT_MOSI_) /* %2 */,
                  "I" (_SFR_IO_ADDR(_PORT_MISO_)) /* %3 */, "I" (_BIT_MISO_) /* %4 */, 
                  "I" (_SFR_IO_ADDR(_PORT_SCK_))  /* %5 */, "I" (_BIT_SCK_)  /* %6 */,
                  "r" (counter) /* %7 */, "d" (data) /* %8 */
            );
            return data;
        }
#endif
#ifdef SPI_MODE_11
        uint8_t ex(uint8_t data) { // Обмен по SPI
            uint8_t counter;
            uint8_t out;
            asm volatile(
                "LDI  %7,8"       "\n\t" //load '8' to counter
                "SBI  %5,%6"      "\n\t" //set SCK pin
                "main_cycle_%=: "//     "\n\t"
                "CBI  %5,%6"      "\n\t" //clear SCK pin
                "SBRS %8,7"       "\n\t" //skip if 7th bit of data is set
                "SBI  %1,%2"      "\n\t" //set MOSI pin
                "SBRC %8,7"       "\n\t" //skip if 7th bit of data is clear
                "CBI  %1,%2"      "\n\t" //clear MOSI pin
                "SBI  %5,%6"      "\n\t" //set SCK pin
                "SBIC %3-2,%4"    "\n\t" //skip if MISO pin clear
                "ORI  %8,1"       "\n\t" //set bit 0 if data reg
                "ROL  %8"         "\n\t" //data reg left shift
                "DEC  %7"         "\n\t" //decrement counter
                "BRNE main_cycle_%=" "\n\t" //branch if not zero
                "SBI  %1,%2"      "\n\t" //set MOSI pin
                "MOV  %0,%8"      "\n\t"
                : "=r" (out)
                : "I" (_SFR_IO_ADDR(_PORT_MOSI_)) /*_%1_*/, "I" (_BIT_MOSI_) /* %2 */,
                  "I" (_SFR_IO_ADDR(_PORT_MISO_)) /* %3 */, "I" (_BIT_MISO_) /* %4 */, 
                  "I" (_SFR_IO_ADDR(_PORT_SCK_))  /* %5 */, "I" (_BIT_SCK_)  /* %6 */,
                  "r" (counter) /* %7 */, "d" (data) /* %8 */
            );
            return data;
        }
#endif

        void activate() {
            asm volatile(
                "SBI %0-1,%1" "\n\t"
                "CBI %2-1,%3" "\n\t"
                "SBI %4-1,%5" "\n\t"
                "SBI %6-1,%7" "\n\t"
                ::"I" (_SFR_IO_ADDR(_PORT_MOSI_)) /*_%0_*/, "I" (_BIT_MOSI_) /* %1 */,
                  "I" (_SFR_IO_ADDR(_PORT_MISO_)) /* %2 */, "I" (_BIT_MISO_) /* %3 */, 
                  "I" (_SFR_IO_ADDR(_PORT_SCK_))  /* %4 */, "I" (_BIT_SCK_)  /* %5 */,
                  "I" (_SFR_IO_ADDR(_PORT_SS_))   /* %6 */, "I" (_BIT_SS_)   /* %7 */
            );
        }
        void deactivate() {
            asm volatile(
                "CBI %0-1,%1" "\n\t"
                "CBI %2-1,%3" "\n\t"
                "CBI %4-1,%5" "\n\t"
                "CBI %6-1,%7" "\n\t"
                ::"I" (_SFR_IO_ADDR(_PORT_MOSI_)) /*_%0_*/, "I" (_BIT_MOSI_) /* %1 */,
                  "I" (_SFR_IO_ADDR(_PORT_MISO_)) /* %2 */, "I" (_BIT_MISO_) /* %3 */, 
                  "I" (_SFR_IO_ADDR(_PORT_SCK_))  /* %4 */, "I" (_BIT_SCK_)  /* %5 */,
                  "I" (_SFR_IO_ADDR(_PORT_SS_))   /* %6 */, "I" (_BIT_SS_)   /* %7 */
            );
        }
};
#endif
