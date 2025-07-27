#ifndef _FIELD_H_
#define _FIELD_H_


#include <stdlib.h>
#include <time.h>


typedef enum {
  Empty, 
  Mine,
  Flag, 
  Mflag,
  Blast,
  False
} CellType;


typedef struct {
  char *field;
  
  int width;
  int height;
  int size;

  int mines;
  int flags_left;
} Field;


Field fdInit(int width, int height, int mines); 
void fdClear(Field *field);

static void fdGenerateMines(Field *field);



#endif 
