#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "lib.h"

// OLED display specifications and pinouts
#define WIDTH 128
#define HEIGHT 64
#define OLED_RST -1
#define SDA 4
#define SCL 5

// Initialize the display interface
Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, OLED_RST);

// Startup initializer for OLED display
void OLED_Setup() {
  Wire.begin(SDA, SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED allocation failed"));
    for (;;);
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  st = 0;
  prev_st = st;
  main_menu(st);
}

// Reset OLED display based on scheduled task
void reset_display() {
  display.display();
  reset_OLED = false;
}

void reset_status() { 
  display.fillRect(0, 44, 128, 20, BLACK);
  reset_OLED = true;
}

// Display status and messages on Transmit & Receive mode
void display_msg(const char* msg) {
  reset_status();
  display.setCursor(0, 44);
  display.setTextColor(WHITE);
  display.println(msg);
  display.display();
}

// Same as the above but with continous cyclic animation 
void display_cont_msg(const char* msg, const char* indi, int time) {
  reset_status();
  display.setCursor(0, 44);
  display.setTextColor(WHITE);
  display.print(msg);
  for (int i = 0; i < time; i++) display.print(indi);
  display.display();
}

// Default UI for Transmit & Receive mode
void default_msg() {
  const char* msg = mode == TRANSMIT ? "RF Remote is active!" : "RF Receiver is active";
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Hello, Thomas!");
  display.println(msg);
  display.println("Standing by...");
  display.display();
}

// Main menu UI
void main_menu(int selected) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.println("Hello, Thomas!");
  display.println("Select a function");
  menu_state(selected);
  display.display();
}

// Handle menu options cycling
void menu_state(int selected) {
  for (int i = 0; i < OPTIONS; i++) {
    int y = OPTION_START_Y + 13 * i;
    if (i == selected) display.fillRect(0, y - 3, 128, 12, WHITE);
    display.setCursor(OPTION_START_X, y);
    display.setTextColor(i == selected ? BLACK : WHITE);
    display.println(options[i]);
  }
}

