/*
 * ============================================================
 * SMART WATER TANK MONITORING AND CONTROL SYSTEM
 * ============================================================
 *
 * Microcontroller : ATmega328P
 * Supply voltage  : 3.3 V
 * Clock           : 8 MHz
 * Compiler        : AVR-GCC / Microchip Studio
 * Framework       : Register-level AVR C
 *
 * Main functions:
 * 1. Ultrasonic water-level measurement
 * 2. Five-sample median filtering
 * 3. Water-level percentage calculation
 * 4. Battery voltage measurement using ADC
 * 5. Battery percentage estimation
 * 6. Sensor timeout detection
 * 7. Invalid-distance detection
 * 8. Consecutive sensor fault detection
 * 9. Pump ON/OFF control
 * 10. Pump hysteresis
 * 11. LCD status display
 * 12. Filling timeout protection
 *
 * ============================================================
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * PIN DEFINITIONS
 * ============================================================
 */

/* ---------- Ultrasonic Sensor ---------- */

#define TRIG_PORT       PORTD
#define TRIG_DDR        DDRD
#define TRIG_PIN        PD2

#define ECHO_PORT       PORTD
#define ECHO_DDR        DDRD
#define ECHO_PIN        PD3


/* ---------- Pump Driver ---------- */

#define PUMP_PORT       PORTB
#define PUMP_DDR        DDRB
#define PUMP_PIN        PB0


/* ---------- LCD ---------- */

#define LCD_PORT        PORTD
#define LCD_DDR         DDRD

#define LCD_RS          PB1
#define LCD_EN          PB2

#define LCD_D4          PD4
#define LCD_D5          PD5
#define LCD_D6          PD6
#define LCD_D7          PD7


/* ---------- Battery ADC ---------- */

/*
 * Battery voltage divider:
 *
 * R1 = 1 M?
 * R2 = 1 M?
 *
 * VADC = VBAT / 2
 *
 * ADC1 = PC1
 */

#define BATTERY_ADC_CHANNEL      1

#define ADC_REFERENCE_MV         3300UL

#define R1_OHMS                 1000000UL
#define R2_OHMS                 1000000UL


/* ============================================================
 * SYSTEM SETTINGS
 * ============================================================
 */

/* Tank calibration values */

#define SENSOR_EMPTY_DISTANCE_CM    100.0f
#define SENSOR_FULL_DISTANCE_CM      20.0f


/* Pump thresholds */

#define PUMP_ON_LEVEL_PERCENT       30
#define PUMP_OFF_LEVEL_PERCENT      80


/* Sensor fault settings */

#define MAX_INVALID_READINGS        3


/* Filling timeout */

#define FILLING_TIMEOUT_SECONDS     600UL


/* Battery limits */

#define BATTERY_FULL_MV             6000UL
#define BATTERY_EMPTY_MV            4200UL


/* ============================================================
 * GLOBAL VARIABLES
 * ============================================================
 */

volatile uint32_t system_seconds = 0;

volatile bool filling_timer_running = false;

volatile uint32_t filling_start_time = 0;

bool pump_state = false;

uint8_t invalid_sensor_count = 0;


/* ============================================================
 * TIMER INTERRUPT
 * ============================================================
 *
 * Timer1 generates a 1-second system time base.
 *
 * F_CPU = 8 MHz
 * Prescaler = 1024
 *
 * Timer frequency:
 *
 * 8,000,000 / 1024 = 7812.5 Hz
 *
 * OCR1A = 7811 gives approximately 1 second.
 *
 * ============================================================
 */

ISR(TIMER1_COMPA_vect)
{
    system_seconds++;

    /*
     * Filling timeout is checked here.
     */

    if (filling_timer_running)
    {
        if ((system_seconds - filling_start_time)
            >= FILLING_TIMEOUT_SECONDS)
        {
            /*
             * Timeout condition.
             *
             * Pump is immediately switched OFF.
             */

            PUMP_PORT &= ~(1 << PUMP_PIN);

            pump_state = false;

            filling_timer_running = false;
        }
    }
}


/* ============================================================
 * TIMER1 INITIALIZATION
 * ============================================================
 */

void timer1_init(void)
{
    TCCR1A = 0;

    TCCR1B = 0;

    /*
     * CTC mode
     */

    TCCR1B |= (1 << WGM12);

    /*
     * 1024 prescaler
     */

    TCCR1B |= (1 << CS12);
    TCCR1B |= (1 << CS10);

    /*
     * Approximately 1 second
     */

    OCR1A = 7811;

    /*
     * Enable compare interrupt
     */

    TIMSK1 |= (1 << OCIE1A);
}


