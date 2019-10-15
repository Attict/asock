# WITH_SSL=1 enables SSL support
ifneq ($(WITH_SSL),1)
	override CFLAGS += -DASOCK_NO_SSL
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
override LDFLAGS += build/asock.a build/ahttp.a
.PHONY: asock ahttp examples clean
clean: rm -rf build

# ASOCK ------------------------------------------------------------------------
asock:
	mkdir -p build
	$(CC) $(CFLAGS) -flto -O3 -c src/core/*.c
	$(AR) rvs build/asock.a *.o
	rm -rf *.o

# AHTTP ------------------------------------------------------------------------
ahttp:
	mkdir -p build
	$(CC) $(CFLAGS) -flto -O3 -c src/http/*.c
	$(AR) rvs build/ahttp.a *.o
	rm -rf *.o

# ASQL -------------------------------------------------------------------------
asql:
	mkdir -p build
	$(CC) $(CFLAGS) -flto -O3 -c src/sql/*.c
	$(AR) rvs build/asql.a *.o
	rm -rf *.o

# EXAMPLES ---------------------------------------------------------------------
examples: asock ahttp
	for f in examples/*.c; do $(CC) -flto -O3 $(CFLAGS) \
	  -o build/$$(basename "$$f" ".c") "$$f" $(LDFLAGS); done

