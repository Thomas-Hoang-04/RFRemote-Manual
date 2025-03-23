#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "lib.h"

#define WIDTH 128
#define HEIGHT 64
#define OLED_RST -1
#define SDA 4
#define SCL 5

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, OLED_RST);

void OLED_Setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED allocation failed"));
    for (;;);
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  default_msg();
}

void reset_display() {
  display.display();
  reset_OLED = false;
}

void reset_status() { 
  display.fillRect(0, 44, 128, 20, BLACK);
  reset_OLED = true;
}

void display_msg(const char* msg) {
  reset_status();
  display.setCursor(0, 44);
  display.setTextColor(WHITE);
  display.println(msg);
  display.display();
}

void display_cont_msg(const char* msg, const char* indi, int time) {
  reset_status();
  display.setCursor(0, 44);
  display.setTextColor(WHITE);
  display.print(msg);
  for (int i = 0; i < time; i++) display.print(indi);
  display.display();
}

void default_msg() {
  const char* msg = transmit ? "RF Remote is active!" : "RF Receiver is active";
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Hello, Thomas!");
  display.println(msg);
  display.println("Standing by...");
  display.display();
}

void menu_1() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello, Thomas!");
  display.println("Select a function");
  display.fillRect(0, 18, 128, 12, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 21);
  display.println("RF Remote");
  display.setTextColor(WHITE);
  display.setCursor(5, 34);
  display.println("RF Receiver");
  display.setCursor(5, 47);
  display.println("Settings");
  display.display();
}

