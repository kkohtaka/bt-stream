CC=g++
CFLAGS=-O2 -Wall -Werror -I/usr/local/include
LIBS=-lrt -pthread -std=gnu++0x

run: bt-stream
	./bt-stream

bt-stream: main.o libuv/out/Debug/libuv.a http-parser/http_parser.o
	${CC} -o $@ main.o libuv/out/Debug/libuv.a http-parser/http_parser.o ${LIBS}

main.o: main.cc
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

