#include <MD_MAX72xx.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES	4

#define CLK_PIN   18  // or SCK
#define DATA_PIN  23  // or MOSI
#define CS_PIN    5  // or SS

#define BTN_CHANGE 27

#define MOVE_PIN 35
#define DOWN_PIN 4

#define  DELAY_TIME  500  // in milliseconds
#define  DELAY_CONTROL 250

#define TOTAL_COL_SIZE (COL_SIZE * MAX_DEVICES)

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


char current_piece = 'X';
char position = '1';

int16_t x = 0;
int16_t moveDown = 0;

unsigned long button_millis;  
unsigned long move_millis;  
unsigned long moveLR_millis;  
unsigned long moveDown_millis;  

int8_t r[4] = {0};
int8_t c[4] = {0};

int8_t screen[8][32] = {0};
int8_t auxr, auxc;

void moveLeft();
void moveRight();
uint8_t checkScreenPos(int8_t r0, int8_t c0, int8_t r1, int8_t c1, int8_t r2, int8_t c2, int8_t r3, int8_t c3); 
void changePositionI();
void changePositionL();
void changePositionJ();
void changePositionT();
void changePositionS();
void changePositionZ();
void initI();
void initL();
void initO();
void initJ();
void initT();
void initS();
void initZ();
void movePiece();
void printScreen();
void drawPiece();
void checkCol();
void redrawScreen();


void setup() {
  pinMode(BTN_CHANGE, INPUT);
  pinMode(MOVE_PIN, INPUT);

  button_millis = millis();
  move_millis = millis();
  moveLR_millis = millis();
  moveDown_millis = millis();
  
  Serial.begin(9600);

  if (!mx.begin())
    Serial.println("MD_MAX72XX initialization failed");
  
  uint16_t columns = mx.getColumnCount();
  uint16_t lines = ROW_SIZE;

  Serial.print("Number of columns = ");
  Serial.println(columns);
  
  Serial.print("Number of lines = ");
  Serial.println(lines);
  
  Serial.print("Current piece no setup:");
  Serial.print(current_piece); 

  newPiece();

  redrawScreen();

}

void loop() {
  if( (millis() - moveLR_millis) >= DELAY_CONTROL) {
    x = analogRead(MOVE_PIN);
  }

  if( (x < 100) && ( (millis() - moveLR_millis) >= DELAY_CONTROL)) {
    moveLR_millis = millis();    
    moveLeft();
  }
  if( (x  > 3900) && ( (millis() - moveLR_millis) >= DELAY_CONTROL)) {
    moveLR_millis = millis();
    moveRight();
  }

  if( (millis() - moveDown_millis) >= (DELAY_CONTROL/2)) {
    moveDown = analogRead(DOWN_PIN);
  }

  if( (moveDown > 3900) && ( (millis() - moveDown_millis) >= (DELAY_CONTROL/2))) {
    moveDown_millis = millis();    
    movePiece();
  }

  if (( (millis() - move_millis) >= DELAY_TIME)) {
    move_millis = millis();  
    movePiece();
  }

  if(digitalRead(BTN_CHANGE) == 0) {

    if ((millis() - button_millis) >= DELAY_TIME) {
      button_millis = millis();
      Serial.println("Clicou botao.");
      changePosition();
    }    
  }
}

void movePiece() {
  if (checkScreenPos(r[0], c[0], r[1], c[1],r[2], c[2],r[3], c[3]) == 1) {
    screen[r[0]][c[0]-1] = 1;
    screen[r[1]][c[1]-1] = 1;
    screen[r[2]][c[2]-1] = 1;
    screen[r[3]][c[3]-1] = 1;
    checkCol();
    newPiece();
    return;
  }
  redrawScreen();

  drawPiece();
  if(((c[0] < TOTAL_COL_SIZE) || (c[1] < TOTAL_COL_SIZE) || (c[2] < TOTAL_COL_SIZE) || (c[3] < TOTAL_COL_SIZE)) ) {
    c[0]++;
    c[1]++;
    c[2]++;
    c[3]++;
  
  }
}

