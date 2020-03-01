// use the cRGB struct hsv method
#define USE_HSV

#include <WS2812.h>

#define RB_HUE_STEPS 10
#define GLOBAL_SAT 255
#define GLOBAL_BRGHT 60
#define frame_tick 20 //delays between frameupdates
#define game_tick 180 //delays between game updates
#define cheating false
/*
 * snake settings
 */
#define X_start 1
#define Y_start 1
#define SNAKE_LENGHT_START 1

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
  BLUE = -4,
  WHITE = -5,
  APPLE = -20,
  GOLDENAPPLE = -21
} color;

typedef enum{
  IDLE = 0,
  RUNNING,
  PAUSED,
  WON,
  GAME_OVER
} game_states;

/*
 * globals
 */
WS2812 LED(LEDCount);
cRGB pixel;
cRGB blank;

int snake_lenght = SNAKE_LENGHT_START;
int game_state = IDLE;
uint64_t time_since_statechange = 0;
int8_t board[LEDHeight][LEDWidth];
bool masked = false;
bool effekt1 = false;
uint8_t current_brightness = GLOBAL_BRGHT;

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
  uint8_t level = snake_lenght / 12;


  if(masked){
    for(int i=0; i<LEDCount; i++)
      LED.set_crgb_at(i, blank);
  }
  else{
    for(int i = 0; i < LEDCount; i++)
    {
      int8_t field_value = board[x][LEDWidth-1-y];

      if(field_value == APPLE){
        /* blinking apples
        int peri = 300 ;
        int t = millis() % peri;
        if(t<peri/2){
          pixel.SetHSV(0, 0, (uint16_t) current_brightness * t / peri/2 +10);
        }
        else{
          t -= peri/2;
          pixel.SetHSV(0, 0, (uint16_t) current_brightness * ((peri/2)-t) / peri/2 +10);
        }*/
        pixel.SetHSV(0, 0, (uint16_t) current_brightness/2);
        LED.set_crgb_at(i, pixel);
      }
      else if(field_value > 0){
        if(game_state == GAME_OVER) {
          if (((millis()-time_since_statechange) % ((snake_lenght)*60)) / 60 == (snake_lenght-field_value)){
            LED.set_crgb_at(i, blank);
          }
          else{
            uint16_t hue = (level*360/(6) )%360;
            uint16_t bri = current_brightness * (field_value / snake_lenght) + 1;
            pixel.SetHSV(hue, GLOBAL_SAT/2, bri);
            LED.set_crgb_at(i, pixel);
          }
        }
        else if(game_state == WON || game_state == IDLE){
          // changing rainbow from head to tail
          pixel.SetHSV((hue+field_value*360/32)%360, GLOBAL_SAT/2, current_brightness);
          LED.set_crgb_at(i, pixel);
        }
        else{
          //change satuation to half
          //pixel.SetHSV(((snake_lenght)*360/64)%360, field_value * (GLOBAL_SAT/2) * 3/4 / snake_lenght + 64, current_brightness);

          // change sat to 0
          // pixel.SetHSV(((snake_lenght)*360/64)%360, field_value * (GLOBAL_SAT/2) / snake_lenght, current_brightness);

          // continous long rainbow snake, fixed
          //pixel.SetHSV(((snake_lenght-field_value+2)*360/64)%360, GLOBAL_SAT/2, current_brightness);

          // changing rainbow from head to tail
          //pixel.SetHSV((hue+field_value*360/32)%360, GLOBAL_SAT/2, current_brightness);

          // changing rainbow from tail to head
          //pixel.SetHSV((hue+(snake_lenght-field_value+2)*360/32)%360, GLOBAL_SAT/2, current_brightness);

          // changing color every 12 steps   + ((snake_lenght - field_value)* (64 / snake_lenght))
          //uint16_t hue = (level*360/(64/6) )%360;  //nice discrete colors

          uint16_t hue = (level*360/(6) )%360;
          uint16_t bri = current_brightness * (field_value / snake_lenght) + 1;
          pixel.SetHSV(hue, GLOBAL_SAT/2, bri);

          LED.set_crgb_at(i, pixel);
        }
      }
      else if(field_value == RB){
        pixel.SetHSV((hue+i*360/9)%360, GLOBAL_SAT, current_brightness/2);
        LED.set_crgb_at(i, pixel);
      }
      else if(field_value == WHITE){
        pixel.SetHSV(0, 0, current_brightness);
        LED.set_crgb_at(i, pixel);
      }
      else if(field_value == RED){
        pixel.SetHSV(0, GLOBAL_SAT, current_brightness);
        LED.set_crgb_at(i, pixel);
      }
      else if(field_value == GREEN){
        pixel.SetHSV(120, GLOBAL_SAT, current_brightness);
        LED.set_crgb_at(i, pixel);
      }
      else if(field_value == BLUE){
        pixel.SetHSV(240, GLOBAL_SAT, current_brightness);
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
  }

  LED.sync();
}

uint8_t getInput(uint8_t in){
  uint8_t inp = 0;
  inp |= !digitalRead(a_but)<<A;
  inp |= !digitalRead(b_but)<<B;
  inp |= !digitalRead(up_but)<<UP;
  inp |= !digitalRead(down_but)<<DOWN;
  inp |= !digitalRead(right_but)<<RIGHT;
  inp |= !digitalRead(left_but)<<LEFT;
  if(inp)
    rng();
  return inp | in;
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

uint64_t rng(){
  uint64_t static seed = 1000;
  int non_deterministic = millis() % 10000;
  seed = ((seed+non_deterministic) * 344534537681+4534535809)%875644564849;
  return seed;
}

void newApple(){
  int8_t* linear_board = (int8_t*)board;
  int free_places[LEDCount];
  int free_places_cnt = 0;
  memset(free_places,0, sizeof(free_places));
  for(int i=0; i<LEDCount; i++)
  {
    if(linear_board[i]==0){
      free_places[free_places_cnt++] = i;
    }
  }
  if(free_places_cnt == 0){
    game_state = WON;
  }
  uint8_t kind_of_apple = APPLE;
  if(rng()%100 == 42)
    kind_of_apple = APPLE;
  linear_board[free_places[rng()%free_places_cnt]] = kind_of_apple;
}

void snake(uint8_t in){
  int static vel[2] = {1,0};
  int static pos[2] = {X_start,Y_start};
  int new_pos[2];

  /*
   * inputs
   */
  if(input(in,A))// || input(in,B))
  {
    if(game_state == GAME_OVER || game_state == IDLE || game_state == WON){
      game_state = RUNNING;
      pos[X_] = X_start;
      pos[Y_] = Y_start;
      vel[X_] = 0;
      vel[Y_] = 1;
      snake_lenght = SNAKE_LENGHT_START;
      memset(board, 0, sizeof(board));
      board[pos[X_]][pos[Y_]] = snake_lenght;
      newApple();
      //newApple();
      //newApple();
      //newApple();
      time_since_statechange = millis();
      current_brightness = GLOBAL_BRGHT;
      masked = false;
      effekt1 = false;
    }
  }

  if(game_state == GAME_OVER || game_state == WON){
  }
  else if(game_state == IDLE)
  {
    board[pos[X_]][pos[Y_]] = snake_lenght;
  }
  else if(game_state == RUNNING)
  {
    /*
     * inputs
     */
    if(vel[X_] == 0){
      in &= ~(1<<UP);
      in &= ~(1<<DOWN);
    }
    else if(vel[Y_] == 0){
      in &= ~(1<<LEFT);
      in &= ~(1<<RIGHT);
    }

    bin_print(in);
    Serial.println();
    if(vel[Y_] >= 0 && input(in,UP)){
      vel[Y_] = 1;
      vel[X_] = 0;
    }
    if(vel[Y_] <= 0 && input(in,DOWN)){
      vel[Y_] = -1;
      vel[X_] = 0;
    }
    if(vel[X_] >= 0 && input(in,RIGHT)){
      vel[Y_] = 0;
      vel[X_] = 1;
    }
    if(vel[X_] <= 0 && input(in,LEFT)){
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
      game_state = GAME_OVER;
    }
    if(new_pos[Y_] < 0){
      new_pos[Y_] = 0;
      game_state = GAME_OVER;
    }
    if(new_pos[X_] >= LEDWidth){
      new_pos[X_] = pos[X_];
      game_state = GAME_OVER;
    }
    if(new_pos[Y_] >= LEDHeight){
      new_pos[Y_] = pos[Y_];
      game_state = GAME_OVER;
    }
    if(board[new_pos[X_]][new_pos[Y_]] > 1)
      game_state = GAME_OVER;

    if(game_state == GAME_OVER){
      if(cheating){
        game_state = RUNNING;
        return;
      }
      else{
        time_since_statechange = millis();
        return;
      }
    }

    /*
     * check for apple, if not shorten the snake
     */
    int8_t* linear_board = (int8_t*)board;
    if(board[new_pos[X_]][new_pos[Y_]] == APPLE){
      snake_lenght++;
      newApple();
    }
    else
      for(int i=0; i<LEDCount && !cheating; i++)
      {
        if(linear_board[i]>0){
          linear_board[i]--;
        }
      }
    if(cheating)
      snake_lenght++;

    pos[X_] = new_pos[X_];
    pos[Y_] = new_pos[Y_];
    board[new_pos[X_]][new_pos[Y_]] = snake_lenght;
    if(snake_lenght == LEDCount)
      snake_lenght = LEDCount;
  }
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
  uint8_t static in = 0;

  if (task(game_tick)){
    snake(in);
    in = 0;
  }

  renderBoard();

  /*
   * timing
   */
  uint64_t time_end = millis();
  if((time_end-time_start)>frame_tick*7/10)
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
  while(millis() < time_start + frame_tick){
    in = getInput(in);
  }
}