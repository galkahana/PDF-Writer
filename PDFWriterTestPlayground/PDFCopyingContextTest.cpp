#include "PDFCopyingContextTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFDocumentCopyingContext.h"

#include <iostream>

using namespace std;

PDFCopyingContextTest::PDFCopyingContextTest(void)
{
}

PDFCopyingContextTest::~PDFCopyingContextTest(void)
{
}

EStatusCode PDFCopyingContextTest::Run()
{
	EStatusCode status;
	PDFWriter pdfWriter;
	PDFDocumentCopyingContext* copyingContext = NULL;

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\PDFCopyingContextTest.PDF",ePDFVersion13,LogConfiguration(true,L"c:\\pdflibtests\\PDFCopyingContextTest.txt"));
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	


		copyingContext = pdfWriter.CreatePDFCopyingContext(L"C:\\PDFLibTests\\TestMaterials\\BasicTIFFImagesTest.PDF");
		if(!copyingContext)
		{
			wcout<<"failed to initialize copying context from BasicTIFFImagesTest\n";
			status = eFailure;
			break;
		}

		EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(1);
		if(result.first != eSuccess)
		{
			wcout<<"failed to append page 1 from BasicTIFFImagesTest.PDF\n";
			status = result.first;
			break;
		}

		result = copyingContext->AppendPDFPageFromPDF(18);
		if(result.first != eSuccess)
		{
			wcout<<"failed to append page 18 from BasicTIFFImagesTest.PDF\n";
			status = result.first;
			break;
		}

		result = copyingContext->AppendPDFPageFromPDF(4);
		if(result.first != eSuccess)
		{
			wcout<<"failed to append page 4 from BasicTIFFImagesTest.PDF\n";
			status = result.first;
			break;
		}

		copyingContext->End(); // delete will call End() as well...so can avoid

		delete copyingContext;
		copyingContext = NULL;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}

	}while(false);

	delete copyingContext;
	return status;

}

ADD_CATEGORIZED_TEST(PDFCopyingContextTest,"PDFEmbedding")