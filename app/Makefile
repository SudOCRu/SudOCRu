BUILD := bin
TARGET := sudocru
LIB_FOLDER := $(BUILD)/shared
LIB := $(LIB_FOLDER)/libipp.a $(LIB_FOLDER)/libsolver.a $(LIB_FOLDER)/libocr.a

CC := gcc
CPPFLAGS = -MMD $(INDIRS)
CFLAGS = -Wall -Wextra -D__NO_INLINE__ `pkg-config --cflags sdl2 SDL2_image SDL2_ttf gtk+-3.0`
LDFLAGS := `pkg-config --libs sdl2 SDL2_image SDL2_ttf gtk+-3.0` -L$(LIB_FOLDER)
LDLIBS :=  $(addprefix -l,\
	m \
	ipp \
	ocr \
	solver \
)

RELEASE := 0
ifneq ($(RELEASE), 0)
	CFLAGS += -O3 -flto
	LDFLAGS += -flto
else
	CFLAGS += -Og -g -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
	CPPFLAGS += -DDEBUG
endif

INDIRS := $(addprefix -I, \
	src \
	../ipp/src \
	../ocr/src \
	../solver/src \
)

MAINS := $(BUILD)/src/main.o
CFILES := $(shell find src -name "*.c" $(TEST_FILES_OPTS))
OFILES := $(CFILES:%.c=$(BUILD)/%.o)
SRCFILES := $(filter-out $(MAINS),$(OFILES))
DFILES := $(OFILES:%.o=%.d)

SRCDIRS := $(shell find . -type d -not -path "*$(BUILD)*")
$(shell mkdir -p $(BUILD)/shared $(SRCDIRS:%=$(BUILD)/%))

all: $(TARGET)

$(LIB):
	make -C ../ipp lib RELEASE=$(RELEASE)
	@cp ../ipp/bin/libipp.a bin/shared/libipp.a
	make -C ../solver lib RELEASE=$(RELEASE)
	@cp ../solver/bin/libsolver.a bin/shared/libsolver.a
	make -C ../ocr lib RELEASE=$(RELEASE)
	@cp ../ocr/bin/libocr.a bin/shared/libocr.a
	@cp ../ocr/bin/ocr_weights.bin bin/ocr_weights.bin

$(TARGET): $(OFILES)
	$(V)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: %.c $(LIB)
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all clean test

-include $(DFILES)