void changePosition() {
  if(current_piece == 'I') {
    changePositionI();
  }
  if(current_piece == 'L') {
    changePositionL();
  }
  if(current_piece == 'J') {
    changePositionJ();
  }
  if(current_piece == 'T') {
    changePositionT();
  }
  if(current_piece == 'S') {
    changePositionS();
  }
  if(current_piece == 'Z') {
    changePositionZ();
  }

}

void changePositionI() {
      if(position == '1') {
      if(checkScreenPos(r[0], c[0], r[0], c[0]-1, r[0], c[0]-2 ,r[0], c[0]-3) == 0) {
        redrawScreen();
        r[0] = r[0];
        r[1] = r[0];
        r[2] = r[0];
        r[3] = r[0];

        c[0] = c[0];
        c[1] = c[0]-1;
        c[2] = c[0]-2;
        c[3] = c[0]-3;

        position = '2';
        drawPiece();
        return;
      }
      return;
    }

    if(position == '2') {
      if(checkScreenPos(r[0], c[0], r[0]+1, c[0], r[0]+2, c[0] ,r[0]+3, c[0]) == 0) {

      redrawScreen();

        r[0] = r[0];
        r[1] = r[0]+1;
        r[2] = r[0]+2;
        r[3] = r[0]+3;

        c[0] = c[0];
        c[1] = c[0];
        c[2] = c[0];
        c[3] = c[0];

        position = '1';
        drawPiece();
        return;
      }
      return;
    }

}


void changePositionL() {
      if(position == '1') {
      if(checkScreenPos(r[2], c[2]-2, r[2], c[2]-1, r[2], c[2] ,r[2]+1, c[2]) == 0) {

        redrawScreen();

        r[0] = r[2];
        r[1] = r[2];
        r[2] = r[2];
        r[3] = r[2]+1;

        c[0] = c[2]-2;
        c[1] = c[2]-1;
        c[2] = c[2];
        c[3] = c[2];


        position = '2';
        drawPiece();
        return;
      }
      return;
    }

    if(position == '2') {
      if(checkScreenPos(r[2]+2, c[2], r[2]+1, c[2], r[2], c[2] ,r[2], c[2]+1) == 0) {

        redrawScreen();

        r[0] = r[2]+2;
        r[1] = r[2]+1;
        r[2] = r[2];
        r[3] = r[2];

        c[0] = c[2];
        c[1] = c[2];
        c[2] = c[2];
        c[3] = c[2]+1;

        position = '3';
        drawPiece();
        return;
      }
      return;
    }

    if(position == '3') {
      if(checkScreenPos(r[2], c[2]+2, r[2], c[2]+1, r[2], c[2] ,r[2]-1, c[2]) == 0) {

        redrawScreen();

        r[0] = r[2];
        r[1] = r[2];
        r[2] = r[2];
        r[3] = r[2]-1;

        c[0] = c[2]+2;
        c[1] = c[2]+1;
        c[2] = c[2];
        c[3] = c[2];

        position = '4';
        drawPiece();
        return;
      }
      return;
    }

    if(position == '4') {
      if(checkScreenPos(r[2]-2, c[2], r[2]-1, c[2], r[2], c[2] ,r[2], c[2]-1) == 0) {
        redrawScreen();

        r[0] = r[2]-2;
        r[1] = r[2]-1;
        r[2] = r[2];
        r[3] = r[2];

        c[0] = c[2];
        c[1] = c[2];
        c[2] = c[2];
        c[3] = c[2]-1;

        position = '1';
        drawPiece();
        return;
      }
      return;
    }
}


