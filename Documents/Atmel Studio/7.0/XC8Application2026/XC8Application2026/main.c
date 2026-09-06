#define F_CPU 8000000UL   // Adjusted for 8 MHz clock

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

#define LCD_I2C_ADDRESS_1 0x27
#define LCD_I2C_ADDRESS_2 0x3F

#define TRIG_PORT PORTD
#define TRIG_DDR  DDRD
#define TRIG_PIN  PD2

#define ECHO_DDR  DDRD
#define ECHO_PIN  PD3

#define BATTERY_ADC_CHANNEL 1

// Set reference voltage to internal 1.1V
#define ADC_REFERENCE_VOLTAGE 1.1

// Divider resistors
#define R1_OHMS 47000UL
#define R2_OHMS 10000UL

#define BATTERY_EMPTY_VOLTAGE 4.0
#define BATTERY_FULL_VOLTAGE  6.0

#define TANK_HEIGHT_CM 225.5
#define TANK_AREA_CM2 260.16

uint8_t lcd_address;

/* I2C FUNCTIONS */

void I2C_Init(void)
{
	TWSR = 0x00;
	TWBR = 32; // SCL 100kHz at 8MHz
	TWCR = (1 << TWEN);
}

uint8_t I2C_Start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWSR & 0xF8;
}

void I2C_Stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	_delay_us(10);
}

uint8_t I2C_Write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWSR & 0xF8;
}

uint8_t LCD_Find_Address(void)
{
	uint8_t status;

	status = I2C_Start();
	if (status != 0x08 && status != 0x10)
	{
		I2C_Stop();
		return 0;
	}

	if (I2C_Write(LCD_I2C_ADDRESS_1 << 1) == 0x18)
	{
		I2C_Stop();
		return LCD_I2C_ADDRESS_1;
	}

	I2C_Stop();

	status = I2C_Start();
	if (status != 0x08 && status != 0x10)
	{
		I2C_Stop();
		return 0;
	}

	if (I2C_Write(LCD_I2C_ADDRESS_2 << 1) == 0x18)
	{
		I2C_Stop();
		return LCD_I2C_ADDRESS_2;
	}

	I2C_Stop();
	return 0;
}

/* LCD FUNCTIONS */
// (unchanged from your original code)

/* ULTRASONIC SENSOR – Adjusted for 8 MHz */

void Ultrasonic_Init(void)
{
	TRIG_DDR |= (1 << TRIG_PIN);
	ECHO_DDR &= ~(1 << ECHO_PIN);
	TRIG_PORT &= ~(1 << TRIG_PIN);
}

uint16_t Ultrasonic_Read(void)
{
	uint32_t timeout = 30000;          // ~30 ms
	uint16_t ticks;
	uint16_t distance_cm;

	// Send 10 µs pulse on Trigger Pin
	TRIG_PORT &= ~(1 << TRIG_PIN);
	_delay_us(2);
	TRIG_PORT |= (1 << TRIG_PIN);
	_delay_us(10);
	TRIG_PORT &= ~(1 << TRIG_PIN);

	// Wait for ECHO pin to go HIGH (with timeout)
	while (!(PIND & (1 << ECHO_PIN)))
	{
		if (--timeout == 0)
		return 0;
		_delay_us(1);
	}

	// Start Timer1 with Prescaler = 8 (1 tick = 1 µs at 8 MHz)
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = (1 << CS11);

	// Wait for ECHO pin to go LOW
	while (PIND & (1 << ECHO_PIN))
	{
		if (TIFR1 & (1 << TOV1))
		{
			TIFR1 |= (1 << TOV1);
			TCCR1B = 0;
			return 0;
		}
		if (TCNT1 > 60000)
		{
			TCCR1B = 0;
			return 0;
		}
	}

	// Stop Timer1
	TCCR1B = 0;
	ticks = TCNT1;

	if (ticks == 0)
	return 0;

	// Distance calculation for 8 MHz:
	// Round to nearest cm: (ticks * 1) / 58 ? ticks / 58
	distance_cm = (ticks + 58) / 58;
	if (distance_cm == 0)
	distance_cm = 1;

	return distance_cm;
}

/* ADC, Battery, Tank functions remain unchanged */

/* MAIN */
// (unchanged except F_CPU and ultrasonic formula)
