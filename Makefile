CC      = gcc
CC_WIN  = x86_64-w64-mingw32-gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic

SRCS      = src/main.c src/crc.c src/binary.c
TEST_SRCS = tests/test_runner.c tests/test_crc.c tests/test_binary.c src/crc.c src/binary.c tests/unity/unity.c

.PHONY: all windows test clean

all: build
	$(CC) $(CFLAGS) -Isrc -o build/create_binary_file $(SRCS)

windows: build
	$(CC_WIN) $(CFLAGS) -Isrc -o build/create_binary_file.exe $(SRCS)

test: build
	$(CC) $(CFLAGS) -Isrc -Itests/unity -o build/test_runner $(TEST_SRCS)
	./build/test_runner

build:
	mkdir -p build

clean:
	rm -rf build output.bin
