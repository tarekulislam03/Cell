CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lreadline

TARGET = chrono
OBJS = chrono.o ui.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

chrono.o: chrono.c ui.h
	$(CC) $(CFLAGS) -c chrono.c

ui.o: ui.c ui.h
	$(CC) $(CFLAGS) -c ui.c

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
