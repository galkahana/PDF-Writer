#include "PDFWriter/PageContentContext.h"
#include "PDFWriter/PDFPage.h"
#include "PDFWriter/PDFWriter.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	PDFWriter pdfWriter;
	pdfWriter.StartPDF("test.pdf", ePDFVersionMax);

	PDFUsedFont* font1 = pdfWriter.GetFontForFile("/Users/da/Library/Fonts/NotoSerifCJK-Regular.ttc", 2);

	AbstractContentContext::TextOptions textOptions1(font1, 20, AbstractContentContext::eGray, 0);

	PDFPage* page = new PDFPage();
	page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
	PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);

	const char* test = "默";
	contentContext->WriteText(50, 750, test, textOptions1);

	pdfWriter.EndPageContentContext(contentContext);
	pdfWriter.WritePage(page);
	delete page;

	std::cout << "before end page" << "\n";
	pdfWriter.EndPDF();
	return 0;
}
