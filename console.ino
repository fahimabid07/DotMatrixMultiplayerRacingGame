const byte ROW_SLC_VAL = 4;
const byte ROW_SLC_CLK = 2;
const byte COL_SLC_VAL = 8;
const byte COL_SLC_CLK = 7;
const byte BUTTON_MSB = 13; // left
const byte BUTTON_LSB = 12; // right

const byte SCRN_W = 8;
const byte SCRN_H = 16;
const byte GAMELVL_LEN = 130;

byte screen[SCRN_H][SCRN_W] ={{1,1,1,1,1,1,1,1},
                              {1,1,1,1,1,1,1,1},
                              {1,1,1,1,1,1,1,1},
                              {1,1,1,1,1,1,1,1},
                              {1,1,1,1,1,1,1,1},
                              {1,1,1,1,1,1,1,1},
                              {1,1,1,1,1,1,1,1},
                              {1,1,1,1,1,1,1,1}};

byte game_level[GAMELVL_LEN][SCRN_W] = {};
                    
int current_row;
int current_row_ctrl;
int player_pos[2];
int player_pos_global[2];
int screen_window_pos[2];
byte user_input[4];
int collision_count = 0;
int flag = 0;
int input_flag = 0;
byte finishline = 30;
String verdict;
bool score_sent = 1;
bool score_shown = 0;

void make_game_level(byte game_level[GAMELVL_LEN][SCRN_W], byte nobs, byte finishline){
  for (int y=0; y<GAMELVL_LEN; y++){
    for (int x=0; x<SCRN_W; x++){
      if (x==0 || x==SCRN_W-1){
        game_level[y][x] = 0;
      }else{
        game_level[y][x] = 1;
      }
    }
  }

  randomSeed(115);
  for (int d=0; d<nobs; d++){
      game_level[random(0,GAMELVL_LEN)][random(1,SCRN_W-1)] = 0;
  }

  // make finish line
  for (int y=0; y<finishline; y++){
    for (int x=0; x<SCRN_W; x++){
      if (y>=finishline-2){
        game_level[y][x] = 0;
      }else{
        game_level[y][x] = 1;
      }
    }
  }
}

//void print_game_level(){
//  for (int y=0; y<128; y++){
//    for (int x=0; x<8; x++){
//      Serial.print(game_level[y][x],BIN);
//    }
//    Serial.println();
//  }
//}

void load_row_bits(int current_row, int SCRN_H, int wait){
  if (current_row!=SCRN_H-1){
    digitalWrite(ROW_SLC_VAL,0);
  }else{
    digitalWrite(ROW_SLC_VAL,1);
  }
  digitalWrite(ROW_SLC_CLK,1);
  delayMicroseconds(wait);
  digitalWrite(ROW_SLC_CLK,0);
  delayMicroseconds(wait);
}

void load_data_bits(byte screen[SCRN_H][SCRN_W], int current_row, int arrlen, int wait){
  for (int idx=0; idx<arrlen; idx++){
    digitalWrite(COL_SLC_VAL,screen[current_row][idx]);
    digitalWrite(COL_SLC_CLK,1);
    delayMicroseconds(wait);
    digitalWrite(COL_SLC_CLK,0);
    delayMicroseconds(wait);
  }
  digitalWrite(COL_SLC_CLK,1);
  delayMicroseconds(wait);
  digitalWrite(COL_SLC_CLK,0);
  delayMicroseconds(wait);
}

void init_global_vars(){
  current_row = 0;
  current_row_ctrl = pow(2,SCRN_H-1);
  player_pos[0] = SCRN_H-4;
  player_pos[1] = SCRN_W/2;
  player_pos_global[0] = SCRN_H-4;
  player_pos_global[1] = SCRN_W/2;
  user_input[0] = 0;
  user_input[1] = 0;
  user_input[2] = 0;
  user_input[3] = 0;
  screen_window_pos[0] = GAMELVL_LEN-SCRN_H-1;
  screen_window_pos[1] = 0;
}

void init_pins(){
  pinMode(ROW_SLC_VAL,OUTPUT);
  pinMode(ROW_SLC_CLK,OUTPUT);
  pinMode(COL_SLC_VAL,OUTPUT);
  pinMode(COL_SLC_CLK,OUTPUT);
  pinMode(BUTTON_MSB,INPUT);
  pinMode(BUTTON_LSB,INPUT);
}

void read_user_input(byte user_input[4]){
  byte button_R = digitalRead(BUTTON_LSB);
  byte button_L = digitalRead(BUTTON_MSB);
//  byte button_ML = 3 - (button_M*2 + button_L);
//  if (button_ML==0){
//    user_input[0] = 1;
//    user_input[1] = 0;
//    user_input[2] = 0;
//    user_input[3] = 0;
//  }else if (button_ML==1){
//    user_input[0] = 0;
//    user_input[1] = 1;
//    user_input[2] = 0;
//    user_input[3] = 0;
//  }else if (button_ML==2){
//    user_input[0] = 0;
//    user_input[1] = 0;
//    user_input[2] = 1;
//    user_input[3] = 0;
//  }else if (button_ML==3){
//    user_input[0] = 0;
//    user_input[1] = 0;
//    user_input[2] = 0;
//    user_input[3] = 1;
//  }
  if (button_L==1){
    user_input[0] = 0;
    user_input[1] = 0;
    user_input[2] = 0;
    user_input[3] = 1;
  }else if (button_R==1){
    user_input[0] = 0;
    user_input[1] = 0;
    user_input[2] = 1;
    user_input[3] = 0;
  }else{
    user_input[0] = 0;
    user_input[1] = 0;
    user_input[2] = 0;
    user_input[3] = 0;
  }
}

