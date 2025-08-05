#ifndef _MINESWEEPER_H_
#define _MINESWEEPER_H_

#include <stdio.h>
#include <raylib.h>
#include <string.h>
#include "field.h"


#define CELL_SIZE 40 

#define MAX_NEIBERS 8 
#define FACES_COUNT 5


typedef enum {
  Default,
  Down,
  Click,
  Lost,
  Win
} SmileType;



void startMinesweeper();

static void newGame(int width, int height, int mines);
static void drawField(const Field *FIELD);

static void countMines(const Field *FIELD, int x, int y);

static void show(Field *field);
static void update(Field *field);
static void setFlag(Field *field, int x, int y);
static int getCell(const Field *FIELD, int *x, int *y);


static void loadResources();

static Sound loadSound(const char *PATH);
static Texture loadTexture(const char *PATH, int width, int height);
static void unloadResources();

#endif
