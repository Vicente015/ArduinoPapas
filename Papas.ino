#include <Keypad.h>
#include <Array.h>
#include <U8g2lib.h>
#include <Tasker.h>
Tasker tasker;
U8G2_SH1106_128X64_NONAME_2_4W_SW_SPI u8g2(U8G2_R0, 10, 11, 12, 13); // rotation, clock, data, cs, dc

const char keys[2][4] = {
  {'1', '2', '3', '9'},
  {'4', '5', '6', '8'}
};
const byte rows[2] = {2, 3};
const byte cols[4] = {4, 5, 6, 7};
Keypad keypad = Keypad( makeKeymap(keys), rows, cols, 2, 4 );

// Variables
#define fontNormal u8g2_font_6x13_tf
#define fontEmoji u8g2_font_unifont_t_emoticons
#define fontWeather u8g2_font_unifont_t_weather
int chX = 50;
byte chY = 60;
const byte movement PROGMEM = 10;
bool startedMenu = false;
byte pC;
byte sC;

class Element {
  public:
    // 0: Cloud / 1: Star / 2: House / 3 - 5: Tree / 6: Flower / 7: Grass / 8: Potato Seed / 9: Potato Sapling / 10: Potato Grown
    byte type = 0;
    int x;
    byte y = 20;
    bool render;
    unsigned long creation = 0;

    void setX(int i) {
      x = i;
      if (x < 0 || x > 128) {
        render = false;
      } else {
        render = true;
      }
    }
    void setY(int i) {
      y = i;
    }
    void setType(byte t) {
      type = t;
    }
    void setCreation() {
      creation = millis() / 1000;
    }
};
// array of elements
Array<Element, 50> elements;

// Generate world
void initRender() {
  randomSeed(15);
  // Screen size: 128 x 64
  byte minV = 5;
  byte maxV = 9;
  // Screen 1
  // House
  Element house;
  house.setX(1);
  house.setY(8);
  house.setType(2);
  elements.push_back(house);

  // Trees
  byte tPos = 20;
  for (int i = 0; i < 4; i++) {
    tPos += random(20, 28);
    Element tree;
    tree.setX(tPos);
    tree.setY(random(18, 30));
    tree.setType(random(3, 6));
    elements.push_back(tree);
  }

  // Clouds
  for (int i = 0; i < 3; i++) {
    Element cloud;
    cloud.setX(random(40, 118));
    cloud.setY(random(10, 16));
    elements.push_back(cloud);
  }

  // Screen 2
  // Trees
  tPos = 118;
  for (int i = 0; i < random(6, 9); i++) {
    Element tree;
    tPos += 15;
    tree.setX(random(tPos, tPos + 10));
    tree.setY(random(5, 20));
    tree.setType(random(3, 6));
    elements.push_back(tree);
  }

  // Grass
  for (int i = 0; i < random(8, 20); i++) {
    Element grass;
    grass.setX(random(118, 374));
    grass.setY(random(40, 60));
    grass.setType(7);
    elements.push_back(grass);
  }

  // Screen 3
  // Flowers
  for (int i = 0; i < random(8, 20); i++) {
    Element flower;
    flower.setX(random(118, 374));
    flower.setY(random(40, 60));
    flower.setType(6);
    elements.push_back(flower);
  }

  // Stars
  for (int i = 0; i < random(minV, maxV); i++) {
    Element star;
    star.setX(random(246, 374));
    star.setY(random(10, 21));
    star.setType(1);
    elements.push_back(star);
  }
  //Serial.println(String(elements.size()));
}

