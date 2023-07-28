#include <MD_MAX72xx.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES	4

#define CLK_PIN   18  // or SCK
#define DATA_PIN  23  // or MOSI
#define CS_PIN    5  // or SS

#define BTN_CHANGE 27

#define MOVE_PIN 35

#define  DELAY_TIME  500  // in milliseconds
#define  DELAY_CONTROL 100

#define TOTAL_COL_SIZE (COL_SIZE * MAX_DEVICES)

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


char current_piece = 'X';
char position = '1';

int16_t x = 0;

unsigned long button_millis;  
unsigned long move_millis;  
unsigned long moveLR_millis;  

int8_t r[4] = {0};
int8_t c[4] = {0};

int8_t screen[8][32] = {0};
int8_t auxr, auxc;

int8_t first = 1;

void moveLeft();
void moveRight();

void setup() {
  pinMode(BTN_CHANGE, INPUT);
  pinMode(MOVE_PIN, INPUT);

  button_millis = millis();
  move_millis = millis();
  moveLR_millis = millis();
  
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
  
}

void loop() {
  // if(first == 1) {
  //   Serial.println("Tá entrando aquio porro!"); 
  //   delay(3000);
  //   first = 0;
  //   newPiece();
  //   Serial.print("\nCurrent piece no começo do loop "); 
  //   Serial.println(current_piece);
  //   delay(5000);
  // }
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

  
//  delay(DELAY_TIME);

}

void movePiece() {
  if (checkScreenPosition() == 1) {
    return;
  }

  mx.setPoint(r[0], c[0]-1, 0);
  mx.setPoint(r[1], c[1]-1, 0);
  mx.setPoint(r[2], c[2]-1, 0);
  mx.setPoint(r[3], c[3]-1, 0);

  mx.setPoint(r[0], c[0], 1);
  mx.setPoint(r[1], c[1], 1);
  mx.setPoint(r[2], c[2], 1);
  mx.setPoint(r[3], c[3], 1);
  if(((c[0] < TOTAL_COL_SIZE) || (c[1] < TOTAL_COL_SIZE) || (c[2] < TOTAL_COL_SIZE) || (c[3] < TOTAL_COL_SIZE)) ) {
    c[0]++;
    c[1]++;
    c[2]++;
    c[3]++;
  
  }
}

void changePosition() {
  Serial.println("Aqui no changeposition");

  if(current_piece == 'I') {
    if(position == '1') {
        c[0] = c[0];
        c[1] = c[0]-1;
        c[2] = c[0]-2;
        c[3] = c[0]-3;

        r[0] = r[0];
        r[1] = r[0];
        r[2] = r[0];
        r[3] = r[0];
        position = '2';
        return;
    }

    if(position == '2') {
        c[0] = c[0];
        c[1] = c[0];
        c[2] = c[0];
        c[3] = c[0];

        r[0] = r[0];
        r[1] = r[0]+1;
        r[2] = r[0]+2;
        r[3] = r[0]+3;
        position = '1';
        return;
    }
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

void newPiece()
{
  int8_t n = random(0,  3);

  if(n == 0) {
    initI();
  } 
  else if(n == 1) {
    initL();
  } 
  else if(n == 2) {
    initO();
  }

 // Serial.print("No fim do newPiece, Current piece: ");
 // Serial.println(current_piece);
 // printScreen();
}

int8_t checkScreen() {
  if(
    (c[0] >= 0 && (screen[r[0]][c[0]] == 1)) || 
    (c[1] >= 0 && (screen[r[1]][c[1]] == 1)) || 
    (c[2] >= 0 && (screen[r[2]][c[2]] == 1)) || 
    (c[3] >= 0 && (screen[r[3]][c[3]] == 1))  ) {
    // Serial.println("Caiu no checkscreen");
    return 1;
  }

  return -1;
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
  // delay(3000);
}


void moveLeft() {
  if((r[0] > 0) && (r[1] > 0) && (r[2] > 0) && (r[3] > 0)) {
      r[0]--;
      r[1]--;
      r[2]--;
      r[3]--;

      if (checkScreenPosition() == 0) {

     //   if((screen[r[0]-1][c[0]] == 0) &&  (screen[r[1]-1][c[1]] == 0) && (screen[r[2]-1][c[2]] == 0) &&  (screen[r[3]-1][c[3]] == 0)) {
          mx.setPoint(r[0]+1, c[0]-1, 0);
          mx.setPoint(r[1]+1, c[1]-1, 0);
          mx.setPoint(r[2]+1, c[2]-1, 0);
          mx.setPoint(r[3]+1, c[3]-1, 0);

          mx.setPoint(r[0]+1, c[0], 0);
          mx.setPoint(r[1]+1, c[1], 0);
          mx.setPoint(r[2]+1, c[2], 0);
          mx.setPoint(r[3]+1, c[3], 0);
    
          mx.setPoint(r[0], c[0], 1);
          mx.setPoint(r[1], c[1], 1);
          mx.setPoint(r[2], c[2], 1);
          mx.setPoint(r[3], c[3], 1);
       // }
      }
      else {
        r[0]++;
        r[1]++;
        r[2]++;
        r[3]++;

      }
  }

}

void moveRight() {
  if((r[0] < (ROW_SIZE-1)) && (r[1] < (ROW_SIZE-1)) && (r[2] < (ROW_SIZE-1)) && (r[3] < (ROW_SIZE-1))) {
    if (checkScreenPosition() == 0) {
      mx.setPoint(r[0], c[0]-1, 0);
      mx.setPoint(r[1], c[1]-1, 0);
      mx.setPoint(r[2], c[2]-1, 0);
      mx.setPoint(r[3], c[3]-1, 0);

      mx.setPoint(r[0], c[0], 0);
      mx.setPoint(r[1], c[1], 0);
      mx.setPoint(r[2], c[2], 0);
      mx.setPoint(r[3], c[3], 0);

      r[0]++;
      r[1]++;
      r[2]++;
      r[3]++;

      mx.setPoint(r[0], c[0], 1);
      mx.setPoint(r[1], c[1], 1);
      mx.setPoint(r[2], c[2], 1);
      mx.setPoint(r[3], c[3], 1);

    }
  }
}

uint8_t checkScreenPosition() {
    if((c[0] >= TOTAL_COL_SIZE) || (c[1] >= TOTAL_COL_SIZE) || (c[2] >= TOTAL_COL_SIZE) || (c[3] >= TOTAL_COL_SIZE)  || (checkScreen() == 1 ) )
    {
      screen[r[0]][c[0]-1] = 1;
      screen[r[1]][c[1]-1] = 1;
      screen[r[2]][c[2]-1] = 1;
      screen[r[3]][c[3]-1] = 1;

      newPiece();
      return 1;
    }

    return 0;

}