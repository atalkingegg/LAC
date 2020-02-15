cmake_minimum_required(VERSION 3.13)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set (LAC_SRCS
	3ds.cpp
	aiobject.cpp
	audio.cpp
	cockpit.cpp
	common.cpp
	conf.cpp
	dirs.cpp
	effects.cpp
	fileloader.cpp
	FlightModel.cpp
	gl.cpp
	glland.cpp
	land.cpp
	loader_tga.cpp
	main.cpp
	mathtab.cpp
	menu.cpp
	mission.cpp
	model.cpp
	NetworkApi.cpp
	object.cpp
	objload.cpp
	pilots.cpp
	vertexarray.cpp
	)

find_package(OpenGL)
find_package(SDL2)
find_package(SDL2_mixer)
find_package(GLUT)

add_executable(lac ${LAC_SRCS})
target_link_libraries(lac ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES})

