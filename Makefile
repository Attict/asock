ifeq ($(WITH_SSL),1)
  override LDFLAGS += -Lssl -Lcrypto
endif

override CFLAGS += -std=c11 -Wall -Isrc
override LDFLAGS += build/asock.a

.PHONY: asock examples test clean

# ASOCK Library
asock:
	mkdir -p build
	$(CC) $(CFLAGS) -c src/*.c
	$(AR) rvs build/asock.a *.o
	rm -rf *.o

examples:
	$(CC) $(CFLAGS) -o build/example_echo examples/example_echo.c $(LDFLAGS)

# Using CHECK
test:
	mkdir -p build/test

clean:
	rm-rf build
