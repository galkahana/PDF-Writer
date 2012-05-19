#include "CustomLogTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "OutputFlateEncodeStream.h"
#include "OutputFlateDecodeStream.h"
#include "OutputStreamTraits.h"
#include "OutputFile.h"
#include "OutputStringBufferStream.h"
#include "PDFPage.h"
#include "Trace.h"
#include "IOBasicTypes.h"

#include <string>
using namespace std;
using namespace IOBasicTypes;
using namespace PDFHummus;

CustomLogTest::CustomLogTest(void)
{
}

CustomLogTest::~CustomLogTest(void)
{
}

EStatusCode CustomLogTest::Run(const TestConfiguration& inTestConfiguration)
{
	// Place log in a compressed stream, for a non-file PDF
	EStatusCode status;
	OutputFlateEncodeStream flateEncodeStream;
	OutputFlateDecodeStream flateDecodeStream;

	do
	{
		PDFWriter pdfWriter;
		OutputFile compressedLogFile;
		OutputStringBufferStream pdfStream;
	
		// setup log file with compression
		status = compressedLogFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"CustomLogEncrypted.txt"));
		if(status != PDFHummus::eSuccess)
			break;
		flateEncodeStream.Assign(compressedLogFile.GetOutputStream());
		
		// generate PDF
		TRACE_LOG("Starting PDF File Writing");
		status = pdfWriter.StartPDFForStream(&pdfStream,ePDFVersion13,LogConfiguration(true,&flateEncodeStream));
		if(status != PDFHummus::eSuccess)
			break;
		TRACE_LOG("Now will add an empty page");
		PDFPage* page = new PDFPage();

		page->SetMediaBox(PDFRectangle(0,0,400,400));
		
		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
			break;

		TRACE_LOG("Added page, now will close");

		status = pdfWriter.EndPDFForStream();
		if(status != PDFHummus::eSuccess)
			break;

		// since log was started by starting PDF...the ending resets it. so let's now begin again
		Singleton<Trace>::GetInstance()->SetLogSettings(&flateEncodeStream,true);
		TRACE_LOG("Finished PDF!!!1");

		// dump PDF to a file, so we can review it
		OutputFile pdfFile;
		status = pdfFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"DumpPDFFile.pdf"));
		if(status != PDFHummus::eSuccess)
			break;

		string pdfString = pdfStream.ToString();
		pdfFile.GetOutputStream()->Write((const Byte*)pdfString.c_str(),pdfString.size());
		pdfFile.CloseFile();

		TRACE_LOG("PDF stream dumped");
		
		// now finalize trace compressed file
		flateEncodeStream.Assign(NULL);
		compressedLogFile.CloseFile();

		// Finish log
		Singleton<Trace>::Reset();


		// now open a new file and decompress the log into it.
		OutputFile decryptedLogFile;

		status = decryptedLogFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"CustomLogDecrypted.txt"));
		if(status != PDFHummus::eSuccess)
			break;


		// place an initial bom (cause the compressed content is unicode)
		unsigned short bom = (0xFE<<8) + 0xFF;
		decryptedLogFile.GetOutputStream()->Write((const Byte*)&bom,2);	

		flateDecodeStream.Assign(decryptedLogFile.GetOutputStream());
		OutputStreamTraits traits(&flateDecodeStream);

		InputFile compressedLogFileInput;
		status = compressedLogFileInput.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"CustomLogEncrypted.txt"));
		if(status != PDFHummus::eSuccess)
			break;

		status = traits.CopyToOutputStream(compressedLogFileInput.GetInputStream());
		if(status != PDFHummus::eSuccess)
			break;

		compressedLogFileInput.CloseFile();
		flateDecodeStream.Assign(NULL);
		decryptedLogFile.CloseFile();
		
	}while(false);

	if(status != PDFHummus::eSuccess)
	{
		// cancel ownership of subsstreams
		flateDecodeStream.Assign(NULL);
		flateEncodeStream.Assign(NULL);
	}

	return status;
}

ADD_CATEGORIZED_TEST(CustomLogTest,"CustomStreams")
