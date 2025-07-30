#include "minesweeper.h"
#include "field.h"


static Sound lose_sound, win_sound, tick_sound;
static Texture2D cell_up, cell_down, flag_cell,  mine_cell, blast_cell, false_mine; 
static Texture2D cells_neibers[MAX_NEIBERS];

static Texture2D faces[FACES_COUNT];
static SmileType cur_face = Default;




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

 
  field.field[0] = Empty;
  countMines(&field, 0, 0);
  while (!WindowShouldClose()) {
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
          texture = &cell_up;
          break;
        case Checked:
          texture = &cell_down;
          break;
        case Mine:
          texture = &mine_cell;
          break;
        case Flag:
        case MFlag:
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


static void countMines(Field *field, int x, int y) {
  int mines = 0, neibers_count = 0;
  int neibersx[MAX_NEIBERS];
  int neibersy[MAX_NEIBERS];


  for (int i = -1; i < 2; i++) {
    int new_y = y + i;
    if (new_y < 0 || new_y >= field->height)
      continue;

    for (int j = -1; j < 2; j++) {
      int new_x = x + j;
      if (new_x < 0 || new_x >= field->width)
        continue;

      int index = (new_y * field->width) + new_x;
      if (field->field[index] == Mine || field->field[index] == MFlag) {
        mines++;
      }
      else if (mines == 0 && field->field[index] == Empty) {
        neibersx[neibers_count] = new_x;
        neibersy[neibers_count++] = new_y;
      }
    }
  }

  int cur = (y * field->width) + x;
  if (mines > 0) {
    field->field[cur] = mines;
    return;
  }

  field->field[cur] = Checked;
  for (int i = 0; i < neibers_count; i++)
    countMines(field, neibersx[i], neibersy[i]);
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
