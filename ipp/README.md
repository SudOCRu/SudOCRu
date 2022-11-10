# Standalone Sudoku Grid Detector
Image Processing Pipeline is a program that filters the input image, extracts
lines from it, detects and extracts the predominant (if present) sudoku board.

Usage: `ipp <input_file> [-<options...>]`

Example:
```shell
$ ./ipp ../examples/processing/image_05.png
```

**Options**:
- "h": Prints help
#### Hough Lines
- "A": Saves the Hough Transform accumulator as acc.png
- "L": Renders all lines in detected.png
- "l": Renders filtered lines in detected.png
#### Rectangle processing
- "R": Renders all rectangles in detected.png
- "r": Renders filtered rectangles in detected.png
#### Image processing
- "g": Saves the result of the grayscale operation
- "G": Saves the result of the grayscale+median operation
- "S": Saves the result of the image processing pipeline
- "C": Saves the result of the edge detector
#### Other
- "p": Print the selection results"
- "T": Saves the result of the corrected (rotated back) input image

## Authors
Sofiane Meftah
