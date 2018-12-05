// HelloM5: A Greeting Application

#include <M5Stack.h>
#include <string>
/* #include "M5Stack.h" */

const int PIN = 36;
const String text = "Hello M5";
const uint8_t text_font = 4;
int mx, my;

bool detected() { return analogRead(PIN) < 100.0f; }

void setup() {
    pinMode(PIN, INPUT);
    M5.begin();
    M5.Lcd.setTextFont(text_font);
    M5.Lcd.setTextColor(TFT_YELLOW);
    mx = M5.Lcd.width() - M5.Lcd.textWidth(text);
    my = M5.Lcd.height() - M5.Lcd.fontHeight(text_font);
}

void loop() {
    M5.Lcd.fillScreen(TFT_BLACK);
    if (detected()) {
        M5.Lcd.drawString("DETECT", 100, 100);
    }
    delay(1000);
    M5.update();
}
