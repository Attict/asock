# WITH_SSL=1 enables SSL support
ifneq ($(WITH_SSL),1)
	override CFLAGS += -DLIBUS_NO_SSL
else
	# With problems on macOS, make sure to pass needed LDFLAGS required to find these
	override LDFLAGS += -L/usr/local/opt/openssl@1.1/lib -lssl -lcrypto
	override CFLAGS += -I/usr/local/opt/openssl@1.1/include
endif

# WITH_LIBUV=1 builds with libuv as event-loop
ifeq ($(WITH_LIBUV),1)
	override CFLAGS += -DLIBUS_USE_LIBUV
	override LDFLAGS += -luv
endif

# WITH_GCD=1 builds with libdispatch as event-loop
ifeq ($(WITH_GCD),1)
	override CFLAGS += -DLIBUS_USE_GCD
	override LDFLAGS += -framework CoreFoundation
endif

# WITH_ASAN builds with sanitizers
ifeq ($(WITH_ASAN),1)
	override CFLAGS += -fsanitize=address
	override LDFLAGS += -lasan
endif

# add -g for debugging, and remove optimization flags

override CFLAGS += -std=c11 -Wno-everything -Isrc
override LDFLAGS += build/asock.a

# By default we build the uSockets.a static library
default:
	mkdir -p build
	$(CC) $(CFLAGS) -flto -O3 -c src/core/*.c
	$(AR) rvs build/asock.a *.o
	rm -rf *.o

# Builds all examples
.PHONY: examples
examples: default
	for f in examples/*.c; do $(CC) -g $(CFLAGS) -o build/$$(basename "$$f" ".c") "$$f" $(LDFLAGS); done
	#for f in examples/*.c; do $(CC) -flto -O3 $(CFLAGS) -o build/$$(basename "$$f" ".c") "$$f" $(LDFLAGS); done

swift_examples:
	swiftc -O -I . examples/swift_http_server/main.swift uSockets.a -o swift_http_server

all: clean default examples

clean:
	rm -rf build
