#include "minesweeper.h"


static Sound lose_sound, win_sound, tick_sound;
static Texture2D cell_up, cell_down, flag_cell,  mine_cell, blast_cell, false_mine; 
static Texture2D bottom_l, bottom_r, mid, mid_l, mid_r, top_l, top_r, top_b;
static Texture2D cells_neibers[MAX_NEIBERS];

static Texture2D nums[10];
static Texture2D faces[FACES_COUNT];
static FaceType cur_face = Default;
static float change_timer = 0.0f; 


static unsigned int cur_time;
static int game_over = 0;
static int mines_all;


void startMinesweeper(int width, int height, int mines) {
  InitWindow((CELL_SIZE * width) + (CELL_SIZE * 2), (CELL_SIZE * height) + (CELL_SIZE * 2) + (CELL_SIZE * 4), "Minesweeper");    
  SetTargetFPS(30);

  InitAudioDevice();
  loadResources();
  mines_all = mines;
  newGame(width, height, mines);
  
  CloseAudioDevice();
  CloseWindow();
}


static void newGame(int width, int height, int mines) {
  Field field = fdInit(width, height, mines);
  if (field.field == NULL) {
    printf("ERROR: Cant allocate field\n");
    exit(1);
  } 


  float start = GetTime();
  float passed;
  cur_time = 0;
  while (!WindowShouldClose()) {
    update(&field);
    if (!game_over) { 
      passed = GetTime();
      if (passed - start >= 1) {
        cur_time++;
        start = passed;
      }
    }
    

   

    BeginDrawing();
    ClearBackground(GRAY);
    drawBorders(&field);
    drawField(&field);
    drawHeader(&field);
    EndDrawing();
  }

  fdClear(&field);
}

static void drawHeader(const Field *FIELD) {
  Vector2 pos = {
    .x = (FIELD->width >> 1) * CELL_SIZE,
    .y = CELL_SIZE + (CELL_SIZE >> 1)
  };

  DrawTextureV(faces[cur_face], pos, WHITE);

  pos.x = CELL_SIZE + (CELL_SIZE >> 1) + (CELL_SIZE << 1);
  drawNums(FIELD, pos, FIELD->flags_left);
  
  pos.x = (FIELD->width * CELL_SIZE) - (CELL_SIZE >> 1);
  drawNums(FIELD, pos, cur_time);
}

static void drawNums(const Field *FIELD, Vector2 pos, unsigned int num) {
  if (num > 999) 
    num %= 999;

  
  for (int i = 0; i < 3; i++, pos.x -= CELL_SIZE) {
    DrawTextureV(nums[num%10], pos, WHITE);
    num /= 10;
  }
}

static void drawBorders(const Field *FIELD) {
  Vector2 pos = {0};


  drawHorBorder(&top_l, &top_b, &top_r, FIELD, &pos);
  drawVerBorder(&mid, 3, &pos);

  drawHorBorder(&mid_l, &top_b, &mid_r, FIELD, &pos);
  drawVerBorder(&mid, FIELD->height, &pos);
  drawHorBorder(&bottom_l, &top_b, &bottom_r, FIELD, &pos);
}


static void drawHorBorder(const Texture2D *left, const Texture2D *mid, const Texture2D *right, const Field *field, Vector2 *pos) {
  DrawTextureV(*left, *pos, WHITE);
  pos->x += CELL_SIZE;
  for (int i = 0; i < field->width; i++, pos->x += CELL_SIZE) 
    DrawTextureV(*mid, *pos, WHITE);
  DrawTextureV(*right, *pos, WHITE);

  pos->y += CELL_SIZE;
}

static void drawVerBorder(const Texture2D *mid, int height, Vector2 *pos) {
  float tmp = pos->x;

  for (int i = 0; i < height; i++, pos->y += CELL_SIZE) {
    pos->x = 0;
    DrawTextureV(*mid, *pos, WHITE);
    pos->x = tmp;
    DrawTextureV(*mid, *pos, WHITE);
  }
  pos->x = 0;
}

static void drawField(const Field *FIELD) {
  Texture *texture;
  Vector2 pos = {CELL_SIZE, CELL_SIZE * HEADER_SIZE};

  int index = 0;
  for (int i = 0; i < FIELD->height; i++, pos.x = CELL_SIZE, pos.y += CELL_SIZE) {
    for (int j = 0; j < FIELD->width; j++, index++, pos.x += CELL_SIZE) {
      switch (FIELD->field[index]) {
        case Empty:
        case Mine:
          texture = &cell_up;
          break;
        case Checked:
          texture = &cell_down;
          break;
        case Flag:
        case Mine | Flag:
          texture = &flag_cell;
          break;
        case Blast:
          texture = &blast_cell;
          break;
        case False:
          texture = &false_mine;
          break;
        case SMine:
          texture = &mine_cell;
          break;
        default:
          if (FIELD->field[index] >= 1 && FIELD->field[index] <= MAX_NEIBERS) {
            texture = &cells_neibers[FIELD->field[index]-1];
          }
          else {
            texture = &cell_down;
          }
          break;
      }

      DrawTextureV(*texture, pos, WHITE);
    }
  }
}


