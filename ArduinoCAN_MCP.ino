#include <SPI.h>
#include <avr/io.h>
#include <EEPROM.h>
#include <TaskScheduler.h>
#include "PinConfig.h"
#include "display.h"

#define CAN_BUFFER_SIZE 4
MCP_CAN CAN(53);  // Создаем объект CAN и задаем CS пин как 53

const unsigned long EXECUTION_INTERVAL = 20; // 50 Гц (1 секунда / 50 = 20 миллисекунд)
const unsigned long MAX_EXECUTION_TIME = 100; // Максимальное время выполнения 100 миллисекунд

TaskScheduler ts;

void driveDisplayTask() {
  unsigned long startTime = millis(); // Запомнить время начала выполнения функции
  driveDisplay();
  unsigned long endTime = millis(); // Запомнить время окончания выполнения функции
  unsigned long executionTime = endTime - startTime; // Рассчитать время выполнения

  if (executionTime > MAX_EXECUTION_TIME) {
    // Если время выполнения превышает максимальное, выполнить необходимые действия
    // например, выйти из функции или выполнить дополнительные действия
  }
}

void sendCanMessagesTask() {
  sendBufferedCanMessages();
}

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

const int TOTAL_ANALOG_PINS = 6;

int analogPins[TOTAL_ANALOG_PINS];

volatile uint16_t adcBuff[TOTAL_ANALOG_PINS];

int16_t adc0, adc1, adc2, adc3, adc4, adc5;

void initializeADCs() {
    adc0 = (adcBuff[0] > 0) ? adcBuff[0] : 0;
    adc1 = (adcBuff[1] > 0) ? adcBuff[1] : 0;
    adc2 = (adcBuff[2] > 0) ? adcBuff[2] : 0;
    adc3 = (adcBuff[3] > 0) ? adcBuff[3] : 0;
    adc4 = (adcBuff[4] > 0) ? adcBuff[4] : 0;
    adc5 = (adcBuff[5] > 0) ? adcBuff[5] : 0;

}

void updateadcBuffAndSave() {
  for (int i = 0; i < TOTAL_ANALOG_PINS; i++) {
    adcBuff[i] = analogPins[i];
  }
  //saveAnalogStatesToEEPROM();
}

Scheduler ts;

struct canMsg {
    long unsigned int can_id;  // CAN id
    unsigned char can_dlc;     // Data length
    unsigned char data[8];     // Data
} canMsg1, canMsg2;
byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

struct CanBuffer {
    struct canMsg messages[CAN_BUFFER_SIZE];
    uint8_t head = 0;  // указатель на первое свободное место
    uint8_t tail = 0;  // указатель на первое сообщение для отправки
} canBuffer;

bool addToCanBuffer(const struct canMsg& message) {
    uint8_t nextHead = (canBuffer.head + 1) % CAN_BUFFER_SIZE;
    if (nextHead == canBuffer.tail) {
        // Буфер переполнен, сообщение не добавлено
        return false;
    }
    canBuffer.messages[canBuffer.head] = message;
    canBuffer.head = nextHead;
    return true;
}

bool removeFromCanBuffer(struct canMsg& message) {
    if (canBuffer.tail == canBuffer.head) {
        // Буфер пуст, нет сообщений для извлечения
        return false;
    }
    message = canBuffer.messages[canBuffer.tail];
    canBuffer.tail = (canBuffer.tail + 1) % CAN_BUFFER_SIZE;
    return true;
}

void sendBufferedCanMessages() {
    struct canMsg messages;
    while (removeFromCanBuffer(messages)) {
        CAN.sendMsgBuf(messages.can_id, 0, messages.can_dlc, messages.data);  // Изменили на новую функцию
    }
}
void setup() {

CLKPR = 0x80;
CLKPR = 0x00;

configPins();

 // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK);
  CAN.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
  
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
ts.addTask(driveDisplayTask, EXECUTION_INTERVAL);
  ts.addTask(sendCanMessagesTask, EXECUTION_INTERVAL);

  ts.enableAll();
}

void loop() {

ts.execute();

canMsg1.can_dlc = 8;
    canMsg1.can_id = 0x690;
    canMsg1.data[0] = adc0 >> 8;
    canMsg1.data[1] = adc0 & 0xFF;
    canMsg1.data[2] = adc1 >> 8;
    canMsg1.data[3] = adc1 & 0xFF;
    canMsg1.data[4] = adc2 >> 8;
    canMsg1.data[5] = adc2 & 0xFF;
    canMsg1.data[6] = adc3 >> 8;
    canMsg1.data[7] = adc3 & 0xFF;
    
    addToCanBuffer(canMsg1);

// Заполняем данные для CAN сообщения 2

    canMsg2.can_dlc = 8;
    canMsg2.can_id = 0x691;
    canMsg2.data[0] = adc4 >> 8;
    canMsg2.data[1] = adc4 & 0xFF;
    canMsg2.data[2] = adc5 >> 8;
    canMsg2.data[3] = adc5 & 0xFF;
    canMsg2.data[4] = 0x00;
    canMsg2.data[5] = 0x00;
    canMsg2.data[6] = 0x00;
    canMsg2.data[7] = 0x00;

    addToCanBuffer(canMsg2);   

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

//Task t1(500, TASK_FOREVER, &driveDisplayTask, &ts, true);
Task t1(500, TASK_FOREVER, &ReadAnalogStatuses, &ts, true);
