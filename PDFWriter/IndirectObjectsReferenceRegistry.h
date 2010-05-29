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
	
	EStatusCode MarkObjectAsWritten(ObjectIDType inObjectID,LongFilePositionType inWritePosition);
	GetObjectWriteInformationResult GetObjectWriteInformation(ObjectIDType inObjectID) const;

	ObjectIDType GetObjectsCount() const;
	// should be used with safe object IDs. use GetObjectsCount to verify the maximum ID
	const ObjectWriteInformation& GetNthObjectReference(ObjectIDType inObjectID) const; 

private:
	ObjectWriteInformationVector mObjectsWritesRegistry;
};
