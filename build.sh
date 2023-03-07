#!/bin/bash
mkdir out
cp *.jpg out/
g++ -I/usr/include** -L/usr/lib** slide/JPEGImage.cpp slide/main.cpp -o out/slide  -lglfw -lGL -ljpeg
