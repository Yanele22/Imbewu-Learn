#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/*
 * =========================================================
 * ATmega328P @ 8 MHz
 * =========================================================
 *
 * I2C LCD
 * ---------------------------------------------------------
 * SDA = PC4 = Physical Pin 27
 * SCL = PC5 = Physical Pin 28
 *
 * HC-SR04
 * ---------------------------------------------------------
 * TRIG = PD2 = Physical Pin 4
 * ECHO = PD3 = Physical Pin 5
 *
 *
 * =========================================================
 * TANK PARAMETERS
 * =========================================================
 *
 * Total tank height       = 225.5 cm
 * Tank cross-sectional area = 260.16 cm^2
 * Air gap                 = 5.5 cm
 *
 * Maximum usable water height:
 *
 * 225.5 - 5.5 = 220.0 cm
 *
 * Maximum tank volume:
 *
 * 260.16 x 220 / 1000
 * = 57.2352 L
 *
 *
 * =========================================================
 * WATER LEVEL LIMITS
 * =========================================================
 *
 * Overfill threshold:
 *
 * Water height > 200 cm
 *
 * Low water:
 *
 * Water level <= 20%
 *
 * Empty:
 *
 * Water level = 0%
 */


/* =========================================================
   TANK CONSTANTS
   ========================================================= */

/*
 * Values stored as tenths of a centimetre.
 *
 * 225.5 cm -> 2255
 * 5.5 cm   -> 55
 * 220.0 cm -> 2200
 * 200.0 cm -> 2000
 */

#define TANK_HEIGHT_X10       2255
#define AIR_GAP_X10           55
#define MAX_WATER_HEIGHT_X10  2200
#define OVERFILL_HEIGHT_X10  2000


/*
 * Tank area:
 *
 * 260.16 cm^2
 *
 * Stored as hundredths.
 */

#define TANK_AREA_X100        26016


/* =========================================================
   HC-SR04 PINS
   ========================================================= */

#define TRIG_PORT  PORTD
#define TRIG_DDR   DDRD
#define TRIG_PIN   PD2

#define ECHO_DDR   DDRD
#define ECHO_PIN   PD3
#define ECHO_PINR  PIND


/* =========================================================
   LCD I2C ADDRESSES
   ========================================================= */

#define LCD_ADDRESS_1  0x20
#define LCD_ADDRESS_2  0x27
#define LCD_ADDRESS_3  0x3F

uint8_t lcd_address;


/* =========================================================
   I2C INITIALIZATION
   ========================================================= */

void I2C_Init(void)
{
    /*
     * TWI prescaler = 1
     */

    TWSR = 0x00;

    /*
     * F_CPU = 8 MHz
     *
     * Desired I2C = approximately 100 kHz
     *
     * Formula:
     *
     * SCL = F_CPU / (16 + 2*TWBR*prescaler)
     *
     * 100 kHz = 8 MHz / (16 + 2*32*1)
     *
     *       = 8 MHz / 80
     *
     *       = 100 kHz
     */

    TWBR = 32;

    /*
     * Enable TWI
     */

    TWCR = (1 << TWEN);
}


/* =========================================================
   I2C START
   ========================================================= */

uint8_t I2C_Start(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWSTA) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));

    return (TWSR & 0xF8);
}


/* =========================================================
   I2C WRITE
   ========================================================= */

uint8_t I2C_Write(uint8_t data)
{
    TWDR = data;

    TWCR = (1 << TWINT) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));

    return (TWSR & 0xF8);
}


/* =========================================================
   I2C STOP
   ========================================================= */

void I2C_Stop(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWSTO) |
           (1 << TWEN);

    _delay_us(10);
}


/* =========================================================
   CHECK I2C DEVICE
   ========================================================= */

uint8_t I2C_DevicePresent(uint8_t address)
{
    uint8_t status;

    status = I2C_Start();

    if ((status != 0x08) &&
        (status != 0x10))
    {
        I2C_Stop();

        return 0;
    }

    status = I2C_Write(address << 1);

    I2C_Stop();

    if (status == 0x18)
    {
        return 1;
    }

    return 0;
}


/* =========================================================
   FIND LCD ADDRESS
   ========================================================= */

