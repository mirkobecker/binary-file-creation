CC      = gcc
CC_WIN  = x86_64-w64-mingw32-gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -Werror
TFLAGS  = $(CFLAGS) -fsanitize=address,undefined

SRCS      = src/main.c src/crc.c src/binary.c
TEST_SRCS = tests/test_runner.c tests/test_crc.c tests/test_binary.c tests/test_serial_number.c tests/test_version.c tests/test_build_binary.c src/crc.c src/binary.c tests/unity/unity.c

.PHONY: all windows test integration-test clean

all: build
	$(CC) $(CFLAGS) -Isrc -o build/create_binary_file $(SRCS)

windows: build
	$(CC_WIN) $(CFLAGS) -Isrc -o build/create_binary_file.exe $(SRCS)

test: build
	$(CC) $(TFLAGS) -Isrc -Itests/unity -o build/test_runner $(TEST_SRCS)
	./build/test_runner

integration-test: all
	bash tests/test_integration.sh

build:
	mkdir -p build

clean:
	rm -rf build output.bin
