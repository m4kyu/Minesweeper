minesweeper: src/main.c  minesweeper.o field.o
	gcc src/main.c build/minesweeper.o build/field.o  -o bin/Minesweeper -lraylib -lGL -lm -lpthread -ldl -lrt -lX11


minesweeper.o: src/minesweeper.c
	gcc -c src/minesweeper.c -o build/minesweeper.o 

field.o: src/field.c
	gcc -c src/field.c -o build/field.o




run:
	./bin/Minesweeper

clean:
	rm bin/Minesweeper
	rm build/*.o


