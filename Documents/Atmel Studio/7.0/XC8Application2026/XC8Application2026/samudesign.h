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
#define BATTERY_FULL_VOLT