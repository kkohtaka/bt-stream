MAKE=make
CC=g++
CFLAGS=-I.. -g -O0 -Wall -Werror -std=gnu++0x
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
	Util.o \
	HCI.o \
	SDP.o \
	RFCOMM.o \

.SUFFIXES: .o .cc

all: ${OBJS}

.cc.o:
	${CC} ${CFLAGS} -c $<

clean:
	rm -f *.o

lint:
	../bin/cpplint.py *.h
	../bin/cpplint.py *.cc

.PHONY:
	clean
	lint

