/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

class GameSpace;

typedef size_t GameObject_ID;

class GameObject
{
public:
	GameObject(GameObject_ID id, GameSpace *gameSpace) : m_objID(id), m_gameSpace(gameSpace)
	{
	}

	template <typename T, typename... Args>
	void addComponent(Args&&... args)
	{
		m_gameSpace->emplaceComponent<T>(m_objID, std::forward<Args>(args)...);
	}

private:
	GameObject_ID m_objID;
	GameSpace *m_gameSpace;
};
