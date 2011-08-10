/*
   Source File : TrueTypePrimitiveWriter.cpp


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
#include "TrueTypePrimitiveWriter.h"
#include "OutputStringBufferStream.h"

TrueTypePrimitiveWriter::TrueTypePrimitiveWriter(OutputStringBufferStream* inTrueTypeFile)
{
	SetOpenTypeStream(inTrueTypeFile);
}

TrueTypePrimitiveWriter::~TrueTypePrimitiveWriter(void)
{
}

void TrueTypePrimitiveWriter::SetOpenTypeStream(OutputStringBufferStream* inTrueTypeFile)
{
	mTrueTypeFile = inTrueTypeFile;
	mInternalState = (inTrueTypeFile ? ePDFSuccess:ePDFFailure);
}

EPDFStatusCode TrueTypePrimitiveWriter::GetInternalState()
{
	return mInternalState;
}

EPDFStatusCode TrueTypePrimitiveWriter::WriteBYTE(Byte inValue)
{
	if(ePDFFailure == mInternalState)
		return ePDFFailure;

	EPDFStatusCode status = (mTrueTypeFile->Write(&inValue,1) == 1 ? ePDFSuccess : ePDFFailure);

	if(ePDFFailure == status)
		mInternalState = ePDFFailure;
	return status;	
}

EPDFStatusCode TrueTypePrimitiveWriter::WriteULONG(unsigned long inValue)
{
	Byte byte1 = (inValue>>24) & 0xff;
	Byte byte2 = (inValue>>16) & 0xff;
	Byte byte3 = (inValue>>8) & 0xff;
	Byte byte4 = inValue & 0xff;

	if(WriteBYTE(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(WriteBYTE(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(WriteBYTE(byte3) != ePDFSuccess)
		return ePDFFailure;

	if(WriteBYTE(byte4) != ePDFSuccess)
		return ePDFFailure;

	return ePDFSuccess;
}

EPDFStatusCode TrueTypePrimitiveWriter::WriteUSHORT(unsigned short inValue)
{	
	Byte byte1 = (inValue>>8) & 0xff;
	Byte byte2 = inValue & 0xff;
	
	if(WriteBYTE(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(WriteBYTE(byte2) != ePDFSuccess)
		return ePDFFailure;

	return ePDFSuccess;
}

EPDFStatusCode TrueTypePrimitiveWriter::WriteSHORT(short inValue)
{
	return WriteUSHORT((unsigned short)inValue);
}

EPDFStatusCode TrueTypePrimitiveWriter::Pad(int inCount)
{

	for(int i=0; i <inCount && (mInternalState == ePDFSuccess); ++i)
		WriteBYTE(0);

	return mInternalState;
}

EPDFStatusCode TrueTypePrimitiveWriter::PadTo4()
{
	int padSize = (4 - (mTrueTypeFile->GetCurrentPosition() % 4)) % 4;

	for(int i=0; i < padSize; ++i)
		WriteBYTE(0);
	return mInternalState;
}