CC = gcc
CXX = g++
RM = rm -f


LDLIBS = \
	-L/usr/X11R6/lib$(LIBSELECT) \
	-L../libs/irrlicht-1.8.0/lib/Linux \
	-L../libs/accidentalnoise/lib/Linux \
	-lIrrlicht \
	-lGL -lXxf86vm -lXext -lX11 \
	-lAccidentalNoise

LDFLAGS = $(shell)

SRCS = \
	./src/canton.cpp \
	./src/enemy.cpp \
	./src/engine.cpp \
	./src/glslmaterial.cpp \
	./src/marchingcubes.cpp \
	./src/mob.cpp \
	./src/player.cpp \
	./src/mouse.cpp \
	./src/pewpew.cpp \
	./src/shadercallback.cpp \
	./src/terrainchunk.cpp \
	./src/terrainmesh.cpp \
	./src/scalarterrain.cpp \
	./src/nullstate.cpp

OBJS = $(subst .cpp,.o,$(SRCS))

.PHONY: default all release debug

default all: release

release:	export EXTRA_CPPFLAGS := -O3 -fexpensive-optimizations
debug:		export EXTRA_CPPFLAGS := -DDEBUG -g

CPPFLAGS = \
	$(shell) \
	-Wall \
	-std=c++0x \
	-ffast-math \
	-I../libs/boost_1_50_0 \
	-I../libs/pugixml/src \
	-I../libs/accidentalnoise/include \
	-I../libs/irrlicht-1.8.0/include \
	-I./include -I/usr/X11R6/include \
	$(EXTRA_CPPFLAGS)

release debug: canton

canton: $(OBJS)
	g++ $(LDFLAGS) -o ./bin/Linux/Canton $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependcanton

include .depend