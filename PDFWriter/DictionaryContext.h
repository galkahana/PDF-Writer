/*
   Source File : DictionaryContext.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#pragma once
#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "PDFRectangle.h"

#include <string>
#include <set>

using namespace std;

typedef set<string> StringSet;

class ObjectsContext;

class DictionaryContext
{
public:
	DictionaryContext(ObjectsContext* inObjectsContext,size_t inIndentLevel);
	~DictionaryContext(void);

	EStatusCode WriteKey(const string& inKey);
	
	void WriteIntegerValue(long long inValue);
	void WriteLiteralStringValue(const string& inValue);
	void WriteNameValue(const string& inValue);
	void WriteObjectReferenceValue(ObjectIDType inObjectReference);
	void WriteRectangleValue(const PDFRectangle& inRectangle);
	void WriteDoubleValue(double inValue);
	void WriteBooleanValue(bool inValue);

	// use WriteIndents when you are not using the above methods
	// for writing, but instead using either primitivewriter, or objectscontext to write directly
	// and you would still like to have the indents (a current example [4/10/2010] would be when writing
	// multiline arrays)
	void WriteIndents();
private:

	ObjectsContext* mObjectsContext;
	StringSet mKeys;
	size_t mIndentLevel;


};
