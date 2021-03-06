CC = gcc
LIB = -I include -I cli -I vm -I parser -I object -I compiler
CFLAGS = -g -DDEBUG -lm -Wall -std=c99 -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers ${LIB}
TARGET = spr
DIRS = include cli vm parser object compiler
CFILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c, %.o, $(CFILES))

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)
clean:
	-$(RM) $(TARGET) $(OBJS)
r: clean $(TARGET)