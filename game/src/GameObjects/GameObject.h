/*
FILE: GameObject.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include <cstdint>
#include <unordered_map>
#include <map>
#include "Components/Component.h"

typedef uint64_t GameObjectID_t;

typedef void * Component_Index;
typedef void * Component_Maps;



template <typename T>
struct COMPONENT_GEN
{
	constexpr static Component_Index Func()
	{
		return static_cast<Component_Index>(Func);
	}
};


template <typename T>
struct Component_Map
{
	std::map<GameObjectID_t, T> mMap;
};



class GameObject_Space
{
public:

	template <typename T>
	void RegisterComponentMap()
	{
		mSpace.emplace({ COMPONENT_GEN<T>::Func, new std::map<GameObjectID_t, T>});
	}

	template <typename T>
	void Add(GameObjectID_t id, T && component)
	{
		static_cast<std::map<GameObjectID_t, T>>(mSpace.find(COMPONENT_GEN<T>::Func)).emplace({ id, component });
	}

	template <typename T>
	T & Find(GameObjectID_t id)
	{
		return *static_cast<std::map<GameObjectID_t, T>>(mSpace.find(COMPONENT_GEN<T>::Func)).at(id);
	}

private:
	std::unordered_map<Component_Index, Component_Maps> mSpace;
};



class GameObject
{
public:

	GameObjectID_t id() const;

	void SetSpace(GameObject_Space * space);
	GameObject_Space * GetSpace() const;


	template <typename T>
	void SetComponent(T && component)
	{
		mContainingSpace->Add(mID, component);
	}


	template <typename T>
	T & GetComponent()
	{
		#ifdef _DEBUG
			try
			{
				return mContainingSpace->Find<T>(mID);
			}
			catch(std::out_of_range)
			{
				throw NoComponent();
			}
		#else
			return mContainingSpace->Find<T>(mID);
		#endif
	}


private:
	GameObject_Space * mContainingSpace = nullptr;

	class NoComponent { public: const char * error = "No Component was found"; };

	GameObjectID_t mID = 0;
};









