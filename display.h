#ifndef DISPLAY_H
#define DISPLAY_H
#include <mcp_can.h>
#include "Arduino.h"
#include "BitmapHelpers.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//display module options .  
//setup display 1
#define DISP1_ACTIVE     1             // 0 == DISABLED , 1 == ENABLED

//only choose one of the following defines for display type selection  , comment out the unused ones
#define DISP1_USE_ST7735_SPI


//set pins for screens in spi mode
//display 1
#define TFT_CS     9
#define TFT_RST    11  
#define TFT_DC     10
#define TFT_SCLK 13  
#define TFT_MOSI 12 

#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN         0xBFF7
#define LTCYAN    0xC7FF
#define LTRED           0xFD34
#define LTMAGENTA       0xFD5F
#define LTYELLOW        0xFFF8
#define LTORANGE        0xFE73
#define LTPINK          0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY          0xE71C
#define BROWN            0XBC40 
#define BLUE            0x001F
#define TEAL    0x0438
#define GREEN           0x07E0
#define CYAN          0x07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define YELLOW        0xFFE0
#define ORANGE        0xFD20
#define PINK          0xF81F
#define PURPLE    0x801F
#define GREY        0xC618
#define WHITE         0xFFFF
#define BLACK         0x0000

#define DKBLUE        0x000D
#define DKTEAL    0x020C
#define DKGREEN       0x03E0
#define DKCYAN        0x03EF
#define DKRED         0x6000
#define DKMAGENTA       0x8008
#define DKYELLOW        0x8400
#define DKORANGE        0x8200
#define DKPINK          0x9009
#define DKPURPLE      0x4010
#define DKGREY        0x4A49
#define BRRED            0XFC07 
#define GRAY             0X8430 //ЧТО ТО ТИПО АСФАЛЬТ 
#define MAGENTA          0xF81F //ЗЕЛЕНЫЙ

extern Adafruit_ST7735 tft;
void driveDisplay();
void initialise_display();

void readEGTValues();
double rawEgt1,rawEgt2,rawEgt3,rawEgt4,rawEgt5,rawEgt6;
double egt1, egt2, egt3, egt4, egt5, egt6;
bool barBlinking[6] = {false, false, false, false, false, false};
unsigned long lastBlinkTime[6] = {0, 0, 0, 0, 0, 0};

extern int16_t adc0, adc1, adc2, adc3, adc4, adc5;
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

boolean graph_1 = true;

#endif