/* ============================================================
 * ADC INITIALIZATION
 * ============================================================
 *
 * AVCC = 3.3 V
 *
 * ADC reference = AVCC
 *
 * 10-bit ADC
 *
 * ============================================================
 */

void adc_init(void)
{
    /*
     * AVCC used as ADC reference.
     *
     * REFS0 = 1
     */

    ADMUX = (1 << REFS0);

    /*
     * Enable ADC
     *
     * Prescaler = 64
     *
     * 8 MHz / 64 = 125 kHz ADC clock
     */

    ADCSRA =
        (1 << ADEN)  |
        (1 << ADPS2) |
        (1 << ADPS1);

    /*
     * Disable digital input buffer on ADC1.
     */

    DIDR0 |= (1 << ADC1D);
}


/* ============================================================
 * ADC READ
 * ============================================================
 */

uint16_t adc_read(uint8_t channel)
{
    /*
     * Select ADC channel.
     */

    ADMUX =
        (ADMUX & 0xF0) |
        (channel & 0x0F);

    /*
     * Start conversion.
     */

    ADCSRA |= (1 << ADSC);

    /*
     * Wait for conversion.
     */

    while (ADCSRA & (1 << ADSC))
    {
        ;
    }

    return ADC;
}


/* ============================================================
 * BATTERY VOLTAGE MEASUREMENT
 * ============================================================
 *
 * R1 = 1 M?
 * R2 = 1 M?
 *
 * VADC = VBAT * R2/(R1+R2)
 *
 * VADC = VBAT/2
 *
 * Therefore:
 *
 * VBAT = 2 * VADC
 *
 * ============================================================
 */

uint16_t read_battery_voltage_mV(void)
{
    uint16_t adc_value;

    uint32_t adc_voltage_mV;

    uint32_t battery_voltage_mV;


    adc_value = adc_read(BATTERY_ADC_CHANNEL);


    /*
     * ADC voltage:
     *
     * VADC = ADC * VREF / 1023
     */

    adc_voltage_mV =
        ((uint32_t)adc_value * ADC_REFERENCE_MV)
        / 1023UL;


    /*
     * Voltage divider:
     *
     * VBAT =
     * VADC * (R1 + R2) / R2
     *
     * For R1 = R2:
     *
     * VBAT = 2 * VADC
     */

    battery_voltage_mV =
        adc_voltage_mV *
        (R1_OHMS + R2_OHMS) /
        R2_OHMS;


    return (uint16_t)battery_voltage_mV;
}


/* ============================================================
 * BATTERY PERCENTAGE
 * ============================================================
 *
 * This is a simplified linear mapping.
 *
 * For the final project, replace these limits with the
 * documented discharge curve of the selected battery chemistry.
 *
 * ============================================================
 */

uint8_t battery_percentage(uint16_t voltage_mV)
{
    int32_t percentage;


    if (voltage_mV >= BATTERY_FULL_MV)
    {
        return 100;
    }


    if (voltage_mV <= BATTERY_EMPTY_MV)
    {
        return 0;
    }


    percentage =
        ((int32_t)(voltage_mV - BATTERY_EMPTY_MV) * 100)
        /
        (BATTERY_FULL_MV - BATTERY_EMPTY_MV);


    return (uint8_t)percentage;
}


/* ============================================================
 * ULTRASONIC SENSOR INITIALIZATION
 * ============================================================
 */

void ultrasonic_init(void)
{
    /*
     * TRIG = output
     */

    TRIG_DDR |= (1 << TRIG_PIN);

    /*
     * ECHO = input
     */

    ECHO_DDR &= ~(1 << ECHO_PIN);

    /*
     * Make sure trigger is LOW.
     */

    TRIG_PORT &= ~(1 << TRIG_PIN);
}


/* ============================================================
 * ULTRASONIC TRIGGER
 * ============================================================
 */

void ultrasonic_trigger(void)
{
    /*
     * Ensure trigger is LOW.
     */

    TRIG_PORT &= ~(1 << TRIG_PIN);

    _delay_us(2);


    /*
     * 10 us trigger pulse.
     */

    TRIG_PORT |= (1 << TRIG_PIN);

    _delay_us(10);

    TRIG_PORT &= ~(1 << TRIG_PIN);
}


/* ============================================================
 * MEASURE ULTRASONIC DISTANCE
 * ============================================================
 *
 * Returns:
 *
 * distance in cm
 *
 * Returns 0 when a timeout occurs.
 *
 * ============================================================
 */

