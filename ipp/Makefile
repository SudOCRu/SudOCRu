BUILD := bin
TARGET := ipp
GENERATED_FILES := grayscale.png median.png edges.png filtered.png acc.png \
		   detected.png rotated.png sudoku.png cells/

CC := gcc
CPPFLAGS = -MMD $(INDIRS)
CFLAGS = -Wall -Wextra -D__NO_INLINE__  `pkg-config --cflags sdl2 SDL2_image`
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

CFILES := $(shell find . -name "*.c" $(TEST_FILES_OPTS))
OFILES := $(CFILES:%.c=$(BUILD)/%.o)
DFILES := $(OFILES:%.o=%.d)

SRCDIRS := $(shell find . -type d -not -path "*$(BUILD)*")
$(shell mkdir -p $(SRCDIRS:%=$(BUILD)/%))

all: $(TARGET)
	mkdir -p cells

$(TARGET): $(OFILES)
	$(V)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	$(call printtask,Linking,$@)

$(BUILD)/%.o: %.c
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)
	$(call printtask,Compiling,$<)

clean:
	rm -rf $(BUILD) $(TARGET) $(GENERATED_FILES)

.PHONY: all clean

-include $(DFILES)
