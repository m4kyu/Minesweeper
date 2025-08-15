#include <stdio.h>
#include "minesweeper.h"


int main(int argc, char *argv[]) {
  if (argc != 4 && argc != 1) {
    printf("USAGE:\n\t%s <width> <height> <minescount>\n", argv[0]);
    return 1; 
  }
  

  if (argc == 1) {
    startMinesweeper(24, 24, 60);
    return 0;
  }


  int width = atoi(argv[1]);
  if (width == 0) {
    printf("Invalid param %s\n", argv[1]);
    return 2;
  }

  int height = atoi(argv[2]);
  if (height == 0) {
    printf("Invalid param %s\n", argv[2]);
    return 3;
  }

  int mines = atoi(argv[3]);
  if (mines == 0) {
    printf("Invalid param %s\n", argv[3]);
    return 4;
  }

  startMinesweeper(width, height, mines);

  return 0;
}
