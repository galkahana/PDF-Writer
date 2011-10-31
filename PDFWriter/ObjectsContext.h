/*
   Source File : ObjectsContext.h


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
#include "IndirectObjectsReferenceRegistry.h"
#include "ETokenSeparator.h"
#include "PrimitiveObjectsWriter.h"
#include "UppercaseSequance.h"
#include <string>
#include <list>

using namespace std;

class IByteWriterWithPosition;
class DictionaryContext;
class PDFStream;
class IObjectsContextExtender;
class ObjectsContext;
class PDFParser;

typedef list<DictionaryContext*> DictionaryContextList;

class ObjectsContext
{
public:
	ObjectsContext(void);
	~ObjectsContext(void);

	void SetOutputStream(IByteWriterWithPosition* inOutputStream);


	// Currently doing pre 1.5 xref writing
	PDFHummus::EStatusCode WriteXrefTable(LongFilePositionType& outWritePosition);

	// Free Context, for direct stream writing
	IByteWriterWithPosition* StartFreeContext();
	void EndFreeContext();

	// Get objects management object
	IndirectObjectsReferenceRegistry& GetInDirectObjectsRegistry();

	// Token Writing

	void WriteTokenSeparator(ETokenSeparator inSeparate);
	void EndLine();

	// Objects Writing

	// write comment line. ends line
	void WriteComment(const string& inCommentText);
	// write keyword. ends line
	void WriteKeyword(const string& inKeyword);

	void WriteName(const string& inName,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteInteger(long long inIntegerToken,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteLiteralString(const string& inString,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteHexString(const string& inString,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteIndirectObjectReference(ObjectIDType inIndirectObjectID,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteDouble(double inDoubleToken,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteBoolean(bool inBooleanToken,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteNull(ETokenSeparator inSeparate = eTokenSeparatorSpace);

	// Dictionary writing. StartDictionary starts a context which allows writing.
	// this context enforces some simple values
	DictionaryContext* StartDictionary();
	// ends dictionary context, releases the dictionary started in "StartDictionary", and now returned
	PDFHummus::EStatusCode EndDictionary(DictionaryContext* inDictionaryContext);

	// Array writing, currently just writing begin and end brackets
	void StartArray();
	void EndArray(ETokenSeparator inSeparate = eTokenSepratorNone);

	// Indirect objects writing
	// override that allocate a new object ID and returns it
	ObjectIDType StartNewIndirectObject();
	// override for objects that already have been allocated in advance, and have an object ID
	void StartNewIndirectObject(ObjectIDType inObjectID);
	void EndIndirectObject();


	// Sets whether streams created by the objects context will be compressed (with flate) or not
	void SetCompressStreams(bool inCompressStreams);

	// Create PDF stream and write it's header. note that stream are written with indirect object for Length, to allow one pass writing.
	// inStreamDictionary can be passed in order to include stream generic information in an already written stream dictionary
	// that is type specific. [the method will take care of closing the dictionary.
	PDFStream* StartPDFStream(DictionaryContext* inStreamDictionary=NULL);
	// same as StartPDFStream but forces the stream to create an unfiltered stream
	PDFStream* StartUnfilteredPDFStream(DictionaryContext* inStreamDictionary=NULL);
	void EndPDFStream(PDFStream* inStream);

	// Extensibility
	void SetObjectsContextExtender(IObjectsContextExtender* inExtender);


	// as the obly common context around...i'm using the objects context to create
	// subset fonts prefixes. might want to consider a more relevant object...
	string GenerateSubsetFontPrefix();


	PDFHummus::EStatusCode WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID);
	PDFHummus::EStatusCode ReadState(PDFParser* inStateReader,ObjectIDType inObjectID);

private:
	IObjectsContextExtender* mExtender;
	IByteWriterWithPosition* mOutputStream;
	IndirectObjectsReferenceRegistry mReferencesRegistry;
	PrimitiveObjectsWriter mPrimitiveWriter;
	bool mCompressStreams;
	UppercaseSequance mSubsetFontsNamesSequance;

	DictionaryContextList mDictionaryStack;

	void WritePDFStreamEndWithoutExtent();
	void WritePDFStreamExtent(PDFStream* inStream);

};
