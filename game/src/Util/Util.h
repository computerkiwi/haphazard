/*
FILE: Util.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include <ostream>
#include <glm/glm.hpp>


// GLM output operators
std::ostream& operator <<(std::ostream& os, glm::vec2 lhs);
std::ostream& operator <<(std::ostream& os, glm::vec3 lhs);
std::ostream& operator <<(std::ostream& os, glm::vec4 lhs);








