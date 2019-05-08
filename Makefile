OBJS=3ds.o aiobject.o audio.o cockpit.o common.o conf.o dirs.o effects.o fileloader.o FlightModel.o gl.o glland.o land.o loader_tga.o main.o mathtab.o menu.o mission.o model.o NetworkApi.o object.o objload.o pilots.o vertexarray.o

CXX=g++ -Ofast -g0

LIBS=`pkg-config --libs SDL_mixer` -lglut -lGL -lglut -lGLU

Lac07p64: $(OBJS)
	g++ -o Lac07p64 $(LIBS) $(OBJS)
	cp Lac07p64 bin/Release/Lac07p64
	chmod +x bin/Release/Lac07p64

clean:
	@rm -f *.o
	@rm Lac07p64
