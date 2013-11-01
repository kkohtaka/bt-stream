MAKE=make
CC=g++
LINT=./bin/cpplint.py
VPATH=src
CFLAGS=-Ilibuv/include -Ihttp-parser -g -O0 -Wall -Werror --pedantic-error -std=gnu++11 -DDEBUG_CONSUMER
LIBS=-lrt -pthread -lbluetooth -lstdc++

.SUFFIXES: .o .cc .h

%.o: %.h
%.o: %.cc
	$(CC) $(CFLAGS) -o $@ -c $<

srcdir := src

objects := \
	$(srcdir)/main.o \
	$(srcdir)/movie_fragment.o \
	$(srcdir)/stream.o \
	$(srcdir)/controlled_stream.o \
	$(srcdir)/client.o \
	$(srcdir)/stream_client.o \
	$(srcdir)/stream_input.o \
	$(srcdir)/stream_input_state.o \
	$(srcdir)/header_detection_state.o \
	$(srcdir)/streaming_state.o \
	$(srcdir)/ebml.o \
	$(srcdir)/Util.o \
	$(srcdir)/HCI.o \
	$(srcdir)/SDP.o \
	$(srcdir)/RFCOMM.o \

bt-stream: $(objects)
bt-stream: libuv/out/Debug/libuv.a
bt-stream: http-parser/http_parser.o
	$(CC) -o $@ $(objects) libuv/out/Debug/libuv.a http-parser/http_parser.o $(LIBS)

libuv/out/Debug/libuv.a:
	( \
		cd libuv; \
		./gyp_uv -f $(MAKE); \
		$(MAKE) -C out/; \
	)

http-parser/http_parser.o: http-parser/http_parser.c
	$(MAKE) -C http-parser/ http_parser.o

run: bt-stream
	./bt-stream

clean:
	rm -f bt-stream
	rm -f $(srcdir)/*.o
	rm -rf libuv/out/
	$(MAKE) -C libuv/ clean
	$(MAKE) -C http-parser/ clean

lint:
	$(LINT) $(srcdir)/*.h
	$(LINT) $(srcdir)/*.cc

.PHONY:
	$(objects)
	clean
	lint

