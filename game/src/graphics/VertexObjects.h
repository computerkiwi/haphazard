#pragma once
/*
FILE: VertexObjects.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "GL\glew.h"
#include "Shaders.h"

///
// VBO
/// 

/*| Vertex Buffer Object (VBO):           |*\
|*|  Holds data to be passed to shader    |*|
|*|  To be used in a shader, call a VAO's |*|
\*|  BindAttributeToBuffer function       |*/
class BufferObject
{
public:
	BufferObject(GLenum target = GL_ARRAY_BUFFER) : m_target(target) { glGenBuffers(1, &m_ID); }
	~BufferObject() { glDeleteBuffers(1, &m_ID); }

	void Use() { glBindBuffer(m_target, m_ID); }
	
	void SetData(size_t typeSize, int size, const void* data) { 
		Use();
		glBufferData(m_target, typeSize * size, data, GL_STATIC_DRAW); 
	}

private:
	GLenum m_target;
	GLuint m_ID;
};


///
// VAO
///

/*| Vertex Array Object (VAO):                           |*\
|*|  Describes how vertex attributes are stored in VBOs  |*|
|*|  VAOs will likely be static for a class,             |*|
\*|  and each instance would have it's own VBO           |*/
class VertexAttributeBindings
{
public:
	VertexAttributeBindings()	{	glGenVertexArrays(1, &m_ID); }
	~VertexAttributeBindings() { glDeleteVertexArrays(1, &m_ID); }

	// Tell OpenGL to use these bindings
	void Use() { glBindVertexArray(m_ID); }

	/* This VAO will remember the attributes bound to this VBO forever once this is called. */
	void BindAttributesToBuffer(ShaderProgram* shader, BufferObject& buffer, int min = 0, int max = -1)
	{
		Use();
		buffer.Use();
		if (max == -1)
			shader->ApplyAttributes();
		else
			shader->ApplyAttributes(min, max);
	}

private:
	GLuint m_ID;
};
