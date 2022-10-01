BUILD := bin
TARGET := ipp

CC := gcc
CPPFLAGS = -MMD $(INDIRS)
CFLAGS = `pkg-config --cflags sdl2`
LDFLAGS := `pkg-config --libs sdl2`
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

INDIRS := $(addprefix -I, \
	src \
	src/utils \
)

CFILES := $(shell find . -name "*.c")
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
