/*
FILE: Serialization.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <iostream>

#include "rapidjson/document.h"

std::string JsonToString(rapidjson::Value& value);

std::string JsonToPrettyString(rapidjson::Value& value);

void JsonToFile(rapidjson::Value & value, const char *fileName);

void JsonToPrettyFile(rapidjson::Value & value, const char *fileName);

rapidjson::Document LoadJsonFile(const char * fileName);
