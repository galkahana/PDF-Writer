#include "PDFCommentWriter.h"
#include "PDFWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "PDFRectangle.h"
#include "Trace.h"
#include "SafeBufferMacrosDefs.h"

using namespace PDFHummus;

PDFCommentWriter::PDFCommentWriter(PDFWriter* inPDFWriter)
{
	mPDFWriter = inPDFWriter;
}

PDFCommentWriter::~PDFCommentWriter(void)
{
}


EStatusCode PDFCommentWriter::AttachCommentTreeToNextPage(PDFComment* inComment,PDFCommentToObjectIDTypeMap& ioPageCommentsContext)
{
    
	EStatusCode status = WriteCommentsTree(inComment,ioPageCommentsContext).first;
    if(status!=eSuccess)
        return status;
    
    PDFCommentToObjectIDTypeMap::iterator it = ioPageCommentsContext.begin();
    
    for(; it != ioPageCommentsContext.end(); ++it)
        mPDFWriter->GetDocumentContext().RegisterAnnotationReferenceForNextPageWrite(it->second);
    
    return status;
}

EStatusCodeAndObjectIDType PDFCommentWriter::WriteCommentsTree(PDFComment* inComment,PDFCommentToObjectIDTypeMap& inCommentsContext)
{
	EStatusCodeAndObjectIDType result;
	ObjectIDType repliedTo = 0;

	// if already written, return
	PDFCommentToObjectIDTypeMap::iterator it = inCommentsContext.find(inComment);
	if(it != inCommentsContext.end())
	{
		result.first = eSuccess;
		result.second = it->second;
		return result;
	}

	// if has a referred comment, write it first
	if(inComment->ReplyTo != NULL)
	{
		EStatusCodeAndObjectIDType repliedtoResult = WriteCommentsTree(inComment->ReplyTo,inCommentsContext);

		if(repliedtoResult.first != eSuccess)
		{
			result.first = eFailure;
			result.second = 0;
			return result;
		}
		else
			repliedTo = repliedtoResult.second;
	}

	do
	{
		ObjectsContext& objectsContext = mPDFWriter->GetObjectsContext();

		// Start new InDirect object for annotation dictionary
		result.second = objectsContext.StartNewIndirectObject();
		
		DictionaryContext* dictionaryContext = objectsContext.StartDictionary();


		// Type
		dictionaryContext->WriteKey("Type");
		dictionaryContext->WriteNameValue("Annot");

		// SubType
		dictionaryContext->WriteKey("Subtype");
		dictionaryContext->WriteNameValue("Text");

		// Rect
		dictionaryContext->WriteKey("Rect");
		dictionaryContext->WriteRectangleValue(
			PDFRectangle(inComment->FrameBoundings[0],
						 inComment->FrameBoundings[1],
						 inComment->FrameBoundings[2],
						 inComment->FrameBoundings[3]));

		// Contents 
		dictionaryContext->WriteKey("Contents");
		dictionaryContext->WriteLiteralStringValue(PDFTextString(inComment->Text).ToString());

		// C (color)
		dictionaryContext->WriteKey("C");
		objectsContext.StartArray();
		if(inComment->Color.UseCMYK)
		{
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[0]/255);
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[1]/255);
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[2]/255);
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[3]/255);
		}
		else
		{
			objectsContext.WriteDouble((double)inComment->Color.RGBComponents[0]/255);
			objectsContext.WriteDouble((double)inComment->Color.RGBComponents[1]/255);
			objectsContext.WriteDouble((double)inComment->Color.RGBComponents[2]/255);
		}
		objectsContext.EndArray(eTokenSeparatorEndLine);

		// T
		dictionaryContext->WriteKey("T");
		dictionaryContext->WriteLiteralStringValue(PDFTextString(inComment->CommentatorName).ToString());
		
		// M
		dictionaryContext->WriteKey("M");
		dictionaryContext->WriteLiteralStringValue(inComment->Time.ToString());

		if(inComment->ReplyTo != NULL)
		{
			// IRT
			dictionaryContext->WriteKey("IRT");
			dictionaryContext->WriteObjectReferenceValue(repliedTo);

			// RT (we're doing always "reply" at this point, being a reply to comment
			dictionaryContext->WriteKey("RT");
			dictionaryContext->WriteNameValue("R");
		}

		// Open (we'll have them all closed to begin with)
		dictionaryContext->WriteKey("Open");
		dictionaryContext->WriteBooleanValue(false);

		// Name
		dictionaryContext->WriteKey("Name");
		dictionaryContext->WriteNameValue("Comment");

		if(objectsContext.EndDictionary(dictionaryContext) != eSuccess)
		{
			result.first = eFailure;
			TRACE_LOG("PDFCommentWriter::WriteCommentsTree, Exception in ending comment dictionary");
			break;
		}
		objectsContext.EndIndirectObject();
	
		inCommentsContext.insert(PDFCommentToObjectIDTypeMap::value_type(inComment,result.second));

		result.first = eSuccess;
	}while(false);
	
	return result;
}

EStatusCode PDFCommentWriter::WriteCommentTree(PDFComment* inComment,PDFCommentToObjectIDTypeMap& ioPageCommentsContext)
{
    
    return WriteCommentsTree(inComment,ioPageCommentsContext).first;

}


