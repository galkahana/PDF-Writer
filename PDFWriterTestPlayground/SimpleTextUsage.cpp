#include "SimpleTextUsage.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "TestsRunner.h"

#include <iostream>

SimpleTextUsage::SimpleTextUsage(void)
{
}

SimpleTextUsage::~SimpleTextUsage(void)
{
}

EStatusCode SimpleTextUsage::Run()
{
	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{

		status = pdfWriter.InitializePDFWriter(L"C:\\PDFLibTests\\SimpleTextUsage.PDF",LogConfiguration(true,L"C:\\PDFLibTests\\SimpleTextUsage.log"));
		if(status != eSuccess)
		{
			wcout<<"failed to initialize PDF Writer\n";
			break;
		}

		status = pdfWriter.StartPDF(ePDFVersion13);
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
			break;
		}

		PDFUsedFont* font = pdfWriter.GetFontForFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\BrushScriptStd.otf");
		if(!font)
		{
			status = eFailure;
			wcout<<"Failed to create font object for brushscriptstd.otf\n";
			break;
		}


		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);

		contentContext->Tf(font,1);

		contentContext->Tm(30,0,0,30,78.4252,662.8997);

		EStatusCode encodingStatus = contentContext->Tj(L"abcd");
		if(encodingStatus != eSuccess)
			wcout<<"Could not find some of the glyphs for this font";

		// continue even if failed...want to see how it looks like
		contentContext->ET();

		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page\n";
			break;
		}

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;	

}

ADD_CETEGORIZED_TEST(SimpleTextUsage,"PDF")