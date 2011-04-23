/*
   Source File : PDFParserTest.cpp


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
#include "PDFParserTest.h"
#include "PDFParser.h"
#include "InputFile.h"
#include "PDFObject.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFIndirectObjectReference.h"
#include "PDFArray.h"
#include "PDFDictionary.h"
#include "PDFStreamInput.h"
#include "OutputFile.h"
#include "IByteWriterWithPosition.h"
#include "PrimitiveObjectsWriter.h"

#include <iostream>

using namespace std;

PDFParserTest::PDFParserTest(void)
{
}

PDFParserTest::~PDFParserTest(void)
{
}

EStatusCode PDFParserTest::Run()
{
	EStatusCode status = eSuccess;
	InputFile pdfFile;
	PDFParser parser;
	OutputFile outputFile;

	do
	{
		status = pdfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\XObjectContent.PDF");
		if(status != eSuccess)
		{
			wcout<<"unable to open file for reading. should be in C:\\PDFLibTests\\TestMaterials\\XObjectContent.PDF\n";
			break;
		}

		status = parser.StartPDFParsing(pdfFile.GetInputStream());
		if(status != eSuccess)
		{
			wcout<<"unable to parse input file";
			break;
		}

		// now let's do something with what got parsed

		if(parser.GetPDFLevel() != 1.3)
		{
			wcout<<"expecting level 1.3, got "<<parser.GetPDFLevel()<<"\n";
			status = eFailure;
			break;
		}

		if(parser.GetPagesCount() != 2)
		{
			wcout<<"expecting 2 pages, got "<<parser.GetPagesCount()<<"\n";
			status = eFailure;
			break;
		}

		// now just iterate, and print the object types
		PDFObjectCastPtr<PDFDictionary> catalog(parser.QueryDictionaryObject(parser.GetTrailer(),"Root"));
		if(!catalog)
		{
			wcout<<"Can't find catalog. fail\n";
			status = eFailure;
			break;
		}

		mTabLevel = 0;
		status = outputFile.OpenFile(L"C:\\PDFLibTests\\PDFParserTestOutput.txt");

		status = IterateObjectTypes(catalog.GetPtr(),parser,outputFile.GetOutputStream());
		if(status != eSuccess)
		{
			wcout<<"Failed iterating object types\n";
			break;
		}

	}while(false);

	return status;
}

static const char* scIndirectStart = "Indirect object reference:\r\n";
static const char* scParsedAlready = "was parsed already\r\n";
static const char* scIteratingStreamDict = "Stream . iterating stream dictionary:\r\n";

EStatusCode PDFParserTest::IterateObjectTypes(PDFObject* inObject,PDFParser& inParser,IByteWriter* inOutput)
{
	PrimitiveObjectsWriter primitivesWriter;

	primitivesWriter.SetStreamForWriting(inOutput);

	for(int i=0;i<mTabLevel;++i)
		inOutput->Write((const Byte*)"  ",2);

	if(inObject->GetType() == ePDFObjectIndirectObjectReference)
	{
		inOutput->Write((const Byte*)scIndirectStart,strlen(scIndirectStart));
		if(mIteratedObjectIDs.find(((PDFIndirectObjectReference*)inObject)->mObjectID) == mIteratedObjectIDs.end())
		{
			mIteratedObjectIDs.insert(((PDFIndirectObjectReference*)inObject)->mObjectID);
			RefCountPtr<PDFObject> pointedObject(inParser.ParseNewObject(((PDFIndirectObjectReference*)inObject)->mObjectID));
			if(!pointedObject)
			{
				wcout<<"\nFailed to retreive object of ID ="<<((PDFIndirectObjectReference*)inObject)->mObjectID<<"\n";
				return eFailure;
			}
			return IterateObjectTypes(pointedObject.GetPtr(),inParser,inOutput);
		}
		else
		{
			for(int i=0;i<mTabLevel;++i)
				inOutput->Write((const Byte*)"  ",2);
			inOutput->Write((const Byte*)scParsedAlready,strlen(scParsedAlready));
			return eSuccess;
		}
		
	}
	else if(inObject->GetType() == ePDFObjectArray)
	{
		primitivesWriter.WriteKeyword(scPDFObjectTypeLabel[inObject->GetType()]);
		++mTabLevel;
		PDFObjectCastPtr<PDFArray> anArray;
		anArray = inObject;  // do assignment here, otherwise it's considered constructor...which won't addref
		SingleValueContainerIterator<PDFObjectVector> it = anArray->GetIterator();
		EStatusCode status = eSuccess;
		while(it.MoveNext() && eSuccess == status)
			status = IterateObjectTypes(it.GetItem(),inParser,inOutput);
		--mTabLevel;
		return status;
	}
	else if(inObject->GetType() == ePDFObjectDictionary)
	{
		primitivesWriter.WriteKeyword(scPDFObjectTypeLabel[inObject->GetType()]);
		++mTabLevel;
		PDFObjectCastPtr<PDFDictionary> aDictionary;
		aDictionary = inObject; // do assignment here, otherwise it's considered constructor...which won't addref
		MapIterator<PDFNameToPDFObjectMap> it = aDictionary->GetIterator();

		EStatusCode status = eSuccess;
		while(it.MoveNext() && eSuccess == status)
		{
			status = IterateObjectTypes(it.GetKey(),inParser,inOutput);
			if(eSuccess == status)
				status = IterateObjectTypes(it.GetValue(),inParser,inOutput);
		}
		--mTabLevel;
		return status;

	}
	else if(inObject->GetType() == ePDFObjectStream)
	{
		inOutput->Write((const Byte*)scIteratingStreamDict,strlen(scIteratingStreamDict));
		PDFObjectCastPtr<PDFDictionary> aDictionary(((PDFStreamInput*)inObject)->QueryStreamDictionary());
		return IterateObjectTypes(aDictionary.GetPtr(),inParser,inOutput);
	}
	else 
	{
		primitivesWriter.WriteKeyword(scPDFObjectTypeLabel[inObject->GetType()]);
		return eSuccess;
	}
	
}

ADD_CATEGORIZED_TEST(PDFParserTest,"PDFEmbedding")
