BUILD := bin
TARGET := solver
LIBNAME := lib$(TARGET).a
TEST_TARGET := $(BUILD)/$(TARGET)_tests

CC := gcc
CPPFLAGS = -MMD $(INDIRS)
CFLAGS = -Wall -Wextra -D__NO_INLINE__
TESTFLAGS = -O2 -DCRIT_TEST `pkg-config --cflags criterion`
TESTLD = `pkg-config --libs criterion`
LDFLAGS :=
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

INDIRS := $(addprefix -I, \
	src \
)

TESTFILES := $(shell find tests -name "*.c")
TESTOFILES := $(TESTFILES:%.c=$(BUILD)/%.o)
MAINS := $(BUILD)/src/main.o $(BUILD)/src/tests_solver.o

CFILES := $(shell find src -name "*.c")
OFILES := $(CFILES:%.c=$(BUILD)/%.o)
SRCFILES := $(filter-out $(MAINS),$(OFILES))
DFILES := $(OFILES:%.o=%.d) $(TESTOFILES:%.o=%.d)

SRCDIRS := $(shell find . -type d -not -path "*$(BUILD)*")
$(shell mkdir -p $(SRCDIRS:%=$(BUILD)/%))

all: $(TARGET)

lib: $(BUILD)/$(LIBNAME)

$(BUILD)/$(LIBNAME): $(SRCFILES)
	ar -rc $@ $^

$(TARGET): $(OFILES)
	$(V)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/tests/%.o: tests/%.c
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS) $(TESTFLAGS)

$(BUILD)/%.o: %.c
	$(V)$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

clean:
	rm -rf $(BUILD) $(TARGET)

$(TEST_TARGET): $(SRCFILES) $(TESTOFILES)
	$(V)$(CC) -o $(TEST_TARGET) $^ $(LDFLAGS) $(LDLIBS) $(TESTLD)

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

.PHONY: all clean test lib

-include $(DFILES)
