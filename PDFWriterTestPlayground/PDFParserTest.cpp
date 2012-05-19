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
using namespace PDFHummus;

PDFParserTest::PDFParserTest(void)
{
}

PDFParserTest::~PDFParserTest(void)
{
}

EStatusCode PDFParserTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;
	InputFile pdfFile;
	PDFParser parser;
	OutputFile outputFile;

	do
	{
		status = pdfFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/XObjectContent.PDF"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to open file for reading. should be in TestMaterials/XObjectContent.PDF\n";
			break;
		}

		status = parser.StartPDFParsing(pdfFile.GetInputStream());
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to parse input file";
			break;
		}

		// now let's do something with what got parsed

		if(parser.GetPDFLevel() != 1.3)
		{
			cout<<"expecting level 1.3, got "<<parser.GetPDFLevel()<<"\n";
			status = PDFHummus::eFailure;
			break;
		}

		if(parser.GetPagesCount() != 2)
		{
			cout<<"expecting 2 pages, got "<<parser.GetPagesCount()<<"\n";
			status = PDFHummus::eFailure;
			break;
		}

		// now just iterate, and print the object types
		PDFObjectCastPtr<PDFDictionary> catalog(parser.QueryDictionaryObject(parser.GetTrailer(),"Root"));
		if(!catalog)
		{
			cout<<"Can't find catalog. fail\n";
			status = PDFHummus::eFailure;
			break;
		}

		mTabLevel = 0;
		status = outputFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"PDFParserTestOutput.txt"));

		status = IterateObjectTypes(catalog.GetPtr(),parser,outputFile.GetOutputStream());
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed iterating object types\n";
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

	if(inObject->GetType() == PDFObject::ePDFObjectIndirectObjectReference)
	{
		inOutput->Write((const Byte*)scIndirectStart,strlen(scIndirectStart));
		if(mIteratedObjectIDs.find(((PDFIndirectObjectReference*)inObject)->mObjectID) == mIteratedObjectIDs.end())
		{
			mIteratedObjectIDs.insert(((PDFIndirectObjectReference*)inObject)->mObjectID);
			RefCountPtr<PDFObject> pointedObject(inParser.ParseNewObject(((PDFIndirectObjectReference*)inObject)->mObjectID));
			if(!pointedObject)
			{
				cout<<"\nFailed to retreive object of ID ="<<((PDFIndirectObjectReference*)inObject)->mObjectID<<"\n";
				return PDFHummus::eFailure;
			}
			return IterateObjectTypes(pointedObject.GetPtr(),inParser,inOutput);
		}
		else
		{
			for(int i=0;i<mTabLevel;++i)
				inOutput->Write((const Byte*)"  ",2);
			inOutput->Write((const Byte*)scParsedAlready,strlen(scParsedAlready));
			return PDFHummus::eSuccess;
		}
		
	}
	else if(inObject->GetType() == PDFObject::ePDFObjectArray)
	{
		primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel[inObject->GetType()]);
		++mTabLevel;
		PDFObjectCastPtr<PDFArray> anArray;
		anArray = inObject;  // do assignment here, otherwise it's considered constructor...which won't addref
		SingleValueContainerIterator<PDFObjectVector> it = anArray->GetIterator();
		EStatusCode status = PDFHummus::eSuccess;
		while(it.MoveNext() && PDFHummus::eSuccess == status)
			status = IterateObjectTypes(it.GetItem(),inParser,inOutput);
		--mTabLevel;
		return status;
	}
	else if(inObject->GetType() == PDFObject::ePDFObjectDictionary)
	{
		primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel[inObject->GetType()]);
		++mTabLevel;
		PDFObjectCastPtr<PDFDictionary> aDictionary;
		aDictionary = inObject; // do assignment here, otherwise it's considered constructor...which won't addref
		MapIterator<PDFNameToPDFObjectMap> it = aDictionary->GetIterator();

		EStatusCode status = PDFHummus::eSuccess;
		while(it.MoveNext() && PDFHummus::eSuccess == status)
		{
			status = IterateObjectTypes(it.GetKey(),inParser,inOutput);
			if(PDFHummus::eSuccess == status)
				status = IterateObjectTypes(it.GetValue(),inParser,inOutput);
		}
		--mTabLevel;
		return status;

	}
	else if(inObject->GetType() == PDFObject::ePDFObjectStream)
	{
		inOutput->Write((const Byte*)scIteratingStreamDict,strlen(scIteratingStreamDict));
		PDFObjectCastPtr<PDFDictionary> aDictionary(((PDFStreamInput*)inObject)->QueryStreamDictionary());
		return IterateObjectTypes(aDictionary.GetPtr(),inParser,inOutput);
	}
	else 
	{
		primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel[inObject->GetType()]);
		return PDFHummus::eSuccess;
	}
	
}

ADD_CATEGORIZED_TEST(PDFParserTest,"PDFEmbedding")
