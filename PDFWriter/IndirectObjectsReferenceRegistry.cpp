#include "IndirectObjectsReferenceRegistry.h"
#include "Trace.h"

IndirectObjectsReferenceRegistry::IndirectObjectsReferenceRegistry(void)
{
	ObjectWriteInformation singleFreeObjectInformation;
	
	singleFreeObjectInformation.mObjectReferenceType = ObjectWriteInformation::Free;
	singleFreeObjectInformation.mObjectWritten = false;
	mObjectsWritesRegistry.push_back(singleFreeObjectInformation);
}

IndirectObjectsReferenceRegistry::~IndirectObjectsReferenceRegistry(void)
{
}


ObjectIDType IndirectObjectsReferenceRegistry::AllocateNewObjectID()
{
	ObjectWriteInformation newObjectInformation;
	ObjectIDType newObjectID = GetObjectsCount();

	newObjectInformation.mObjectWritten = false;
	newObjectInformation.mObjectReferenceType = ObjectWriteInformation::Used;
	
	mObjectsWritesRegistry.push_back(newObjectInformation);
	return newObjectID;
}

EStatusCode IndirectObjectsReferenceRegistry::MarkObjectAsWritten(ObjectIDType inObjectID,LongFilePositionType inWritePosition)
{
	if(mObjectsWritesRegistry.size() <= inObjectID)
	{
		TRACE_LOG1("IndirectObjectsReferenceRegistry::MarkObjectAsWritten, Out of range failure. An Object ID is marked as written, which was not allocated before. ID = %ld",inObjectID);
		return eFailure; 
	}

	if(mObjectsWritesRegistry[inObjectID].mObjectWritten)
	{
		TRACE_LOG3("IndirectObjectsReferenceRegistry::MarkObjectAsWritten, Object rewrite faiulre. The object %ld was already marked as written at %lld. New position is %lld",
			inObjectID,mObjectsWritesRegistry[inObjectID].mWritePosition,inWritePosition);
		return eFailure; // trying to mark as written an object that was already marked as such in the past. probably a mistake [till we have revisions]
	}

	if(inWritePosition > 999999999L) // if write position is larger than what can be represented by 10 digits, xref write will fail
	{
		TRACE_LOG1("IndirectObjectsReferenceRegistry::MarkObjectAsWritten, Write position out of bounds. Trying to write an object at position that cannot be represented in Xref = %lld. probably means file got too long",inWritePosition);
		return eFailure;
	}

	mObjectsWritesRegistry[inObjectID].mWritePosition = inWritePosition;
	mObjectsWritesRegistry[inObjectID].mObjectWritten = true;
	return eSuccess;
}

GetObjectWriteInformationResult IndirectObjectsReferenceRegistry::GetObjectWriteInformation(ObjectIDType inObjectID) const
{
	GetObjectWriteInformationResult result;

	if(mObjectsWritesRegistry.size() <= inObjectID)
	{
		result.first = false;
	}
	else
	{
		result.first = true;
		result.second = mObjectsWritesRegistry[inObjectID];
	}
	return result;
}

const ObjectWriteInformation& IndirectObjectsReferenceRegistry::GetNthObjectReference(ObjectIDType inObjectID) const
{
	return mObjectsWritesRegistry[inObjectID];
}

ObjectIDType IndirectObjectsReferenceRegistry::GetObjectsCount() const
{
	return static_cast<ObjectIDType>(mObjectsWritesRegistry.size());
}