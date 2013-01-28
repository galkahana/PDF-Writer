#include "Trace.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFFormXObject.h"
#include "PageContentContext.h"
#include "PDFImageXObject.h"
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFObject.h"
#include "PDFIndirectObjectReference.h"
#include "PDFStreamInput.h"
#include "PDFArray.h"
#include "PDFDocumentCopyingContext.h"
#include "ObjectsContext.h"
#include "PDFStream.h"
#include "DictionaryContext.h"
#include "OutputStreamTraits.h"

#include <iostream>

using namespace PDFHummus;

int main()
{ std::string testFileName = "A1 PL.pdf";
	PDFWriter pdfWriter;
	EStatusCode status = eSuccess;
	status = pdfWriter.ModifyPDF(testFileName, ePDFVersion13, std::string("mod") + testFileName);
	PDFParser* inParser;
	inParser =&pdfWriter.GetModifiedFileParser();
	
	//inParser->ParseAcroForm();
	
	
return 0;
}
