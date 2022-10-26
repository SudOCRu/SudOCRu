tests:
	gcc -g -pthread -Werror -std=c99 -O1 -o tests train.c datapoint.c \
		layer.c main.c neuralnetwork.c -lm `pkg-config --cflags sdl2 \
		SDL2_image` `pkg-config --libs sdl2 SDL2_image`
clean: tests
	rm tests
