// use the cRGB struct hsv method
#define USE_HSV

#include <WS2812.h>

#define RB_HUE_STEPS 10
#define GLOBAL_SAT 255
#define GLOBAL_BRGHT 80
#define frame_tick 20 //delays between frameupdates
#define game_tick 400 //delays between game updates

/*
 * board definitions
 */
#define LEDHeight 8
#define LEDWidth 8
#define LEDCount (LEDHeight * LEDWidth)

/*
 * button mapping
 */
#define ledDataPin 7
#define a_but 3
#define b_but 4
#define down_but 5
#define up_but 6
#define right_but 8
#define left_but 9

/*
 * typedefs
 */
typedef enum{
  A = 0,
  B,
  UP,
  DOWN,
  RIGHT,
  LEFT
} key;

typedef enum{
  X_ = 0,
  Y_
} cord;

typedef enum{
  BLANK = 0,
  RB = -1,
  RED = -2,
  GREEN = -3,
  BLUE = -4
} color;

/*
 * globals
 */
WS2812 LED(LEDCount);
cRGB pixel;
cRGB blank;
int snake_lenght = 15;

uint8_t board[LEDHeight][LEDWidth];

void setup() {
  pinMode(a_but, INPUT_PULLUP);
  pinMode(b_but, INPUT_PULLUP);
  pinMode(up_but, INPUT_PULLUP);
  pinMode(down_but, INPUT_PULLUP);
  pinMode(left_but, INPUT_PULLUP);
  pinMode(right_but, INPUT_PULLUP);
  LED.setOutput(ledDataPin);
  Serial.begin(115200);

  blank.r=0;
  blank.g=0;
  blank.b=0;

  memset(board, 0, sizeof(board));
}

void renderBoard(){
  int x=0, y=0;
  int static hue = 0;   //stores 0 to 614
  hue += RB_HUE_STEPS;
  hue %= 360;

  for(int i = 0; i < LEDCount; i++)
  {
    uint8_t field_value = board[x][LEDWidth-1-y];
    if(field_value > 0){
      pixel.SetHSV(((snake_lenght-field_value+2)*360/64)%360, GLOBAL_SAT, GLOBAL_BRGHT);
      LED.set_crgb_at(i, pixel);
    }
    else if(field_value == RB){
      pixel.SetHSV((hue+i*360/9)%360, GLOBAL_SAT, GLOBAL_BRGHT);
      LED.set_crgb_at(i, pixel);
    }
    else if(field_value == RED){
      pixel.SetHSV(0, GLOBAL_SAT, GLOBAL_BRGHT);
      LED.set_crgb_at(i, pixel);
    }
    else if(field_value == GREEN){
      pixel.SetHSV(120, GLOBAL_SAT, GLOBAL_BRGHT);
      LED.set_crgb_at(i, pixel);
    }
    else if(field_value == BLUE){
      pixel.SetHSV(240, GLOBAL_SAT, GLOBAL_BRGHT);
      LED.set_crgb_at(i, pixel);
    }
    else
    {
      LED.set_crgb_at(i, blank);
    }
    x++;
    if(x==LEDHeight){
      x=0;
      y++;
    }
  }
  LED.sync();
}

uint8_t getInput(){
  uint8_t inp = 0;
  inp |= !digitalRead(a_but)<<A;
  inp |= !digitalRead(b_but)<<B;
  inp |= !digitalRead(up_but)<<UP;
  inp |= !digitalRead(down_but)<<DOWN;
  inp |= !digitalRead(right_but)<<RIGHT;
  inp |= !digitalRead(left_but)<<LEFT;
  return inp;
}

void bin_print(uint8_t b){
  Serial.print("0b");
  for(int i=0; i<8; i++){
    if( b & (1<<(7-i)) )
      Serial.print("1");
    else
      Serial.print("0");
  }
}

bool inline input(uint8_t in, uint8_t button){
  return (in & (1 << button));
}

void snake(uint8_t in){
  int static vel[2] = {0,1};
  int static pos[2] = {2,2};
  int new_pos[2];

  //memset(board, 0, sizeof(board));
  /*
   * shorten the snake
   */
  uint8_t* linear_board = (uint8_t*)board;
  for(int i=0; i<LEDCount; i++)
  {
    if(linear_board[i]>0){
      linear_board[i]--;
    }
  }

  /*
   * inputs
   */
  if(input(in,UP)){
    vel[Y_] = 1;
    vel[X_] = 0;
  }
  if(input(in,DOWN)){
    vel[Y_] = -1;
    vel[X_] = 0;
  }
  if(input(in,RIGHT)){
    vel[Y_] = 0;
    vel[X_] = 1;
  }
  if(input(in,LEFT)){
    vel[Y_] = 0;
    vel[X_] = -1;
  }

  /*
   * translation
   */
  new_pos[X_] = pos[X_]+vel[X_];
  new_pos[Y_] = pos[Y_]+vel[Y_];


  /*
   * bounding box
   */
  if(new_pos[X_] < 0){
    new_pos[X_] = 0;
  }
  if(new_pos[Y_] < 0){
    new_pos[Y_] = 0;
  }
  if(new_pos[X_] >= LEDWidth){
    new_pos[X_] = pos[X_];
  }
  if(new_pos[Y_] >= LEDHeight){
    new_pos[Y_] = pos[Y_];
  }

  pos[X_] = new_pos[X_];
  pos[Y_] = new_pos[Y_];
  board[new_pos[X_]][new_pos[Y_]] = snake_lenght;
  if(snake_lenght == LEDCount)
    snake_lenght = LEDCount;
}

bool task(int period){
  uint64_t static time_start = 0;
  if(millis() >= time_start + period){
    time_start = millis();
    return true;
  }
  return false;
}

void loop() {
  uint64_t time_start = millis();
  uint8_t static in;
  renderBoard();
  in |= getInput();

  if (task(game_tick)){
    bin_print(in);
    Serial.println();
    snake(in);
    in = 0;
  }

  /*
   * timing
   */
  uint64_t time_end = millis();
  if(true)//(time_end-time_start)>frame_tick*7/10)
  {
    Serial.print(" flow: ");
    Serial.print((int)((time_end-time_start)*100)/frame_tick);
    Serial.println("%");
  }
  if((time_end-time_start)>frame_tick)
  {
    Serial.println(" REALTIME VIOLATION");
    pixel.SetHSV(0, 255, GLOBAL_BRGHT); //red
    for(int i=0; i<64; i++){
      LED.set_crgb_at(i, pixel);
    }
    LED.sync();
  }
  while(millis() < time_start + frame_tick)
    ;
}