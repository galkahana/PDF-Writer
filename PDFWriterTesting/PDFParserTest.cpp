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
#include "PDFParser.h"
#include "InputFile.h"
#include "InputStringStream.h"
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
#include "ObjectsBasicTypes.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <set>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

// Process-wide allocation counter, used only by StartPDFParsing_KidsAsIndirectReference_DoesNotLeak
// below to prove a parse-and-destroy cycle returns to net zero live allocations -- functional
// assertions alone (page count, successful parse) can't distinguish a leaked-but-otherwise-correct
// parse from a clean one.
static long gLiveAllocationCount = 0;

void* operator new(size_t inSize)
{
	void* result = malloc(inSize);
	if(!result)
		throw std::bad_alloc();
	++gLiveAllocationCount;
	return result;
}

void operator delete(void* inPointer) noexcept
{
	if(inPointer)
	{
		--gLiveAllocationCount;
		free(inPointer);
	}
}

void operator delete(void* inPointer, size_t) noexcept
{
	if(inPointer)
	{
		--gLiveAllocationCount;
		free(inPointer);
	}
}

static const char* scIndirectStart = "Indirect object reference:\r\n";
static const char* scParsedAlready = "was parsed already\r\n";
static const char* scIteratingStreamDict = "Stream . iterating stream dictionary:\r\n";

typedef set<ObjectIDType> ObjectIDTypeSet;

static EStatusCode IterateObjectTypes(PDFObject* inObject,PDFParser& inParser,IByteWriter* inOutput, int inTabLevel, ObjectIDTypeSet& ioIteratedObjectIDs)
{
	PrimitiveObjectsWriter primitivesWriter;
	int tabLevel = inTabLevel;

	primitivesWriter.SetStreamForWriting(inOutput);

	for(int i=0;i<tabLevel;++i)
		inOutput->Write((const Byte*)"  ",2);

	if(inObject->GetType() == PDFObject::ePDFObjectIndirectObjectReference)
	{
		inOutput->Write((const Byte*)scIndirectStart,strlen(scIndirectStart));
		if(ioIteratedObjectIDs.find(((PDFIndirectObjectReference*)inObject)->mObjectID) == ioIteratedObjectIDs.end())
		{
			ioIteratedObjectIDs.insert(((PDFIndirectObjectReference*)inObject)->mObjectID);
			RefCountPtr<PDFObject> pointedObject(inParser.ParseNewObject(((PDFIndirectObjectReference*)inObject)->mObjectID));
			if(!pointedObject)
			{
				cout<<"\nFailed to retreive object of ID ="<<((PDFIndirectObjectReference*)inObject)->mObjectID<<"\n";
				return PDFHummus::eFailure;
			}
			return IterateObjectTypes(pointedObject.GetPtr(),inParser,inOutput, tabLevel, ioIteratedObjectIDs);
		}
		else
		{
			for(int i=0;i<tabLevel;++i)
				inOutput->Write((const Byte*)"  ",2);
			inOutput->Write((const Byte*)scParsedAlready,strlen(scParsedAlready));
			return PDFHummus::eSuccess;
		}
		
	}
	else if(inObject->GetType() == PDFObject::ePDFObjectArray)
	{
		primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
		++tabLevel;
		PDFObjectCastPtr<PDFArray> anArray;
		anArray = inObject;  // do assignment here, otherwise it's considered constructor...which won't addref
		SingleValueContainerIterator<PDFObjectVector> it = anArray->GetIterator();
		EStatusCode status = PDFHummus::eSuccess;
		while(it.MoveNext() && PDFHummus::eSuccess == status)
			status = IterateObjectTypes(it.GetItem(),inParser,inOutput, tabLevel, ioIteratedObjectIDs);
		--tabLevel;
		return status;
	}
	else if(inObject->GetType() == PDFObject::ePDFObjectDictionary)
	{
		primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
		++tabLevel;
		PDFObjectCastPtr<PDFDictionary> aDictionary;
		aDictionary = inObject; // do assignment here, otherwise it's considered constructor...which won't addref
		MapIterator<PDFNameToPDFObjectMap> it = aDictionary->GetIterator();

		EStatusCode status = PDFHummus::eSuccess;
		while(it.MoveNext() && PDFHummus::eSuccess == status)
		{
			status = IterateObjectTypes(it.GetKey(),inParser,inOutput, tabLevel, ioIteratedObjectIDs);
			if(PDFHummus::eSuccess == status)
				status = IterateObjectTypes(it.GetValue(),inParser,inOutput, tabLevel, ioIteratedObjectIDs);
		}
		--tabLevel;
		return status;

	}
	else if(inObject->GetType() == PDFObject::ePDFObjectStream)
	{
		inOutput->Write((const Byte*)scIteratingStreamDict,strlen(scIteratingStreamDict));
		PDFObjectCastPtr<PDFDictionary> aDictionary(((PDFStreamInput*)inObject)->QueryStreamDictionary());
		return IterateObjectTypes(aDictionary.GetPtr(),inParser,inOutput, tabLevel, ioIteratedObjectIDs);
	}
	else 
	{
		primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
		return PDFHummus::eSuccess;
	}
	
}

