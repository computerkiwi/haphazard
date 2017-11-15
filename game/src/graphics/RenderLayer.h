#pragma once
#include "GL\glew.h"
#include "glm\glm.hpp"
#include <vector>

enum FX
{
	DEFAULT,
	EDGE_DETECTION,
	SHARPEN,
	BLUR,
	BLUR_CORNERS,
	BLOOM,
};

class FrameBuffer
{
	friend class Screen; // Give access to framebuffer IDs
public:
	FrameBuffer(int layer, int numColBfrs = 1);
	~FrameBuffer();

	static void InitFrameBuffers();

	void SetDimensions(int width, int height);
	void SetClearColor(float r, float g, float b, float a);

	void Use();
	void Clear();
	GLuint GetColorBuffer(int attachment = 0) { return m_ColorBuffers[attachment]; }
	void BindColorBuffer(int attachment = 0);

	void AddEffect(FX fx);
	void SetEffects(int count, FX fx[]);
	void RenderEffects();

	int GetLayer() const { return m_Layer; }
	
	void SetBlurAmount(float amt);

	static void ResizePrivateFrameBuffers(int w, int h);

private:
	void GenerateColorBuffers();
	void GenerateDepthStencilObject();

	// FX 
	static bool UseFxShader(FX fx, FrameBuffer& source, FrameBuffer& target);
	static void RenderBloom(FrameBuffer& source, FrameBuffer& target);
	static void RenderBlur(GLuint colorBuffer, FrameBuffer& target);
	void ApplyFXSettings(FX fx);

	static FrameBuffer* fb_FX;

	int m_Layer;
	GLuint m_ID;
	GLuint m_DepthStencilBuffer;
	GLuint m_ColorBuffers[2] = { 0,0 }; // Max color buffers = 2
	int m_Width, m_Height;
	int m_NumColBfrs;
	glm::vec4 m_ClearColor = glm::vec4(0,0,0,0);
	std::vector<FX> m_FXList;

	bool m_UsedThisUpdate = false;

	// FX Variables
	float m_BlurAmount = 1;
};

struct LayerComp 
{ 
	bool operator()(const FrameBuffer* lhs, const FrameBuffer* rhs) const
	{
		return lhs->GetLayer() < rhs->GetLayer();
	}
};