uint16_t ultrasonic_read_cm(void)
{
    uint32_t timeout;

    uint16_t pulse_width;


    ultrasonic_trigger();


    /*
     * Wait for ECHO to become HIGH.
     */

    timeout = 0;

    while (!(ECHO_PORT & (1 << ECHO_PIN)))
    {
        _delay_us(1);

        timeout++;

        /*
         * Timeout protection.
         */

        if (timeout > 30000UL)
        {
            return 0;
        }
    }


    /*
     * Measure HIGH pulse width.
     */

    pulse_width = 0;

    while (ECHO_PORT & (1 << ECHO_PIN))
    {
        _delay_us(1);

        pulse_width++;

        /*
         * Maximum measurement timeout.
         */

        if (pulse_width > 30000)
        {
            return 0;
        }
    }


    /*
     * For approximately 1 us timing:
     *
     * distance(cm) ? pulse_width / 58
     */

    return pulse_width / 58;
}


/* ============================================================
 * DISTANCE VALIDATION
 * ============================================================
 */

bool valid_distance(uint16_t distance_cm)
{
    if (distance_cm == 0)
    {
        return false;
    }


    /*
     * Distance must be physically within the tank
     * measurement range.
     */

    if (distance_cm < 5)
    {
        return false;
    }


    if (distance_cm > 400)
    {
        return false;
    }


    return true;
}


/* ============================================================
 * MEDIAN FILTER
 * ============================================================
 */

uint16_t median5(uint16_t values[5])
{
    uint16_t temp;

    uint8_t i;
    uint8_t j;


    /*
     * Sort the five values.
     */

    for (i = 0; i < 5; i++)
    {
        for (j = i + 1; j < 5; j++)
        {
            if (values[j] < values[i])
            {
                temp = values[i];

                values[i] = values[j];

                values[j] = temp;
            }
        }
    }


    /*
     * Middle value = median.
     */

    return values[2];
}


/* ============================================================
 * FILTERED DISTANCE
 * ============================================================
 */

bool get_filtered_distance(uint16_t *result)
{
    uint16_t samples[5];

    uint8_t i;


    /*
     * Acquire five valid measurements.
     */

    for (i = 0; i < 5; i++)
    {
        samples[i] = ultrasonic_read_cm();


        if (!valid_distance(samples[i]))
        {
            return false;
        }


        /*
         * Small delay between samples.
         */

        _delay_ms(50);
    }


    /*
     * Median filtering.
     */

    *result = median5(samples);


    return true;
}


/* ============================================================
 * WATER LEVEL CALCULATION
 * ============================================================
 *
 * Sensor measures distance from sensor to water.
 *
 * Empty tank distance:
 *
 * SENSOR_EMPTY_DISTANCE_CM
 *
 * Full tank distance:
 *
 * SENSOR_FULL_DISTANCE_CM
 *
 * Water level:
 *
 * Level =
 *
 * (EmptyDistance - CurrentDistance)
 * /
 * (EmptyDistance - FullDistance)
 *
 * × 100
 *
 * ============================================================
 */

uint8_t calculate_water_level(uint16_t distance_cm)
{
    float level;


    level =
        ((float)SENSOR_EMPTY_DISTANCE_CM -
         (float)distance_cm)
        /
        (SENSOR_EMPTY_DISTANCE_CM -
         SENSOR_FULL_DISTANCE_CM);


    level *= 100.0f;


    /*
     * Clamp to 0–100%.
     */

    if (level < 0.0f)
    {
        level = 0.0f;
    }


    if (level > 100.0f)
    {
        level = 100.0f;
    }


    return (uint8_t)level;
}


/* ============================================================
 * PUMP CONTROL
 * ============================================================
 *
 * Pump ON:
 *
 * Level <= PUMP_ON_LEVEL_PERCENT
 *
 * Pump OFF:
 *
 * Level >= PUMP_OFF_LEVEL_PERCENT
 *
 * Between the two thresholds, the previous pump state
 * is maintained.
 *
 * ============================================================
 */

void pump_control(uint8_t water_level)
{
    /*
     * Low water level.
     *
     * Start filling.
     */

    if (water_level <= PUMP_ON_LEVEL_PERCENT)
    {
        if (!pump_state)
        {
            PUMP_PORT |= (1 << PUMP_PIN);

            pump_state = true;

            /*
             * Start filling timer.
             */

            filling_timer_running = true;

            filling_start_time = system_seconds;
        }
    }


    /*
     * High water level.
     *
     * Stop filling.
     */

    else if (water_level >= PUMP_OFF_LEVEL_PERCENT)
    {
        PUMP_PORT &= ~(1 << PUMP_PIN);

        pump_state = false;

        filling_timer_running = false;
    }


    /*
     * Between thresholds:
     *
     * Maintain previous pump state.
     */
}