static void countMines(const Field *FIELD, int x, int y) {
  int mines = 0, neibers_count = 0;
  int neibersx[MAX_NEIBERS];
  int neibersy[MAX_NEIBERS];


  int cur = (y * FIELD->width) + x;
  FIELD->field[cur] = Checked;
  for (int i = -1; i < 2; i++) {
    int new_y = y + i;
    if (new_y < 0 || new_y >= FIELD->height)
      continue;

    for (int j = -1; j < 2; j++) {
      int new_x = x + j;
      if (new_x < 0 || new_x >= FIELD->width)
        continue;

      int index = (new_y * FIELD->width) + new_x;
      if (FIELD->field[index] & Mine) {
        mines++;
      }
      else if (mines == 0 && FIELD->field[index] == Empty) {
        neibersx[neibers_count] = new_x;
        neibersy[neibers_count++] = new_y;
      }
    }
  }

  if (mines > 0) {
    FIELD->field[cur] = mines;
    return;
  }

  for (int i = 0; i < neibers_count; i++) 
    countMines(FIELD, neibersx[i], neibersy[i]);
}


static void show(Field *field) {
  for (int i = 0; i < field->size; i++) {
    switch (field->field[i]) {
      case Mine:
        field->field[i] = SMine;
        break;
      case Flag:
        field->field[i] = False;
        break;
    }
  }
}

static void updateFace() {
  if (cur_face == Default) 
    return;

  change_timer += GetFrameTime();
  if (change_timer >= FACE_CHANGE_TIME) {
    cur_face = Default;
    change_timer = 0.0f;
  }
}

static void update(Field *field) {
  if (!game_over) {  
    updateFace();
    if (!field->flags_left) {
      game_over = 1;
      PlaySound(win_sound);
      cur_face = Win;

      return;
    }
  }

 
  int x, y;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (!game_over && getCell(field, &x, &y)) {
      int index = (y * field->width) + x;
      
      cur_face = Click;
      change_timer = 0.0f;
      PlaySound(tick_sound);
      if (field->field[index] == Mine) {
        game_over = 1;
        
        field->field[index] = Blast;
        show(field);
        cur_face = Lost;
        PlaySound(lose_sound);
        return;
      }

      if (field->field[index] == Empty)
        countMines(field, x, y);
    }
    else if (facePressed(field)) {
      cur_face = Down;
      PlaySound(tick_sound);
      restart(field);
    }
  }
  else if (!game_over && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && getCell(field, &x, &y)) {
    cur_face = Click;
    change_timer = 0.0f;
    PlaySound(tick_sound);
    setFlag(field, x, y);
  }
}


static void restart(Field *field) {
  memset(field->field, 0, field->size);
  field->mines = mines_all;
  field->flags_left = mines_all;
  fdGenerateMines(field);

  game_over = 0;
  cur_time = 0;
}

static int facePressed(const Field *FIELD) {
  Vector2 pos = GetMousePosition();
  if ((pos.x >= (FIELD->width >> 1) * CELL_SIZE && pos.x <= ((FIELD->width >> 1) * CELL_SIZE) + 50) 
      && (pos.y >= CELL_SIZE + (CELL_SIZE >> 1) && pos.y <= CELL_SIZE + (CELL_SIZE >> 1) + 50))
      return 1;

  return 0;
}

static void setFlag(Field *field, int x, int y) {
  int cur = (y * field->width) + x;
  if (field->field[cur] & Flag) {
    field->field[cur] ^= Flag;
    if (field->field[cur] == Mine)  
      field->mines++;
    field->flags_left++;
  }
  else if (field->field[cur] == Empty || field->field[cur] == Mine) {
    if (field->field[cur] == Mine)
      field->mines--;
    field->field[cur] |= Flag;
    field->flags_left--;
  }
}


