TARGET = prog
LIBS =
CC = g++
CFLAGS = -g -Wall
OBJDIR = obj
.PHONY: default all clean

default: $(TARGET)

OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/buffer_queue.o

$(OBJDIR)/%.o: %.cc
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

#.PRECIOUS: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -g $(LIBS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o
	rm -f $(TARGET)
