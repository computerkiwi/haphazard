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

# Returns the argument passed to a one-argument macro in a given cpp file represented as a string.
def FindMacroValue(fileString, macro):
	startIndex = fileString.find(macro)

	if (startIndex == -1):
		return None

	# Make sure we're not picking up the initial definition of the macro.
	DEFINE_SYNTAX = "#define "
	if (fileString.find(DEFINE_SYNTAX + macro) != -1):
		return None

	openParenIndex  = fileString.find('(', startIndex)
	closeParenIndex = fileString.find(')', openParenIndex)

	return fileString[openParenIndex + 1 : closeParenIndex]
	

def ParseSingleHeader(fileName, outputTypesList, outputNamespacesList, outputHeadersList):
	with open(fileName, 'r') as file:
		fString = file.read()

	namespace = FindMacroValue(fString, "META_NAMESPACE")
	if (not (namespace is None)):
		outputNamespacesList.append(namespace)

	typeName = FindMacroValue(fString, "META_REGISTER")
	if (typeName is None):
		return
	else:
		outputTypesList.append(typeName)
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
