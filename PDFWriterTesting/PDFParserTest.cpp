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
#include "InputByteArrayStream.h"

#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <stdio.h>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

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

static EStatusCode ParseXObjectContentAndIterateObjects(char* argv[])
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

	return status;
}

// ---- Synthetic PDFs for xref sizing -----------------------------------------
//
// Both builders produce a valid PDF whose catalog (object 1) and pages
// (object 2) are listed in the first xref segment, plus one caller-supplied
// extra segment whose entries all point at the catalog. The trailer /Size is
// caller-supplied too. Only the extra segment and /Size vary between cases.

static const char* scCatalogObject = "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";
static const char* scPagesObject = "2 0 obj\n<< /Type /Pages /Kids [] /Count 0 >>\nendobj\n";
static const LongFilePositionType scCatalogPosition = 9; // right after "%PDF-1.x\n"

// Highest object number the parser accepts, and the xref size it implies
static const ObjectIDType scHighestObjectNumber = 8388607;
static const ObjectIDType scMaxXrefSize = 8388608;

static void AppendXrefStreamEntry(string& ioData, unsigned char inType, unsigned long inOffset, unsigned int inGeneration)
{
	// W [1 4 2]
	ioData.push_back((char)inType);
	ioData.push_back((char)((inOffset >> 24) & 0xff));
	ioData.push_back((char)((inOffset >> 16) & 0xff));
	ioData.push_back((char)((inOffset >> 8) & 0xff));
	ioData.push_back((char)(inOffset & 0xff));
	ioData.push_back((char)((inGeneration >> 8) & 0xff));
	ioData.push_back((char)(inGeneration & 0xff));
}

// Xref stream (object 3) with /Index [0 4 <inExtraSegment>], where
// inExtraSegment is "<start> <count>" and inExtraEntries is the number of
// entries written for it.
static string BuildPDFWithXrefStream(const char* inTrailerSize, const char* inExtraSegment, unsigned long inExtraEntries)
{
	string pdf = "%PDF-1.7\n";
	unsigned long catalogPosition = (unsigned long)pdf.size();
	pdf += scCatalogObject;
	unsigned long pagesPosition = (unsigned long)pdf.size();
	pdf += scPagesObject;
	unsigned long xrefPosition = (unsigned long)pdf.size();

	string data;
	AppendXrefStreamEntry(data, 0, 0, 65535);
	AppendXrefStreamEntry(data, 1, catalogPosition, 0);
	AppendXrefStreamEntry(data, 1, pagesPosition, 0);
	AppendXrefStreamEntry(data, 1, xrefPosition, 0);
	for(unsigned long i = 0; i < inExtraEntries; ++i)
		AppendXrefStreamEntry(data, 1, catalogPosition, 0);

	ostringstream xrefObject;
	xrefObject << "3 0 obj\n<< /Type /XRef /Size " << inTrailerSize
	           << " /Root 1 0 R /W [1 4 2] /Index [0 4 " << inExtraSegment
	           << "] /Length " << data.size() << " >>\nstream\n";
	pdf += xrefObject.str();
	pdf += data;
	pdf += "\nendstream\nendobj\n";

	ostringstream tail;
	tail << "startxref\n" << xrefPosition << "\n%%EOF\n";
	pdf += tail.str();
	return pdf;
}

static string FormatXrefTableEntry(unsigned long inOffset, unsigned int inGeneration, char inType)
{
	char entry[21];
	snprintf(entry, sizeof(entry), "%010lu %05u %c\r\n", inOffset, inGeneration, inType);
	return string(entry, 20);
}

// Classic xref table with subsection "0 3" followed by the subsection header
// inExtraSegment ("<start> <count>") and inExtraEntries entries for it.
static string BuildPDFWithXrefTable(const char* inTrailerSize, const char* inExtraSegment, unsigned long inExtraEntries)
{
	string pdf = "%PDF-1.4\n";
	unsigned long catalogPosition = (unsigned long)pdf.size();
	pdf += scCatalogObject;
	unsigned long pagesPosition = (unsigned long)pdf.size();
	pdf += scPagesObject;
	unsigned long xrefPosition = (unsigned long)pdf.size();

	pdf += "xref\n0 3\n";
	pdf += FormatXrefTableEntry(0, 65535, 'f');
	pdf += FormatXrefTableEntry(catalogPosition, 0, 'n');
	pdf += FormatXrefTableEntry(pagesPosition, 0, 'n');
	pdf += inExtraSegment;
	pdf += "\n";
	for(unsigned long i = 0; i < inExtraEntries; ++i)
		pdf += FormatXrefTableEntry(catalogPosition, 0, 'n');

	ostringstream tail;
	tail << "trailer\n<< /Size " << inTrailerSize << " /Root 1 0 R >>\nstartxref\n" << xrefPosition << "\n%%EOF\n";
	pdf += tail.str();
	return pdf;
}

typedef string (*BuildPDFFunction)(const char* inTrailerSize, const char* inExtraSegment, unsigned long inExtraEntries);

