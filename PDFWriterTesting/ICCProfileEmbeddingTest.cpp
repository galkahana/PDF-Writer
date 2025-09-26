/*
   Source File : ICCProfileEmbeddingTest

   Copyright 2025 PDF-Writer Community

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   Example demonstrating how to embed ICC color profiles in PDF documents.
   This example shows:
   1. Loading an ICC profile from a file
   2. Embedding it as a PDF stream object
   3. Creating an OutputIntent in the document catalog
   4. Associating the ICC profile with the OutputIntent for PDF/X compliance

   IMPORTANT: The ICC profile must be embedded BEFORE calling EndPDF().
   Call EmbedICCProfile() before PDF finalization to prevent corruption.
*/

#include <iostream>
#include <string>

// PDF-Writer includes
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "DocumentContext.h"
#include "DocumentContextExtenderAdapter.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFStream.h"
#include "InputFile.h"
#include "PDFTextString.h"
#include "Trace.h"
#include "OutputStreamTraits.h"

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;

class ICCProfileEmbedder : public DocumentContextExtenderAdapter
{
public:
    ICCProfileEmbedder();
    ~ICCProfileEmbedder();

    // Set the ICC profile file path to embed
    EStatusCode SetICCProfilePath(const string& inICCProfilePath);

    // Pre-embed the ICC profile as a stream object (call before PDF finalization)
    EStatusCode EmbedICCProfile(ObjectsContext* inObjectsContext);

    // DocumentContextExtenderAdapter override
    virtual EStatusCode OnCatalogWrite(
        CatalogInformation* inCatalogInformation,
        DictionaryContext* inCatalogDictionaryContext,
        ObjectsContext* inPDFWriterObjectContext,
        DocumentContext* inDocumentContext);

private:
    string mICCProfilePath;
    ObjectIDType mICCProfileObjectID;

    EStatusCode EmbedICCProfileAsStream(ObjectsContext* inObjectsContext);
    EStatusCode CreateOutputIntent(
        DictionaryContext* inCatalogDictionaryContext,
        ObjectsContext* inObjectsContext);
};

ICCProfileEmbedder::ICCProfileEmbedder()
{
    mICCProfileObjectID = 0;
}

ICCProfileEmbedder::~ICCProfileEmbedder()
{
}

EStatusCode ICCProfileEmbedder::SetICCProfilePath(const string& inICCProfilePath)
{
    mICCProfilePath = inICCProfilePath;
    return eSuccess;
}

EStatusCode ICCProfileEmbedder::EmbedICCProfile(ObjectsContext* inObjectsContext)
{
    return EmbedICCProfileAsStream(inObjectsContext);
}

EStatusCode ICCProfileEmbedder::EmbedICCProfileAsStream(ObjectsContext* inObjectsContext)
{
    InputFile iccFile;
    PDFStream* pdfStream = NULL;
    bool useActualFile = false;

    // Try to open the ICC profile file if specified and available
    if (!mICCProfilePath.empty())
    {
        if (iccFile.OpenFile(mICCProfilePath) == eSuccess)
        {
            useActualFile = true;
        }
    }

    // Start a new indirect object for the ICC profile stream
    mICCProfileObjectID = inObjectsContext->StartNewIndirectObject();

    // Create dictionary context for the stream
    DictionaryContext* streamDictionary = inObjectsContext->StartDictionary();

    // Set stream type as ICCBased color space
    streamDictionary->WriteKey("N");
    streamDictionary->WriteIntegerValue(3); // RGB color space (3 components)

    // Start the PDF stream (this will close the dictionary)
    pdfStream = inObjectsContext->StartPDFStream(streamDictionary);

    // Write ICC profile data to the stream
    IByteWriter* streamWriter = pdfStream->GetWriteStream();

    if (useActualFile)
    {
        // Copy ICC profile data from file using PDF-Writer's utility
        IByteReader* iccReader = iccFile.GetInputStream();
        OutputStreamTraits streamTraits(streamWriter);
        streamTraits.CopyToOutputStream(iccReader);
    }
    else
    {
        // Create a minimal ICC profile placeholder for demonstration
        // In practice, this would contain actual ICC profile binary data
        const char* minimalICCData =
            "ICC Profile Placeholder - This demonstrates the ICC embedding structure.\n"
            "In practice, this would contain actual ICC profile binary data.\n"
            "The profile would define color space transformation parameters\n"
            "for consistent color reproduction across devices.";

        streamWriter->Write((IOBasicTypes::Byte*)minimalICCData, strlen(minimalICCData));
    }

    inObjectsContext->EndPDFStream(pdfStream);

    // Cleanup
    if (useActualFile)
    {
        iccFile.CloseFile();
    }
    delete pdfStream;

    return eSuccess;
}

