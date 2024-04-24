
#ifndef SPIEX_Barsotion_lib
#define SPIEX_Barsotion_lib

#define PORT_DEFAULT    _SFR_IO_ADDR(PORTB)

#define SPI_MODE_00
//#define SPI_MODE_01
//#define SPI_MODE_10
//#define SPI_MODE_11

const int _mosi =    0;
const int _miso =    1;
const int _sck =     2;
const int _ss =      3;

/*#define _mosi   0
#define _miso   1
#define _sck    2
#define _ss 3*/


class SPIELIB {
    private:
        uint8_t _mosi_, _miso_, _sck_, _ss_;
        int _port_miso_, _port_mosi_, _port_sck_, _port_ss_;
    
    public:
        SPIELIB() {}
        
        void begin(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ss){
            _mosi_ = mosi & 0b00000111;
            _miso_ = miso & 0b00000111;
            _sck_ = sck & 0b00000111;
            _ss_ = ss & 0b00000111;
        }
        

    public:
        // Управление линией \CS
        inline void down() { //ss_down
            asm volatile(
                "CBI %0,%1" "\n\t"
                //:: "I" (_port_miso_) /* %0 */, "I" (_mosi_) /* %1 */
                :: "I" (PORT_DEFAULT) /* %0 */, "I" (_ss) /* %1 */
            );
        }
        inline void up() { //ss_up
            asm volatile(
                "SBI %0,%1" "\n\t"
                :: "I" (PORT_DEFAULT) /* %0 */, "I" (_ss) /* %1 */
            );
        }
        //Организация обмена данными по SPI
#ifdef SPI_MODE_00
        uint8_t ex(uint8_t data) { // Обмен по SPI
            uint8_t counter;
            asm volatile(
                "LDI  %7,8"       "\n\t" //load '8' to counter
                "main_cycle:"     "\n\t"
                "SBRS %0,7"       "\n\t" //skip if 7th bit of data is set
                "SBI  %1,%2"      "\n\t" //set MOSI pin
                "SBRC %0,7"       "\n\t" //skip if 7th bit of data is clear
                "CBI  %1,%2"      "\n\t" //clear MISO pin
                "SBI  %5,%6"      "\n\t" //set SCK pin
                "SBIC %3-2,%4"    "\n\t" //skip if MISO pin clear
                "ORI  %0,1"       "\n\t" //set bit 0 if data reg
                "ROL  %0"         "\n\t" //data reg left shift
                "CBI  %5,%6"      "\n\t" //clear SCK pin
                "DEC  %7"         "\n\t" //decrement counter
                "BRCC main_cycle" "\n\t" //branch if not zero
                : "=r" (data)
                : "I" (PORT_DEFAULT) /*_%1_*/, "I" (_mosi) /* %2 */,
                  "I" (PORT_DEFAULT) /* %3 */, "I" (_miso) /* %4 */, 
                  "I" (PORT_DEFAULT)  /* %5 */, "I" (_sck)  /* %6 */,
                  "r" (counter) /* %7 */
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
                ::"I" (PORT_DEFAULT) /*_%0_*/, "I" (_mosi) /* %1 */,
                  "I" (PORT_DEFAULT) /* %2 */, "I" (_miso) /* %3 */, 
                  "I" (PORT_DEFAULT)  /* %4 */, "I" (_sck)  /* %5 */,
                  "I" (PORT_DEFAULT)   /* %6 */, "I" (_ss)   /* %7 */
            );
        }
        void disactivate() {
            asm volatile(
                "CBI %0-1,%1" "\n\t"
                "CBI %2-1,%3" "\n\t"
                "CBI %4-1,%5" "\n\t"
                "CBI %6-1,%7" "\n\t"
                ::"I" (PORT_DEFAULT) /*_%0_*/, "I" (_mosi) /* %1 */,
                  "I" (PORT_DEFAULT) /* %2 */, "I" (_miso) /* %3 */, 
                  "I" (PORT_DEFAULT)  /* %4 */, "I" (_sck)  /* %5 */,
                  "I" (PORT_DEFAULT)   /* %6 */, "I" (_ss)   /* %7 */
            );
        }
};
#endif
