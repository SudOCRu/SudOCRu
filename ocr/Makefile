BUILD := bin
TARGET := ocr
LIBNAME := lib$(TARGET).a

CC := gcc
CPPFLAGS = -MMD $(INDIRS)
CFLAGS = -Wall -Wextra -D__NO_INLINE__
LDFLAGS :=
LDLIBS :=  $(addprefix -l,\
	m \
)

RELEASE := 0
ifneq ($(RELEASE), 0)
	CFLAGS += -O2 -flto
	LDFLAGS += -flto
else
	CFLAGS += -Og -g -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
	CPPFLAGS += -DDEBUG
endif

INDIRS := $(addprefix -I, \
	src \
)

MAINS := $(BUILD)/src/main.o

CFILES := $(shell find src -name "*.c")
OFILES := $(CFILES:%.c=$(BUILD)/%.o)
SRCFILES := $(filter-out $(MAINS),$(OFILES))
DFILES := $(OFILES:%.o=%.d) $(TESTOFILES:%.o=%.d)

SRCDIRS := $(shell find . -type d -not -path "*$(BUILD)*")
$(shell mkdir -p $(SRCDIRS:%=$(BUILD)/%))

all: $(TARGET)

lib: $(BUILD)/$(LIBNAME)
	@cp src/network $(BUILD)/ocr_weights.bin

$(BUILD)/$(LIBNAME): $(SRCFILES)
	ar -rc $@ $^

$(TARGET): $(OFILES)
	$(V)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: %.c
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all clean

-include $(DFILES)