uint8_t LCD_Find_Address(void)
{
    uint8_t address;

    /*
     * Search PCF8574:
     *
     * 0x20 - 0x27
     */

    for (address = 0x20;
         address <= 0x27;
         address++)
    {
        if (I2C_DevicePresent(address))
        {
            return address;
        }
    }

    /*
     * Search PCF8574A:
     *
     * 0x38 - 0x3F
     */

    for (address = 0x38;
         address <= 0x3F;
         address++)
    {
        if (I2C_DevicePresent(address))
        {
            return address;
        }
    }

    return 0;
}


/* =========================================================
   LCD LOW LEVEL WRITE
   ========================================================= */

void LCD_Write(uint8_t data)
{
    uint8_t status;

    status = I2C_Start();

    if ((status != 0x08) &&
        (status != 0x10))
    {
        I2C_Stop();

        return;
    }

    if (I2C_Write(lcd_address << 1) != 0x18)
    {
        I2C_Stop();

        return;
    }

    I2C_Write(data);

    I2C_Stop();
}


/* =========================================================
   LCD ENABLE PULSE
   ========================================================= */

void LCD_Pulse(uint8_t data)
{
    /*
     * EN = HIGH
     */

    LCD_Write(data | 0x04);

    _delay_us(1);

    /*
     * EN = LOW
     */

    LCD_Write(data & ~(0x04));

    _delay_us(100);
}


/* =========================================================
   SEND LCD NIBBLE
   ========================================================= */

void LCD_Nibble(uint8_t nibble,
                uint8_t rs)
{
    uint8_t data;

    /*
     * P3 = Backlight
     */

    data = 0x08;

    /*
     * P0 = RS
     */

    if (rs)
    {
        data |= 0x01;
    }

    /*
     * P4 = D4
     */

    if (nibble & 0x01)
    {
        data |= 0x10;
    }

    /*
     * P5 = D5
     */

    if (nibble & 0x02)
    {
        data |= 0x20;
    }

    /*
     * P6 = D6
     */

    if (nibble & 0x04)
    {
        data |= 0x40;
    }

    /*
     * P7 = D7
     */

    if (nibble & 0x08)
    {
        data |= 0x80;
    }

    LCD_Pulse(data);
}


/* =========================================================
   LCD COMMAND
   ========================================================= */

void LCD_Command(uint8_t command)
{
    LCD_Nibble(command >> 4, 0);

    LCD_Nibble(command & 0x0F, 0);

    if ((command == 0x01) ||
        (command == 0x02))
    {
        _delay_ms(2);
    }
    else
    {
        _delay_us(50);
    }
}


/* =========================================================
   LCD DATA
   ========================================================= */

void LCD_Data(uint8_t data)
{
    LCD_Nibble(data >> 4, 1);

    LCD_Nibble(data & 0x0F, 1);

    _delay_us(50);
}


/* =========================================================
   LCD PRINT
   ========================================================= */

void LCD_Print(const char *text)
{
    while (*text)
    {
        LCD_Data(*text);
        text++;
    }
}


/* =========================================================
   LCD CLEAR
   ========================================================= */

void LCD_Clear(void)
{
    LCD_Command(0x01);

    _delay_ms(2);
}


/* =========================================================
   LCD GOTO
   ========================================================= */

void LCD_Goto(uint8_t row,
              uint8_t column)
{
    if (row == 0)
    {
        LCD_Command(0x80 + column);
    }
    else
    {
        LCD_Command(0xC0 + column);
    }
}


/* =========================================================
   LCD INITIALIZATION
   ========================================================= */

void LCD_Init(void)
{
    _delay_ms(50);

    /*
     * LCD reset sequence
     */

    LCD_Nibble(0x03, 0);
    _delay_ms(5);

    LCD_Nibble(0x03, 0);
    _delay_us(150);

    LCD_Nibble(0x03, 0);
    _delay_us(150);

    /*
     * 4-bit mode
     */

    LCD_Nibble(0x02, 0);

    _delay_us(150);

    /*
     * 4-bit, 2-line, 5x8
     */

    LCD_Command(0x28);

    /*
     * Display ON
     * Cursor OFF
     * Blink OFF
     */

    LCD_Command(0x0C);

    /*
     * Entry mode
     */

    LCD_Command(0x06);

    /*
     * Clear
     */

    LCD_Command(0x01);

    _delay_ms(2);
}


