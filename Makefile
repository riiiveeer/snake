snakegame: main.o game.o snake.o map.o
	g++ -o snakegame main.o game.o snake.o map.o -lcurses
main.o: main.cpp game.h
	g++ -c main.cpp
game.o: game.cpp snake.h map.h
	g++ -c game.cpp
snake.o: snake.cpp map.h
	g++ -c snake.cpp
map.o: map.cpp
	g++ -c map.cpp
clean:
	rm *.o 
	rm snakegame
	rm record.dat
