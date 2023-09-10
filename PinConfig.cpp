#include "PinConfig.h"
#include <Arduino.h>

void initTFTBacklight() __attribute__((constructor));
void initTFTBacklight() {
    DDRH |= (1 << PH5);  // Устанавливаем пин 8 (PH5) как выходной на Arduino Mega
    PORTH &= ~(1 << PH5);  // Устанавливаем низкий уровень на пине 8 (PH5)
}

void configPins() {
  // Настройка аналоговых входов от 0 до 5
  DDRC &= ~PORT_A_MASK;

  // Настройка аналоговых выходов от 10 до 15
  DDRA |= PORT_L_MASK;

  // Настройка цифровых входов от 22 до 29
  //DDRB &= ~PORT_B_MASK;

  // Настройка выходов ШИМ от 3 до 7
 // DDRL |= PORT_L_MASK;
}

    