void changePositionJ() {
  if(position == '1') {
    if(checkScreenPos(r[1]+1, c[1], r[1], c[1], r[1], c[1]+1 ,r[1], c[1]+2) == 0) {
      redrawScreen();

      r[0] = r[1]+1;
      r[1] = r[1];
      r[2] = r[1];
      r[3] = r[1];

      c[0] = c[1];
      c[1] = c[1];
      c[2] = c[1]+1;
      c[3] = c[1]+2;

      position = '2';
      drawPiece();
      return;
    }
    return;
  }

  if(position == '2') {
    if(checkScreenPos(r[1], c[1]+1, r[1], c[1], r[1]-1, c[1] ,r[1]-2, c[1]) == 0) {
      redrawScreen();

      r[0] = r[1];
      r[1] = r[1];
      r[2] = r[1]-1;
      r[3] = r[1]-2;

      c[0] = c[1]+1;
      c[1] = c[1];
      c[2] = c[1];
      c[3] = c[1];

      position = '3';
      drawPiece();      
      return;
    }
    return;
  }

  if(position == '3') {
    if(checkScreenPos(r[1]-1, c[1], r[1], c[1], r[1], c[1]-1 ,r[1], c[1]-2) == 0) {
      redrawScreen();

      r[0] = r[1]-1;
      r[1] = r[1];
      r[2] = r[1];
      r[3] = r[1];

      c[0] = c[1];
      c[1] = c[1];
      c[2] = c[1]-1;
      c[3] = c[1]-2;

      position = '4';
      drawPiece();
      return;
    }
    return;
  }

  if(position == '4') {
    if(checkScreenPos(r[1], c[1]-1, r[1], c[1], r[1]+1, c[1] ,r[1]+2, c[1]) == 0) {
      redrawScreen();

      r[0] = r[1];
      r[1] = r[1];
      r[2] = r[1]+1;
      r[3] = r[1]+2;

      c[0] = c[1]-1;
      c[1] = c[1];
      c[2] = c[1];
      c[3] = c[1];

      position = '1';
      drawPiece();
      return;
    }
    return;
  }

}

void changePositionT() {
  if(position == '1') {
    if(checkScreenPos(r[1], c[1]-1, r[1], c[1], r[1]+1, c[1] ,r[1], c[1]+1) == 0) {
      redrawScreen();

      r[0] = r[1];
      r[1] = r[1];
      r[2] = r[1]+1;
      r[3] = r[1];

      c[0] = c[1]-1;
      c[1] = c[1];
      c[2] = c[1];
      c[3] = c[1]+1;

      position = '2';
      drawPiece();
      return;
    }
    return;
  }

  if(position == '2') {
    if(checkScreenPos(r[1]+1, c[1], r[1], c[1], r[1], c[1]+1 ,r[1]-1, c[1]) == 0) {
      redrawScreen();

      r[0] = r[1]+1;
      r[1] = r[1];
      r[2] = r[1];
      r[3] = r[1]-1;

      c[0] = c[1];
      c[1] = c[1];
      c[2] = c[1]+1;
      c[3] = c[1];

      position = '3';
      drawPiece();
      return;
    }
    return;
  }

  if(position == '3') {
    if(checkScreenPos(r[1], c[1]+1, r[1], c[1], r[1]-1, c[1] ,r[1], c[1]-1) == 0) {
      redrawScreen();

      r[0] = r[1];
      r[1] = r[1];
      r[2] = r[1]-1;
      r[3] = r[1];

      c[0] = c[1]+1;
      c[1] = c[1];
      c[2] = c[1];
      c[3] = c[1]-1;

      position = '4';
      drawPiece();
      return;
    }
    return;
  }

  if(position == '4') {
    if(checkScreenPos(r[1]-1, c[1], r[1], c[1], r[1], c[1]-1 ,r[1]+1, c[1]) == 0) {
      redrawScreen();

      r[0] = r[1]-1;
      r[1] = r[1];
      r[2] = r[1];
      r[3] = r[1]+1;

      c[0] = c[1];
      c[1] = c[1];
      c[2] = c[1]-1;
      c[3] = c[1];

      position = '1';
      drawPiece();
      return;
    }
    return;
  }
}