/* =========================================================
   HC-SR04 INITIALIZATION
   ========================================================= */

void Ultrasonic_Init(void)
{
    /*
     * TRIG = OUTPUT
     */

    TRIG_DDR |= (1 << TRIG_PIN);

    /*
     * ECHO = INPUT
     */

    ECHO_DDR &= ~(1 << ECHO_PIN);

    /*
     * Trigger LOW
     */

    TRIG_PORT &= ~(1 << TRIG_PIN);

    /*
     * Timer1 Normal mode
     */

    TCCR1A = 0x00;

    /*
     * Timer stopped initially
     */

    TCCR1B = 0x00;

    /*
     * Clear Timer1
     */

    TCNT1 = 0;
}


/* =========================================================
   GET ONE HC-SR04 DISTANCE
   =========================================================
 *
 * Returns distance in tenths of a centimetre.
 *
 * Examples:
 *
 * 5.5 cm  -> 55
 * 100 cm  -> 1000
 * 225 cm  -> 2250
 *
 *
 * TIMER CALCULATION @ 8 MHz
 * ---------------------------------------------------------
 *
 * Timer1 prescaler = 8
 *
 * Timer frequency:
 *
 * 8 MHz / 8 = 1 MHz
 *
 * Therefore:
 *
 * 1 timer tick = 1 us
 *
 *
 * HC-SR04:
 *
 * Distance(cm) = echo_time_us / 58
 *
 * Therefore:
 *
 * distance_x10 =
 *
 * echo_time_us * 10 / 58
 *
 * Since Timer1 count = echo_time_us:
 *
 * distance_x10 =
 *
 * count * 10 / 58
 */

uint16_t Ultrasonic_GetDistanceX10(void)
{
    uint16_t count;
    uint32_t timeout;

    /*
     * Make sure TRIG is LOW
     */

    TRIG_PORT &= ~(1 << TRIG_PIN);

    _delay_us(2);

    /*
     * 10 us trigger pulse
     */

    TRIG_PORT |= (1 << TRIG_PIN);

    _delay_us(10);

    TRIG_PORT &= ~(1 << TRIG_PIN);

    /*
     * Wait for ECHO HIGH
     */

    timeout = 0;

    while (!(ECHO_PINR & (1 << ECHO_PIN)))
    {
        _delay_us(1);

        timeout++;

        if (timeout >= 30000UL)
        {
            return 0;
        }
    }

    /*
     * Reset Timer1
     */

    TCNT1 = 0;

    /*
     * Start Timer1
     *
     * Prescaler = 8
     *
     * At 8 MHz:
     *
     * 8 MHz / 8 = 1 MHz
     *
     * 1 tick = 1 us
     */

    TCCR1B = (1 << CS11);

    /*
     * Wait for ECHO LOW
     */

    timeout = 0;

    while (ECHO_PINR & (1 << ECHO_PIN))
    {
        timeout++;

        if (timeout >= 30000UL)
        {
            TCCR1B = 0x00;

            return 0;
        }
    }

    /*
     * Stop Timer1
     */

    TCCR1B = 0x00;

    /*
     * Read Timer1
     */

    count = TCNT1;

    /*
     * At 8 MHz with prescaler 8:
     *
     * Timer tick = 1 us
     *
     * Distance(cm) = count / 58
     *
     * Distance in tenths of cm:
     *
     * count * 10 / 58
     */

    return (uint16_t)(((uint32_t)count * 10UL) / 58UL);
}


/* =========================================================
   SORT FIVE MEASUREMENTS
   ========================================================= */

void SortFive(uint16_t x[])
{
    uint8_t i;
    uint8_t j;

    uint16_t temp;

    for (i = 0; i < 4; i++)
    {
        for (j = i + 1; j < 5; j++)
        {
            if (x[j] < x[i])
            {
                temp = x[i];

                x[i] = x[j];

                x[j] = temp;
            }
        }
    }
}


