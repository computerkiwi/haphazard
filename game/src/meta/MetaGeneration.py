# 
# FILE: MetaGeneration.py
# PRIMARY AUTHOR: Kieran Williams
# 
# Prebuild step to parse through header files and generate a meta registration file.
#
# Copyright (c) 2017 DigiPen (USA) Corporation.
# 

import sys
import glob


# Produces global vars outputPath and sourceDirectory
def ParseCommandLine():
	lastArgWasOutput = False # Did we just read a -o for the output directory?

	for argIndex in range(1, len(sys.argv)):
	
		global outputPath
		global sourceDirectory

		if (lastArgWasOutput):
			outputPath = sys.argv[argIndex]
			lastArgWasOutput = False

		elif (sys.argv[argIndex] == "-o"):
			lastArgWasOutput = True

		else:
			sourceDirectory = sys.argv[argIndex]

def GetHeaderFiles(sourceDir):
	return glob.glob(sourceDir + "/**/*.h", recursive = True)

def FindMacroValueFromIndex(fileString, macro, index, outArray):
	startIndex = fileString.find(macro, index)

	if (startIndex == -1):
		return -1

	# Make sure we're not picking up the initial definition of the macro.
	DEFINE_SYNTAX = "#define "
	if (fileString.find(DEFINE_SYNTAX + macro) != -1):
		return -1

	openParenIndex  = fileString.find('(', startIndex)
	closeParenIndex = fileString.find(')', openParenIndex)

	outArray.append(fileString[openParenIndex + 1 : closeParenIndex])

	return closeParenIndex

# Returns the argument passed to a one-argument macro in a given cpp file represented as a string.
def FindMacroValue(fileString, macro):
	values = []

	FindMacroValueFromIndex(fileString, macro, 0, values)

	if (len(values) == 0):
		return None
	else:
		return values[0]

def FindMacroValues(fileString, macro):
	index = 0
	values = []

	while(index >= 0):
		index = FindMacroValueFromIndex(fileString, macro, index, values)

	return values

def ParseSingleHeader(fileName, outputTypesList, outputNamespacesList, outputHeadersList):
	with open(fileName, 'r') as file:
		fString = file.read()

	namespaces = FindMacroValues(fString, "META_NAMESPACE")
	for namespace in namespaces:
		outputNamespacesList.append(namespace)

	typeNames = FindMacroValues(fString, "META_REGISTER")
	for typeName in typeNames:
		outputTypesList.append(typeName)

	if (len(typeNames) > 0):
		outputHeadersList.append(fileName)

def ParseHeaders(headerFilesList, outputTypesList, outputNamespacesList, outputHeadersList):
	for fileName in headerFilesList:
		ParseSingleHeader(fileName, outputTypesList, outputNamespacesList, outputHeadersList)
		
def GenerateFile(filePath, typesList, namespacesList, headersList):
	REGISTRATION_FUNCTION_PREFIX = "Meta__Register__"

	with open(filePath, 'w') as file:
		for headerPath in headersList:
			file.write('#include "' + headerPath + '"\n')
		for namespace in namespacesList:
			file.write("using namespace " + namespace + ";\n")
			
		file.write("namespace meta\n{\nvoid Init()\n{\n")

		for typeName in typesList:
			file.write(typeName + "::" + REGISTRATION_FUNCTION_PREFIX + typeName +"();\n")

		file.write("}\n}\n")

		
print("Generating meta info with Python...")	

ParseCommandLine()
headerFilePaths = GetHeaderFiles(sourceDirectory)

typesList = []
namespacesList = []
headersList = []

ParseHeaders(headerFilePaths, typesList, namespacesList, headersList)
GenerateFile(outputPath, typesList, namespacesList, headersList)
