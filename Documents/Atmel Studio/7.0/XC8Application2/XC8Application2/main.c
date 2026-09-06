#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

// ================= BARE-METAL LCD DRIVER =================
// D4->PD5, D5->PD4, D6->PD3, D7->PD2 (PORTD bits shifted via mask layout)
// RS->PB0, EN->PB3 (PORTB control lines)

void LCD_Enable(void) {
	PORTB |= (1<<PB3);   // EN High
	_delay_us(1);
	PORTB &= ~(1<<PB3);  // EN Low
	_delay_ms(2);
}

void LCD_Send4Bit(uint8_t data) {
	// Isolate current PORTD lower pins (UART lines), overwrite top nibble space
	PORTD = (PORTD & 0xC3) | ((data & 0x10) << 1) | ((data & 0x20) >> 1) | ((data & 0x40) >> 3) | ((data & 0x80) >> 5);
	LCD_Enable();
}

void LCD_Command(uint8_t cmd) {
	PORTB &= ~(1<<PB0);  // RS Low (Command Mode)
	LCD_Send4Bit(cmd);   // High Nibble
	LCD_Send4Bit(cmd<<4); // Low Nibble
}

void LCD_Char(char data) {
	PORTB |= (1<<PB0);   // RS High (Data Mode)
	LCD_Send4Bit(data);   // High Nibble
	LCD_Send4Bit(data<<4); // Low Nibble
}

void LCD_String(char *str) {
	while(*str) LCD_Char(*str++);
}

void LCD_Init() {
	// Configure data orientation directions for PORTD pins 2,3,4,5 as output paths
	DDRD |= 0x3C;
	DDRB |= (1<<PB0) | (1<<PB3); // RS and EN outputs
	_delay_ms(20);

	LCD_Send4Bit(0x30); _delay_ms(5);
	LCD_Send4Bit(0x30); _delay_us(100);
	LCD_Send4Bit(0x30); _delay_us(100);
	LCD_Send4Bit(0x20); // Force into 4-bit operational mode configuration

	LCD_Command(0x28); // 2-line mode, 5x8 matrix layout setup
	LCD_Command(0x0C); // Display ON, Cursor Blinking line Hidden
	LCD_Command(0x06); // Auto shift memory entry right pointer
	LCD_Command(0x01); // Frame clear wipe
}

// ================= BARE-METAL I2C / RTC DRIVER =================
void I2C_Init() {
	TWSR = 0x00; // Prescaler = 1
	TWBR = 0x47; // 100kHz standard speed bus setup mapping criteria
}

void I2C_Start() {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

void I2C_Stop() {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void I2C_Write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

uint8_t I2C_Read(uint8_t ack) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(ack<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}

uint8_t bcd_to_dec(uint8_t val) { return (val>>4)*10 + (val & 0x0F); }
uint8_t dec_to_bcd(uint8_t val) { return ((val/10)<<4) | (val%10); }

void DS1307_SetTime(uint8_t hr, uint8_t min, uint8_t sec) {
	I2C_Start(); I2C_Write(0xD0); I2C_Write(0x00);
	I2C_Write(dec_to_bcd(sec));  // Clear CH bit to ensure clock counts upward
	I2C_Write(dec_to_bcd(min));
	I2C_Write(dec_to_bcd(hr));
	I2C_Stop();
}

void DS1307_GetTime(char *t) {
	uint8_t sec, min, hour;
	I2C_Start(); I2C_Write(0xD0); I2C_Write(0x00); I2C_Stop();
	I2C_Start(); I2C_Write(0xD1);
	sec  = bcd_to_dec(I2C_Read(1));
	min  = bcd_to_dec(I2C_Read(1));
	hour = bcd_to_dec(I2C_Read(0));
	I2C_Stop();
	sprintf(t, "%02d:%02d:%02d", hour, min, sec);
}

// ================= UART BUS STRING INTERRUPT RECEIVER =================
void UART_Init_Rx(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXEN0)|(1<<RXCIE0); // Enable Rx circuitry and line interrupts
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

volatile char rxString[32];
volatile uint8_t rxIndex = 0;
volatile uint8_t dataReadyFlag = 0;

ISR(USART_RX_vect) {
	char receivedChar = UDR0;
	if (receivedChar == '\n' || rxIndex >= 31) {
		rxString[rxIndex] = '\0';
		rxIndex = 0;
		dataReadyFlag = 1; // Mark packet as complete for processing
		} else {
		rxString[rxIndex++] = receivedChar;
	}
}

// ================= SYSTEM EXECUTIVE CONTROL MAIN =================
int main() {
	char clockString[16], uiLine1[20], uiLine2[20];
	float tempVal = 0.0;
	int lightVal = 500;
	
	// Configure PB1(Green LED) and PB2(Red LED) as output lanes
	DDRB |= (1<<PB1) | (1<<PB2);
	
	LCD_Init();
	I2C_Init();
	UART_Init_Rx(103); // Match 9600 baud execution space
	sei();             // Enable global CPU interrupt flags
	
	// Establish target benchmark starting runtime context baseline
	DS1307_SetTime(12, 0, 0);
	
	while(1) {
		// Check if an updated data string arrived from MC1
		if (dataReadyFlag) {
			sscanf((char*)rxString, "%f,%d", &tempVal, &lightVal);
			dataReadyFlag = 0;
		}
		
		DS1307_GetTime(clockString);
		
		// Assert System Threshold Safety Rule Mapping Boundaries
		uint8_t safeStateBreached = (tempVal > 30.0 || lightVal < 300);
		
		if (safeStateBreached) {
			PORTB |= (1<<PB2);   // Red alarm warning state activated
			PORTB &= ~(1<<PB1);  // Clear Green indicator line
			// Relay shares PB0 pin tracks via parallel breadboard junction layout rules
			PORTB |= (1<<PB0);
			} else {
			PORTB &= ~(1<<PB2);  // Safe nominal parameters verified
			PORTB |= (1<<PB1);   // Light up green normal operation path
		}
		
		// Render Output Display Page
		LCD_Command(0x01); // Screen clear operation
		
		LCD_Command(0x80); // Line 1 Origin Pointer Positioning
		sprintf(uiLine1, "Time: %s", clockString);
		LCD_String(uiLine1);
		
		LCD_Command(0xC0); // Line 2 Origin Pointer Positioning
		if (safeStateBreached) {
			sprintf(uiLine2, "ALERT! T:%d.%dC", (int)tempVal, ((int)(tempVal*10))%10);
			} else {
			sprintf(uiLine2, "T:%d.%dC L:%d", (int)tempVal, ((int)(tempVal*10))%10, lightVal);
		}
		LCD_String(uiLine2);
		
		_delay_ms(250); // Frame refresh cadence controller delay
	}
}