#include "LittleFS.h"
#include "lib.h"

char* code[CHN];

bool write_file() {
  File file = LittleFS.open("/code.txt", "w");
  if (!file) return false;

  const char* curr_chn_str = curr_chn ? "-true\n" : "-false\n";
  file.print(curr_code);
  file.print(curr_chn_str);
  file.close();

  const int buf_size = CODE + CHN + 1;
  char code_buffer[buf_size];
  if (curr_chn)
    for (int i = 0; i < 2; i++) {
      memset(code_buffer, 0, buf_size);
      strcpy(code_buffer, curr_code);
      strncat(code_buffer, dual_channels[i], 4);
      free(code[i]);
      code[i] = strdup(code_buffer);
    }
  else
    for (int i = 0; i < 4; i++) {
      memset(code_buffer, 0, buf_size);
      strcpy(code_buffer, curr_code);
      strncat(code_buffer, quad_channels[i], 4);
      free(code[i]);
      code[i] = strdup(code_buffer);
    }

  return true;
}

void default_config() {
  File file = LittleFS.open("/code.txt", "w");

  if (file) {
    file.print(org_code);
    file.print("-false\n");
    file.close();
    strcpy(curr_code, org_code);
    curr_code[8] = '\0';
    curr_chn = false;
  }
}

void setup_broadcast_code() {
  if (!LittleFS.begin()) {
    delay(1500);
    ESP.restart();
  }

  File file = LittleFS.open("/code.txt", "r");
  if (!file) default_config();
  else {
    for (int i = 0; i < 8; i++) curr_code[i] = file.read();
    curr_code[8] = '\0';
    file.read();
    curr_chn = file.readStringUntil('\n') == "true";
    file.close();
  }

  const int buf_size = CODE + CHN + 1;
  char code_buffer[buf_size];
  if (curr_chn)
    for (int i = 0; i < 2; i++) {
      memset(code_buffer, 0, buf_size);
      strcpy(code_buffer, curr_code);
      strncat(code_buffer, dual_channels[i], 4);
      code[i] = strdup(code_buffer);
    }
  else
    for (int i = 0; i < 4; i++) {
      memset(code_buffer, 0, buf_size);
      strcpy(code_buffer, curr_code);
      strncat(code_buffer, quad_channels[i], 4);
      code[i] = strdup(code_buffer);
    }

  locked = curr_chn ? 0 : 1;
}

void handleMove() {
  if (!chn_sel) {
    st_opt = (st_opt + 1) % CODE;
  } else {
    curr_chn = !curr_chn;
  }
}
void handleSwitch() {
  chn_sel = !chn_sel;
}
void handleOne() {
  curr_code[st_opt] = '1';
}
void handleZero() {
  curr_code[st_opt] = '0';
}
void handleFloat() {
  curr_code[st_opt] = 'F';
}
void handleSaveAndExit() {
  bool status = write_file();
  delay(750);
  display_saved(status);
  delay(1500);
  if (status) {
    st = resolve_opt(mode);
    prev_st = st;
    main_menu(st);
    mode = MENU;
    st_opt = 0;
  }
}

ISRFunction setting_handle[BUTTON_COUNT] = {handleFloat, handleSaveAndExit, handleOne, handleZero, handleMove, handleSwitch};

void settings_handler() {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (buttonState[i]) {
      // Handle event correspondingly
      setting_handle[i]();
      delay(100); // Avoid state debounce & flickering
      buttonState[i] = false;
    } 
  }
}
