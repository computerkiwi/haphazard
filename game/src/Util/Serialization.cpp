/*
FILE: Serialization.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Universal.h"

#include <fstream>

#include "Serialization.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

namespace
{
	template <typename WriterType, typename BufferType>
	std::string WriteJsonString(rapidjson::Value& value)
	{
		// Setup the document and writer.
		BufferType buffer;
		WriterType writer(buffer);

		// Write out the document.
		value.Accept(writer);
		return buffer.GetString();
	}

	void WriteToFile(const char *contents, const char *fileName)
	{
		std::ofstream file;
		file.open(fileName);
		if (!file.is_open())
		{
			Logging::Log(Logging::Channel::DEFAULT, Logging::HIGH_PRIORITY, "Couldn't open file ", fileName, " while saving json data");
			return;
		}

		file << contents;
	}
}

std::string JsonToString(rapidjson::Value& value)
{
	return WriteJsonString<rapidjson::Writer<rapidjson::StringBuffer>, rapidjson::StringBuffer>(value);
}

std::string JsonToPrettyString(rapidjson::Value& value)
{
	return WriteJsonString<rapidjson::PrettyWriter<rapidjson::StringBuffer>, rapidjson::StringBuffer>(value);
}

void JsonToFile(rapidjson::Value & value, const char *fileName)
{
	Logging::Log(Logging::Channel::DEFAULT, Logging::MEDIUM_PRIORITY, "Attempting to save json object to file ", fileName);

	WriteToFile(JsonToString(value).c_str(), fileName);
}

void JsonToPrettyFile(rapidjson::Value & value, const char *fileName)
{
	Logging::Log(Logging::Channel::DEFAULT, Logging::MEDIUM_PRIORITY, "Attempting to save pretty json object to file ", fileName);

	WriteToFile(JsonToPrettyString(value).c_str(), fileName);
}

rapidjson::Document LoadJsonFile(const char *fileName)
{
	std::ifstream inStream;
	inStream.open(fileName);
	if (!inStream.is_open())
	{
		Logging::Log(Logging::Channel::DEFAULT, Logging::HIGH_PRIORITY, "Couldn't open file ", fileName, " while saving json data");
		return rapidjson::Document();
	}

	std::stringstream jsonString;
	jsonString << inStream.rdbuf();

	rapidjson::Document doc;
	doc.Parse(jsonString.str().c_str());

	return doc;
}


