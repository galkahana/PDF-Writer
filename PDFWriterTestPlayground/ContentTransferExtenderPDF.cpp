/*
   Source File : ContentTransferExtenderPDF.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#include "ContentTransferExtenderPDF.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFDocumentCopyingContext.h"
#include "IPDFParserExtenderAdapter.h"
#include "OutputStreamTraits.h"
#include "ResourcesDictionary.h"
#include "WriterContentContext.h"
#include "IPDFPageContentHandlerPass.h"
#include "PDFPageContentParser.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

namespace {

class ContentColorizer
	: public IPDFParserExtenderAdapter
	, public IPDFPageContentHandlerPass<AbstractContentContext>
{
public:
	ContentColorizer()
		: m_next( NULL )
	{
	}

public:
	AbstractContentContext& next() { return *m_next; }

public: // IPDFPageContentHandler interface
	void TfLow( const std::string& font, double size )
	{
		if ( size < 10 )
			next().rg( 1, 0, 0 );
		else if ( size < 20 ) 
			next().rg( 0, 1, 0 );
		else
			next().rg( 0, 0, 1 );
	 	next().TfLow( font, size );
	}

public: // IPDFParserExtender interface
	bool DoesSupportContentTransfer() { return true; }

	bool TransferContent( IByteWriter* inStreamWriter, IByteReader* inStreamReader ) 
	{
		EStatusCode status = PDFHummus::eSuccess;
 
		ResourcesDictionary resources;
		WriterContentContext ctx( inStreamWriter, &resources );
		m_next = &ctx;
		bool succeeded = ParsePDFPageContent( *inStreamReader, *this );
		m_next = NULL;
		if ( !succeeded )
			cout << "ContentColorizer::TransferContent, failed to parse content stream\n";
		return succeeded;
	}

private:
	AbstractContentContext* m_next;
};

} // anonymous namespace

ContentTransferExtenderPDF::ContentTransferExtenderPDF(void)
{
}

ContentTransferExtenderPDF::~ContentTransferExtenderPDF(void)
{
}


EStatusCode ContentTransferExtenderPDF::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;
	PDFWriter pdfWriter;
	PDFDocumentCopyingContext* copyingContext = NULL;
	ContentColorizer colorizer;

	do
	{
		status = pdfWriter.StartPDF(
                        RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"PDFContentTransferExtenderTest.PDF"),ePDFVersion13,
                        LogConfiguration(true,true,
                        RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"PDFContentTransferExtenderTest.txt")));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		copyingContext = pdfWriter.CreatePDFCopyingContext(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/typography.pdf"));
		if(!copyingContext)
		{
			cout<<"failed to initialize copying context from typography.pdf\n";
			status = PDFHummus::eFailure;
			break;
		}

		copyingContext->GetSourceDocumentParser()->SetParserExtender( &colorizer );

		EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(0);
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append first page from typography.pdf\n";
			status = result.first;
			break;
		}

		copyingContext->GetSourceDocumentParser()->SetParserExtender( NULL );

		copyingContext->End(); // delete will call End() as well...so can avoid

		delete copyingContext;
		copyingContext = NULL;

		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}

	}while(false);

	delete copyingContext;
	return status;

}

ADD_CATEGORIZED_TEST(ContentTransferExtenderPDF,"PDF")
