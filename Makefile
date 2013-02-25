CC = gcc
CXX = g++
RM = rm -f


LDLIBS = \
	-L/usr/X11R6/lib$(LIBSELECT) \
	-L../libs/irrlicht-1.8.0/lib/Linux \
	-L../libs/accidentalnoise/lib/Linux \
	-L../../dev/libs/boost-svn/stage/lib \
	-lboost_system \
	-lboost_thread \
	-lIrrlicht \
	-lGL -lXxf86vm -lXext -lX11 \
	-lAccidentalNoise

LDFLAGS = $(shell)

SRCS = $(wildcard src/*.cpp)

OBJS = $(subst .cpp,.o,$(SRCS)) \

STATIC = \
	../../dev/libs/boost-svn/stage/lib/libboost_system.a \
	../../dev/libs/boost-svn/stage/lib/libboost_thread.a

.PHONY: default all release debug

default all: release

release:	export EXTRA_CPPFLAGS := -O3 -fexpensive-optimizations
debug:		export EXTRA_CPPFLAGS := -DDEBUG -g

CPPFLAGS = \
	$(shell) \
	-Wall \
	-std=c++0x \
	-ffast-math \
	-I../../dev/libs/boost-svn \
	-I../libs/threadpool/boost \
	-I../libs/pugixml/src \
	-I../libs/accidentalnoise/include \
	-I../libs/irrlicht-1.8.0/include \
	-I./include -I/usr/X11R6/include \
	$(EXTRA_CPPFLAGS)

release debug: canton

canton: $(OBJS)
	g++ $(LDFLAGS) -o ./bin/Linux/Canton $(OBJS) $(STATIC) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependcanton

include .depend