/* =========================================================
   FILTER FIVE MEASUREMENTS
   =========================================================
 *
 * Lecturer's algorithm:
 *
 * 1. Read 5 measurements
 * 2. Sort smallest -> largest
 * 3. Calculate:
 *
 *    R1 = x[2] - x[0]
 *    R2 = x[3] - x[1]
 *    R3 = x[4] - x[2]
 *
 * 4. Select smallest range
 * 5. Keep corresponding 3 measurements
 * 6. Average those 3
 *
 * Result returned in tenths of a centimetre.
 */

uint16_t GetFilteredDistanceX10(void)
{
    uint16_t x[5];

    uint16_t range1;
    uint16_t range2;
    uint16_t range3;

    uint32_t sum;

    uint8_t i;


    /*
     * =====================================================
     * TAKE FIVE MEASUREMENTS
     * =====================================================
     */

    for (i = 0; i < 5; i++)
    {
        x[i] = Ultrasonic_GetDistanceX10();

        /*
         * Sensor failure
         */

        if (x[i] == 0)
        {
            return 0;
        }

        /*
         * Allow HC-SR04 echo to settle before
         * another measurement.
         */

        _delay_ms(60);
    }


    /*
     * =====================================================
     * SORT
     * =====================================================
     */

    SortFive(x);


    /*
     * =====================================================
     * CALCULATE RANGES
     * =====================================================
     */

    range1 = x[2] - x[0];

    range2 = x[3] - x[1];

    range3 = x[4] - x[2];


    /*
     * =====================================================
     * FIND CLOSEST GROUP OF THREE
     * =====================================================
     */

    if ((range1 <= range2) &&
        (range1 <= range3))
    {
        /*
         * G1:
         *
         * x[0], x[1], x[2]
         */

        sum =
            (uint32_t)x[0] +
            (uint32_t)x[1] +
            (uint32_t)x[2];
    }

    else if ((range2 <= range1) &&
             (range2 <= range3))
    {
        /*
         * G2:
         *
         * x[1], x[2], x[3]
         */

        sum =
            (uint32_t)x[1] +
            (uint32_t)x[2] +
            (uint32_t)x[3];
    }

    else
    {
        /*
         * G3:
         *
         * x[2], x[3], x[4]
         */

        sum =
            (uint32_t)x[2] +
            (uint32_t)x[3] +
            (uint32_t)x[4];
    }


    /*
     * =====================================================
     * AVERAGE RETAINED THREE
     * =====================================================
     */

    return (uint16_t)(sum / 3UL);
}


/* =========================================================
   CALCULATE WATER HEIGHT
   =========================================================
 *
 * distance_x10 =
 * distance from sensor to water surface
 *
 * water_height =
 * tank height - measured distance
 */

uint16_t CalculateWaterHeightX10(
    uint16_t distance_x10)
{
    uint16_t water_height;


    /*
     * Sensor reading beyond physical tank height
     */

    if (distance_x10 >= TANK_HEIGHT_X10)
    {
        return 0;
    }


    /*
     * Water height
     */

    water_height =
        TANK_HEIGHT_X10 - distance_x10;


    /*
     * Maximum usable water height:
     *
     * 220 cm
     *
     * Anything above this is considered
     * physically at full capacity.
     */

    if (water_height > MAX_WATER_HEIGHT_X10)
    {
        water_height = MAX_WATER_HEIGHT_X10;
    }


    return water_height;
}


/* =========================================================
   CALCULATE WATER LEVEL %
   ========================================================= */

uint8_t CalculateWaterLevel(
    uint16_t water_height_x10)
{
    uint32_t level;


    level =
        ((uint32_t)water_height_x10 * 100UL)
        / MAX_WATER_HEIGHT_X10;


    /*
     * Limit to 100%
     */

    if (level > 100)
    {
        level = 100;
    }


    return (uint8_t)level;
}


/* =========================================================
   CALCULATE OVERFILL VOLUME
   ========================================================= */

