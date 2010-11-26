#include "TrueTypeTest.h"
#include "OpenTypeFileInput.h"
#include "InputFile.h"
#include "TestsRunner.h"

#include <iostream>

TrueTypeTest::TrueTypeTest(void)
{
}

TrueTypeTest::~TrueTypeTest(void)
{
}

/*#include "OutputFile.h"
#include "OutputFlateDecodeStream.h"*/
EStatusCode TrueTypeTest::Run()
{
	EStatusCode status;
	InputFile ttfFile;
/*	Byte buffer[12139];
	
	IByteReader* fileStream;

	status = ttfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\TrueTypeEmbeddedSubsetArialMT.pdf");
	fileStream = ttfFile.GetInputStream();
	fileStream->SetPosition(10025 + 67);
	fileStream->Read(buffer,12139);

	ttfFile.CloseFile();

	OutputFile trueTypeEncoded;
	trueTypeEncoded.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\TrueTypeEmbeddedSubsetArialMTSegment.ttf");

	IByteWriterWithPosition* outputStream = trueTypeEncoded.GetOutputStream();
	OutputFlateDecodeStream flateDecodeStream(outputStream);

	flateDecodeStream.Write(buffer,12139);
	flateDecodeStream.Assign(NULL);

	trueTypeEncoded.CloseFile();
*/

	do
	{
		//status = ttfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\TrueTypeEmbeddedSubsetArialMTSegment.ttf");
		status = ttfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\ARIALMT_.TTF");
		//status = ttfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\TrueTypeSegment.pdf");
		if(status != eSuccess)
		{
			wcout<<"cannot read arial font file\n";
			break;
		}

		OpenTypeFileInput trueTypeReader;

		status = trueTypeReader.ReadOpenTypeFile(ttfFile.GetInputStream());
		if(status != eSuccess)
		{
			wcout<<"could not read true type file\n";
			break;
		}
	}while(false);

	return status;
}

ADD_CETEGORIZED_TEST(TrueTypeTest,"TrueType")