// true: left // false: right
void moveObjects(bool dir) {
  if (dir == true) {
    for (int i = 0; i < elements.size(); i++) {
      elements[i].setX(elements[i].x - movement);
    }
  } else {
    for (int i = 0; i < elements.size(); i++) {
      elements[i].setX(elements[i].x + movement);
    }
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  u8g2.begin();
  u8g2.setBitmapMode(1);
  tasker.setInterval(task1, 1000);
  initRender();
}

void drawCh() {
  u8g2.setFont(fontEmoji);
  u8g2.drawGlyph(50, chY, 0x0080);
}

void drawCounter() {
  u8g2.setFont(fontNormal);
  byte w = 0;
  if (pC > 9) w = 1;
  if (pC > 99) w = 2;
  u8g2.setCursor(120 - (w * 10), 10);
  u8g2.print(pC);
}

void startMenu() {
  u8g2.setFont(u8g2_font_7x14_tf);
  int x = (128 / 2) - 30;
  u8g2.drawUTF8(x - 5, 20, "Welcome to");
  u8g2.drawUTF8(x + 10, 35, "Papas");
  u8g2.setFont(fontNormal);
  u8g2.drawButtonUTF8(x + 30, 55, U8G2_BTN_INV | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 1, 1, "Start");
}

void draw() {
  if (!startedMenu) {
    startMenu();
    return;
  }

  // Draw elements
  for (int i = 0; i < elements.size(); i++) {
    if (elements[i].render == false) continue;
    int x = elements[i].x;
    int y = elements[i].y;
    switch (elements[i].type) {
      case 0:
        u8g2.setFont(fontWeather);
        u8g2.drawGlyph(x, y, 0x0037);
        break;
      case 1:
        u8g2.setFont(u8g2_font_unifont_t_76);
        u8g2.drawGlyph(x, y, 0x2605);
        break;
      case 2:
        u8g2.setFont(fontNormal);
        u8g2.setCursor(x, y);
        u8g2.print(F("    ##"));
        u8g2.setCursor(x, y + 10);
        u8g2.print(F(" __||"));
        u8g2.setCursor(x, y + 20);
        u8g2.print(F("/.\\__\\"));
        u8g2.setCursor(x, y + 30);
        u8g2.print(F("|O | |"));
        break;
      case 3:
        u8g2.setFont(fontNormal);
        u8g2.setCursor(x, y);
        u8g2.print(F(" ^"));
        u8g2.setCursor(x, y + 5);
        u8g2.print(F("/|\\"));
        u8g2.setCursor(x, y + 15);
        u8g2.print(F(".|"));
        break;
      case 4:
        u8g2.setFont(u8g2_font_6x13_tf);
        u8g2.setCursor(x, y);
        u8g2.print(F(" ^"));
        u8g2.setCursor(x, y + 5);
        u8g2.print(F("/|\\"));
        u8g2.setCursor(x, y + 15);
        u8g2.print(F("#|."));
        break;
      case 5:
        u8g2.setFont(u8g2_font_6x13_tf);
        u8g2.setCursor(x, y);
        u8g2.print(F(" ^"));
        u8g2.setCursor(x, y + 5);
        u8g2.print(F("/|\\"));
        u8g2.setCursor(x, y + 15);
        u8g2.print(F("/|\\"));
        break;
      case 6:
        u8g2.setFont(u8g2_font_unifont_t_77);
        u8g2.drawGlyph(x, y, 0x2698);
        break;
      case 7:
        u8g2.setFont(u8g2_font_6x13_tf);
        u8g2.setCursor(x, y);
        u8g2.print(F("#"));
        break;
      case 8:
        static char potato_stage_1[] = {
          0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x7C, 0x00, 0xFC, 0x00, 0xFC, 0x01,
          0xF8, 0x01, 0xF8, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        u8g2.drawXBM(x, y, 11, 11, potato_stage_1);
        break;
      case 9:
        static char potato_stage_2[] = {
          0x8C, 0x01, 0xDE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00, 0xFC, 0x00,
          0xFE, 0x01, 0xFE, 0x03, 0xFC, 0x03, 0xF8, 0x03, 0xF0, 0x00,
        };
        u8g2.drawXBM(x, y, 11, 11, potato_stage_2);
        break;
      case 10:
        static char potato_stage_3[] = {
          0x1C, 0x00, 0x7E, 0x00, 0xFF, 0x00, 0xFF, 0x01, 0xFF, 0x03, 0xFF, 0x07,
          0xFF, 0x07, 0xFE, 0x07, 0xFC, 0x07, 0xFC, 0x03, 0xF0, 0x01,
        };
        u8g2.drawXBM(x, y, 11, 11, potato_stage_3);
        break;
    }
  }
  drawCh();
  drawCounter();
}

void listenKeypad() {
  byte key = keypad.getKey() - '0';
  if (key == 208) return;

  if (!startedMenu) {
    // start game
    if (key) {
      startedMenu = true;
      u8g2.clear();
      delay(500);
    }
  } else {
    if (key == 9) {
      startedMenu = false;
    }
    if (key == 1) {
      for (int i = 0; i < elements.size(); i++) {
        if (!elements[i].render || elements[i].type < 6 || elements[i].type > 10) continue;
        if (elements[i].x >= 40 && elements[i].x < 70 && elements[i].y >= chY - 10 && elements[i].y < chY + 10) {
          elements.remove(i);
          sC++;
        }
      }
    }
    if (key == 3) {
      if (sC < 1) return;
      Element potato;
      potato.setX(65);
      potato.setY(chY - 10);
      potato.setType(8);
      potato.setCreation();
      elements.push_back(potato);
      sC--;
      pC++;
    }
    if (key == 2) {
      chY -= 5;
    }
    if (key == 5) {
      chY += 5;
    }
    if (key == 6) {
      moveObjects(true);
    }
    if (key == 4) {
      moveObjects(false);
    }
  }
}

void task1() {
  for (int i = 0; i < elements.size(); i++) {
    if (elements[i].type < 8 || elements[i].type > 9) continue;
    unsigned long current = millis() / 1000;
    byte grownDuration = 60;
    if (elements[i].type == 9) grownDuration = grownDuration * 2;
    if (current - elements[i].creation >= grownDuration) {
      elements[i].setType(elements[i].type + 1);
    }
  }
}

void loop() {
  listenKeypad();
  tasker.loop();

  u8g2.firstPage();
  do {
    draw();
  } while ( u8g2.nextPage() );

  delay(10); // loop every 10ms
}
