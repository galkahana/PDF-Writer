#pragma once
#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"
#include "InfoDictionary.h"

#include <utility>

using namespace IOBasicTypes;

typedef std::pair<bool,LongFilePositionType> BoolAndLongFilePositionType;
typedef std::pair<bool,ObjectIDType> BoolAndObjectIDType;

class TrailerInformation
{
public:
	TrailerInformation(void);
	~TrailerInformation(void);

	void SetPrev(LongFilePositionType inPrev);
	void SetRoot(ObjectIDType inRootReference);
	void SetEncrypt(ObjectIDType inEncryptReference);
	void SetInfoDictionaryReference(ObjectIDType inInfoDictionaryReference);


	// return with existance validation. for example, if Root is undefined, will return false
	// as the first parameter, otherwise true and the value
	BoolAndLongFilePositionType GetPrev();
	BoolAndObjectIDType GetRoot();
	BoolAndObjectIDType GetEncrypt();
	BoolAndObjectIDType GetInfoDictionaryReference();

	InfoDictionary& GetInfo();

private:
	LongFilePositionType mPrev;

	ObjectIDType mRootReference;
	ObjectIDType mEncryptReference;

	InfoDictionary mInfoDictionary;
	ObjectIDType mInfoDictionaryReference;
};
