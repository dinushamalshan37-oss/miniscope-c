CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
TARGET = miniscope

.PHONY: all test clean
all: $(TARGET)

$(TARGET): src/miniscope.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(TARGET)
	bash tests/smoke.sh
 
clean:
	rm -f $(TARGET)
