rotate:
	gcc -Wall -Wextra -o Rotate rotate.c `pkg-config --cflags --libs sdl2 SDL2_image` -lm -ldl -g -fsanitize=address
tests:
	gcc -o RotationTest windowrotation.c -Wall `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -export-dynamic
tests_2:
	gcc -Who-format -o part1-bin tests.c -Who-deprecated-declarations -Who-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
