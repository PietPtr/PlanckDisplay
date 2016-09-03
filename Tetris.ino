
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
                 

int tetrimo = 6;
// 0, 1, 2, 3 represnting 0, 90, 180, 270 degrees
int rotation = 0;
int posx = 0;
int posy = 5;
int frame = 0;
int level = 1;
int score = 0;
int framesPerTick = 62;

bool collidedLastTick = false;

int base[ROWS][COLS] = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                        OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                        ON,  OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                        ON,  OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};

void initTetris() {
  /*int newmatrix[ROWS][COLS] = {{ON,  ON,  ON,  OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
                               {ON,  ON,  ON,  ON,  OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},
                               {OFF, OFF, OFF, OFF, OFF, OFF, OFF, ON,  ON,  OFF, OFF, OFF},
                               {ON,  OFF, OFF, OFF, OFF, OFF, OFF, OFF, ON,  ON,  OFF, OFF}};*/
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

  // This should be calculated on level change, not every frame.
  framesPerTick = level * -2.8 + 62.8;

  if (frame % framesPerTick == 0) {
    updateTetrimo();
  }

  frame++;

  drawTetris();
}

void updateTetrimo() {
  //check if there is a collision
  //  loop through the six bits of the tetrimo and compare
  //  their new position with the base.
  int newposy = posy - 1;
  bool collision = getCollision(posx, newposy);
  
  if (!collision) {
    posy = newposy;
  }
  else if (!collidedLastTick && collision) {
    //  if there is a collision when the tetrimo goes down 1 pixel:
    //  allow 1 more "tick", afterwards.
    collidedLastTick = true;
    
  }
  else if (collidedLastTick) {
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

    posy = 9;
    tetrimo = random(0, 6);
    for (int row = 0; row < 12; row++) {
      if (base[row][0] == base[row][1] == base[row][2] == base[row][3] == 1) {
        for (int i = 0; i < 4; i++) {
          
        }
        for (int i = 0; i < 
      }
    }
  }
  
  

  //  if the newly spawned tetrimo collides with base immidiatly, game over.;
  // posy = posy < 0 ? 9 : posy - 1;
}

// usage: pass the new coordinates to this function
// it wil calculate wether a collision with base or walls has occured
// it assumes the current tetrimo and rotation
bool getCollision(int newx, int newy) {
  int index = tetrimo + rotation * 7;
  
  int tetrimoWidth = 3 - ((tetrimos[index] >> 6) & 1);
  int tetrimoLength = ((tetrimos[index] >> 6) & 1) + 2;

  if (newx < 0 || newx > 3 - tetrimoWidth) {
    return true;
  }
  if (newy < 0 || newy > 11 - tetrimoLength) {
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
void drawTetris() {
  setMatrix(base);

  /*int index = tetrimo + rotation * 7;

  // check if the orientation bit is 0, so the tetrimo is 3x2
  int ctr = 0;
  if (~(tetrimos[index] >> 6) & 1) {
    for (int y = 0; y < 2; y++) {
      for (int x = 0; x < 3; x++) {
        matrix[posx + x][posy + y] = ((tetrimos[index] >> ctr) & 1) * ON;
        ctr++;
      }
    }
  }
  // otherwise the tetrimo is 2x3
  else {
    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 2; x++) {
        matrix[posx + x][posy + y] = ((tetrimos[index] >> ctr) & 1) * ON;
        ctr++;
      }
    }
  }*/
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


void setMatrix(int newmatrix[][COLS]) {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      matrix[row][col] = newmatrix[row][col];
    }
  }
}

