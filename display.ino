#if DISP1_ACTIVE == 1

void driveDisplay() {

    addToCanBuffer(canMsg2);   
    sendBufferedCanMessages();
    readEGTValues();  
double egtValues[6] = {egt1, egt2, egt3, egt4, egt5, egt6};  
  bool shouldBlinkFrame = false;

for (int i = 0; i < 6; i++) {
    if (egtValues[i] > 850) {
        shouldBlinkFrame = true;
        break;
    }
}
// Определяем цвет рамки на основе значений EGT
unsigned int frameColor = shouldBlinkFrame ? (millis() % 3000 < 1500 ? ST7735_WHITE : ST7735_RED) : determineFrameColor();  

    // Отображаем рамку нужного цвета
    for (int i = 0; i < 2; i++) {
        tft.drawRect(i, i, tft.width() - i*2, tft.height() - i*2, frameColor);
    tft.fillRect(2, 74, 125, 2, WHITE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ
    }

for (int i = 0; i < 6; i++) {
    if (egtValues[i] > 800) {
        barBlinking[i] = true;
        if (millis() - lastBlinkTime[i] > 1500) { // Переключение каждые 1500 мс
            lastBlinkTime[i] = millis();
        }
    } else {
        barBlinking[i] = false;
    }
}
  updateEgtLeds();
}

void updateEgtLeds() {

  DrawBarChartV(tft, 105,  75, 15, 48, 0, 1000 , 200, egt6 , 4 , 0, RED ,  GREEN, BLACK, WHITE, BLACK, "EGT6", graph_1);
  DrawBarChartV(tft, 85,  75, 15, 48, 0, 1000 , 200, egt5 , 4 , 0, RED ,  GREEN, BLACK, WHITE, BLACK, "EGT5", graph_1);
  DrawBarChartV(tft, 65,  75, 15, 48, 0, 1000 , 200, egt4 , 4 , 0, RED ,  GREEN, BLACK, WHITE, BLACK, "EGT4", graph_1);
  DrawBarChartV(tft, 45,  75, 15, 48, 0, 1000 , 200, egt3 , 4 , 0, RED ,  GREEN, BLACK, WHITE, BLACK, "EGT3", graph_1);
  DrawBarChartV(tft, 25,  75, 15, 48, 0, 1000 , 200, egt2 , 4 , 0, RED ,  GREEN, BLACK, WHITE, BLACK, "EGT2", graph_1);
  DrawBarChartV(tft, 5,  75, 15, 48, 0, 1000 , 200, egt1 , 4 , 0, RED ,  GREEN, BLACK, WHITE, BLACK, "EGT1", graph_1);

}

unsigned int determineFrameColor() {
    if (egt1 > 800 || egt2 > 800 || egt3 > 800 || egt4 > 800 || egt5 > 800 || egt6 > 800) {
        return ST7735_RED;
    } else if ((egt1 >= 300 && egt1 < 800) || 
               (egt2 >= 300 && egt2 < 800) || 
               (egt3 >= 300 && egt3 < 800) || 
               (egt4 >= 300 && egt4 < 800) || 
               (egt5 >= 300 && egt5 < 800) || 
               (egt6 >= 300 && egt6 < 800)) {
        return ST7735_YELLOW;
    } else {
        return ST7735_BLUE;
    }
}

void readEGTValues() {
  // Чтение значений с аналоговых входов
   rawEgt1 = adc0;
   rawEgt2 = adc1;
   rawEgt3 = adc2;
   rawEgt4 = adc3;
   rawEgt5 = adc4;
   rawEgt6 = adc5;

  // Масштабирование значений с 0-1023 до 0-1000
   egt1 = map(rawEgt1, 0, 1023, 0, 1000);
   egt2 = map(rawEgt2, 0, 1023, 0, 1000);
   egt3 = map(rawEgt3, 0, 1023, 0, 1000);
   egt4 = map(rawEgt4, 0, 1023, 0, 1000);
   egt5 = map(rawEgt5, 0, 1023, 0, 1000);
   egt6 = map(rawEgt6, 0, 1023, 0, 1000);

}
 
void DrawBarChartV(Adafruit_ST7735 & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw)
{
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

  double stepval, range;
  double my, level;
  double i, data;
      // Здесь мы вычисляем значение level
    level = (h * (((curval - loval) / (hival - loval))));

    int barIndex = (x - 5) / 20;

    if(barBlinking[barIndex] && (millis() - lastBlinkTime[barIndex] < 2000)) {
        d.fillRect(x, y - h, w, h, WHITE); // "Выключение" бара
    } else {
        d.fillRect(x, y - level, w, level, barcolor); // Обычное отображение
    }
  // draw the border, scale, and label once
  // avoid doing this on every update to minimize flicker
  if (redraw == true) {
    redraw = false;

    d.drawRect(x - 1, y - h - 1, w + 2, h + 2, bordercolor);
    d.setTextColor(textcolor, backcolor);
    d.setTextSize(1);
    d.setCursor(x , y + 10);
    //d.println(label);
    // step val basically scales the hival and low val to the height
    // deducting a small value to eliminate round off errors
    // this val may need to be adjusted
    stepval = ( inc) * (double (h) / (double (hival - loval))) - .001;
    for (i = 0; i <= h; i += stepval) {
      my =  y - h + i;
      d.drawFastHLine(x + w + 1, my,  5, textcolor);
      // draw lables
      d.setTextSize(1);
      d.setTextColor(textcolor, backcolor);
      d.setCursor(x + w + 12, my - 3 );
      data = hival - ( i * (inc / stepval));
      d.println(Format(data, dig, dec));
    }
  }
  // compute level of bar graph that is scaled to the  height and the hi and low vals
  // this is needed to accompdate for +/- range
  level = (h * (((curval - loval) / (hival - loval))));
  
  // draw the bar graph
  // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
  d.fillRect(x, y - h, w, h - level,  voidcolor);
  d.fillRect(x, y - level, w,  level, barcolor);
  // write the current value
  d.setTextColor(textcolor, backcolor);
  d.setTextSize(1.6);
  d.setCursor(x , y - h - 23);
  //d.println(Format(curval, dig, dec)); ///////////////////////////////////////////////////ВКЛЮЧИТЬ ЕСЛИ НУЖНО ОТОБРАЖЕНИЕ ЦИВРАМИ!!!!!!
  d.println("EGT");///////////////////////////////////////////////////
}

String Format(double val, int dec, int dig ) {
  int addpad = 0;
  char sbuf[200];
  String condata = (dtostrf(val, dec, dig, sbuf));


  int slen = condata.length();
  for ( addpad = 1; addpad <= dec + dig - slen; addpad++) {
    condata = " " + condata;
  }
  return (condata);
  
#endif

}

void initialise_display()
{
#if DISP1_ACTIVE == 1

 // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK);
  CAN.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted

  tft.initR(INITR_MINI160x80_PLUGIN);
  //tft.invertDisplay(true);
  tft.invertDisplay(false);
  tft.setRotation(3);
  tft.fillScreen(WHITE);
   // Плавное включение подсветки
    for (int brightness = 0; brightness <= 255; brightness++) {
        analogWrite(TFT_LED, brightness);
        delay(20);  // задержка для плавности; можно уменьшить или увеличить по желанию
    }
 // tft.setTextColor(RED);
  //tft.setTextSize(3);
  //tft.setCursor(30, 30);
  //tft.println("MS CAN");
  //tft.drawRGBBitmap (0, 0, epd_bitmap_BMW_1, 160, 80);
  //tft.drawRGBBitmap (0, 0, epd_bitmap_BMW_2, 160, 80);
    tft.drawRGBBitmap (0,15, epd_bitmap_BMW_3, 160, 80);
  //tft.drawRGBBitmap (0,0, epd_bitmap_BMW_4, 160, 80); 
  tft.invertDisplay(false);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  tft.setRotation(3);
  tft.setTextColor(ORANGE);
  tft.setTextSize(2);
  tft.fillRect(0, 12, 158, 1, BLUE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ
  tft.fillRect(0, 22, 158, 1, BLUE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ  
  tft.fillRect(2, 26, 125, 2, WHITE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ
  tft.fillRect(2, 36, 120, 1, WHITE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ
  tft.fillRect(2, 46, 120, 1, WHITE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ
  tft.fillRect(2, 55, 120, 1, WHITE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ
  tft.fillRect(2, 65, 120, 1, WHITE);  // color bar 05 ПОЛОСА РАЗДЕЛИТЕЛЬНАЯ
  tft.setTextColor(YELLOW);
  tft.setTextSize(1.5);
  tft.setCursor(125, 5);
  tft.println("CAN"); 
  tft.setTextColor(RED);
  tft.setTextSize(1.5);
  tft.setCursor(143, 5);
  tft.println("OK");
  tft.setTextColor(RED);
  tft.setTextSize(1.5);
  tft.setCursor(120, 15);
  tft.println("MS3"); 
  tft.setTextColor(YELLOW);
  tft.setTextSize(1.5);
  tft.setCursor(138, 15);
  tft.println("PRO"); 
  tft.setTextColor(YELLOW);
  tft.setTextSize(1.5);
  tft.setCursor(10, 15);
  tft.println("1");
  tft.setTextSize(1.5);
  tft.setCursor(30, 15);
  tft.println("2");
  tft.setTextSize(1.5);
  tft.setCursor(50, 15);
  tft.println("3");
  tft.setTextSize(1.5);
  tft.setCursor(70, 15);
  tft.println("4");
  tft.setTextSize(1.5);
  tft.setCursor(90, 15);
  tft.println("5");
  tft.setTextSize(1.5);
  tft.setCursor(110, 15);
  tft.println("6");
#endif

}
