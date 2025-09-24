/*
 Source File : PDFAddOutlinesToExistingTest.cpp


 Copyright 2025 Gal Kahana PDFWriter

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
#include "PDFWriter.h"
#include "PDFParser.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFDocumentCopyingContext.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFIndirectObjectReference.h"
#include "PDFPageInput.h"
#include "PDFRectangle.h"

#include <iostream>

#include "testing/TestIO.h"
#include "OutlineBuilder.h"

using namespace std;
using namespace PDFHummus;



int PDFAddOutlinesToExistingTest(int argc, char* argv[])
{
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;

    do
    {
        status = pdfWriter.ModifyPDF(
            BuildRelativeInputPath(argv, "BasicTIFFImagesTest.pdf"),
            ePDFVersion13,
            BuildRelativeOutputPath(argv, "PDFWithAddedOutlines.pdf"));
        if(status != eSuccess)
        {
            cout << "failed to start PDF modification\n";
            break;
        }

        PDFDocumentCopyingContext* copyingContext = pdfWriter.CreatePDFCopyingContextForModifiedFile();
        if(!copyingContext)
        {
            cout << "failed to create copying context for modified file\n";
            status = eFailure;
            break;
        }

        // Get page IDs from existing PDF
        PDFParser* parser = copyingContext->GetSourceDocumentParser();
        unsigned long pageCount = parser->GetPagesCount();

        if(pageCount >= 3)
        {
            // Get actual page heights from parsed pages
            PDFPageInput page0Input(parser, parser->ParsePage(0));
            PDFPageInput page1Input(parser, parser->ParsePage(1));
            PDFPageInput page2Input(parser, parser->ParsePage(2));

            double page0Height = page0Input.GetMediaBox().GetHeight();
            double page1Height = page1Input.GetMediaBox().GetHeight();
            double page2Height = page2Input.GetMediaBox().GetHeight();

            // Create nested outline structure: Page 1 and 3 are top level, Page 2 is sub-level of Page 1
            OutlineItem page1Child("Subsection", parser->GetPageObjectID(1), page1Height);

            OutlineItem outlineItems[2];
            outlineItems[0] = OutlineItem("Section 1: Beginning", parser->GetPageObjectID(0), &page1Child, 1, page0Height);
            outlineItems[1] = OutlineItem("Section 2: End", parser->GetPageObjectID(2), page2Height);

            // Create outline tree and get the outline object ID
            ObjectIDType outlinesObjectId;
            OutlineBuilder outlineBuilder;
            status = outlineBuilder.CreateOutlineTree(&pdfWriter.GetObjectsContext(), outlineItems, 2, outlinesObjectId);
            if(status != eSuccess)
            {
                cout << "failed to write outline tree\n";
                delete copyingContext;
                break;
            }

            // Manually update the catalog to reference the outline
            PDFDictionary* trailer = parser->GetTrailer();
            if(trailer)
            {
                PDFObjectCastPtr<PDFIndirectObjectReference> rootRef(trailer->QueryDirectObject("Root"));
                if(rootRef.GetPtr())
                {
                    ObjectIDType catalogObjectId = rootRef->mObjectID;
                    PDFObjectCastPtr<PDFDictionary> catalogObject(parser->ParseNewObject(catalogObjectId));

                    if(catalogObject.GetPtr())
                    {
                        status = pdfWriter.GetObjectsContext().StartModifiedIndirectObject(catalogObjectId);
                        if(status == eSuccess)
                        {
                            DictionaryContext* catalogDict = pdfWriter.GetObjectsContext().StartDictionary();

                            // Copy all existing keys except Outlines
                            MapIterator<PDFNameToPDFObjectMap> catalogIt = catalogObject->GetIterator();
                            while(catalogIt.MoveNext())
                            {
                                if(catalogIt.GetKey()->GetValue() != "Outlines")
                                {
                                    catalogDict->WriteKey(catalogIt.GetKey()->GetValue());
                                    copyingContext->CopyDirectObjectAsIs(catalogIt.GetValue());
                                }
                            }

                            // Add Outlines reference
                            catalogDict->WriteKey("Outlines");
                            catalogDict->WriteObjectReferenceValue(outlinesObjectId);

                            pdfWriter.GetObjectsContext().EndDictionary(catalogDict);
                            pdfWriter.GetObjectsContext().EndIndirectObject();
                        }
                    }
                }
            }
        }

        status = pdfWriter.EndPDF();
        if(status != eSuccess)
        {
            cout << "failed in end PDF\n";
            delete copyingContext;
            break;
        }

        delete copyingContext;
    }
    while(false);

    return status == eSuccess ? 0:1;
}