// Minimal, valid PDF where the Pages node's /Kids is given as an indirect
// reference ("/Kids 4 0 R") rather than an inline array -- a common, fully
// legal shape that ParsePagesIDs must resolve via PDFParser::QueryDictionaryObject.
static string BuildPDFWithIndirectKids()
{
	string header = "%PDF-1.4\n";
	string obj1 = "1 0 obj\n<< /Type /Page /Parent 2 0 R /MediaBox [ 0 0 100 100 ] /Resources << >> >>\nendobj\n";
	string obj2 = "2 0 obj\n<< /Type /Pages /Count 1 /Kids 4 0 R >>\nendobj\n";
	string obj3 = "3 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";
	string obj4 = "4 0 obj\n[ 1 0 R ]\nendobj\n";

	size_t offsets[4];
	offsets[0] = header.size();
	offsets[1] = offsets[0] + obj1.size();
	offsets[2] = offsets[1] + obj2.size();
	offsets[3] = offsets[2] + obj3.size();
	size_t xrefStart = offsets[3] + obj4.size();

	string xref = "xref\n0 5\n0000000000 65535 f \n";
	char xrefEntry[32];
	for(int i = 0; i < 4; ++i)
	{
		snprintf(xrefEntry, sizeof(xrefEntry), "%010lu 00000 n \n", (unsigned long)offsets[i]);
		xref += xrefEntry;
	}

	char trailer[128];
	snprintf(trailer, sizeof(trailer), "trailer\n<< /Size 5 /Root 3 0 R >>\nstartxref\n%lu\n%%%%EOF", (unsigned long)xrefStart);

	return header + obj1 + obj2 + obj3 + obj4 + xref + trailer;
}

static bool StartPDFParsing_KidsAsIndirectReference_DoesNotLeak()
{
	// Arrange
	string pdfBytes = BuildPDFWithIndirectKids();
	long before = gLiveAllocationCount;

	// Act
	{
		InputStringStream inputStream(pdfBytes);
		PDFParser parser;
		EStatusCode status = parser.StartPDFParsing(&inputStream);
		if(status != PDFHummus::eSuccess)
		{
			cout << "StartPDFParsing_KidsAsIndirectReference_DoesNotLeak: failed to parse\n";
			return false;
		}
		if(parser.GetPagesCount() != 1)
		{
			cout << "StartPDFParsing_KidsAsIndirectReference_DoesNotLeak: expected 1 page, got " << parser.GetPagesCount() << "\n";
			return false;
		}
	}
	long after = gLiveAllocationCount;

	// Assert
	if(after != before)
	{
		cout << "StartPDFParsing_KidsAsIndirectReference_DoesNotLeak: leaked "
		     << (after - before) << " allocation(s) resolving an indirect /Kids reference\n";
		return false;
	}
	return true;
}

int PDFParserTest(int argc, char* argv[])
{
	EStatusCode status = PDFHummus::eSuccess;
	InputFile pdfFile;
	PDFParser parser;
	OutputFile outputFile;

	do
	{
		status = pdfFile.OpenFile(BuildRelativeInputPath(argv, "XObjectContent.pdf"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to open file for reading. should be in TestMaterials/XObjectContent.pdf\n";
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

		status = outputFile.OpenFile(BuildRelativeOutputPath(argv,"PDFParserTestOutput.txt"));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"unable to open file writing log output\n";
			break;
		}

		ObjectIDTypeSet iteratedObjectIDs;
		status = IterateObjectTypes(catalog.GetPtr(),parser,outputFile.GetOutputStream(), 0, iteratedObjectIDs);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed iterating object types\n";
			break;
		}

	}while(false);

	if(status == PDFHummus::eSuccess && !StartPDFParsing_KidsAsIndirectReference_DoesNotLeak())
		status = PDFHummus::eFailure;

	return status;
}


