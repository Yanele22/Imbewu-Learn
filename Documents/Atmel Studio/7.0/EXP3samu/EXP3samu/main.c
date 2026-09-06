/*
 * main.c
 *
 * Created: 4/23/2026 10:47:55 PM
 *  Author: samke
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define PCF8574_ADDR 0x20

void TWI_init(void)
{
	TWSR = 0x00;
	TWBR = 72; // 100kHz
	TWCR = (1 << TWEN);
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

void PCF8574_write(unsigned char data)
{
	TWI_start();
	TWI_write((PCF8574_ADDR << 1) | 0); // write
	TWI_write(data);
	TWI_stop();
}

#define CS_LOW()  PORTB &= ~(1<<PB2)
#define CS_HIGH() PORTB |= (1<<PB2)

// Registers
#define IODIR 0x00
#define GPIO  0x09

void SPI_init(void)
{
	DDRB |= (1<<PB2)|(1<<PB3)|(1<<PB5); // CS, MOSI, SCK output
	DDRB &= ~(1<<PB4); // MISO input

	PORTB |= (1<<PB2); // CS HIGH

	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); // SPI enable, master, f/16
}

unsigned char SPI_transfer(unsigned char data)
{
	SPDR = data;
	while (!(SPSR & (1<<SPIF)));
	return SPDR;
}

// Write to MCP23S08
void MCP_write(unsigned char reg, unsigned char data)
{
	CS_LOW();
	SPI_transfer(0x40); // opcode write
	SPI_transfer(reg);
	SPI_transfer(data);
	CS_HIGH();
}

// Read from MCP23S08
unsigned char MCP_read(unsigned char reg)
{
	unsigned char data;

	CS_LOW();
	SPI_transfer(0x41); // opcode read
	SPI_transfer(reg);
	data = SPI_transfer(0x00);
	CS_HIGH();

	return data;
}


int main(void)
{
	unsigned char input, lower, output;

	TWI_init();
	SPI_init();

	// Configure MCP23S08:
	// Lower 4 bits input (switches), upper 4 bits output
	MCP_write(IODIR, 0x0F);

	while (1)
	{
		// Read switches from MCP23S08
		input = MCP_read(GPIO);

		lower = input & 0x0F;   // get P0–P3
		output = lower << 4;    // move to upper nibble

		// Send to PCF8574 (LEDs)
		PCF8574_write(output);

		_delay_ms(50);
	}
}