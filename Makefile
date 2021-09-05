PKGS=sdl2
CXXFLAGS=-Wall -Wextra -std=c++11 `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)`
SRCS=$(wildcard *.cpp)
TARGET=snake

all:
	g++ $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)