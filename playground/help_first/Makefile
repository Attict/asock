override CFLAGS += -std=c11

.PHONY: main clean

main:
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/main src/main.c

clean:
	rm -rf bin
