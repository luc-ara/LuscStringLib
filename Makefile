CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -Werror

test: test.c strings.o
	$(CC) -o $@ $^ $(CFLAGS)
	./test

clean:
	rm *.o test

.PHONY: test clean
