/*
    (E) Barsotion Koneenrakennuksen Yritys (Barsotion KY)
    Библиотека TWI
    v1.2
*/
#ifndef TWIEX_Barsotion
#define TWIEX_Barsotion

#define TWI_ON_INTERRUPTS

class TWI0 {
    public:
        TWI0() {}
        
#ifdef TWI_ON_INTERRUPTS
    public:
        begin() {
            TWBR0 = 12;
            cli();
            TCCR2A = 
                (0<<COM0A1)|
                (0<<COM0A0)|
                (0<<COM0B1)|
                (0<<COM0B0)|
                (1<<WGM01)| // CTC timer mode
                (0<<WGM00);
            TCCR2B = 
                (0<<FOC0A)|
                (0<<FOC0B)|
                (0<<WGM02)|
                (0<<CS02)| // 1/8 prescaling
                (0<<CS01)|
                (0<<CS00);
            OCR2A = 7; // 28us
            TIMSK2 = 
                (0<<OCIE0B)|
                (0<<OCIE0A)|  // disable twi timer
                (0<<TOIE0);
            sei();
        }
    private:
        inline void disable_twi_timer() {
            TIMSK2 = 0;
            TCCR2B = 0;
            TIFR2 = 0;
        }
        inline void enable_twi_timer() {
            TCNT2 = 0;
            TIMSK2 = (1<<OCIE0A);
            TCCR2B = 4;
        }
#else
    public:
        begin() {
            TWBR0 = 12;
        }
#endif

    public:
        bool writeRegister(uint8_t addr, uint8_t reg, uint8_t data) {
            if (_start() == false)        return false;
            if (_write(addr<<1) == false) return false;
            if (_write(reg) == false)     return false;
            if (_write(data) == false)    return false;
            if (_stop() == false)         return false;
            return true;
        }
        /*uint8_t readRegister(uint8_t addr, uint8_t reg, uint8_t *out) {
            if (_start() == false)        return false;
            if (_write(addr<<1) == false) return false;
            if (_write(reg) == false)     return false; 
            if (_start() == false)        return false;
            if (_write((addr<<1)|1) == false) return false;
            *out = _read(false);
            if (_stop() == false)         return false;
            return true;
        }*/
        uint8_t readRegister(uint8_t addr, uint8_t reg, uint8_t *out) {
            if (_start() == false)        return false;
            if (_write(addr<<1) == false) return false;
            if (_write(reg) == false)     return false; 
            if (_start() == false)        return false;
            if (_write((addr<<1)|1) == false) return false;
            if (_read(false,out) == false)    return false;
            if (_stop() == false)         return false;
            return true;
        }
    private:
        bool _start() {
            // <Send START condition>
            // Установка TWINT -> временный запрет работы TWI
            // Установка TWSTA -> генерация стартовой последовательности
            // Установка TWEN -> разрешение работы модуля TWI
            // Установка TWIE -> разрешение прерываний по вектору TWI
            // Установка TWEA -> разрешение выдачи сигнала подтверждения, когда идет
            // прием данных
            //sei();
            TWCR0 = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
            uint8_t counter = 0;
            bool flag = false;
            while (!flag && (counter < 80)) {
                flag = ((TWCR0 & (1<<TWINT)) != 0);
                counter += 1;
            }
            return flag;
        }
        bool _write(uint8_t data) {
            TWDR0 = data;
            //sei();
            TWCR0 = (1<<TWINT)|(1<<TWEN);
            uint8_t counter = 0;
            bool flag = false;
            while (!flag && (counter < 160)) {
                flag = ((TWCR0 & (1<<TWINT)) != 0);
                counter += 1;
            }
            return flag;
        }
        /*uint8_t _read(bool ACKM) {
            TWDR0 = 0xFF;
            if (ACKM == true) TWCR0 = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
            else              TWCR0 = (1<<TWINT)|(1<<TWEN);
            uint8_t counter = 0;
            bool flag = false;
            while (!flag && (counter < 160)) {
                flag = ((TWCR0 & (1<<TWINT)) != 0);
                counter += 1;
            }
            if (flag) return TWDR0;
            else return 0;
        }*/
        uint8_t _read(bool ACKM, uint8_t* data) {
            TWDR0 = 0xFF;
            if (ACKM == true) TWCR0 = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
            else              TWCR0 = (1<<TWINT)|(1<<TWEN);
            uint8_t counter = 0;
            bool flag = false;
            while (!flag && (counter < 160)) {
                flag = ((TWCR0 & (1<<TWINT)) != 0);
                counter += 1;
            }
            *data = TWDR0;
            return flag;
        }
        bool _stop() {
            TWCR0 = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
            uint8_t counter = 0;
            bool flag = false;
            while (!flag && (counter < 80)) {
                flag = ((TWCR0 & (1<<TWINT)) != 0);
                counter += 1;
            }
            return true;
        }
    
    
#ifdef TWI_ON_INTERRUPTS
    public:
        inline void startQueue() {
            start();
        }
        inline void start() {
            //sei();
            TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE);
            enable_twi_timer();
        }
        inline void write(uint8_t data) {
            TWDR0 = data;
            //sei();
            TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
            enable_twi_timer();
        }
        inline void request(bool ACKM) {
            TWDR0 = 0xFF;
            //sei();
            if (ACKM == true) TWCR0 = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
            else              TWCR0 = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
            enable_twi_timer();
        }
        inline uint8_t read() {
            return TWDR0;
        }
        inline void stop() {
            //sei();
            TWCR0 = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
            enable_twi_timer();
        }
        inline void end() {
            //sei();
            TWCR0 = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
            disable_twi_timer();
        }
        void setErrorTime(uint8_t mks) {
            mks >>= 2;
            if (mks < 7) OCR2A = 7;
            else OCR2A = mks;
        }
#else
    public:
        inline void end() {
            //sei();
            TWCR0 = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
        }
#endif   
};

TWI0 twi0;
#endif
