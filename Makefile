##master
CC = /usr/bin/g++

SRCS = $(wildcard *.cpp */*.cpp)
OBJS = $(patsubst %cpp, %o, $(SRCS))
INCLUDE = -I ./ -I ./glfw/include -I ./stb_image

LIB = -L ./glfw/lib64 -L ./glad/lib/  -lglad -lGL -lglfw3 -lpthread -lXrandr -lXi  -lX11  -ldl

TARGET = main

.PHONY:all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIB)

%.o:%.cpp
	$(CC) -c $^ $(INCLUDE)
clean:
	rm -f $(OBJS) $(TARGET)
run:clean all
	./$(TARGET)
