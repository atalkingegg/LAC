
all: main.o aiobject.o

TARGET := Lac07p64

OBJS := 3ds.o aiobject.o audio.o cockpit.o common.o conf.o dirs.o effects.o fileloader.o FlightModel.o gl.o glland.o land.o loader_tga.o main.o mathtab.o menu.o mission.o model.o NetworkApi.o object.o objload.o pilots.o vertexarray.o


UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	CXX        := /usr/bin/g++ -fdiagnostics-color=never
	CXXFLAGS   := -m64 -Wall -g -I/usr/include
	SDL2FLAGS  := -lSDL2_mixer -lSDL2 -lGL -lGLU
endif

ifeq ($(UNAME), Darwin)
	CXX       := /usr/bin/llvm-g++ -fno-color-diagnostics
	CXXFLAGS  := -std=c++17 -m64 -Wall -g -I/opt/local/include -I/opt/X11/include
	SYS_FLAGS := -framework Cocoa -framework OpenGL -L/opt/local/lib -L/opt/X11/lib
	SDL2FLAGS := -lSDL2 -lSDL2main -lSDL2_mixer -lGLU
endif


${TARGET}: ${OBJS}
	${CXX} ${CXXFLAGS} -o $@ ${OBJS} ${SYS_FLAGS} ${SDL2FLAGS}
	cp ${TARGET} bin/Release/${TARGET}
	chmod +x bin/Release/${TARGET}

.phony: clean

clean:
	rm -f ${OBJS}
	rm -f ${TARGET}

% : %.cpp
	${CXX} ${CXXFLAGS} -o $@ $<
