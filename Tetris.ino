
byte tetrimos[] = {0b0111000, // I 0
                   0b0111001, // J 1
                   0b0111100, // L 2
                   0b0110110, // O 3
                   0b0011110, // S 4
                   0b0010111, // T 5
                   0b0110011, // Z 6

                   0b1010101, // I
                   0b1010111, // J
                   0b1110101, // L
                   0b1111100, // O
                   0b1101101, // S
                   0b1101110, // T
                   0b1011110, // Z

                   0b0000111, // I
                   0b0100111, // J
                   0b0001111, // L
                   0b0110110, // O
                   0b0011110, // S
                   0b0111010, // T
                   0b0110011, // Z

                   0b1101010, // I
                   0b1111010, // J
                   0b1101011, // L
                   0b1111100, // O
                   0b1101101, // S
                   0b1011101, // T
                   0b1011110, // Z
                  }; 
                 

int tetrimo = 1;
// 0, 1, 2, 3 represnting 0, 90, 180, 270 degrees
int rotation = 0;
int posx = 1;
int posy = 9;
int frame = 0;
int level = 1;
int score = 0;
int totalRowsCleared = 0;
int framesPerTick = 62;

bool gameOver = false;
bool collidedLastTick = false;
bool displayGameOverScreen = false;

int base[ROWS][COLS] = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                        OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                        OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                        OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};

int gameOverScreen [ROWS][COLS] = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                                   OFF, ON,  OFF, OFF, ON,  ON,  ON,  OFF, OFF, OFF, OFF, OFF,
                                   ON,  ON,  ON,  OFF, OFF, ON,  OFF, OFF, OFF, OFF, OFF, OFF,
                                   OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};

void initTetris() {
  //setMatrix(newmatrix);
}

void updateTetris() {
  //tetrimo = (currentTime / 1000000) % 7;
  //rotation = (currentTime / 1000000) % 4;
  //posy = 9 - (currentTime / 100000) % 10;

  // -- after user input --
  //both for rotation and translation:
  //check for a collision with base and the walls
  //  no collision: apply the rotation and translation
  //  collision: ignore input, carry on
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();

    if (incomingByte == 44) {
      int newRotation = rotation + 1 > 3 ? 0 : rotation + 1;
      if (!getCollision(posx, posy, newRotation)) {
        rotation = newRotation;
      }
      else {
        Serial.println("Can't rotate, there is a collision");
      }
    }
    else if (incomingByte == 97) {
      int newx = posx - 1;
      if (!getCollision(newx, posy, rotation)) {
        posx = newx;
      }
    }
    else if (incomingByte == 101) {
      int newx = posx + 1;
      if (!getCollision(newx, posy, rotation)) {
        posx = newx;
      }
    }
  }

  // This should be calculated on level change, not every frame.
  

  if (frame % framesPerTick == 0) {
    updateTetrimo();
  }

  frame++;

  if (displayGameOverScreen) {
    drawTetris(gameOverScreen);
  }
  else {
    drawTetris(base);
  }
}