/* ============================================================
 * LCD LOW-LEVEL FUNCTIONS
 * ============================================================
 */

void lcd_enable_pulse(void)
{
    PORTB |= (1 << LCD_EN);

    _delay_us(1);

    PORTB &= ~(1 << LCD_EN);

    _delay_us(100);
}


/* ============================================================
 * LCD SEND 4-BIT DATA
 * ============================================================
 */

void lcd_send_nibble(uint8_t data)
{
    /*
     * Clear LCD data pins.
     */

    LCD_PORT &= ~(
        (1 << LCD_D4) |
        (1 << LCD_D5) |
        (1 << LCD_D6) |
        (1 << LCD_D7)
    );


    /*
     * Set data pins.
     */

    if (data & 0x01)
        LCD_PORT |= (1 << LCD_D4);

    if (data & 0x02)
        LCD_PORT |= (1 << LCD_D5);

    if (data & 0x04)
        LCD_PORT |= (1 << LCD_D6);

    if (data & 0x08)
        LCD_PORT |= (1 << LCD_D7);


    lcd_enable_pulse();
}


/* ============================================================
 * LCD COMMAND
 * ============================================================
 */

void lcd_command(uint8_t command)
{
    PORTB &= ~(1 << LCD_RS);

    lcd_send_nibble(command >> 4);

    lcd_send_nibble(command & 0x0F);

    _delay_ms(2);
}


/* ============================================================
 * LCD CHARACTER
 * ============================================================
 */

void lcd_data(uint8_t data)
{
    PORTB |= (1 << LCD_RS);

    lcd_send_nibble(data >> 4);

    lcd_send_nibble(data & 0x0F);

    _delay_us(100);
}


/* ============================================================
 * LCD STRING
 * ============================================================
 */

void lcd_print(const char *str)
{
    while (*str)
    {
        lcd_data(*str);

        str++;
    }
}


/* ============================================================
 * LCD NUMBER
 * ============================================================
 */

void lcd_print_number(uint16_t number)
{
    char buffer[6];

    uint8_t i = 0;


    if (number == 0)
    {
        lcd_data('0');

        return;
    }


    while (number > 0)
    {
        buffer[i++] = '0' + (number % 10);

        number /= 10;
    }


    while (i > 0)
    {
        lcd_data(buffer[--i]);
    }
}


/* ============================================================
 * LCD INITIALIZATION
 * ============================================================
 */

void lcd_init(void)
{
    /*
     * LCD control pins as outputs.
     */

    DDRB |=
        (1 << LCD_RS) |
        (1 << LCD_EN);


    /*
     * LCD data pins as outputs.
     */

    LCD_DDR |=
        (1 << LCD_D4) |
        (1 << LCD_D5) |
        (1 << LCD_D6) |
        (1 << LCD_D7);


    _delay_ms(20);


    /*
     * Initialize LCD in 4-bit mode.
     */

    PORTB &= ~(1 << LCD_RS);


    lcd_send_nibble(0x03);

    _delay_ms(5);


    lcd_send_nibble(0x03);

    _delay_us(150);


    lcd_send_nibble(0x03);


    lcd_send_nibble(0x02);


    /*
     * 4-bit, 2-line display.
     */

    lcd_command(0x28);


    /*
     * Display ON, cursor OFF.
     */

    lcd_command(0x0C);


    /*
     * Clear display.
     */

    lcd_command(0x01);

    _delay_ms(2);


    /*
     * Entry mode.
     */

    lcd_command(0x06);
}


/* ============================================================
 * LCD SET CURSOR
 * ============================================================
 */

void lcd_set_cursor(uint8_t row, uint8_t column)
{
    uint8_t address;


    if (row == 0)
    {
        address = 0x00 + column;
    }

    else
    {
        address = 0x40 + column;
    }


    lcd_command(0x80 | address);
}


/* ============================================================
 * DISPLAY SYSTEM STATUS
 * ============================================================
 */

