/*
FILE: GameObject.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include <cstdint>
#include <unordered_map>
#include <map>
#include "Components/Component.h"

// size_t for GameObjectID
typedef size_t GameObjectID_t;

// Defining void * so it is actually something when read
typedef void * Component_Index;
typedef void * Component_Maps;

template <typename T>
struct COMPONENT_GEN
{
	constexpr static Component_Index Func()
	{
		return Func;
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
		mSpace.emplace(COMPONENT_GEN<T>::Func, new std::map<GameObjectID_t, T>);
	}


	template <typename T>
	void Add(GameObjectID_t id, T && component)
	{
		reinterpret_cast<std::map<GameObjectID_t, T> *>(mSpace.find(COMPONENT_GEN<T>::Func)->second)->emplace(id, component);
	}


	template <typename T>
	T & Find(GameObjectID_t id)
	{
		return *static_cast<std::map<GameObjectID_t, T>>(mSpace.find(COMPONENT_GEN<T>::Func)).at(id);
	}


	GameObjectID_t AssignID()
	{
		return mCurrentID++;
	}


	// WIP function
	~GameObject_Space()
	{
		for (auto & iter : mSpace)
		{
			delete reinterpret_cast<std::map<GameObjectID_t, decltype(iter.second)> *>(iter.second);
		}
	}


private:
	// Component_Index is static func pointer
	// Component_Maps is the map of the component
	std::unordered_map<Component_Index, Component_Maps> mSpace;

	// The id to assign to the next gameobject
	GameObjectID_t mCurrentID = 0;

};



class GameObject
{
public:

	explicit GameObject(GameObject_Space & space);


	// Gets the ID of the object
	GameObjectID_t id() const;

	GameObject_Space & GetSpace() const;


	template <typename T>
	void SetComponent(T && component)
	{
		mContainingSpace.Add(mID, component);
	}


	// Adds custom components to an object
	template <typename ...Args>
	void SetComponent(Args && ...args)
	{
		using expand = int[];
		expand
		{
			0, (SetComponent(args), 0)...
		};
	}


	// Adds default components to an object.
	template <typename ...Args>
	void SetComponent()
	{
		using expand = int[];
		expand
		{
			0, (SetComponent(Args()), 0)...
		};
	}


	// GetComponent, will throw NoComponent in Debug if no component was found on an object
	template <typename T>
	T & GetComponent()
	{
		#ifdef _DEBUG
			try
			{
				return mContainingSpace.Find<T>(mID);
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

	// The parent of the game object
	GameObject_Space & mContainingSpace;

	// Class to throw if no object was found
	class NoComponent { public: const char * error = "No Component was found"; };

	// ID of the game object
	GameObjectID_t mID = 0;
};









