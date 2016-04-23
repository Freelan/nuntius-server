all:
	g++ -std=c++11 main.cpp -o nuntius-server -lsfml-network -lsfml-system

Release:
	g++ -std=c++11 main.cpp -o bin/Release/nuntius-server -lsfml-network -lsfml-system

