/*
FILE: Serialization.inl
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
*/
#pragma once
#include "Serialization.h"
#include <map>

namespace detail
{
	class SerializeIdUpdateBase
	{
	public:
		virtual void operator()(const std::map<GameObject_ID, GameObject_ID>& ids) = 0;
	};

	template <typename Functor>
	class SerializeIdUpdate : public SerializeIdUpdateBase
	{
		Functor m_functor;

	public:
		SerializeIdUpdate(const Functor& functor) : m_functor(functor) {}

		virtual void operator()(const std::map<GameObject_ID, GameObject_ID>& ids)
		{
			m_functor(ids);
		}
	};

	inline std::vector<SerializeIdUpdateBase *>& GetSerializeIdUpdateVector()
	{
		static std::vector<SerializeIdUpdateBase *> vector;
		return vector;
	}
}

template<typename Functor>
inline void RegisterSerializedIdUpdate(const Functor& functor)
{
	using namespace detail;
	// Put the functor in a form we can use.
	// deleted in ApplySerializedIdUpdates in Serialization.cpp
	auto *idUpdate = new SerializeIdUpdate<Functor>(functor);

	detail::GetSerializeIdUpdateVector().push_back(idUpdate);
}
