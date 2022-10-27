# SudOCRu
SudOCRu is a fully _student-written_ OCR Sudoku Solver. This software
automatically detects Sudoku Boards in an image, extract its cells, identify
the digits in each cell, solves and reconstruct the original sudoku. This
projet is fully written in C using SDL2, SDL2\_image and glade.

**Folders overview**:
- "app": UI app and utilities
- "examples": Folder containing files used to demonstrate the different parts
- "ipp": Image Processing Pipeline, Grid Detection and Cell extracton
- "ocr": OCR Neural Network implementation
- "solver": Sudoku Solver CLI

### Requirements
- SDL version 2
- SDL\_image version 2
- Glade
- A C Compiler (gcc or clang)
- GNU Make

## Building
To generate all the necessary binaries please use the following command at the
root of the repository:
```shell
$ make all
```

The folder "bin/" will be generated in each subfolder and at the root, they
contain all the files that were used to compile the binaries.

To remove all the generated files and the binaraies, please ue the following
command at the root of the repository:
```shell
$ make clean
```

## Usage
The All-In-One CLI is still work in progress. Therefore, for the moment you may
use the different available CLIs.

### Solver
The `solver` program is a CLI sudoku solver. See the appropriate README in the
solver folder for more details.
Usage: `solver <input_grid> [-<options...>] [arguments]`
Options:
- "d" or debug: Show debug information about the loading/saving of the grid
- "i" or interactive: Animate the sudoku solving process in the terminal (you
        must have ANSI escape codes enabled)
- "p" or print: Print the before/after sudoku boards
- "o" or output: Specify the output file where the solved board will be saved
Note that these options can be combined, for example, "-pio" is valid.

Examples:
```shell
$ bin/solver examples/solver/grid_00 -p
```
```shell
$ bin/solver examples/solver/grid_00 -pio solved00
```

### Grid Extraction
The `ipp` program is a sudoku grid extractor from images. See the appropriate
README in the ocr folder for more details.
Usage: `ipp <input>`

During the extraction process and for *debugging* purposes, many files will be
generated upon running the command:
- "grayscale.png": The grayscale version of the input image
- "median.png": The input image filtered using a 3x3 median filter
- "filtered.png": The input image filtered by the image processing pipeline.
- "edges.png": The result of Canny Edge Detection being applied on filtered.png
- "detected.png": The detected rects and lines by Hough Transform
- "sudoku.png": The cropped and correctly rotated sudoku board
- "cell/cell\_i.png": Cell number `i` of the sudoku board padded by a leading 0

Note that these generated files will be **deleted** after running `make clean`.

Example:
```shell
$ bin/ipp examples/processing/image_05.jpg
```

### OCR
The `ocr` program is a CLI OCR. See the appropriate README in the ocr folder
for more details.
Usage: `ocr <command> [parameters...] [options...]`
Available commands:
- `train` to train a predefined neural network on the XOR truth table.
Parameters: `ocr train <iterations> <learning_rate>`. To specify the output
file, use the `-f` option, example: `ocr train -f <output> <iterations <rate>`
- `eval` to evaluate the outputs of a neural network by a given user input.
Parameters: `ocr eval <network_file> <user_input>` where `user_input` is a
**comma-seperated** list of floating-point numbers representing the values of
the input neurons.

Example:
```shell
$ bin/ocr train -f net.bin 1000 10
[...]
$ bin/ocr eval net.bin 1,1
```

### Rotate Utility
The `rotimg` program will rotate your image by a given angle in degrees. The
result will be saved in "rotated.png" or, if defined, the output file
Usage: `rotimg <input_image> <angle> [output_file]`

Example:
```shell
$ bin/rotimg examples/rotate/landscape.jpg 123 landscape.jpg
```

## Authors

- Sofiane Meftah (Head of the project)
- Kevin Jamet
- Mehdi El Alaoui El Aoufoussi
