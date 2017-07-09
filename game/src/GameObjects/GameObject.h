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
#include <iostream>

// size_t for GameObjectID
typedef size_t GameObjectID_t;

// Defining void * so it is actually something when read
typedef void * Component_Index;




// This is to generate a unique function id for the Componets.
//    Acts as a key
template <typename T>
struct COMPONENT_GEN
{
	constexpr static Component_Index Func()
	{
		return reinterpret_cast<Component_Index>(Func);
	}
};


// Basically unique_ptr, for memory allocation only
template <typename T>
struct Component_Pointer
{
	explicit Component_Pointer(T * data) : m_Data(data)
	{
	}

	~Component_Pointer()
	{
		delete m_Data;
	}

	T * Get()
	{
		return m_Data;
	}

	template <typename U>
	U * As() const
	{
		return reinterpret_cast<U *>(m_Data);
	}

	T * m_Data = nullptr;
};


// The base class for the map
class Component_Maps_Base
{
public:
	virtual ~Component_Maps_Base()
	{
	}
};


// The map of each components
template <typename T>
class Component_Maps : public Component_Maps_Base, public std::map<GameObjectID_t, T>
{
public:
	// The non-virtual destructor from std::map is ok
	virtual ~Component_Maps() final
	{
	}
};



// Component_Index is static func pointer
// Component_Pointer is basically a unique_ptr
// Component_Maps_Base is for polymorphism on the destructor
class GameObject_Space : public std::unordered_map<Component_Index, Component_Pointer<Component_Maps_Base>>
{
public:

	// Adds component type to the space
	template <typename T>
	void Register()
	{
		emplace(COMPONENT_GEN<T>::Func, new Component_Maps<T>);
	}


	// Adds a component to the space, used in GameObjects
	template <typename T>
	void Add(GameObjectID_t id, T & component)
	{
		at(COMPONENT_GEN<T>::Func).As<Component_Maps<T>>()->emplace(id, component);
	}


	// Finds a component in the space, used in GameObjects
	template <typename T>
	T & Find(GameObjectID_t id)
	{
		return at(COMPONENT_GEN<T>::Func).As<Component_Maps<T>>()->at(id);
	}


	// Assigns an ID to the GameObject
	GameObjectID_t AssignID()
	{
		return mCurrentID++;
	}


	// Removes a Component Type
	template <typename T>
	void Remove()
	{
		erase(COMPONENT_GEN<T>::Func);
	}


private:
	// The id to assign to the next gameobject
	GameObjectID_t mCurrentID = 0;

};



class GameObject
{
public:

	explicit GameObject(GameObject_Space & space);
	explicit GameObject(GameObject_Space * space); // For lua


	// Gets the ID of the object
	GameObjectID_t id() const;

	GameObject_Space & GetSpace() const;

	// L-value SetComponent
	template <typename T>
	void SetComponent(T & component)
	{
		mContainingSpace.Add<T>(mID, component);
	}


	// R-value SetComponent
	template <typename T>
	void SetComponent(T && component)
	{
		mContainingSpace.Add<T>(mID, component);
	}


	// Adds custom components to an object
	template <typename First, typename ...Rest>
	void SetComponent(First & first, Rest & ...args)
	{
		mContainingSpace.Add<First>(mID, first);
		SetComponent(std::forward(args)...);
	}


	// Adds custom components to an object
	template <typename First, typename ...Rest>
	void SetComponent(First && first, Rest && ...args)
	{
		mContainingSpace.Add<First>(mID, first);
		SetComponent(std::forward(args)...);
	}


	// Adds default components to an object.
	template <typename ...Args>
	void SetComponent()
	{
		using expand = int[];
		expand
		{
			0, (SetComponent(Args(this)), 0)...
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
				throw NoComponent("No Component was found for this object.");
			}
		#else
			return mContainingSpace.Find<T>(mID);
		#endif
	}


	void print()
	{
		std::cout << *this;
	}

	friend std::ostream & operator<<(std::ostream & os, GameObject & object);

private:

	// The parent of the game object
	GameObject_Space & mContainingSpace;

	// Class to throw if no object was found
	class NoComponent { public: explicit NoComponent(const char * error) { (void)error; } };

	// ID of the game object
	GameObjectID_t mID = 0;
};









