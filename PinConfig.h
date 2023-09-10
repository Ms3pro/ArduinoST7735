#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

//СВЕТОДИОДЫ ПЕРЕГРЕВА ЕСЛИ ЕГТ БОЛЬШЕ 800 ГРАДУСОВ 
#define EGT1_LED A10
#define EGT2_LED A11
#define EGT3_LED A12
#define EGT4_LED A13
#define EGT5_LED A14
#define EGT6_LED A15
#define TFT_LED  8  // Пин для управления подсветкой дисплея


// Определение масок для порта A, B, C и L
#define PORT_A_MASK 0b00111111
//#define PORT_B_MASK 0b11111111
//#define PORT_C_MASK 0b11111111
#define PORT_L_MASK 0b11111000

// Функция для настройки пинов
void configPins();

#endif