void changePositionS() {
  if(position == '1') {
    if(checkScreenPos(r[1], c[1]-1, r[1], c[1], r[1]-1, c[1] ,r[1]-1, c[1]+1) == 0) {
      redrawScreen();

      r[0] = r[1];
      r[1] = r[1];
      r[2] = r[1]-1;
      r[3] = r[1]-1;

      c[0] = c[1]-1;
      c[1] = c[1];
      c[2] = c[1];
      c[3] = c[1]+1;

      position = '2';
      drawPiece();
      return;
    }
    return;
  }

  if(position == '2') {
    if(checkScreenPos(r[1]-1, c[1], r[1], c[1], r[1], c[1]+1 ,r[1]+1, c[1]+1) == 0) {
      redrawScreen();

      r[0] = r[1]-1;
      r[1] = r[1];
      r[2] = r[1];
      r[3] = r[1]+1;

      c[0] = c[1];
      c[1] = c[1];
      c[2] = c[1]+1;
      c[3] = c[1]+1;

      position = '1';
      drawPiece();
      return;
    }
    return;
  }
}

void changePositionZ() {
  if(position == '1') {
    if(checkScreenPos(r[1], c[1]-1, r[1], c[1], r[1]+1, c[1] ,r[1]+1, c[1]+1) == 0) {
      redrawScreen();

      r[0] = r[1];
      r[1] = r[1];
      r[2] = r[1]+1;
      r[3] = r[1]+1;

      c[0] = c[1]-1;
      c[1] = c[1];
      c[2] = c[1];
      c[3] = c[1]+1;

      position = '2';
      drawPiece();
      return;
    }
    return;
  }

  if(position == '2') {
    if(checkScreenPos(r[1]-1, c[1], r[1], c[1], r[1], c[1]-1 ,r[1]+1, c[1]-1) == 0) {
      redrawScreen();

      r[0] = r[1]-1;
      r[1] = r[1];
      r[2] = r[1];
      r[3] = r[1]+1;

      c[0] = c[1];
      c[1] = c[1];
      c[2] = c[1]-1;
      c[3] = c[1]-1;

      position = '1';
      drawPiece();
      return;
    }
    return;
  }
}


void initI() {
  current_piece = 'I';  
  position = '1';
  
  r[0] = 2;
  r[1] = 3;
  r[2] = 4;
  r[3] = 5;
  
  c[0] = 0;
  c[1] = 0;
  c[2] = 0;
  c[3] = 0;
}

void initL() {
  current_piece = 'L';  
  position = '1';

  r[0] = 3;
  r[1] = 4;
  r[2] = 5;
  r[3] = 5;
  
  c[0] = 0;
  c[1] = 0;
  c[2] = 0;
  c[3] = -1;
}

void initO() {
  current_piece = 'O';
  
  r[0] = 4;
  r[1] = 5;
  r[2] = 4;
  r[3] = 5;
  
  c[0] = 0;
  c[1] = 0;
  c[2] = -1;
  c[3] = -1;
}

void initJ() {
  current_piece = 'J';  
  position = '1';

  r[0] = 3;
  r[1] = 3;
  r[2] = 4;
  r[3] = 5;
  
  c[0] = -1;
  c[1] = 0;
  c[2] = 0;
  c[3] = 0;
}

void initT() {
  current_piece = 'T';  
  position = '1';

  r[0] = 2;
  r[1] = 3;
  r[2] = 3;
  r[3] = 4;
  
  c[0] = 0;
  c[1] = 0;
  c[2] = -1;
  c[3] = 0;
}

void initS() {
  current_piece = 'S';  
  position = '1';

  r[0] = 2;
  r[1] = 3;
  r[2] = 3;
  r[3] = 4;
  
  c[0] = -1;
  c[1] = -1;
  c[2] = 0;
  c[3] = 0;
}

void initZ() {
  current_piece = 'Z';  
  position = '1';

  r[0] = 2;
  r[1] = 3;
  r[2] = 3;
  r[3] = 4;
  
  c[0] = 0;
  c[1] = 0;
  c[2] = -1;
  c[3] = -1;
}


