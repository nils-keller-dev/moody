#include <GY21.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#include "faces.h"
#include "facesConfig.h"

#define DEFAULT_FACE_DELAY 20
#define SHOCK_FACE_DELAY 4
#define TEMP_HOT_THRESHOLD 27
#define TEMP_COLD_THRESHOLD 8
#define MAX_SHOCK_DURATION 6

#define SHOCK_PIN 13

GY21 sht;
Adafruit_SSD1306 display(128, 64, &Wire, -1); 

uint8_t currentFace;
int8_t numberRepeats;
uint8_t faceDelay = DEFAULT_FACE_DELAY;
uint8_t faceTimer = faceDelay;

bool shockSensorSate = 0;
float temp;

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
  if (faceTimer <= 0) {
    numberRepeats--;
    faceTimer = faceDelay;
  }
  
  if (numberRepeats <= 0) {
    setFaceDelay(DEFAULT_FACE_DELAY);
    
    temp = sht.GY21_Temperature();
    //Serial.println(temp);
    float constrainedTemp = constrain(temp, TEMP_COLD_THRESHOLD, TEMP_HOT_THRESHOLD);

    if (constrainedTemp != temp) {
      numberRepeats = 2;
      currentFace = constrainedTemp == TEMP_COLD_THRESHOLD ? COLD : HOT;
    } else {
      resetNumberRepeats();
      selectNextFace();
    }
  }

  shockSensorSate = digitalRead(SHOCK_PIN);
  //Serial.println(shockSensorSate);
  if (shockSensorSate) {
    if (currentFace != SHOCK) {
      currentFace = SHOCK;
      numberRepeats = 2;
      setFaceDelay(SHOCK_FACE_DELAY);
    }

    if (numberRepeats < MAX_SHOCK_DURATION - 1) {
      numberRepeats = min(numberRepeats + 2, MAX_SHOCK_DURATION);
    }
  }

  loopFace(currentFace);
}

void setFaceDelay(uint8_t delay) {
  faceDelay = delay;
  faceTimer = faceDelay;
}

void resetNumberRepeats() {
  numberRepeats = random(6, 18);
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

void loopFace(uint8_t face) {
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
