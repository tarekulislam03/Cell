CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lreadline

TARGET = cell
OBJS = cell.o ui.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

cell.o: cell.c ui.h
	$(CC) $(CFLAGS) -c cell.c

ui.o: ui.c ui.h
	$(CC) $(CFLAGS) -c ui.c

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
