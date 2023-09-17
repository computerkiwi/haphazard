/*
FILE: Serialization.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <iostream>

#include "rapidjson/document.h"
#include "GameObjectSystem/GameObject.h"


std::string JsonToString(rapidjson::Value& value);

std::string JsonToPrettyString(rapidjson::Value& value);

void JsonToFile(rapidjson::Value & value, const char *fileName);

void JsonToPrettyFile(rapidjson::Value & value, const char *fileName);

rapidjson::Document LoadJsonFile(const char * fileName);

template <typename Functor>
void RegisterSerializedIdUpdate(const Functor& functor); 

void RegisterSerializedIdRelationship(GameObject_ID oldID, GameObject_ID newID);

// This should be called if the file we're loading from is changed.
void ClearSerializedIdRelationships();

void ApplySerializedIdUpdates();

#include "Serialization.inl"