void newPiece()
{
  int8_t n = random(0,  7);

  switch(n) {
    case 0: 
      initI();
      break;
    case 1:
      initL();
      break;
    case 2: 
      initO();
      break;
    case 3: 
      initJ();
      break;
    case 4: 
      initT();
      break;
    case 5: 
      initS();
      break;
    case 6: 
      initZ();
      break;


  }
}

void printScreen() {
  uint8_t i, j = 0;
  for(i = 0; i< 8;i++ ) {
    for(j =0; j<32; j++) {
      Serial.print(" ");
      Serial.print(screen[i][j]);
    }
    Serial.println("");
  }   
}

void moveLeft() {
  if((r[0] > 0) && (r[1] > 0) && (r[2] > 0) && (r[3] > 0)) {
    if (checkScreenPos(r[0]-1, c[0], r[1]-1, c[1], r[2]-1, c[2], r[3]-1, c[3]) == 0) {

      redrawScreen();

      r[0]--;
      r[1]--;
      r[2]--;
      r[3]--;

      drawPiece();
    }
  }
}

void moveRight() {
  if((r[0] < (ROW_SIZE-1)) && (r[1] < (ROW_SIZE-1)) && (r[2] < (ROW_SIZE-1)) && (r[3] < (ROW_SIZE-1))) {
    if (checkScreenPos(r[0]+1, c[0], r[1]+1, c[1], r[2]+1, c[2], r[3]+1, c[3]) == 0) {

      redrawScreen();

      r[0]++;
      r[1]++;
      r[2]++;
      r[3]++;

      drawPiece();
    }
  }
}

uint8_t checkScreenPos(int8_t r0, int8_t c0, int8_t r1, int8_t c1, int8_t r2, int8_t c2, int8_t r3, int8_t c3) {
    
    if((c0 >= TOTAL_COL_SIZE) || (c1 >= TOTAL_COL_SIZE) || (c2 >= TOTAL_COL_SIZE) || (c3 >= TOTAL_COL_SIZE)  || 
        (r0 < 0) || (r1 < 0) || (r2 < 0) || (r3 < 0) ||
        (r0 >= ROW_SIZE) || (r1 >= ROW_SIZE) || (r2 >= ROW_SIZE) || (r3 >= ROW_SIZE) ||
        (c0 >= 0 && (screen[r0][c0] == 1)) || 
        (c1 >= 0 && (screen[r1][c1] == 1)) || 
        (c2 >= 0 && (screen[r2][c2] == 1)) || 
        (c3 >= 0 && (screen[r3][c3] == 1))  ) 
    {
      return 1;
    }

    return 0;

}

void redrawScreen() {
  int i = 0;
  int j = 0;
  for(i = 0; i < 8; i++) {
    for(j=0; j < 32; j++) {
      mx.setPoint(i, j, screen[i][j]);     
    }
  }
}
 

void drawPiece() {
  mx.setPoint(r[0], c[0], 1);
  mx.setPoint(r[1], c[1], 1);
  mx.setPoint(r[2], c[2], 1);
  mx.setPoint(r[3], c[3], 1);
}

void checkCol() {
  for(int i = 0; i < 4; i++)
  {
    // If line is full, clean the line and get down the pieces
    if( (screen[0][c[i]] == 1) && (screen[1][c[i]] == 1) && (screen[2][c[i]] == 1) && (screen[3][c[i]] == 1) && (screen[4][c[i]] == 1) && (screen[5][c[i]] == 1) && (screen[6][c[i]] == 1) && (screen[7][c[i]] == 1)) {
      for(int k = 0; k < 8; k++) {
          screen[k][c[i]] = 0;
      }

      for(int m = 0; m < 8; m++) {
        for(int l = c[i]; l > 0; l--) {
          screen[m][l] = screen[m][(l-1)];
        }
      }
    }
  }
 redrawScreen();
}