void display_status(
    uint8_t level,
    uint16_t battery_mV,
    bool pump_on,
    bool fault
)
{
    lcd_command(0x01);

    _delay_ms(2);


    /*
     * Line 1:
     *
     * Level: XX%
     */

    lcd_set_cursor(0, 0);

    lcd_print("Level:");

    lcd_print_number(level);

    lcd_print("%");


    /*
     * Line 2:
     *
     * Battery voltage / pump status
     */

    lcd_set_cursor(1, 0);

    lcd_print("Bat:");

    lcd_print_number(battery_mV / 1000);

    lcd_data('.');

    lcd_print_number((battery_mV % 1000) / 100);

    lcd_print("V");


    if (fault)
    {
        lcd_set_cursor(0, 11);

        lcd_print("FAULT");
    }

    else if (pump_on)
    {
        lcd_set_cursor(1, 10);

        lcd_print("P:ON");
    }

    else
    {
        lcd_set_cursor(1, 10);

        lcd_print("P:OFF");
    }
}


/* ============================================================
 * SAFE STATE
 * ============================================================
 */

void system_safe_state(void)
{
    /*
     * Turn pump OFF.
     */

    PUMP_PORT &= ~(1 << PUMP_PIN);

    pump_state = false;

    filling_timer_running = false;
}


/* ============================================================
 * PORT INITIALIZATION
 * ============================================================
 */

void gpio_init(void)
{
    /*
     * Pump output.
     */

    PUMP_DDR |= (1 << PUMP_PIN);

    /*
     * Pump initially OFF.
     */

    PUMP_PORT &= ~(1 << PUMP_PIN);


    /*
     * Ultrasonic sensor.
     */

    ultrasonic_init();
}


/* ============================================================
 * MAIN PROGRAM
 * ============================================================
 */

int main(void)
{
    uint16_t distance_cm;

    uint8_t water_level;

    uint16_t battery_voltage;

    uint8_t battery_level;

    bool sensor_fault;


    /*
     * Initialize hardware.
     */

    gpio_init();

    adc_init();

    timer1_init();

    lcd_init();


    /*
     * Start in safe state.
     */

    system_safe_state();


    /*
     * Enable global interrupts.
     */

    sei();


    /*
     * Startup message.
     */

    lcd_command(0x01);

    lcd_set_cursor(0, 0);

    lcd_print("SMART WATER");

    lcd_set_cursor(1, 0);

    lcd_print("SYSTEM START");

    _delay_ms(2000);


    /* ========================================================
     * MAIN CONTROL LOOP
     * ========================================================
     */

    while (1)
    {
        /*
         * ----------------------------------------------------
         * 1. Read ultrasonic sensor
         * ----------------------------------------------------
         */

        sensor_fault =
            !get_filtered_distance(&distance_cm);


        /*
         * ----------------------------------------------------
         * 2. Sensor fault detection
         * ----------------------------------------------------
         */

        if (sensor_fault)
        {
            /*
             * Increase consecutive failure counter.
             */

            if (invalid_sensor_count <
                MAX_INVALID_READINGS)
            {
                invalid_sensor_count++;
            }


            /*
             * Three consecutive failures:
             *
             * Put pump into safe OFF state.
             */

            if (invalid_sensor_count >=
                MAX_INVALID_READINGS)
            {
                system_safe_state();
            }


            /*
             * Display fault.
             */

            battery_voltage =
                read_battery_voltage_mV();


            display_status(
                0,
                battery_voltage,
                false,
                true
            );


            _delay_ms(500);


            continue;
        }


        /*
         * ----------------------------------------------------
         * 3. Valid sensor reading
         * ----------------------------------------------------
         */

        invalid_sensor_count = 0;


        /*
         * ----------------------------------------------------
         * 4. Convert distance to water-level percentage
         * ----------------------------------------------------
         */

        water_level =
            calculate_water_level(distance_cm);


        /*
         * ----------------------------------------------------
         * 5. Read battery voltage
         * ----------------------------------------------------
         */

        battery_voltage =
            read_battery_voltage_mV();


        /*
         * ----------------------------------------------------
         * 6. Calculate battery percentage
         * ----------------------------------------------------
         */

        battery_level =
            battery_percentage(battery_voltage);


        /*
         * ----------------------------------------------------
         * 7. Pump control
         * ----------------------------------------------------
         */

        pump_control(water_level);


        /*
         * ----------------------------------------------------
         * 8. Display system information
         * ----------------------------------------------------
         */

        display_status(
            water_level,
            battery_voltage,
            pump_state,
            false
        );


        /*
         * ----------------------------------------------------
         * 9. Main loop delay
         * ----------------------------------------------------
         */

        _delay_ms(500);
    }


    return 0;
}