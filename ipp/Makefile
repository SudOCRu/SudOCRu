BUILD := bin
TARGET := ipp

CC := gcc
CPPFLAGS = -MMD $(INDIRS)
CFLAGS = `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS := `pkg-config --libs sdl2 SDL2_image`
LDLIBS :=  $(addprefix -l,\
	m \
)

RELEASE := 0
ifneq ($(RELEASE), 0)
	CFLAGS += -O3 -flto
	LDFLAGS += -flto
else
	CFLAGS += -fsanitize=address,undefined
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

$(TARGET): $(OFILES)
	$(V)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	$(call printtask,Linking,$@)

$(BUILD)/%.o: %.c
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)
	$(call printtask,Compiling,$<)

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all clean

-include $(DFILES)
