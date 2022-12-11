GENERATED_FILES := grayscale.png median.png edges.png filtered.png acc.png \
		   detected.png sudoku.png cells/ *.result rotated.png

all: bin/ bin/ocr bin/ipp bin/solver bin/sudocru

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

bin/sudocru: bin/ocr bin/ipp bin/solver
	make -C app/ RELEASE=1
	cp app/sudocru bin/
	mkdir bin/assets/
	cp -R app/assets bin/
	cp app/bin/ocr_weights.bin bin/

clean:
	rm -rf bin $(GENERATED_FILES)
	make -C app/ clean
	make -C ocr/ clean
	make -C ipp/ clean
	make -C solver/ clean

.PHONY: clean
