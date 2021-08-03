build:
	cd libraries/ && make build && cd ../
	mkdir -p build/
	g++ -std=c++11  -g -I ./libraries/glad/include -I ./libraries/glm/include src/game.cpp ./libraries/build/glad.o -lglfw -ldl -o build/game 