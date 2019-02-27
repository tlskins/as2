TARGET = prog
LIBS =
CC = g++
CFLAGS = -g -Wall
OBJDIR = obj
INCDIR = include
.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(OBJDIR)/main.o

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

prog: $(OBJECTS)
	$(CC) $(OBJECTS) -g $(LIBS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o
	rm -f $(TARGET)
