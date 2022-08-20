#include <GY21.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#include "facesConfig.h"
#include "faces.h"

#define DEFAULT_FACE_DELAY 1000

GY21 sht;
Adafruit_SSD1306 display(128, 64, &Wire, -1); 

uint8_t currentFace;
uint8_t numberRepeats;

void loopFace(uint8_t face, uint16_t faceDelay = DEFAULT_FACE_DELAY);

void setup() {
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  randomSeed(analogRead(0));
  resetNumberRepeats();
  selectStartingFace();
  //Serial.begin(9600);
  //while (!Serial);
}

void loop() {
  loopFace(currentFace);
  numberRepeats--;
  if (numberRepeats == 0) {
    resetNumberRepeats();
    selectNextFace();
  }
}

void resetNumberRepeats() {
  numberRepeats = random(3, 10);
}

void selectStartingFace() {
  currentFace = random(NUMBER_FACES);
  selectNextFace();
}

void selectNextFace() {  
  int8_t nextFace;
  do {
    nextFace = nextFaces[currentFace][random(3)];
    Serial.println(nextFace);
  } while (nextFace == -1);
  
  currentFace = nextFace;
}

void loopFace(uint8_t face, uint16_t faceDelay = DEFAULT_FACE_DELAY) {
  for (uint8_t i = 0; i < 2; i++) {
    drawFace(allFaces[face][i]);
    delay(faceDelay);
  }
}

void drawFace(const uint8_t * bmp) {
  display.clearDisplay();
  uint8_t b = 0;
  for (uint8_t j = 0; j < 16; j++) {
    for (uint8_t i = 0; i < 32; i++) {
      if (i & 7)
        b <<= 1;
      else
        b = pgm_read_byte(&bmp[j * 4 + i / 8]);
      if (b & 0x80)
        display.fillRect(i * 4, j * 4, 4, 4, SSD1306_WHITE);
    }
  }
  display.display();
}
