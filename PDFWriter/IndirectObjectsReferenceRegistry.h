/*
   Source File : IndirectObjectsReferenceRegistry.h


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
/*
	IndirectObjectsReferenceRegistry does two jobs:
	1. It maintains the reference list for all indirect objects (initially just their total count), and allowing to get a new object number
	2. It maintains file writing information, such as whether the object was written and if so at what position
*/

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "IOBasicTypes.h"
#include <vector>
#include <utility>

using namespace IOBasicTypes;

class ObjectsContext;
class PDFParser;

struct ObjectWriteInformation
{
	// although there's no "update" method in this project (being a single flat write), the free marker is used
	// for the first element in the registry.
	enum EObjectReferenceType
	{
		Free,
		Used
	};

	bool mObjectWritten;
	LongFilePositionType mWritePosition; // value is undefined if mObjectWritten is false
	EObjectReferenceType mObjectReferenceType;
};

typedef std::pair<bool,ObjectWriteInformation> GetObjectWriteInformationResult;
typedef std::vector<ObjectWriteInformation> ObjectWriteInformationVector;

class IndirectObjectsReferenceRegistry
{
public:
	IndirectObjectsReferenceRegistry(void);
	~IndirectObjectsReferenceRegistry(void);

	ObjectIDType AllocateNewObjectID();

	PDFHummus::EStatusCode MarkObjectAsWritten(ObjectIDType inObjectID,LongFilePositionType inWritePosition);
	GetObjectWriteInformationResult GetObjectWriteInformation(ObjectIDType inObjectID) const;

	ObjectIDType GetObjectsCount() const;
	// should be used with safe object IDs. use GetObjectsCount to verify the maximum ID
	const ObjectWriteInformation& GetNthObjectReference(ObjectIDType inObjectID) const;

	PDFHummus::EStatusCode WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID);
	PDFHummus::EStatusCode ReadState(PDFParser* inStateReader,ObjectIDType inObjectID);

private:
	ObjectWriteInformationVector mObjectsWritesRegistry;
};