static int getCell(const Field *FIELD, int *x, int *y) {
  const Vector2 MOUSE_POS = GetMousePosition();
  if (!(MOUSE_POS.x >= CELL_SIZE && MOUSE_POS.x < CELL_SIZE + (CELL_SIZE * FIELD->width)) ||
      !(MOUSE_POS.y >= CELL_SIZE*HEADER_SIZE && MOUSE_POS.y < CELL_SIZE * HEADER_SIZE + (CELL_SIZE * FIELD->height)))
      return 0;

  *x = (MOUSE_POS.x - CELL_SIZE) / CELL_SIZE;
  *y = (MOUSE_POS.y - CELL_SIZE * HEADER_SIZE) / CELL_SIZE;
  return 1;
}



static void loadResources() {
    lose_sound = loadSound("./assets/sounds/lose.wav");
    win_sound  = loadSound("./assets/sounds/win.wav");
    tick_sound = loadSound("./assets/sounds/tick.wav");



    cell_up    = loadTexture("./assets/textures/cells/cellup.png", CELL_SIZE, CELL_SIZE);
    cell_down  = loadTexture("./assets/textures/cells/celldown.png", CELL_SIZE, CELL_SIZE);
    flag_cell  = loadTexture("./assets/textures/cells/cellflag.png", CELL_SIZE, CELL_SIZE);
    mine_cell  = loadTexture("./assets/textures/cells/cellmine.png", CELL_SIZE, CELL_SIZE);
    blast_cell = loadTexture("./assets/textures/cells/blast.png", CELL_SIZE, CELL_SIZE);
    false_mine = loadTexture("./assets/textures/cells/falsemine.png", CELL_SIZE, CELL_SIZE);


    bottom_l = loadTexture("./assets/textures/borders/bottomleft.png", CELL_SIZE, CELL_SIZE);
    bottom_r = loadTexture("./assets/textures/borders/bottomright.png", CELL_SIZE, CELL_SIZE);
    mid      = loadTexture("./assets/textures/borders/leftright.png", CELL_SIZE, CELL_SIZE); 
    mid_l    = loadTexture("./assets/textures/borders/middleleft.png", CELL_SIZE, CELL_SIZE);
    mid_r    = loadTexture("./assets/textures/borders/middleright.png", CELL_SIZE, CELL_SIZE);
    top_l    = loadTexture("./assets/textures/borders/topleft.png", CELL_SIZE, CELL_SIZE);
    top_r    = loadTexture("./assets/textures/borders/topright.png", CELL_SIZE, CELL_SIZE);
    top_b    = loadTexture("./assets/textures/borders/topbottom.png", CELL_SIZE, CELL_SIZE);

    char cell_path[] = "assets/textures/cells/cell1.png";
    int path_len = strlen(cell_path);
    for (int i = 1; i <= MAX_NEIBERS; i++) {
        cell_path[path_len-5] = (char)i + '0'; 
        cells_neibers[i-1] = loadTexture(cell_path, CELL_SIZE, CELL_SIZE);
    }



    faces[Default] = loadTexture("assets/textures/faces/smileface.png", 50, 50);
    faces[Down] = loadTexture("assets/textures/faces/smilefacedown.png", 50, 50);
    faces[Click] = loadTexture("assets/textures/faces/clickface.png", 50, 50);
    faces[Lost] = loadTexture("assets/textures/faces/lostface.png", 50, 50);
    faces[Win] = loadTexture("assets/textures/faces/winface.png", 50, 50);




    char counters_path[] = "assets/textures/counter/counter0.png";
    path_len = strlen(counters_path);
    for (int i = 0; i < 10; i++) {
      counters_path[path_len-5] = (char)i + '0';
      nums[i] = loadTexture(counters_path, CELL_SIZE, CELL_SIZE << 1);
    }
}

static Sound loadSound(const char *PATH) {
    Wave wave = LoadWave(PATH);
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);

    return sound;
}


static Texture loadTexture(const char *PATH, int width, int height) {
    Image texture_image = LoadImage(PATH);
    ImageResize(&texture_image, width, height);
    Texture2D res = LoadTextureFromImage(texture_image);
    SetTextureFilter(res, TEXTURE_FILTER_BILINEAR);
    UnloadImage(texture_image);

    return res;
}


static void unloadResources() {
    UnloadSound(lose_sound);
    UnloadSound(win_sound);
    UnloadSound(tick_sound);


    UnloadTexture(cell_up);
    UnloadTexture(flag_cell);
    UnloadTexture(mine_cell);
    UnloadTexture(blast_cell);
    UnloadTexture(false_mine);

    for (int i = 0; i <= MAX_NEIBERS; i++) 
        UnloadTexture(cells_neibers[i]);

    for (int i = 0; i < FACES_COUNT; i++)
        UnloadTexture(faces[i]);
}
