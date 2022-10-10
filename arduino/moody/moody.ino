#include <GY21.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#include "faces.h"
#include "facesConfig.h"

#define DEFAULT_FACE_DELAY 10
#define TEMP_HOT_THRESHOLD 27
#define TEMP_COLD_THRESHOLD 8

#define SHOCK_PIN 13

GY21 sht;
Adafruit_SSD1306 display(128, 64, &Wire, -1); 

uint8_t currentFace;
int8_t numberRepeats;
uint16_t faceTimer = DEFAULT_FACE_DELAY;

bool shockSensorSate = 0;
float temp;

void loopFace(uint8_t face, uint16_t faceDelay = DEFAULT_FACE_DELAY);

void setup() {
  pinMode(SHOCK_PIN, INPUT);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  randomSeed(analogRead(0));
  resetNumberRepeats();
  selectStartingFace();
  Serial.begin(9600);
  //while (!Serial);
}

void loop() {
  shockSensorSate = digitalRead(SHOCK_PIN);
  Serial.println(shockSensorSate);
  
  loopFace(currentFace);

  if (faceTimer <= 0) {
    numberRepeats--;
    faceTimer = DEFAULT_FACE_DELAY;
  }
  
  if (numberRepeats <= 0) {
    resetNumberRepeats();
    selectNextFace();
  }

  temp = sht.GY21_Temperature();
  //Serial.println(temp);
  if (temp > TEMP_HOT_THRESHOLD) {
    currentFace = HOT;
  } else if (currentFace == HOT) {
    numberRepeats = 0;
  }

  if (temp < TEMP_COLD_THRESHOLD) {
    currentFace = COLD;
  } else if (currentFace == COLD) {
    numberRepeats = 0;
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
  } while (nextFace == -1);
  
  currentFace = nextFace;
}

void loopFace(uint8_t face, uint16_t faceDelay = DEFAULT_FACE_DELAY) {
  faceTimer--;
  drawFace(allFaces[face][numberRepeats % 2]);
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
