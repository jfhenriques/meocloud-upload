CC      = g++
INCLUDE = -I../include
CXXFLAGS= $(INCLUDE) -std=c++0x -O3 -W -Wall -Wextra -march=native
LDFLAGS = -lcurl

TARGET=meocloud
SOURCES=Http.cpp Meocloud_API.cpp Utils.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -s -o $@ $(OBJECTS) $(LDFLAGS)

$(SRCS:.c=.d):%.c
	$(CC) $(CXXFLAGS)  $< >$@
	
install:
	cp $(TARGET) -f $(DESTDIR)/usr/local/bin/$(TARGTET)

.PHONY: clean

clean:
	rm -f *.o
