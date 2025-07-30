#include "minesweeper.h"
#include "field.h"


static Sound lose_sound, win_sound, tick_sound;
static Texture2D cell_up, cell_down, flag_cell,  mine_cell, blast_cell, false_mine; 
static Texture2D cells_neibers[MAX_NEIBERS];

static Texture2D faces[FACES_COUNT];
static SmileType cur_face = Default;


static int game_over = 0;


void startMinesweeper() {
  InitWindow(CELL_SIZE * 24, CELL_SIZE * 24, "Minesweeper");    
  SetTargetFPS(30);

  InitAudioDevice();
  loadResources();
  newGame(24, 24, 50);

  CloseAudioDevice();
  CloseWindow();
}


static void newGame(int width, int height, int mines) {
  Field field = fdInit(width, height, mines);
  if (field.field == NULL) {
    printf("ERROR: Cant allocate field\n");
    exit(1);
  } 

 
  while (!WindowShouldClose()) {
    if (!game_over)
      update(&field);    

    BeginDrawing();
    drawField(&field);
    EndDrawing();
  }

  fdClear(&field);
}


static void drawField(const Field *FIELD) {
  Texture *texture;
  Vector2 pos = {0};

  int index = 0;
  for (int i = 0; i < FIELD->height; i++, pos.x = 0, pos.y += CELL_SIZE) {
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

  int cur = (y * FIELD->width) + x;
  if (mines > 0) {
    FIELD->field[cur] = mines;
    return;
  }

  FIELD->field[cur] = Checked;
  for (int i = 0; i < neibers_count; i++)
    countMines(FIELD, neibersx[i], neibersy[i]);
}


static void update(Field *field) {
  int x, y;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && getCell(field, &x, &y)) {
    int index = (y * field->width) + x;
    if (field->field[index] == Mine) {
      game_over = 1;
      PlaySound(lose_sound);
      return;
    }

    if (field->field[index] == Empty)
      countMines(field, x, y);
  }
  else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && getCell(field, &x, &y)) {
    setFlag(field, x, y);
  }
}


static void setFlag(Field *field, int x, int y) {
  int cur = (y * field->width) + x;
  if (field->field[cur] & Flag) {
    field->field[cur] ^= Flag;
  }
  else if (field->field[cur] == Empty || field->field[cur] == Mine) {
    field->field[cur] |= Flag;
  }
}


static int getCell(const Field *FIELD, int *x, int *y) {
  const Vector2 MOUSE_POS = GetMousePosition();
  if (!(MOUSE_POS.x >= 0 && MOUSE_POS.x < CELL_SIZE * FIELD->width) ||
      !(MOUSE_POS.y >= 0 && MOUSE_POS.y < CELL_SIZE * FIELD->height))
      return 0;

  *x = MOUSE_POS.x / CELL_SIZE;
  *y = MOUSE_POS.y / CELL_SIZE;
  return 1;
}



static void loadResources() {
    lose_sound = loadSound("./assets/sounds/lose.wav");
    win_sound = loadSound("./assets/sounds/win.wav");
    tick_sound = loadSound("./assets/sounds/tick.wav");



    cell_up   =  loadTexture("./assets/textures/cells/cellup.png", CELL_SIZE, CELL_SIZE);
    cell_down =  loadTexture("./assets/textures/cells/celldown.png", CELL_SIZE, CELL_SIZE);
    flag_cell =  loadTexture("./assets/textures/cells/cellflag.png", CELL_SIZE, CELL_SIZE);
    mine_cell =  loadTexture("./assets/textures/cells/cellmine.png", CELL_SIZE, CELL_SIZE);
    blast_cell = loadTexture("./assets/textures/cells/blast.png", CELL_SIZE, CELL_SIZE);
    false_mine = loadTexture("./assets/textures/cells/falsemine.png", CELL_SIZE, CELL_SIZE);



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




    //char counters_path[] = "assets/textures/counter/counter0.png";
    //path_len = strlen(cell_path);
    //for (int i = 1; i <= 10; i++) {
      //cell_path[path_len-5] = (char)i + '0'; 
      //counter[i-1] = loadTexture(cell_path);
    //}
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
