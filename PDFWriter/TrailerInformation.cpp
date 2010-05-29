#include "TrailerInformation.h"

TrailerInformation::TrailerInformation(void)
{
	mPrev = 0;
	mRootReference = 0;
	mEncryptReference = 0;
	mInfoDictionaryReference = 0;
}

TrailerInformation::~TrailerInformation(void)
{
}

void TrailerInformation::SetInfoDictionaryReference(ObjectIDType inInfoDictionaryReference)
{
	mInfoDictionaryReference = inInfoDictionaryReference;
}

void TrailerInformation::SetPrev(LongFilePositionType inPrev)
{
	mPrev = inPrev;
}

void TrailerInformation::SetRoot(ObjectIDType inRootReference)
{
	mRootReference = inRootReference;
}

void TrailerInformation::SetEncrypt(ObjectIDType inEncryptReference)
{
	mEncryptReference = inEncryptReference;
}

BoolAndLongFilePositionType TrailerInformation::GetPrev()
{
	return BoolAndLongFilePositionType(mPrev != 0,mPrev);
}

BoolAndObjectIDType TrailerInformation::GetRoot()
{
	return BoolAndObjectIDType(mRootReference != 0, mRootReference);
}

BoolAndObjectIDType TrailerInformation::GetEncrypt()
{
	return BoolAndObjectIDType(mEncryptReference != 0, mEncryptReference);
}

InfoDictionary& TrailerInformation::GetInfo()
{
	return mInfoDictionary;
}

BoolAndObjectIDType TrailerInformation::GetInfoDictionaryReference()
{
	return BoolAndObjectIDType(mInfoDictionaryReference != 0, mInfoDictionaryReference);
}