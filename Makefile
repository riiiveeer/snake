snakegame: main.o game.o snake.o
	g++ -o snakegame main.o game.o snake.o -lcurses
main.o: main.cpp game.h
	g++ -c main.cpp
game.o: game.cpp snake.h
	g++ -c game.cpp
snake.o: snake.cpp
	g++ -c snake.cpp
clean:
	del *.o 
	del snakegame
	del record.dat
