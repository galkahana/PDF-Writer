#pragma once

#include "PDFComment.h"
#include "ObjectsBasicTypes.h"
#include "EStatusCode.h"
#include <map>
#include <utility>

using namespace std;

class PDFWriter;

typedef map<PDFComment*,ObjectIDType> PDFCommentToObjectIDTypeMap;
typedef pair<PDFHummus::EStatusCode,ObjectIDType> EStatusCodeAndObjectIDType;
typedef pair<PDFHummus::EStatusCode,PDFCommentToObjectIDTypeMap> EStatusCodeAndPDFCommentToObjectIDTypeMap;

class PDFCommentWriter
{
public:
	PDFCommentWriter(PDFWriter* inPDFWriter);
	~PDFCommentWriter(void);


	// Write a comment (now) and attach it to the page that will be written next.
	// Technically speaking, the comment will be written at this point (so don't have an open content stream, or something),
	// as well as any comments that this one is connected to through "In Reply To" relationship.
    PDFHummus::EStatusCode AttachCommentTreeToNextPage(PDFComment* inComment,PDFCommentToObjectIDTypeMap& ioPageCommentsContext);

    // Write a comment (now), and its connected comments, and return the comment object ID. This workflow
    // is for when you don't wish to use the internal infrastracture to write to the next page.
    // Then - you have to do the page attachment on your own. returns a map
    // of comments to ids. note that since you may have replytos, this may be more than one comments
    PDFHummus::EStatusCode WriteCommentTree(PDFComment* inComment,PDFCommentToObjectIDTypeMap& ioPageCommentsContext);

private:
	PDFWriter* mPDFWriter;

	void ListenOnPageWrites();
	EStatusCodeAndObjectIDType WriteCommentsTree(PDFComment* inComment,PDFCommentToObjectIDTypeMap& inCommentsContext);
	
};
