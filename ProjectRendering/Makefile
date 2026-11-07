CC = gcc
CFLAGS = -Wall -std=c99 -I./include
LDFLAGS = -L./lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,'$$ORIGIN/lib'

TARGET = main
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
