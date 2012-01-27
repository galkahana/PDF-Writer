/*
   Source File : TrailerInformation.cpp


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

void TrailerInformation::Reset()
{
	mPrev = 0;
	mRootReference = 0;
	mEncryptReference = 0;
	mInfoDictionaryReference = 0;
	mInfoDictionary.Reset();
}