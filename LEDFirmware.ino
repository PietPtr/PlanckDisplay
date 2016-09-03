#include "Tlc5940.h"

#define ON  4095
#define OFF 0

#define ROWS 4
#define COLS 12

const int rows[] = {8, 7, 6, 5};
const int cols[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

unsigned long oldDisplayTime = 0;
unsigned long oldGameTime = 0;
unsigned long currentTime = 0;
const int rowDuration = 4000;
const int frameDuration = 16000; // 62.5 FPS
int row = 0;

//int matrix[numrows][numcols] = {0};
int matrix[][COLS] = {{100, 340, 680, 1020, 1360, 1700, 2040, 2380, 2720, 3060, 3400, 4095},
                      {100, 340, 680, 1020, 1360, 1700, 2040, 2380, 2720, 3060, 3400, 4095},
                      {100, 340, 680, 1020, 1360, 1700, 2040, 2380, 2720, 3060, 3400, 4095},
                      {100, 340, 680, 1020, 1360, 1700, 2040, 2380, 2720, 3060, 3400, 4095}};
                         
void setup() {
  Serial.begin(9600);
  for (int pin = 0; pin < ROWS; pin++) {
    pinMode(rows[pin], OUTPUT);
  }
  Tlc.init();
  initTetris();
}

void loop() {
  currentTime = micros();
  if (currentTime >= oldDisplayTime) {
    oldDisplayTime = currentTime + rowDuration;
  
    updateMatrix(row);
    row++;
    row = row >= ROWS ? 0 : row;
  }

  if (currentTime >= oldGameTime) {
    oldGameTime = currentTime + frameDuration;

    
    updateTetris();
    //updateDasher();
    //updateSnakes();
  }
}

void updateMatrix(int row) {
  for (int tlcpin = 0; tlcpin < 16; tlcpin++) {
    Tlc.set(tlcpin, matrix[row][tlcpin]);
  }
  
  for (int pin = 0; pin < ROWS; pin++) {
    digitalWrite(rows[pin], HIGH);
  }
  Tlc.update();
  delay(2);
  digitalWrite(rows[row], LOW);
}

