/*
FILE: Type.cpp
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "meta.h"
#include <assert.h>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "GameObjectSystem/GameObject.h"

#include "SerializationKeys.h"

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
		Assert(IsPointerType());

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
		Assert(m_members.find(name) == m_members.end());

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

		Any metaObject(&object, this->GetPointerType());

		// Make the json object.
		rapidjson::Value jsonMetaObject;
		jsonMetaObject.SetObject();

		// Store the type of the object we're serializing.
		jsonMetaObject.AddMember(rapidjson::StringRef(META_TYPE_NAME[0]), rapidjson::Value().SetString(this->GetName().c_str(), allocator), allocator);

		// Serialize the actual data of the object.
		rapidjson::Value jsonData;
		// Use a custom serialization function if we have one.
		if (m_serializeFunction != nullptr)
		{
			jsonData = m_serializeFunction(object, allocator);
		}
		// If we don't have a custom serialization function, store all the members.
		else
		{
			jsonData.SetObject();

			std::vector<Member *> members = GetMembers(); 
			
			// Serialize each member.
			for (const auto& member : members)
			{
				Any memberObject = metaObject.GetPointerMember(member);
				std::string memberName = member->GetName();

				jsonData.AddMember(rapidjson::Value().SetString(memberName.c_str(), allocator), memberObject.Serialize(allocator), allocator);
			}
		}

		// Put our object data into the meta object.
		jsonMetaObject.AddMember(rapidjson::StringRef(META_OBJECT_DATA[0]), jsonData, allocator);

		return jsonMetaObject;
	}

	void Type::DeserializeConstruct(void *objectBuffer, rapidjson::Value& jsonObject)
	{
		defaultConstructor(objectBuffer);
		DeserializeAssign(objectBuffer, jsonObject);
	}

	void Type::DeserializeAssign(void *object, rapidjson::Value& jsonObject)
	{
		Assert(GetName() == GetMemberBackwardsCompatible(jsonObject, META_TYPE_NAME).GetString());

		// Get the members from the object.
		rapidjson::Value jsonData;
		jsonData = GetMemberBackwardsCompatible(jsonObject, META_OBJECT_DATA);

		// Do custom deserialization if we have it.
		if (m_deserializeAssignFunction != nullptr)
		{
			return m_deserializeAssignFunction(object, jsonData);
		}

		Assert(jsonData.IsObject());
		// Go through the members and deserialize each of them.
		auto members = GetMembers();
		for (const auto& member : members)
		{
			std::string memberName = member->GetName();

			if (jsonData.HasMember(memberName.c_str()))
			{

				Any deserializedMember(jsonData[memberName.c_str()]);
				Assert(deserializedMember.GetType() == member->GetType());
				member->Set(object, deserializedMember.GetDataPointer());
			}
			else
			{
				logger.SetNextChannel(Logging::Channel::META);
				logger << "Couldn't find member " << memberName << " when deserializing a(n) " << this->m_name << '\n';
			}
		}
	}

	void Type::SetGameObjectID(void * object, GameObject_ID id)
	{
		if (m_setGameObjectIDFunction != nullptr)
		{
			m_setGameObjectIDFunction(object, id);
		}
	}
}

