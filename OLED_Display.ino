#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "lib.h"

// OLED display specifications and pinouts (SSD1306 128x64 OLED 0.96')
#define WIDTH 128
#define HEIGHT 64
#define OLED_RST -1
#define SDA 4 // GPIO4 (D2 on NodeMCU)
#define SCL 5 // GPIO5 (D1 on NodeMCU)

// Initialize the display interface
Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, OLED_RST);

// Startup initializer for OLED display
void OLED_Setup() {
  Wire.begin(SDA, SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    delay(1500);
    ESP.restart();
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

void cursor(int selected) {
  int sel_opt = NUM_X_START + (BOX_WIDTH + SPACE) * selected;
  display.fillRect(sel_opt, NUM_Y_START + CHAR_HEIGHT, BOX_WIDTH, BOX_HEIGHT, WHITE);
  display.display();
  delay(500);
  display.fillRect(sel_opt, NUM_Y_START + CHAR_HEIGHT, BOX_WIDTH, BOX_HEIGHT, BLACK);
  display.display();
  delay(500);
}

void chn_cursor() {
  int st = curr_chn ? 98 : 113;
  const char* curr_chn_str = curr_chn ? "2" : "4";
  display.fillRect(st, 49, 9, 10, WHITE);
  display.setCursor(st + 2, 50);
  display.setTextColor(BLACK);
  display.print(curr_chn_str);
  display.display();
  delay(500);
  display.fillRect(st, 49, 9, 10, BLACK);
  display.setCursor(st + 2, 50);
  display.setTextColor(WHITE);
  display.print(curr_chn_str);
  display.display();
  delay(500);
}

void setting_menu(int selected) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(7, 10);
  display.println("Enter transmit code");
  display.setTextSize(2);
  int sel_opt = NUM_X_START + (BOX_WIDTH + SPACE) * selected;
  for (int i = 0; i < 8; i++) {
    int st = NUM_X_START + (BOX_WIDTH + SPACE) * i;
    display.setCursor(st, NUM_Y_START);
    display.print(curr_code[i]);
  }
  display.fillRect(sel_opt, NUM_Y_START + CHAR_HEIGHT, BOX_WIDTH, BOX_HEIGHT, WHITE);
  display.setTextSize(1);
  display.setCursor(4, 50);
  display.print("Select channel:");
  selected_chn();

  display.display();
}

void selected_chn() {
  int st = curr_chn ? 98 : 113;
  display.fillRect(st, 49, 9, 10, WHITE);
  display.setTextSize(1);
  display.setTextColor(curr_chn ? BLACK : WHITE);
  display.setCursor(100, 50);
  display.print("2");
  display.setTextColor(curr_chn ? WHITE : BLACK);
  display.setCursor(115, 50);
  display.print("4");
}

void display_saved(bool state) {
  const char* prompt = state ? "Configuration Saved!" : "Configuration Failed!";
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(6, 30);
  display.setTextColor(WHITE);
  display.println(prompt);
  display.display();
}