void load_screen_window(byte screen[SCRN_H][SCRN_W],byte game_level[GAMELVL_LEN][SCRN_W],int screen_window_pos[2]){
  for (int y=0; y<SCRN_H; y++){
    for (int x=0; x<SCRN_W; x++){
      screen[y][x] = game_level[screen_window_pos[0]+y+1][x];
    }
  }
}

void update_screen(byte screen[SCRN_H][SCRN_W],byte game_level[GAMELVL_LEN][SCRN_W],int player_pos[2],int screen_window_pos[2]){
  load_screen_window(screen,game_level,screen_window_pos);
  
  screen[player_pos[0]][player_pos[1]] = 1;
  screen[player_pos[0]+1][player_pos[1]] = 1;
  //player_pos[0] = player_pos[0] - user_input[0] + user_input[1];
  //screen_window_pos[0] = screen_window_pos[0] - user_input[0] + user_input[1];
  screen_window_pos[0] -= 1;
  player_pos[1] = player_pos[1] + user_input[2] - user_input[3];
//  if (player_pos[0]<0){
//    player_pos[0] = 0;
//  }
//  if (player_pos[0]>6){
//    player_pos[0] = 6;
//  }
  if (screen_window_pos[0]<0){
    screen_window_pos[0] = 0;
  }
  if (screen_window_pos[0]>GAMELVL_LEN-SCRN_H-1){
    screen_window_pos[0] = GAMELVL_LEN-SCRN_H-1;
  }
  if (player_pos[1]<1){
    player_pos[1] = 1;
  }
  if (player_pos[1]>SCRN_W-2){
    player_pos[1] = SCRN_W-2;
  }
  screen[player_pos[0]][player_pos[1]] = 0; // not changing
  screen[player_pos[0]+1][player_pos[1]] = 0;
}

void collision_detection(byte screen[SCRN_H][SCRN_W],byte game_level[GAMELVL_LEN][SCRN_W],int player_pos[2],int screen_window_pos[2]){
  if (game_level[screen_window_pos[0]+player_pos[0]][player_pos[1]] == 0){
    collision_count++;
  }
}

void show_winner(byte game_level[GAMELVL_LEN][SCRN_W],int player_pos[2],int screen_window_pos[2]){
  for (int y=0; y<SCRN_H; y++){
    for (int x=0; x<SCRN_W; x++){
      if (x==SCRN_W/2){
        game_level[y][x] = 0;
      }else{
        game_level[y][x] = 1;
      }
    }
  }
}

void show_looser(byte game_level[GAMELVL_LEN][SCRN_W],int player_pos[2],int screen_window_pos[2]){
  for (int y=0; y<SCRN_H; y++){
    for (int x=0; x<SCRN_W; x++){
      if (y==SCRN_H/2){
        game_level[y][x] = 0;
      }else{
        game_level[y][x] = 1;
      }
    }
  }
}

void show_tie(byte game_level[GAMELVL_LEN][SCRN_W],int player_pos[2],int screen_window_pos[2]){
  for (int y=0; y<SCRN_H; y++){
    for (int x=0; x<SCRN_W; x++){
      if (y==SCRN_H/2 || x==SCRN_W/2){
        game_level[y][x] = 0;
      }else{
        game_level[y][x] = 1;
      }
    }
  }
}

void setup() {
  init_pins();
  init_global_vars();
  make_game_level(game_level,10,finishline);
  Serial.begin(9600);while (! Serial);
}

void loop() {
  load_row_bits(current_row,SCRN_H,2);
  load_data_bits(screen,current_row,SCRN_W,2);

  if (input_flag == 10){
    read_user_input(user_input); input_flag = 0;
  }else{
    input_flag++;
  }


  if (flag==1){
    flag = 0;
    update_screen(screen,game_level,player_pos,screen_window_pos);
    collision_detection(screen,game_level,player_pos,screen_window_pos);
  }
  
  
  current_row++;
  current_row_ctrl/=2;
  delay(5);
  if (current_row==SCRN_H){
    current_row = 0; current_row_ctrl = pow(2,SCRN_H-1); flag = 1;
  }else{
    flag = 0;
  }

  if (screen_window_pos[0]==0 && score_sent==1){
    // game finished
    Serial.println(collision_count-2); // send to Rpi
    score_sent = 0;
  }
  //Serial.println(69);
  if (Serial.available()){
    verdict = Serial.readStringUntil('\n');
    verdict.trim();
    //Serial.println(verdict);
    if (verdict.equals("1") && score_shown==0){
      show_winner(game_level,player_pos,screen_window_pos); score_shown = 1;
    }else if (verdict.equals("0") && score_shown==0){
      show_looser(game_level,player_pos,screen_window_pos); score_shown = 1;
    }else if (score_shown==0){
      show_tie(game_level,player_pos,screen_window_pos); score_shown = 1;
    }
  } 
}
