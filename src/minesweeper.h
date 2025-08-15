#ifndef _MINESWEEPER_H_
#define _MINESWEEPER_H_

#include <stdio.h>
#include <raylib.h>
#include <string.h>
#include "field.h"


#define CELL_SIZE 30
#define HEADER_SIZE 5

#define MAX_NEIBERS 8 
#define FACES_COUNT 5

#define FACE_CHANGE_TIME 0.3f

typedef enum {
  Default,
  Down,
  Click,
  Lost,
  Win
} FaceType;




void startMinesweeper();

static void newGame(int width, int height, int mines);


static void drawHeader(const Field *FIELD);
static void drawBorders(const Field *FIELD);
static void drawField(const Field *FIELD);

static void drawHorBorder(const Texture2D *left, const Texture2D *mid, const Texture2D *right, const Field *field, Vector2 *pos);
static void drawVerBorder(const Texture2D *mid, int height, Vector2 *pos);

static void drawNums(const Field *FIELD, Vector2 pos, unsigned int num);


static void countMines(const Field *FIELD, int x, int y);

static void show(Field *field);
static void update(Field *field);
static void updateFace();
static void setFlag(Field *field, int x, int y);
static int getCell(const Field *FIELD, int *x, int *y);


static void loadResources();

static Sound loadSound(const char *PATH);
static Texture loadTexture(const char *PATH, int width, int height);
static void unloadResources();

#endif
