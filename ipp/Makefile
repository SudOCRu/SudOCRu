BUILD := bin
TARGET := ipp
TEST_TARGET := $(BUILD)/$(TARGET)_tests
GENERATED_FILES := grayscale.png median.png edges.png filtered.png acc.png \
		   detected.png rotated.png sudoku.png cells/

CC := gcc
CPPFLAGS = -MMD $(INDIRS)
CFLAGS = -Wall -Wextra -D__NO_INLINE__  `pkg-config --cflags sdl2 SDL2_image`
TESTFLAGS = -O2 -DCRIT_TEST `pkg-config --cflags criterion`
TESTLD = `pkg-config --libs criterion`
LDFLAGS := `pkg-config --libs sdl2 SDL2_image`
LDLIBS :=  $(addprefix -l,\
	m \
)

RELEASE := 0
ifneq ($(RELEASE), 0)
	CFLAGS += -O3 -flto
	LDFLAGS += -flto
else
	CFLAGS += -g -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
	CPPFLAGS += -DDEBUG
endif

TEST := 0
ifneq ($(TEST), 0)
	TEST_FILES_OPTS = -not -name "main.c"
else
	TEST_FILES_OPTS = -not -name "test[0-9]*.c"
endif

INDIRS := $(addprefix -I, \
	src \
	src/utils \
)

TESTFILES := $(shell find tests -name "*.c")
TESTOFILES := $(TESTFILES:%.c=$(BUILD)/%.o)
MAINS := $(BUILD)/src/main.o $(BUILD)/src/test[0_9]*.o

CFILES := $(shell find src -name "*.c" $(TEST_FILES_OPTS))
OFILES := $(CFILES:%.c=$(BUILD)/%.o)
SRCFILES := $(filter-out $(MAINS),$(OFILES))
DFILES := $(OFILES:%.o=%.d)

SRCDIRS := $(shell find . -type d -not -path "*$(BUILD)*")
$(shell mkdir -p $(SRCDIRS:%=$(BUILD)/%))

all: $(TARGET)
	@mkdir -p cells

$(TARGET): $(OFILES)
	$(V)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/tests/%.o: tests/%.c
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS) $(TESTFLAGS)

$(BUILD)/%.o: %.c
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

clean:
	rm -rf $(BUILD) $(TARGET) $(GENERATED_FILES)

$(TEST_TARGET): $(SRCFILES) $(TESTOFILES)
	$(V)$(CC) -o $(TEST_TARGET) $^ $(LDFLAGS) $(LDLIBS) $(TESTLD)

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

.PHONY: all clean test

-include $(DFILES)
