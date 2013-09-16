MAKE=make
CC=g++
CFLAGS=-g -O0 -Wall -Werror -std=gnu++0x
LIBS=-lrt -pthread

OBJS= \
	main.o \
	movie_fragment.o \
	stream.o \
	controlled_stream.o \
	client.o \
	stream_client.o \
	stream_input.o \
	stream_input_state.o \
	header_detection_state.o \
	streaming_state.o \
	ebml.o \

.SUFFIXES: .o .cc

run: bt-stream
	./bt-stream

bt-stream: ${OBJS} libuv/out/Debug/libuv.a http-parser/http_parser.o
	${CC} -o $@ ${OBJS} libuv/out/Debug/libuv.a http-parser/http_parser.o ${LIBS}

.cc.o:
	${CC} ${CFLAGS} -c $<

libuv/out/Debug/libuv.a:
	( \
		cd libuv; \
		./gyp_uv -f ${MAKE}; \
		${MAKE} -C out/; \
	)

http-parser/http_parser.o: http-parser/http_parser.c
	${MAKE} -C http-parser/ http_parser.o

clean:
	rm -f bt-stream
	rm -f *.o
	rm -f libuv/out/Debug/libuv.a
	${MAKE} -C libuv/ clean
	${MAKE} -C http-parser/ clean

.PHONY:
	clean

