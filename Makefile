run: bt-stream
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib ./bt-stream

bt-stream: main.c
	g++ -O2 -Wall -Werror -I/usr/local/include -o bt-stream $< -luv -L/usr/local/lib -std=gnu++0x
