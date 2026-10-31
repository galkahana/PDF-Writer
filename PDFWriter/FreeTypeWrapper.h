/*
   Source File : FreeTypeWrapper.h


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
#include "IOBasicTypes.h"

#include <string>
#include <vector>
#include <map>
#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H



typedef std::list<FT_Stream> FTStreamList;
typedef std::map<FT_Face,FTStreamList> FTFaceToFTStreamListMap;

class IByteReaderWithPosition;

struct StreamWithOwnership {
	IByteReaderWithPosition* stream;
	bool ownsStream;
};

typedef std::list<StreamWithOwnership> StreamWithOwnershipList;
typedef std::map<FT_Face,StreamWithOwnershipList> FTFaceToStreamWithOwnershipListMap;

class IByteReaderWithPosition;

class FreeTypeWrapper
{
public:
	FreeTypeWrapper(void);
	~FreeTypeWrapper(void);

	// For fonts loaded from file
	FT_Face NewFace(const std::string& inFilePath,FT_Long inFontIndex);
	// specific for type 1 fonts loaded from file with an additional file (e.g., .afm or .pfm)
	FT_Face NewFace(const std::string& inFilePath,const std::string& inSecondaryFilePath,FT_Long inFontIndex);
	// For fonts loaded from memory buffers (1 file variant)
	FT_Face NewFace(const std::vector<IOBasicTypes::Byte>& inFontBuffer,FT_Long inFontIndex);
	// For fonts loaded from read streams (1 file variant)
	FT_Face NewFace(IByteReaderWithPosition* inFontStream,FT_Long inFontIndex);
	FT_Error DoneFace(FT_Face ioFace);

	FT_Library operator->();
	operator FT_Library() const;


	IByteReaderWithPosition* GetFontReader(FT_Face inFace);

private:

	FT_Library mFreeType;
	// Freetype streams. those are provided to FTFace to provide custom reading
	FTFaceToFTStreamListMap mOpenFTStreams;
	// Underlying readers that are provided to FreeType streams. 
	// including ownership information to allow different handling of internal and external streams
	FTFaceToStreamWithOwnershipListMap mOpenReaders;

	FT_Face NewFace(IByteReaderWithPosition* inStream, bool inOwnsStream, IOBasicTypes::LongFilePositionType inSize, FT_Long inFontIndex);
	FT_Stream CreateFTStreamForIByteWriterWithPosition(IByteReaderWithPosition* inInputStream, unsigned long inSize);
	void FillOpenFaceArgumentsForFTStream(FT_Stream inStream, FT_Open_Args& ioArgs);
	void RegisterStreamForFace(FT_Face inFace,FT_Stream inFtStream, IByteReaderWithPosition* inReader, bool inOwnsStream);
	void ReleaseStreamForFace(FT_Stream inFtStream, IByteReaderWithPosition* inReader, bool inOwnsStream);


	void CleanStreamsForFace(FT_Face inFace);


};
