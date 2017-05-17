
# We may be using CMake to build our project (it makes a few things easier). We'll decide if we want it after some discussions and experimentation.
# To open this project with Visual Studio 2017 right click in this folder and click the "Open in Visual Studio" button.

project(Game)

SET ( CMAKE_CXX_FLAGS "-std=c++0x" )

# GLOB all the files for now, but I've been told that this may cause issues sometimes. Need to ask some people. -K
file(GLOB_RECURSE SOURCES "./src" "*.cpp")

add_executable(game ${SOURCES})
