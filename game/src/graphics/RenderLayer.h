/*
FILE: RenderLayer.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "Universal.h"
#include "GL\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include <map>

const char * const FX_Names[] =
{
	"Default",
	"Dropshadow",
	"Edge Detection",
	"Sharpen",
	"Blur",
	"Blur Corners",
	"Bloom"
};

enum FX
{
	DEFAULT,
	DROPSHADOW,
	EDGE_DETECTION,
	SHARPEN,
	BLUR,
	BLUR_CORNERS,
	BLOOM,
};

class FXManager
{
public:
	void AddEffect(int layer, FX fx);
	void SetEffects(int layer, int count, FX fx[]);
	void ClearEffects();

	std::map<int, std::vector<FX>> m_layerFX;

private:

	static rapidjson::Value FXManagerSerialize(const void *object, rapidjson::Document::AllocatorType& allocator)
	{
		const FXManager& fxManager = *reinterpret_cast<const FXManager *>(object);

		rapidjson::Value jsonManager;
		jsonManager.SetObject();

		// Go through each layer.
		for (auto layerPair : fxManager.m_layerFX)
		{
			// Make an array of all the effects in the layer.
			rapidjson::Value jsonEffectArray;
			jsonEffectArray.SetArray();
			for (FX effect : layerPair.second)
			{
				jsonEffectArray.PushBack(static_cast<int>(effect), allocator);
			}

			// Turn the layer value into a string.
			const size_t BUFFER_SIZE = 100;
			char buffer[BUFFER_SIZE];
			rapidjson::Value key;
			_itoa_s(layerPair.first, buffer, BUFFER_SIZE, 10);
			key.SetString(buffer, allocator);

			// Add the array to the layer.
			jsonManager.GetObject().AddMember(key, jsonEffectArray, allocator);
		}

		return jsonManager;
	}

	static void FXManagerDeserializeAssign(void *objectPtr, rapidjson::Value& jsonObject)
	{
		FXManager& fxManager = *reinterpret_cast<FXManager *>(objectPtr);
		fxManager.ClearEffects();

		Assert(jsonObject.IsObject());
		for (auto& layerPair : jsonObject.GetObject())
		{
			int layer = atoi(layerPair.name.GetString());
			rapidjson::Value& fxArray = layerPair.value;

			for (auto& jsonEffect : fxArray.GetArray())
			{
				int effect = jsonEffect.GetInt();
				fxManager.AddEffect(layer, static_cast<FX>(effect));
			}
		}
	}

	META_REGISTER(FXManager)
	{
		META_DefineSerializeFunction(FXManager, &FXManagerSerialize);
		META_DefineDeserializeAssignFunction(FXManager, &FXManagerDeserializeAssign);
	}
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

	void RenderEffects();

	int GetLayer() const { return m_Layer; }

	void SetBlurAmount(float amt);

	static void ResizePrivateFrameBuffers(int w, int h);

	std::vector<FX>& GetFXList() { return m_FXManager->m_layerFX[m_Layer]; }

private:
	static FXManager* m_FXManager;

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
	glm::vec4 m_ClearColor = glm::vec4(0, 0, 0, 0);
	//std::vector<FX> m_FXList = {DROPSHADOW};

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