#include <GY21.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#include "faces.h" //Generated file containing data of face images
#include "facesConfig.h" //Generated file containing configuration data for the connections of faces

#define DEFAULT_FACE_DELAY (F_CPU / 300) //Delay between frames of animation, calculated to show about 100 faces in 60 seconds
#define SHOCK_FACE_DELAY (F_CPU / 1400) //Delay between frames of animation for the "shock" face, faster than the default delay
#define MIN_FACE_DURATION 100 //Minimum duration a face will be shown, equals about 1 minute
#define MAX_FACE_DURATION 200 //Maximum duration a face will be shown, equals about 2 minute
#define MIN_SHOCK_DURATION 6 //Minimum duration the "shock" face will be shown

#define TEMP_HOT_THRESHOLD 27 //Temperature in C above which the "hot" face should appear
#define TEMP_COLD_THRESHOLD 8 //Temperature in C below which the "cold" face should appear

#define SHOCK_PIN 6

GY21 sht;
Adafruit_SSD1306 display(128, 64, &Wire, -1); 

uint8_t currentFace;
uint8_t numberRepeats;
int8_t currentlyDrawnFaceFrame;
uint32_t faceDelay = DEFAULT_FACE_DELAY;
uint32_t faceTimer = faceDelay;

bool shockSensorState = 0;
float temp;

void setup() {
  pinMode(SHOCK_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  randomSeed(analogRead(0));
  resetNumberRepeats();
  selectStartingFace();
}

void loop() {
  if (faceTimer == 0) {
    numberRepeats--;
    faceTimer = faceDelay;
  }
  
  if (numberRepeats == 0) {
    setFaceDelay(DEFAULT_FACE_DELAY);
    
    temp = sht.GY21_Temperature();
    float constrainedTemp = constrain(temp, TEMP_COLD_THRESHOLD, TEMP_HOT_THRESHOLD);
  
    if (constrainedTemp != temp) {
      numberRepeats = 2;
      currentFace = constrainedTemp == TEMP_COLD_THRESHOLD ? COLD : HOT;
    } else {
      resetNumberRepeats();
      selectNextFace();
    }
  }

  shockSensorState = digitalRead(SHOCK_PIN);

  if (shockSensorState) {
    if (currentFace != SHOCK) {
      currentFace = SHOCK;
      numberRepeats = 2;
      setFaceDelay(SHOCK_FACE_DELAY);
    }

    if (numberRepeats < MIN_SHOCK_DURATION - 1) {
      numberRepeats = min(numberRepeats + 2, MIN_SHOCK_DURATION);
    }
  }

  loopFace(currentFace);
}

void setFaceDelay(uint32_t delay) {
  faceDelay = delay;
  faceTimer = faceDelay;
}

void resetNumberRepeats() {
  numberRepeats = random(MIN_FACE_DURATION, MAX_FACE_DURATION);
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
  currentlyDrawnFaceFrame = -1;
}

void loopFace(uint8_t face) {
  faceTimer--;
  uint8_t faceFrameToDraw = numberRepeats % 2;
  if (currentlyDrawnFaceFrame != faceFrameToDraw) {
    drawFace(allFaces[face][faceFrameToDraw]);
    currentlyDrawnFaceFrame = faceFrameToDraw;
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
