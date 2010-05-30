#pragma once

#include "EStatusCode.h"
#include "IndirectObjectsReferenceRegistry.h"
#include "ETokenSeparator.h"
#include "PrimitiveObjectsWriter.h"
#include <string>
#include <list>

using namespace std;

class IByteWriterWithPosition;
class DictionaryContext;
class PDFStream;

typedef list<DictionaryContext*> DictionaryContextList;

class ObjectsContext
{
public:
	ObjectsContext(void);
	~ObjectsContext(void);

	void SetOutputStream(IByteWriterWithPosition* inOutputStream);
	

	// Currently doing pre 1.5 xref writing
	EStatusCode WriteXrefTable(LongFilePositionType& outWritePosition);

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

	// Dictionary writing. StartDictionary starts a context which allows writing.
	// this context enforces some simple values
	DictionaryContext* StartDictionary();
	// ends dictionary context, releases the dictionary started in "StartDictionary", and now returned
	EStatusCode EndDictionary(DictionaryContext* inDictionaryContext); 

	// Array writing, currently just writing begin and end brackets
	void StartArray();
	void EndArray(ETokenSeparator inSeparate = eTokenSepratorNone);

	// Indirect objects writing
	// override that allocate a new object ID and returns it
	ObjectIDType StartNewIndirectObject();
	// override for objects that already have been allocated in advance, and have an object ID
	void StartNewIndirectObject(ObjectIDType inObjectID);
	void EndIndirectObject();

	// Stream writing [assumes that is already in an indirect object context]
	// Use inStreamDictionary in case this is not just a "stream" but actually a higher level
	// object that is *also* a stream. in that case creat the dictionary externally, write whatever keys you want,
	// and Call WritePDFStream to write the common stream keys (such as length and filters) and the stream itself
	EStatusCode WritePDFStream(PDFStream* inStream,DictionaryContext* inStreamDictionary=NULL);

private:
	IByteWriterWithPosition* mOutputStream;
	IndirectObjectsReferenceRegistry mReferencesRegistry;
	PrimitiveObjectsWriter mPrimitiveWriter;

	DictionaryContextList mDictionaryStack;

};
