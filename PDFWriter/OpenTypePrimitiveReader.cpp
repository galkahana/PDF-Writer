/*
   Source File : OpenTypePrimitiveReader.cpp


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
#include "OpenTypePrimitiveReader.h"

OpenTypePrimitiveReader::OpenTypePrimitiveReader(IByteReaderWithPosition* inOpenTypeFile)
{
	SetOpenTypeStream(inOpenTypeFile);
}

OpenTypePrimitiveReader::~OpenTypePrimitiveReader(void)
{
}

void OpenTypePrimitiveReader::SetOpenTypeStream(IByteReaderWithPosition* inOpenTypeFile)
{
	mOpenTypeFile = inOpenTypeFile;
	if(inOpenTypeFile)
	{
		mInitialPosition = inOpenTypeFile->GetCurrentPosition();
		mInternalState = ePDFSuccess;
	}
	else
	{
		mInternalState = ePDFFailure;
	}
}

EPDFStatusCode OpenTypePrimitiveReader::ReadBYTE(unsigned char& outValue)
{
	if(ePDFFailure == mInternalState)
		return ePDFFailure;

	Byte buffer;
	EPDFStatusCode status = (mOpenTypeFile->Read(&buffer,1) == 1 ? ePDFSuccess : ePDFFailure);

	if(ePDFFailure == status)
		mInternalState = ePDFFailure;
	outValue = buffer;
	return status;
}

EPDFStatusCode OpenTypePrimitiveReader::ReadCHAR(char& outValue)
{
	Byte buffer;

	if(ReadBYTE(buffer) != ePDFSuccess)
		return ePDFFailure;

	outValue = (char)buffer;
	return ePDFSuccess;
}

EPDFStatusCode OpenTypePrimitiveReader::ReadUSHORT(unsigned short& outValue)
{
	Byte byte1,byte2;

	if(ReadBYTE(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte2) != ePDFSuccess)
		return ePDFFailure;


	outValue = ((unsigned short)byte1 << 8) + byte2;

	return ePDFSuccess;

}

EPDFStatusCode OpenTypePrimitiveReader::ReadSHORT(short& outValue)
{
	unsigned short buffer;

	if(ReadUSHORT(buffer) != ePDFSuccess)
		return ePDFFailure;

	outValue = (short)buffer;
	return ePDFSuccess;

}

EPDFStatusCode OpenTypePrimitiveReader::ReadULONG(unsigned long& outValue)
{
	Byte byte1,byte2,byte3,byte4;

	if(ReadBYTE(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte3) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte4) != ePDFSuccess)
		return ePDFFailure;

	outValue = ((unsigned long)byte1 << 24) + ((unsigned long)byte2 << 16) + 
									((unsigned long)byte3 << 8) + byte4;

	return ePDFSuccess;
}

EPDFStatusCode OpenTypePrimitiveReader::ReadLONG(long& outValue)
{
	unsigned long buffer;

	if(ReadULONG(buffer) != ePDFSuccess)
		return ePDFFailure;

	outValue = (long)buffer;
	return ePDFSuccess;
}

EPDFStatusCode OpenTypePrimitiveReader::ReadLongDateTime(long long& outValue)
{
	Byte byte1,byte2,byte3,byte4,byte5,byte6,byte7,byte8;

	if(ReadBYTE(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte3) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte4) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte5) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte6) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte7) != ePDFSuccess)
		return ePDFFailure;

	if(ReadBYTE(byte8) != ePDFSuccess)
		return ePDFFailure;

	outValue =(long long)(	((unsigned long long)byte1 << 56) + ((unsigned long long)byte2 << 48) + 
							((unsigned long long)byte3 << 40) + ((unsigned long long)byte4 << 32) + 
							((unsigned long long)byte5 << 24) + ((unsigned long long)byte6 << 16) + 
							((unsigned long long)byte7 << 8) + byte8);
	return ePDFSuccess;	
}

void OpenTypePrimitiveReader::SetOffset(LongFilePositionType inNewOffset)
{
	if(mInternalState != ePDFFailure)
		mOpenTypeFile->SetPosition(mInitialPosition + inNewOffset);
}	

void OpenTypePrimitiveReader::Skip(LongBufferSizeType inToSkip)
{
	if(mInternalState != ePDFFailure)
		mOpenTypeFile->Skip(inToSkip);
}

EPDFStatusCode OpenTypePrimitiveReader::GetInternalState()
{
	return mInternalState;
}

EPDFStatusCode OpenTypePrimitiveReader::ReadFixed(double& outValue)
{
	unsigned short integer,fraction;

	if(ReadUSHORT(integer) != ePDFSuccess)
		return ePDFFailure;

	if(ReadUSHORT(fraction) != ePDFSuccess)
		return ePDFFailure;

	outValue = (double)integer + ((double)fraction) / (1<<16);

	return ePDFSuccess;
}

LongFilePositionType OpenTypePrimitiveReader::GetCurrentPosition()
{
	if(mInternalState != ePDFFailure)
		return mOpenTypeFile->GetCurrentPosition() - mInitialPosition;	
	else
		return 0;
}

EPDFStatusCode OpenTypePrimitiveReader::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(ePDFFailure == mInternalState)
		return ePDFFailure;

	EPDFStatusCode status = (mOpenTypeFile->Read(inBuffer,inBufferSize) == inBufferSize ? ePDFSuccess : ePDFFailure);

	if(ePDFFailure == status)
		mInternalState = ePDFFailure;
	return status;	
}

IByteReaderWithPosition* OpenTypePrimitiveReader::GetReadStream()
{
	return mOpenTypeFile;
}