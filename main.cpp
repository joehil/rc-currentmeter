#include <Arduino.h>
#include <RCSwitch.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

volatile unsigned long strom = 0;
volatile unsigned int count = 99;


ISR(WDT_vect)
  /* Watchdog Timer Interrupt Service Routine */
  {
    count++;
  }

void countStrom() {
  strom++;;
}

void enter_sleep(void)
  /* Arduino schlafen legen */
  {
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* Es geht auch SLEEP_MODE_PWR_DOWN */
  sleep_enable();
  power_adc_disable();    /* Analog-Eingaenge abschalten */
  power_spi_disable();    /* SPI abschalten */
  power_timer0_disable(); /* Timer0 abschalten */
  power_timer2_disable(); /* Timer0 abschalten */
  power_twi_disable();    /* TWI abschalten */
  sleep_mode();
  sleep_disable();
  power_all_enable();     /* Komponenten wieder aktivieren */
  }


RCSwitch mySwitch = RCSwitch();

void setup() {

  Serial.begin(9600);

  /* Setup des Watchdog Timers */
  MCUSR &= ~(1<<WDRF);             /* WDT reset flag loeschen */
  WDTCSR |= (1<<WDCE) | (1<<WDE);  /* WDCE setzen, Zugriff auf Presclaler etc. */
  WDTCSR = 1<<WDP0 | 1<<WDP3;      /* Prescaler auf 8.0 s */
  WDTCSR |= 1<<WDIE;               /* WDT Interrupt freigeben */
  
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(10);
  
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(1);

  // Optional set pulse length.
  mySwitch.setPulseLength(320);
  
  // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(3);

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), countStrom, RISING);

  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  
}

void loop() {
  if (count > 30) {
    mySwitch.send((unsigned long)1879048192 + strom,32); // 1879048192 legt das Device fest
    count = 0;
  }
  delay(100);  
  enter_sleep();
}
