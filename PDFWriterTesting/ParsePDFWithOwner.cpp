#include "PDFParser.h"
#include "EStatusCode.h"
#include "InputFile.h"
#include "RefCountPtr.h"
#include "PDFDictionary.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace PDFHummus;

int ParsePDFWithOwner(int argc, char* argv[])
{
    int pIndex = 0;
	EStatusCode status; 
    PDFParser parser;

	do
	{
        InputFile pdfFile;
		status = pdfFile.OpenFile(BuildRelativeInputPath(argv, "password.pdf"));
		if(status != eSuccess)
		{
			cout<<"unable to open file for reading. should be in TestMaterials/password.pdf\n";
			break;
		}

		status = parser.StartPDFParsing(pdfFile.GetInputStream(), PDFParsingOptions("111111"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to parse input file";
			break;
		}        

        if(!parser.IsEncryptionSupported()) {
            cout<<"PDFParser does not support encryption, cannot continue\n";
            status = PDFHummus::eFailure;
            break;
        }

        RefCountPtr<PDFDictionary> pageDict(parser.ParsePage(pIndex));
        if(pageDict.GetPtr() == NULL)
        {
            cout<<"unable to parse page "<<pIndex<<"\n";
            status = PDFHummus::eFailure;
            break;
        }
    }while(false);


    return status == eSuccess ? 0:1;


}