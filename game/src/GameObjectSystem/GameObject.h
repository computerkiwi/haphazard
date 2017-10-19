/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

class GameSpace;

// GameObject ID Gen
//GameObject_ID GenerateID();

#define implicit

template <typename T>
class ComponentHandle;


typedef int GameObject_ID;
typedef int GameSpaceIndex;
typedef int    dummy;

class GameObject
{
public:
	GameObject(int id, GameSpaceIndex gameSpace);

	implicit GameObject(GameObject_ID id);

	template <typename T, typename... Args>
	void AddComponent(Args&&... args)
	{
		GetSpace()->EmplaceComponent<T>(m_objID, std::forward<Args>(args)...);
	}

	template <typename T>
	ComponentHandle<T> GetComponent() const
	{
		// Make sure the component exists before we hand it off.
		if (GetSpace()->GetInternalComponent<T>(m_objID) != nullptr)
		{
			// Why the hell does constructing a ComponentHandle work?
			// We never forward declare the constructor and we shouldn't know what sizeof(ComponentHandle<T>) is, right? -Kieran

			// TEMPLATES -Sweet
			return ComponentHandle<T>(m_objID);
		}
		else
		{
			return ComponentHandle<T>(0, false);
		}
	}

	GameObject_ID GetObject_id() const;

	GameObject_ID Getid() const;

	GameObject_ID Duplicate() const;

	void Delete();

	GameSpace *GetSpace() const;

	GameSpaceIndex GetIndex() const;

	void SetSpace(GameSpaceIndex index);

	template <typename T>
	void DeleteComponent()
	{
		GetSpace()->DeleteComponent<T>(m_objID);
	}

	operator bool() const { return m_objID; }

private:
	union
	{
		struct
		{
			int m_id     : 24;
			int m_space  : 8;
		};
		GameObject_ID m_objID;
	};
};
