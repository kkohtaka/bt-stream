CC=g++
CFLAGS=-O2 -Wall -Werror -std=gnu++0x
LIBS=-lrt -pthread

OBJS= \
	main.o \
	movie_fragment.o \
	stream.o \
	controlled_stream.o \
	client.o \
	stream_client.o \

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
		rm -rf build/; \
		git clone https://git.chromium.org/external/gyp.git build/gyp; \
		./gyp_uv -f make; \
		make -C out/; \
	)

http-parser/http_parser.o:
	make -C http-parser/ http_parser.o

clean:
	rm bt-stream
	rm main.o
	rm libuv/out/Debug/libuv.a
	make -C libuv/ clean
	make -C http-parser/ clean

.PHONY:
	clean