void updateTetrimo() {
  //check if there is a collision
  //  loop through the six bits of the tetrimo and compare
  //  their new position with the base.
  int newposy = posy - 1;
  bool collision = getCollision(posx, newposy, rotation);
  
  if (!collision) {
    posy = newposy;
  }
  else if (!collidedLastTick && collision && !gameOver) {
    //  if there is a collision when the tetrimo goes down 1 pixel:
    //  allow 1 more "tick", afterwards.
    collidedLastTick = true;
    
  }
  else if (collidedLastTick && !gameOver) {
    //  lock the block to the base and spawn a new one.
    //  after locking, check if a row is full.
    //    if full, remove the row, add points to the score, 
    //    and lower all the other blocks by one pixel
    //    lock the block to the base and spawn a new one.
    collidedLastTick = false;

    int index = tetrimo + rotation * 7;
    int tetrimoWidth = 3 - ((tetrimos[index] >> 6) & 1);
    int tetrimoLength = ((tetrimos[index] >> 6) & 1) + 2;
    int ctr = 0;
    for (int y = 0; y < tetrimoLength; y++) {
      for (int x = 0; x < tetrimoWidth; x++) {
        if ((tetrimos[index] >> ctr) & 1)
          base[posx + x][posy + y] = ON;
        ctr++;
      }
    }

   

    // Search the rows thrice, because it is impossible to
    // clear more than 3 rows at once. The search for a row
    // is cut off immidietly if one is found so the next
    // iteration of this loop can start.
    int clearedRows = 0;
    
    for (int search = 0; search < 3; search++) {
      for (int col = 0; col < 12; col++) {
        if (base[0][col] == ON &&
            base[1][col] == ON &&
            base[2][col] == ON &&
            base[3][col] == ON) {
          clearedRows++;
          for (int i = 0; i < 4; i++) {
            base[i][col] = OFF;
          }
          for (int r = col + 1; r < 12 - col; r++) {
            for (int j = 0; j < 4; j++) {
              base[j][r - 1] = base[j][r];
              base[j][r] = OFF;
            }
          }
          break;
        }
      }
    }

    // handling score stuff
    score += clearedRows * clearedRows;
    totalRowsCleared += clearedRows;

    level = totalRowsCleared / 10 + 1;
    framesPerTick = level * -2.8 + 62.8;
    

    // Spawn a new tetrimo and check for game over
    posy = 9;
    posx = 1;
    rotation = 1;
    tetrimo = random(0, 6);

    if (getCollision(posx, posy, rotation)) {
      // display the score in binary
      // only supports scores up to 65536...
      int ctr = 0;
      for (int col = 8; col < 12; col++) {
        for (int row = 3; row >= 0; row--) {
          gameOverScreen[row][col] = ((score >> ctr) & 1) * ON;
          ctr++;
        }
      }
      gameOver = true;
    }
  }
  else if (gameOver) {
    displayGameOverScreen = true;
  }
  

  //  if the newly spawned tetrimo collides with base immidiatly, game over.;
  // posy = posy < 0 ? 9 : posy - 1;
}

// usage: pass the new coordinates to this function
// it wil calculate wether a collision with base or walls has occured
// it assumes the current tetrimo and rotation
bool getCollision(int newx, int newy, int newrot) {
  int index = tetrimo + newrot * 7;
  
  int tetrimoWidth = 3 - ((tetrimos[index] >> 6) & 1);
  int tetrimoLength = ((tetrimos[index] >> 6) & 1) + 2;

  if (newx < 0 || newx > 4 - tetrimoWidth) {
    Serial.println("X");
    return true;
  }
  if (newy < 0 || newy > 12 - tetrimoLength) {
    return true;
  }
  
  int ctr = 0;
  for (int y = 0; y < tetrimoLength; y++) {
    for (int x = 0; x < tetrimoWidth; x++) {
      if (((tetrimos[index] >> ctr) & 1) && base[newx + x][newy + y]) {
        return true;
      }
      ctr++;
    }
  }
  return false;
}

// writes the base and the block matrix to the main matrix
// first this function rotates the base matrix 90 degrees.
void drawTetris(int baseScreen[][12]) {
  setMatrix(baseScreen);

  if (!gameOver) {
    int index = tetrimo + rotation * 7;
  
    int tetrimoWidth = 3 - ((tetrimos[index] >> 6) & 1);
    int tetrimoLength = ((tetrimos[index] >> 6) & 1) + 2;
    int ctr = 0;
    for (int y = 0; y < tetrimoLength; y++) {
      for (int x = 0; x < tetrimoWidth; x++) {
        if ((tetrimos[index] >> ctr) & 1)
          matrix[posx + x][posy + y] = 500;
        ctr++;
      }
    }
  }
  
}


void setMatrix(int newmatrix[][COLS]) {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      matrix[row][col] = newmatrix[row][col];
    }
  }
}

