/*
 * main.c
 *
 * Created: 4/23/2026 7:50:46 PM
 *  Author: samke
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>

// PCF8574 address
#define PCF8574_ADDR 0x20


void TWI_init(void)
{
	TWSR = 0x00;                  // Prescaler = 1
	TWBR = 72;                    // 100kHz for 16MHz clock
	TWCR = (1 << TWEN);           // Enable TWI
}

void TWI_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

void TWI_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void TWI_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

unsigned char TWI_read_NACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}


void PCF8574_write(unsigned char data)
{
	TWI_start();
	TWI_write((PCF8574_ADDR << 1) | 0); // Write mode
	TWI_write(data);
	TWI_stop();
}

unsigned char PCF8574_read(void)
{
	unsigned char data;

	TWI_start();
	TWI_write((PCF8574_ADDR << 1) | 1); // Read mode
	data = TWI_read_NACK();
	TWI_stop();

	return data;
}


int main(void)
{
	unsigned char input, lower, output;

	TWI_init();

	// Set lower nibble as input (write 1s)
	PCF8574_write(0x0F);

	while (1)
	{
		input = PCF8574_read();   // Read switches

		lower = input & 0x0F;     // Extract lower nibble
		output = lower << 4;      // Move to upper nibble

		PCF8574_write(output);    // Display on LEDs
	}
}