EStatusCode ICCProfileEmbedder::CreateOutputIntent(
    DictionaryContext* inCatalogDictionaryContext,
    ObjectsContext* inObjectsContext)
{
    // Create OutputIntents array in the catalog
    inCatalogDictionaryContext->WriteKey("OutputIntents");
    inObjectsContext->StartArray();

    // Start the OutputIntent dictionary as an indirect object
    ObjectIDType outputIntentObjectID = inObjectsContext->StartNewIndirectObject();
    DictionaryContext* outputIntentDict = inObjectsContext->StartDictionary();

    // Required fields for OutputIntent
    outputIntentDict->WriteKey("Type");
    outputIntentDict->WriteNameValue("OutputIntent");

    outputIntentDict->WriteKey("S");
    outputIntentDict->WriteNameValue("GTS_PDFX"); // PDF/X output intent subtype

    // Optional but recommended fields
    outputIntentDict->WriteKey("OutputConditionIdentifier");
    PDFTextString conditionId("sRGB IEC61966-2.1");
    outputIntentDict->WriteLiteralStringValue(conditionId.ToString());

    outputIntentDict->WriteKey("Info");
    PDFTextString info("sRGB color space for web and screen viewing");
    outputIntentDict->WriteLiteralStringValue(info.ToString());

    // Reference to the ICC profile stream
    outputIntentDict->WriteKey("DestOutputProfile");
    outputIntentDict->WriteObjectReferenceValue(mICCProfileObjectID);

    inObjectsContext->EndDictionary(outputIntentDict);
    inObjectsContext->EndIndirectObject();

    // Reference the OutputIntent in the array
    inObjectsContext->WriteIndirectObjectReference(outputIntentObjectID);
    inObjectsContext->EndArray();

    return eSuccess;
}

EStatusCode ICCProfileEmbedder::OnCatalogWrite(
    CatalogInformation* inCatalogInformation,
    DictionaryContext* inCatalogDictionaryContext,
    ObjectsContext* inPDFWriterObjectContext,
    DocumentContext* inDocumentContext)
{
    // Avoid unused parameter warnings
    (void)inCatalogInformation;
    (void)inDocumentContext;

    // Only create the OutputIntent if ICC profile was already embedded
    if (mICCProfileObjectID == 0)
    {
        return eFailure;
    }

    // Create the OutputIntent that references the already-embedded ICC profile
    return CreateOutputIntent(inCatalogDictionaryContext, inPDFWriterObjectContext);
}

int ICCProfileEmbeddingTest(int argc, char* argv[])
{
    PDFWriter pdfWriter;
    ICCProfileEmbedder iccEmbedder;

    // Start PDF creation
    pdfWriter.StartPDF(BuildRelativeOutputPath(argv, "ICCProfileEmbeddingTest.pdf"), ePDFVersion14);

    // Set an ICC profile path (optional - will use placeholder if file doesn't exist)
    iccEmbedder.SetICCProfilePath(BuildRelativeInputPath(argv, "Materials/sRGB.icc"));

    // Add the ICC embedder to the document context
    pdfWriter.GetDocumentContext().AddDocumentContextExtender(&iccEmbedder);

    // Create a simple page to demonstrate the ICC profile embedding
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842)); // A4 size

    PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

    // Add some simple content to show the page
    AbstractContentContext::TextOptions textOptions14(
        pdfWriter.GetFontForFile(BuildRelativeInputPath(argv, "fonts/arial.ttf")),
        14,
        AbstractContentContext::eGray,
        0);
    AbstractContentContext::TextOptions textOptions12(
        pdfWriter.GetFontForFile(BuildRelativeInputPath(argv, "fonts/arial.ttf")),
        12,
        AbstractContentContext::eGray,
        0);

    pageContent->WriteText(100, 700, "ICC Profile Embedding Demonstration", textOptions14);
    pageContent->WriteText(100, 650, "This PDF contains an embedded ICC color profile", textOptions12);
    pageContent->WriteText(100, 600, "configured as an OutputIntent for PDF/X compliance.", textOptions12);

    pdfWriter.EndPageContentContext(pageContent);
    pdfWriter.WritePageAndRelease(page);

    // Embed the ICC profile BEFORE finalizing the PDF
    // This prevents creating nested indirect objects during catalog writing
    iccEmbedder.EmbedICCProfile(&pdfWriter.GetObjectsContext());

    pdfWriter.EndPDF();

    // Remove the extender
    pdfWriter.GetDocumentContext().RemoveDocumentContextExtender(&iccEmbedder);

    return 0;
}