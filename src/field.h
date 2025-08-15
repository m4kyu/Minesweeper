#ifndef _FIELD_H_
#define _FIELD_H_


#include <stdlib.h>
#include <time.h>


typedef enum {
  Empty = 0,    // From 1 to 8 field contain mines count   
  Checked = 9,
  Mine = 16,
  Flag = 32,
  Blast,
  False,
  SMine,
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

void fdGenerateMines(Field *field);



#endif 
