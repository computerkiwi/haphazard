# We may be using CMake to build our project (it makes a few things easier). We'll decide if we want it after some discussions and experimentation.
# To open this project with Visual Studio 2017 right click in this folder and click the "Open in Visual Studio" button.

project(Game)

SET ( CMAKE_CXX_FLAGS "-std=c++0x" )

# Sets Exception handling
SET ( CMAKE_CXX_FLAGS "/EHsc" )

#
# LIBRARIES
#

# GLM (Math) (Header-only)
include_directories("external/glm")

# Lua
include_directories("external/lua")
find_library(LUA_LIB 
			 NAMES lua53
			 PATHS "external/lua/lib")




# GLOB all the files for now, but I've been told that this may cause issues sometimes. Need to ask some people. -K
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(game ${SOURCES})

# Library Link
target_link_libraries(game "${LUA_LIB}")
