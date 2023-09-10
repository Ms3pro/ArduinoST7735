#include <SPI.h>
#include <avr/io.h>
#include <EEPROM.h>
#include <TaskScheduler.h>
#include "PinConfig.h"
#include "display.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#if DISP1_ACTIVE && defined DISP1_USE_ST7735_SPI
 Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#endif

#if !defined(__AVR_ATmega2560__)
#error "This code is intended for an Arduino Mega 2560 only!"
#endif

//ПРОВЕРКА РАБОТЫ СВЕТОДИОДОВ  ЕГТ КОГДА АРДУИНО ВКЛЮЧАЕТСЯ ИЛИ ПЕРЕЗАГРУЖАЕТСЯ 
const int EGT_LEDS[] = {EGT1_LED, EGT2_LED, EGT3_LED, EGT4_LED, EGT5_LED, EGT6_LED};
const unsigned long interval = 500;  // задержка в 500 миллисекунд
unsigned long previousMillis = 0;
int ledIndex = 0;
bool turningOn = true;  // флаг для проверки, включаем ли мы LED или выключаем

const int TOTAL_ANALOG_PINS = 16;

int analogPins[TOTAL_ANALOG_PINS];

volatile uint16_t adcBuff[TOTAL_ANALOG_PINS];

int16_t adc0, adc1, adc2, adc3, adc4, adc5, adc6, adc7,
        adc8, adc9, adc10, adc11, adc12, adc13, adc14, adc15;

void initializeADCs() {
    adc0 = (adcBuff[0] > 0) ? adcBuff[0] : 0;
    adc1 = (adcBuff[1] > 0) ? adcBuff[1] : 0;
    adc2 = (adcBuff[2] > 0) ? adcBuff[2] : 0;
    adc3 = (adcBuff[3] > 0) ? adcBuff[3] : 0;
    adc4 = (adcBuff[4] > 0) ? adcBuff[4] : 0;
    adc5 = (adcBuff[5] > 0) ? adcBuff[5] : 0;
    adc6 = (adcBuff[6] > 0) ? adcBuff[6] : 0;
    adc7 = (adcBuff[7] > 0) ? adcBuff[7] : 0;
    adc8 = (adcBuff[8] > 0) ? adcBuff[8] : 0;
    adc9 = (adcBuff[9] > 0) ? adcBuff[9] : 0;
    adc10 = (adcBuff[10] > 0) ? adcBuff[10] : 0;
    adc11 = (adcBuff[11] > 0) ? adcBuff[11] : 0;
    adc12 = (adcBuff[12] > 0) ? adcBuff[12] : 0;
    adc13 = (adcBuff[13] > 0) ? adcBuff[13] : 0;
    adc14 = (adcBuff[14] > 0) ? adcBuff[14] : 0;
    adc15 = (adcBuff[15] > 0) ? adcBuff[15] : 0;

}

void updateadcBuffAndSave() {
  for (int i = 0; i < TOTAL_ANALOG_PINS; i++) {
    adcBuff[i] = analogPins[i];
  }
  //saveAnalogStatesToEEPROM();
}

Scheduler ts;

void setup() {

CLKPR = 0x80;
CLKPR = 0x00;

configPins();
SPI.begin();

//ПРОВЕРКА РАБОТЫ СВЕТОДИОДОВ  ЕГТ КОГДА АРДУИНО ВКЛЮЧАЕТСЯ ИЛИ ПЕРЕЗАГРУЖАЕТСЯ   
  
  while(ledIndex < 6) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      
      if (turningOn) {
        digitalWrite(EGT_LEDS[ledIndex], HIGH);
        ledIndex++;
        if (ledIndex == 6) {
          turningOn = false;
          ledIndex--;
        }
      } else {
        digitalWrite(EGT_LEDS[ledIndex], LOW);
        ledIndex--;
        if (ledIndex < 0) {
          break;  // Завершаем цикл, когда все светодиоды выключены        
        }
      }
    }
  }  

 initialise_display();     

}

void loop() {

ts.execute();

canMsg1.can_dlc = 8;
    canMsg1.can_id = 0x690;
    canMsg1.data[0] = adc0 >> 8;
    canMsg1.data[1] = adc1 & 0xFF;
    canMsg1.data[2] = adc2 >> 8;
    canMsg1.data[3] = adc3 & 0xFF;
    canMsg1.data[4] = adc4 >> 8;
    canMsg1.data[5] = adc5 & 0xFF;
    canMsg1.data[6] = adc6 >> 8;
    canMsg1.data[7] = adc7 & 0xFF;
    
    addToCanBuffer(canMsg1);

// Заполняем данные для CAN сообщения 2

    canMsg2.can_dlc = 8;
    canMsg2.can_id = 0x691;
    canMsg2.data[0] = adc0 >> 8;
    canMsg2.data[1] = adc1 & 0xFF;
    canMsg2.data[2] = adc2 >> 8;
    canMsg2.data[3] = adc3 & 0xFF;
    canMsg2.data[4] = adc4 >> 8;
    canMsg2.data[5] = adc5 & 0xFF;
    canMsg2.data[6] = adc6 >> 8;
    canMsg2.data[7] = adc7 & 0xFF;

    addToCanBuffer(canMsg2);   
    sendBufferedCanMessages();

}

void driveDisplayTask() {
        
        driveDisplay();
}

void ReadAnalogStatuses() {
  for (int i = 0; i < TOTAL_ANALOG_PINS; i++) {
    analogPins[i] = analogRead(i);   
  }  
 
    updateadcBuffAndSave();   

initializeADCs(); // Здесь вызываем функцию инициализации переменных adcX  
  digitalWrite(EGT1_LED, egt1 > 800 ? HIGH : LOW);
  digitalWrite(EGT2_LED, egt2 > 800 ? HIGH : LOW);
  digitalWrite(EGT3_LED, egt3 > 800 ? HIGH : LOW);
  digitalWrite(EGT4_LED, egt4 > 800 ? HIGH : LOW);
  digitalWrite(EGT5_LED, egt5 > 800 ? HIGH : LOW);
  digitalWrite(EGT6_LED, egt6 > 800 ? HIGH : LOW);
}

Task t1(500, TASK_FOREVER, &driveDisplayTask, &ts, true);
Task t2(500, TASK_FOREVER, &ReadAnalogStatuses, &ts, true);
