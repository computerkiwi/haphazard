/*
FILE: UnorderedIterableTests.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
*/

#include "UnorderedIterable.h"
#include <vector>
#include <algorithm>

#define RETURN_IF_FALSE(Expr) if (!(Expr)) return false

template <typename T>
std::vector<T> IterableToVector(UnorderedIterable<T>& data)
{
	std::vector<T> dataVector;
	for (UnorderedIterable<int>::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		dataVector.push_back(*iter);
	}

	return dataVector;
}

static bool SimpleTest1(int testSize)
{
	UnorderedIterable<int> data;
	std::vector<int> controlVector;

	for (int i = 0; i < testSize; ++i)
	{
		controlVector.push_back(i);
		data.insert(i);
	}

	std::vector<int> dataVector = IterableToVector(data);

	std::sort(controlVector.begin(), controlVector.end());
	std::sort(dataVector.begin(), dataVector.end());
	RETURN_IF_FALSE(controlVector == dataVector);

	return true;
}

static bool SimpleTest2(int testSize)
{
	UnorderedIterable<int> data;
	std::vector<std::pair<UnorderedIterable<int>::ID, int>> idSaved;

	for (int i = 0; i < testSize; ++i)
	{
		idSaved.push_back(std::make_pair(data.insert(i), i));
	}

	for (auto pair : idSaved)
	{
		RETURN_IF_FALSE(pair.second == data.at(pair.first));
	}
	for (auto pair : idSaved)
	{
		RETURN_IF_FALSE(pair.second == data[pair.first]);
	}

	return true;
}

static bool RemoveTest1()
{
	UnorderedIterable<int> data;
	
	// Add a few things.
	data.insert(5);
	data.insert(9);
	UnorderedIterable<int>::ID id7 = data.insert(7);
	data.insert(3);
	data.insert(1000);
	UnorderedIterable<int>::ID id55 = data.insert(55);
	data.insert(123);

	// Erase a few things.
	data.remove(id7);
	data.remove(id55);

	std::vector<int> controlVec = { 5, 9, 3, 1000, 123 };
	std::vector<int> dataVec = IterableToVector(data);

	std::sort(controlVec.begin(), controlVec.end());
	std::sort(dataVec.begin(), dataVec.end());

	RETURN_IF_FALSE(controlVec == dataVec);

	return true;
}

static bool RemoveTest2()
{
	UnorderedIterable<int> data;

	// Add a few things.
	data.insert(1);
	data.insert(545);
	UnorderedIterable<int>::ID id897 = data.insert(897);
	data.insert(2);
	data.insert(-5);
	UnorderedIterable<int>::ID id71 = data.insert(71);
	UnorderedIterable<int>::ID id20 = data.insert(20);

	// Remove some stuff.
	data.remove(id897);
	data.remove(id20);
	data.remove(id71);

	// Add some more stuff.
	data.insert(1111);
	data.insert(123456);

	std::vector<int> controlVec = { 1, 545, 2, -5, 1111, 123456 };
	std::vector<int> dataVec = IterableToVector(data);

	std::sort(controlVec.begin(), controlVec.end());
	std::sort(dataVec.begin(), dataVec.end());

	RETURN_IF_FALSE(controlVec == dataVec);

	return true;
}

static bool RemoveFrontTest1()
{
	UnorderedIterable<int> data;
	std::vector<UnorderedIterable<int>::ID> idSaved;

	std::vector<int> controlVec;

	// Insert values we're going to remove.
	for (int i = 0; i < 20; ++i)
	{
		idSaved.push_back(data.insert(i));
	}
	// Insert values that will stick around.
	for (int i = 20; i < 50; ++i)
	{
		data.insert(i);
		controlVec.push_back(i);
	}

	// Remove front values.
	for (auto id : idSaved)
	{
		data.remove(id);
	}

	// Make sure we have the data we're supposed to.
	std::vector<int> dataVec = IterableToVector(data);
	std::sort(controlVec.begin(), controlVec.end());
	std::sort(dataVec.begin(), dataVec.end());

	RETURN_IF_FALSE(controlVec == dataVec);

	return true;
}

static bool EmptyTest()
{
	UnorderedIterable<int> data;
	for (int i = 0; i < 300; ++i)
	{
		data.insert(i);
	}
	data.clear();

	RETURN_IF_FALSE(data.empty());
	RETURN_IF_FALSE(data.size() == 0);

	std::vector<int> dataVec = IterableToVector(data);
	RETURN_IF_FALSE(dataVec.empty());

	return true;
}

bool UnorderedIterablePassesTests()
{
	RETURN_IF_FALSE(SimpleTest1(300));
	RETURN_IF_FALSE(SimpleTest2(300));

	RETURN_IF_FALSE(RemoveTest1());
	RETURN_IF_FALSE(RemoveTest2());
	RETURN_IF_FALSE(RemoveFrontTest1());

	RETURN_IF_FALSE(EmptyTest());

	return true;
}

//static bool initializationTimeHelper = UnorderedIterablePassesTests();
