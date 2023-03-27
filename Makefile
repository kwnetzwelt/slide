CC=g++
OPTIONS=-std=c++17
CFLAGS=-Wall -g -I/usr/include** -pthread
SRCS=main.cpp JPEGImage.cpp Files.cpp
LDFLAGS=-L/usr/lib** -lstdc++fs -lglfw -lGL -ljpeg

slide: $(SRCS)
	$(CC) $(OPTIONS) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

OPTIONSOSX=-std=gnu++17
CFLAGSOSX=-Wno-deprecated-declarations -Wall -g -I/opt/homebrew/include -pthread
LDFLAGSOSX=-L/opt/homebrew/lib -lglfw -framework OpenGL -ljpeg


slide-mac: $(SRCS)
	$(CC) $(OPTIONSOSX) $(CFLAGSOSX) -o $@ $^ $(LDFLAGSOSX) 

clean:
	rm -f slide
