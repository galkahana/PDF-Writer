#include "PDFParser.h"
#include "InputFile.h"
#include "PDFDocumentCopyingContext.h"
#include "PDFWriter.h"
#include <iostream>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

int EncryptedPDF(int argc, char* argv[])
{
	EStatusCode status = PDFHummus::eSuccess;
	InputFile pdfFile;
	PDFWriter pdfWriter;
	PDFDocumentCopyingContext* copyingContext = NULL;


	PDFParser parser;

	do
	{
		status = pdfWriter.StartPDF(
			BuildRelativeOutputPath(argv, "PDFWithPasswordDecrypted.pdf"), ePDFVersion13,
			LogConfiguration(true, true,
				BuildRelativeOutputPath(argv, "PDFWithPasswordDecrypted.txt")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		copyingContext = pdfWriter.CreatePDFCopyingContext(
			BuildRelativeInputPath(argv,"PDFWithPassword.pdf"),
			PDFParsingOptions("user"));
		if (!copyingContext)
		{
			cout << "failed to initialize copying context from PDFWithPassword.pdf\n";
			status = PDFHummus::eFailure;
			break;
		}

		unsigned long pagesCount = copyingContext->GetSourceDocumentParser()->GetPagesCount();
		for (unsigned long i = 0; i < pagesCount && status == PDFHummus::eSuccess; ++i) {
			EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(i);
			if (result.first != PDFHummus::eSuccess)
			{
				cout << "failed to append page "<<(i+1)<<" from PDFWithPassword.pdf\n";
				status = result.first;
				break;
			}

		}

		delete copyingContext;
		copyingContext = NULL;

		status = pdfWriter.EndPDF();
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed in end PDF\n";
			break;
		}

	} while (false);

	delete copyingContext;
	return status == eSuccess ? 0:1;
}
