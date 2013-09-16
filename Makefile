MAKE=make
CC=g++
CFLAGS=-g -O0 -Wall -Werror -std=gnu++0x
LIBS=-lrt -pthread

srcdir := src

OBJS= \
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

run: bt-stream
	./bt-stream

bt-stream: ${OBJS} libuv/out/Debug/libuv.a http-parser/http_parser.o
	${CC} -o $@ ${OBJS} libuv/out/Debug/libuv.a http-parser/http_parser.o ${LIBS}

$(srcdir)/%.o: $(srcdir)/%.cc $(srcdir)/%.c
	${CC} ${CFLAGS} -c $<

libuv/out/Debug/libuv.a:
	( \
		cd libuv; \
		./gyp_uv -f ${MAKE}; \
		${MAKE} -C out/; \
	)

http-parser/http_parser.o: http-parser/http_parser.c
	${MAKE} -C http-parser/ http_parser.o

${OBJS}:
	${MAKE} -C $(srcdir)

clean:
	rm -f bt-stream
	rm -f *.o
	${MAKE} -C libuv/ clean
	${MAKE} -C http-parser/ clean
	${MAKE} -C $(srcdir) clean

.PHONY:
	clean

