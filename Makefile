CC=g++
OPTIONS=-std=c++17
CFLAGS=-Wall -g -I/usr/include**
SRCS=main.cpp JPEGImage.cpp Files.cpp
LDFLAGS=-L/usr/lib** -lstdc++fs -lglfw -lGL -ljpeg

slide: $(SRCS)
	$(CC) $(OPTIONS) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

clean:
	rm -f myprogram
