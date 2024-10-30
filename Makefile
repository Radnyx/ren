CC := gcc

CFLAGS := -Wall -O3
INCLUDES := -I./include
LFLAGS := -Llib/x64
LIBS := -lSDL2 -lSDL2_image

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

MAIN := ren

.PHONY: clean all

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN)