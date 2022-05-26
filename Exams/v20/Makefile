# SETUP INSPIRED BY: http://ianfinlayson.net/posts/06-make?fbclid=IwAR2ZHiJm8FH6WjlQxntdFeusVu6zYT6yGA5rDGG3onN6LVey4nq9oD5r8WY

# options
CC=gcc
CFLAGS=-W -Wall -g -fopenmp
TARGET=main

# globs
SRCS := $(wildcard source/*.c)
HDRS := $(wildcard source/*.h)
OBJS := $(patsubst source/%.c,objects/%.o,$(SRCS))

# link it all together
$(TARGET): $(OBJS) $(HDRS) makefile
	@mkdir -p objects
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

# compile an object based on source and headers
objects/%.o: source/%.c $(HDRS) makefile
	@mkdir -p objects
	$(CC) -c $< -o $@ $(CFLAGS)

# tidy up
clean:
	rm -f $(TARGET) $(OBJS)