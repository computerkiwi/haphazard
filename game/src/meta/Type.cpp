/*
FILE: Type.cpp
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "meta.h"
#include <assert.h>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

namespace meta
{

	Type::~Type()
	{
		delete m_pointerType;

		for (auto iter = m_members.begin(); iter != m_members.end(); ++iter)
		{
			delete iter->second;
		}
	}

	//---------
	// Getters
	//---------
	std::string Type::GetName()
	{
		return m_name;
	}

	size_t Type::GetSize()
	{
		return m_size;
	}

	Type *Type::GetPointerType()
	{
		// Construct the pointer type if we have to.
		if (m_pointerType == nullptr)
		{
			std::string pointerName = "ptr to ";
			pointerName += m_name;

			m_pointerType = new Type(sizeof(void *), pointerName.c_str(), internal::DefaultConstructor<void *>, internal::CopyConstructor<void *>, internal::MoveConstructor<void *>, internal::Assignment<void *>, internal::MoveAssignment<void *>, internal::Destructor<void *>, nullptr, this);
		}

		return m_pointerType;
	}

	Type *Type::GetDereferenceType()
	{
		assert(IsPointerType());

		return m_dereferenceType;
	}

	Type *Type::GetDeepestDereference()
	{
		Type *type = this;

		while (type->m_dereferenceType != nullptr)
		{
			type = type->m_dereferenceType;
		}

		return type;
	}

	bool Type::IsPointerType()
	{
		return (m_dereferenceType != nullptr);
	}

	void Type::RegisterMember(const char *name, Type *type, size_t offset)
	{
		assert(m_members.find(name) == m_members.end());

		m_members.emplace(name, new MemberOffset(name, type, offset));
	}

	std::vector<Member *> Type::GetMembers()
	{
		std::vector<Member *> membersVector;

		for (auto iter = m_members.begin(); iter != m_members.end(); ++iter)
		{
			membersVector.push_back(iter->second);
		}

		// Add the members of our pointer type.
		if (m_dereferenceType != nullptr)
		{
			std::vector<Member *> dereferenceMembers = m_dereferenceType->GetMembers();

			for (auto member : dereferenceMembers)
			{
				membersVector.push_back(member);
			}
		}

		return membersVector;
	}

	// Returns nullptr if the member name doesn't exist.
	Member *Type::GetMember(const char *name)
	{
		auto memberIter = m_members.find(name);

		if (memberIter == m_members.end())
		{
			return nullptr;
		}
		else
		{
			return memberIter->second;
		}
	}

	//---------------
	// Serialization
	//---------------

	rapidjson::Value Type::Serialize(const void *object, rapidjson::Document::AllocatorType& allocator)
	{
		if (m_serializeFunction != nullptr)
		{
			return m_serializeFunction(object, allocator);
		}

		Any metaObject(&object, this->GetPointerType());

		rapidjson::Value jsonObject;
		jsonObject.SetObject();

		std::vector<Member *> members = GetMembers();

		jsonObject.AddMember(rapidjson::StringRef("meta_type_name"), rapidjson::Value().SetString(this->GetName().c_str(), allocator), allocator);

		rapidjson::Value jsonMembers;
		jsonMembers.SetObject();

		for (const auto& member : members)
		{
			Any memberObject = metaObject.GetMember(member);
			std::string memberName = member->GetName();

			jsonMembers.AddMember(rapidjson::Value().SetString(memberName.c_str(), allocator), memberObject.Serialize(allocator), allocator);
		}

		jsonObject.AddMember(rapidjson::StringRef("meta_members"), jsonMembers, allocator);

		return jsonObject;
	}

	void Type::DeserializeConstruct(void *objectBuffer, rapidjson::Value& jsonObject)
	{
		defaultConstructor(objectBuffer);
		DeserializeAssign(objectBuffer, jsonObject);
	}

	void Type::DeserializeAssign(void *object, rapidjson::Value& jsonObject)
	{
		// Make sure we have the right type.
		assert(GetName() == jsonObject["meta_type_name"].GetString());

		// Get the members from the object.
		rapidjson::Value jsonMembers;
		jsonMembers = jsonObject["meta_members"];

		assert(jsonMembers.IsObject());

		// Go through the members and deserialize each of them.
		auto members = GetMembers();
		for (const auto& member : members)
		{
			Any deserializedMember(jsonMembers[member->GetName().c_str()]);

			member->Set(object, deserializedMember.GetDataPointer());
		}
	}
}

