GENERATED_FILES := grayscale.png median.png edges.png filtered.png acc.png \
		   detected.png sudoku.png cells/ *.result

all: bin/ bin/ocr bin/ipp bin/solver

bin/:
	mkdir bin

bin/ocr:
	make -C ocr/ RELEASE=1
	cp ocr/ocr bin/

bin/ipp:
	make -C ipp/ RELEASE=1
	cp ipp/ipp bin/

bin/solver:
	make -C solver/ RELEASE=1
	cp solver/solver bin/

clean:
	rm -rf bin $(GENERATED_FILES)
	make -C ocr/ clean
	make -C ipp/ clean
	make -C solver/ clean

.PHONY: clean