uint16_t CalculateOverfillVolumeML(
    uint16_t water_height_x10)
{
    uint16_t excess_height_x10;

    uint32_t volume_ml;


    /*
     * No overfill
     */

    if (water_height_x10 <= OVERFILL_HEIGHT_X10)
    {
        return 0;
    }


    /*
     * Determine how many cm above 2 m
     */

    excess_height_x10 =
        water_height_x10 -
        OVERFILL_HEIGHT_X10;


    /*
     * Volume:
     *
     * Area = 260.16 cm^2
     * Height = excess_height_x10 / 10 cm
     *
     * Area is stored as x100.
     *
     * Therefore:
     *
     * 26016 x height_x10
     * -------------------
     *       1000
     *
     * gives mL.
     */

    volume_ml =
        ((uint32_t)TANK_AREA_X100 *
         (uint32_t)excess_height_x10)
        / 1000UL;


    return (uint16_t)volume_ml;
}


/* =========================================================
   CALCULATE TOTAL WATER VOLUME
   =========================================================
 *
 * Returns volume in millilitres.
 */

uint16_t CalculateWaterVolumeML(
    uint16_t water_height_x10)
{
    uint32_t volume_ml;


    /*
     * Area = 260.16 cm^2
     *
     * Height = water_height_x10 / 10
     *
     * Since 1 cm^3 = 1 mL:
     *
     * volume_ml =
     *
     * 26016 x water_height_x10
     * -------------------------
     *           1000
     */

    volume_ml =
        ((uint32_t)TANK_AREA_X100 *
         (uint32_t)water_height_x10)
        / 1000UL;


    return (uint16_t)volume_ml;
}


/* =========================================================
   PRINT INTEGER
   ========================================================= */

void LCD_PrintNumber(uint16_t number)
{
    /*
     * Hundreds
     */

    if (number >= 100)
    {
        LCD_Data((number / 100) + '0');

        number %= 100;

        LCD_Data((number / 10) + '0');

        LCD_Data((number % 10) + '0');
    }

    /*
     * Tens
     */

    else if (number >= 10)
    {
        LCD_Data((number / 10) + '0');

        LCD_Data((number % 10) + '0');
    }

    /*
     * Single digit
     */

    else
    {
        LCD_Data(number + '0');
    }
}


/* =========================================================
   DISPLAY LEVEL AND VOLUME
   ========================================================= */

void LCD_ShowNormal(
    uint8_t level,
    uint16_t volume_ml)
{
    uint16_t litres;
    uint16_t tenths;


    /*
     * Convert volume to litres
     *
     * Example:
     *
     * 57235 mL
     * = 57.235 L
     *
     * Display one decimal place.
     */

    litres = volume_ml / 1000;

    tenths = (volume_ml % 1000) / 100;


    /*
     * LINE 1
     */

    LCD_Goto(0, 0);

    LCD_Print("Level: ");

    LCD_PrintNumber(level);

    LCD_Print("%");


    /*
     * LINE 2
     */

    LCD_Goto(1, 0);

    LCD_Print("Vol: ");

    LCD_PrintNumber(litres);

    LCD_Data('.');

    LCD_Data(tenths + '0');

    LCD_Print(" L");
}


/* =========================================================
   OVERFILL SCREEN
   ========================================================= */

void LCD_ShowOverfill(
    uint16_t overfill_ml)
{
    uint16_t litres;
    uint16_t decimal1;
    uint16_t decimal2;
    uint16_t decimal3;


    /*
     * Convert mL to litres
     *
     * Example:
     *
     * 1301 mL
     * = 1.301 L
     */

    litres = overfill_ml / 1000;

    decimal1 = (overfill_ml % 1000) / 100;

    decimal2 = (overfill_ml % 100) / 10;

    decimal3 = overfill_ml % 10;


    /*
     * LINE 1
     */

    LCD_Clear();

    LCD_Goto(0, 0);

    LCD_Print("Tank overfilled");


    /*
     * LINE 2
     */

    LCD_Goto(1, 0);

    LCD_Print("SAVE BY ");

    LCD_PrintNumber(litres);

    LCD_Data('.');

    LCD_Data(decimal1 + '0');

    LCD_Data(decimal2 + '0');

    LCD_Data(decimal3 + '0');

    LCD_Print(" L");
}


/* =========================================================
   LOW WATER SCREEN
   ========================================================= */

void LCD_ShowLowWater(void)
{
    LCD_Clear();

    LCD_Goto(0, 0);

    LCD_Print("LOW WATER LVL");

    LCD_Goto(1, 0);

    LCD_Print("REFILL");
}


