TARGET ?= server.out
TARGET2 ?= client.out
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name server*.c -or -name *.s)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

SRCS2 := $(shell find $(SRC_DIRS) -name *.cpp -or -name client*.c -or -name *.s)
OBJS2 := $(addsuffix .o,$(basename $(SRCS2)))
DEPS2 := $(OBJS2:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

OFLAGS := -pthread -lpthread -Wall

OTHERLIBS := -lrt

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS) $(OTHERLIBS)

$(TARGET2): $(OBJS2)
	$(CC) $(LDFLAGS) $(OFLAGS) $(OBJS2) -o $@ $(LOADLIBES) $(LDLIBS) $(OTHERLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS) $(TARGET2) $(OBJS2) $(DEPS2)
all: $(TARGET) $(TARGET2)

-include $(DEPS) $(DEPS2)