struct XrefSegmentCase
{
	const char* mLabel;
	const char* mTrailerSize;
	const char* mExtraSegment;
	unsigned long mExtraEntries;
};

static bool ExpectAllParseFailures(const char* inFunction, BuildPDFFunction inBuilder, const XrefSegmentCase* inCases, size_t inCount)
{
	for(size_t i = 0; i < inCount; ++i)
	{
		// Arrange
		string pdf = inBuilder(inCases[i].mTrailerSize, inCases[i].mExtraSegment, inCases[i].mExtraEntries);
		InputByteArrayStream stream((const IOBasicTypes::Byte*)pdf.data(), (LongFilePositionType)pdf.size());
		PDFParser parser;

		// Act
		EStatusCode status = parser.StartPDFParsing(&stream);

		// Assert
		if(status != PDFHummus::eFailure)
		{
			cout << "PDFParserTest [" << inFunction << "::" << inCases[i].mLabel << "]: expected parsing failure, got success with xref size " << parser.GetXrefSize() << "\n";
			return false;
		}
	}
	return true;
}

static bool ExpectHighestObjectNumberAccepted(const char* inFunction, const char* inLabel, BuildPDFFunction inBuilder)
{
	// Arrange
	string pdf = inBuilder("4", "8388607 1", 1);
	InputByteArrayStream stream((const IOBasicTypes::Byte*)pdf.data(), (LongFilePositionType)pdf.size());
	PDFParser parser;

	// Act
	EStatusCode status = parser.StartPDFParsing(&stream);

	// Assert
	if(status != PDFHummus::eSuccess)
	{
		cout << "PDFParserTest [" << inFunction << "::" << inLabel << "]: expected parsing success\n";
		return false;
	}
	if(parser.GetXrefSize() != scMaxXrefSize)
	{
		cout << "PDFParserTest [" << inFunction << "::" << inLabel << "]: expected xref size " << scMaxXrefSize << ", got " << parser.GetXrefSize() << "\n";
		return false;
	}
	XrefEntryInput* entry = parser.GetXrefEntry(scHighestObjectNumber);
	if(NULL == entry || entry->mType != eXrefEntryExisting || entry->mObjectPosition != scCatalogPosition)
	{
		cout << "PDFParserTest [" << inFunction << "::" << inLabel << "]: expected object " << scHighestObjectNumber << " to be an in-use entry at position " << scCatalogPosition << "\n";
		return false;
	}
	return true;
}

// Rows with a trailer /Size covering the extra segment show that the bound
// holds regardless of the trailer's declared size.
static const XrefSegmentCase scParseXrefFromXrefStreamCases[] = {
	{"SegmentStartBeyondTrailerSize_ReturnsFailure", "4", "2147483648 2", 2},
	{"SegmentStartWithinTrailerSize_ReturnsFailure", "2147483650", "2147483648 2", 2},
	{"SegmentPastHighestObjectNumber_ReturnsFailure", "4", "8388608 1", 1},
	{"SegmentOverflowsObjectIDSpace_ReturnsFailure", "4", "-1 2", 2},
};

static bool RunParseXrefFromXrefStreamCases()
{
	if(!ExpectAllParseFailures("ParseXrefFromXrefStream",
	                           BuildPDFWithXrefStream,
	                           scParseXrefFromXrefStreamCases,
	                           sizeof(scParseXrefFromXrefStreamCases) / sizeof(scParseXrefFromXrefStreamCases[0])))
		return false;
	return ExpectHighestObjectNumberAccepted("ParseXrefFromXrefStream", "SegmentAtHighestObjectNumber_ReturnsSuccess", BuildPDFWithXrefStream);
}

static const XrefSegmentCase scParseXrefFromXrefTableCases[] = {
	{"SubsectionStartBeyondTrailerSize_ReturnsFailure", "3", "2147483648 1", 1},
	{"SubsectionStartWithinTrailerSize_ReturnsFailure", "2147483649", "2147483648 1", 1},
	{"SubsectionPastHighestObjectNumber_ReturnsFailure", "3", "8388608 1", 1},
};

static bool RunParseXrefFromXrefTableCases()
{
	if(!ExpectAllParseFailures("ParseXrefFromXrefTable",
	                           BuildPDFWithXrefTable,
	                           scParseXrefFromXrefTableCases,
	                           sizeof(scParseXrefFromXrefTableCases) / sizeof(scParseXrefFromXrefTableCases[0])))
		return false;
	return ExpectHighestObjectNumberAccepted("ParseXrefFromXrefTable", "SubsectionAtHighestObjectNumber_ReturnsSuccess", BuildPDFWithXrefTable);
}

int PDFParserTest(int argc, char* argv[])
{
	if(ParseXObjectContentAndIterateObjects(argv) != PDFHummus::eSuccess) return 1;
	if(!RunParseXrefFromXrefStreamCases()) return 1;
	if(!RunParseXrefFromXrefTableCases()) return 1;
	return 0;
}