/* =========================================================
   EMPTY TANK SCREEN
   ========================================================= */

void LCD_ShowEmpty(void)
{
    LCD_Clear();

    LCD_Goto(0, 0);

    LCD_Print("TANK EMPTY");

    LCD_Goto(1, 0);

    LCD_Print("URGENT");
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    uint16_t distance_x10;

    uint16_t water_height_x10;

    uint8_t water_level;

    uint16_t water_volume_ml;

    uint16_t overfill_volume_ml;


    /*
     * =====================================================
     * INITIALIZE I2C
     * =====================================================
     */

    I2C_Init();

    _delay_ms(100);


    /*
     * =====================================================
     * FIND LCD ADDRESS
     * =====================================================
     */

    lcd_address =
        LCD_Find_Address();


    /*
     * Fallback to 0x27
     */

    if (lcd_address == 0)
    {
        lcd_address =
            LCD_ADDRESS_2;
    }


    /*
     * =====================================================
     * INITIALIZE LCD
     * =====================================================
     */

    LCD_Init();


    /*
     * =====================================================
     * INITIALIZE HC-SR04
     * =====================================================
     */

    Ultrasonic_Init();


    /*
     * =====================================================
     * STARTUP SCREEN
     * =====================================================
     */

    LCD_Clear();

    LCD_Goto(0, 0);

    LCD_Print("WATER TANK");

    LCD_Goto(1, 0);

    LCD_Print("SYSTEM READY");

    _delay_ms(2000);


    /*
     * =====================================================
     * MAIN LOOP
     * =====================================================
     */

    while (1)
    {
        /*
         * -------------------------------------------------
         * TAKE 5 MEASUREMENTS AND FILTER THEM
         * -------------------------------------------------
         */

        distance_x10 =
            GetFilteredDistanceX10();


        /*
         * -------------------------------------------------
         * SENSOR ERROR
         * -------------------------------------------------
         */

        if (distance_x10 == 0)
        {
            LCD_Clear();

            LCD_Goto(0, 0);

            LCD_Print("SENSOR ERROR");

            LCD_Goto(1, 0);

            LCD_Print("CHECK HC-SR04");

            _delay_ms(1500);

            continue;
        }


        /*
         * -------------------------------------------------
         * CALCULATE WATER HEIGHT
         * -------------------------------------------------
         */

        water_height_x10 =
            CalculateWaterHeightX10(
                distance_x10
            );


        /*
         * -------------------------------------------------
         * CALCULATE LEVEL %
         * -------------------------------------------------
         */

        water_level =
            CalculateWaterLevel(
                water_height_x10
            );


        /*
         * -------------------------------------------------
         * CALCULATE TOTAL VOLUME
         * -------------------------------------------------
         */

        water_volume_ml =
            CalculateWaterVolumeML(
                water_height_x10
            );


        /*
         * -------------------------------------------------
         * 1. OVERFILL DETECTION
         *
         * Water height > 200 cm
         * -------------------------------------------------
         */

        if (water_height_x10 >
            OVERFILL_HEIGHT_X10)
        {
            /*
             * Calculate excess volume
             * above 2 m.
             */

            overfill_volume_ml =
                CalculateOverfillVolumeML(
                    water_height_x10
                );


            LCD_ShowOverfill(
                overfill_volume_ml
            );


            /*
             * Keep warning visible
             */

            _delay_ms(2000);
        }


        /*
         * -------------------------------------------------
         * 2. EMPTY TANK
         *
         * 0%
         * -------------------------------------------------
         */

        else if (water_level == 0)
        {
            LCD_ShowEmpty();

            _delay_ms(2000);
        }


        /*
         * -------------------------------------------------
         * 3. LOW WATER
         *
         * 20% or less
         * -------------------------------------------------
         */

        else if (water_level <= 20)
        {
            LCD_ShowLowWater();

            _delay_ms(2000);
        }


        /*
         * -------------------------------------------------
         * 4. NORMAL DISPLAY
         * -------------------------------------------------
         */

        else
        {
            LCD_Clear();

            LCD_ShowNormal(
                water_level,
                water_volume_ml
            );

            _delay_ms(500);
        }
    }
}