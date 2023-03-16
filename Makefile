CC=g++
CFLAGS=-Wall -g -I/usr/include**
SRCS=main.cpp JPEGImage.cpp Files.cpp
LDFLAGS=-L/usr/lib** -lglfw -lGL -ljpeg

slide: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

clean:
	rm -f myprogram
