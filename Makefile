ifeq ($(WITH_SSL),1)
  override LDFLAGS += -Lssl -Lcrypto
endif

override CFLAGS += -std=c11
override LDFLAGS += asock.a

.PHONY: asock test clean

# ASOCK Library
asock:
	mkdir -p build
	$(CC) $(CFLAGS) -flto -O3 -c src/*.c
	$(AR) rvs build/asock.a *.o
	rm -rf *.o

# Using CHECK
test:
	mkdir -p build/test

clean:
	rm-rf build
