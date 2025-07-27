#include "field.h"
#include <stdio.h>
#include <strings.h>
#include <time.h>



Field fdInit(int width, int height, int mines) {
  int size = width * height;
  if (size <= mines) 
    return (Field){NULL};
  

  Field field = {
    .field = calloc(sizeof(char), size),
    .width = width,
    .height = height,
    .size = size,
    .mines = mines,
    .flags_left = mines
  };


  fdGenerateMines(&field);
  return field;
}


void fdClear(Field *field) {
  free(field->field);
}


static void fdGenerateMines(Field *field) {
  if (field->field == NULL)
    return; 

  srand(time(NULL));
  for (int i = 0; i < field->mines; i++) {
    int index = rand() % field->size;

    if (field->field[index] == Mine) {
      i--;
    }
    else {
      field->field[index] = Mine;
    }
  }
}


