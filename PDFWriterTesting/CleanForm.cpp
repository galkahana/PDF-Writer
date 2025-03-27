#include "PDFWriter.h"
#include "DictionaryContext.h"
#include "PDFDocumentCopyingContext.h"
#include "PDFObjectCast.h"
#include "PDFArray.h"
#include "RefCountPtr.h"
#include "PDFIndirectObjectReference.h"
#include <iostream>

#include "testing/TestIO.h"

using namespace PDFHummus;

int CleanForm(int argc, char* argv[])
{
	EStatusCode status = eSuccess; 

    std::string inputPath = BuildRelativeInputPath(argv,"form.pdf");
    std::string outputPath = BuildRelativeOutputPath(argv,"emptyForm.pdf");

	do
	{

        PDFWriter writer;
        status = writer.ModifyPDF(inputPath, ePDFVersion16, outputPath);
        if(status != eSuccess){
            std::cerr << "Unable to open " << inputPath << std::endl;
            break;
        }
        PDFParser &parser = writer.GetModifiedFileParser();
        PDFDocumentCopyingContext* copyingContext = writer.CreatePDFCopyingContextForModifiedFile();
        ObjectsContext& objectContext = writer.GetObjectsContext();
    
        unsigned long pageCount = parser.GetPagesCount();
    
        for(unsigned long i = 0; i < pageCount && (eSuccess == status); i++ ) {
            PDFObjectCastPtr<PDFDictionary> pageDictionary = parser.ParsePage(i);
            if(!pageDictionary->Exists("Annots")) {
                continue;
            }

            // rewrite page dictionary, with Annots array without wigets
            status = objectContext.StartModifiedIndirectObject(parser.GetPageObjectID(i));
            if(status != eSuccess) {
                std::cerr << "Failed to start modified page object" << std::endl;
                break;
            }
            DictionaryContext *newDict = objectContext.StartDictionary();
            MapIterator<PDFNameToPDFObjectMap> pageDictionaryObjectIt = pageDictionary->GetIterator();
            while (pageDictionaryObjectIt.MoveNext() && (eSuccess == status)) {
                if (pageDictionaryObjectIt.GetKey()->GetValue() == "Annots") {
                    continue;
                }

                newDict->WriteKey(pageDictionaryObjectIt.GetKey()->GetValue());
                status = copyingContext->CopyDirectObjectAsIs(pageDictionaryObjectIt.GetValue());
            }
            if(status != eSuccess) {
                std::cerr << "Failed to copy page object" << std::endl;
                break;
            }

            newDict->WriteKey("Annots");
            objectContext.StartArray();
            PDFObjectCastPtr<PDFArray> anArray(parser.QueryDictionaryObject(pageDictionary.GetPtr(), "Annots"));
            std::cout << "Number of Annots: " << anArray->GetLength() << std::endl;
            unsigned long annotArrayCount = anArray->GetLength();
            for(unsigned long i = 0; i < annotArrayCount && (eSuccess == status); i++) {
                PDFObjectCastPtr<PDFDictionary> annotationObject(parser.QueryArrayObject(anArray.GetPtr(),i));
                PDFObjectCastPtr<PDFName> objectType = annotationObject->QueryDirectObject("Subtype");
                if(!objectType) {
                    continue;
                }
                RefCountPtr<PDFObject> directItem(anArray->QueryObject(i));
                if (objectType->GetValue() != "Widget") {
                    std::cout << "Copying annotation: " << objectType->GetValue() << std::endl;
                    status = copyingContext->CopyDirectObjectAsIs(directItem.GetPtr());
                } else {
                    // let's also delete this object if it's a widget
                    if(directItem->GetType() == PDFObject::ePDFObjectIndirectObjectReference) {
                        // lets also mark the original object for deletion
                        status = objectContext.GetInDirectObjectsRegistry().DeleteObject(((PDFIndirectObjectReference*)(directItem.GetPtr()))->mObjectID);
                    }

                }
            }
            if(status != eSuccess) {
                std::cerr << "Failed to copy Annots array" << std::endl;
                break;
            }

            objectContext.EndArray();
            objectContext.EndLine();
        
            status = objectContext.EndDictionary(newDict);
            if(status != eSuccess) {
                std::cerr << "Failed to end page dictionary" << std::endl;
                break;
            }            
            objectContext.EndIndirectObject();  
        }
        if(status != eSuccess) {
            std::cerr << "Failed to update pages" << std::endl;
            break;
        }            

    
        PDFObjectCastPtr<PDFDictionary> catalogDict(parser.QueryDictionaryObject(parser.GetTrailer(),"Root"));
        PDFObjectCastPtr<PDFIndirectObjectReference> catalogDictObjId(parser.GetTrailer()->QueryDirectObject("Root"));
        status = objectContext.StartModifiedIndirectObject(catalogDictObjId->mObjectID);
        if(status != eSuccess) {
            std::cerr << "failed to start modifying catalog" << std::endl;
            break;
        }  

        DictionaryContext *newDict = objectContext.StartDictionary();
        MapIterator<PDFNameToPDFObjectMap>  pageDictionaryObjectIt = catalogDict->GetIterator();
        while (pageDictionaryObjectIt.MoveNext() && (eSuccess == status)) {
            if (pageDictionaryObjectIt.GetKey()->GetValue() == "AcroForm") {
                if(pageDictionaryObjectIt.GetValue()->GetType() == PDFObject::ePDFObjectIndirectObjectReference) {
                    objectContext.GetInDirectObjectsRegistry().DeleteObject(((PDFIndirectObjectReference*)(pageDictionaryObjectIt.GetValue()))->mObjectID);
                }
            } else {
                newDict->WriteKey(pageDictionaryObjectIt.GetKey()->GetValue());
                status = copyingContext->CopyDirectObjectAsIs(pageDictionaryObjectIt.GetValue());
            }
        }
        if(status != eSuccess) {
            std::cerr << "failed to write updated catalog object" << std::endl;
            break;
        }  
    
        status = objectContext.EndDictionary(newDict);
        if (status != eSuccess) {
            std::cerr << "failed to end catalog dictionary" << std::endl;
            break;
        }
        objectContext.EndIndirectObject();

        if (catalogDict->Exists("AcroForm")) {
            PDFObjectCastPtr<PDFIndirectObjectReference> formId = catalogDict->QueryDirectObject("AcroForm");
            if(formId->GetType() == PDFObject::ePDFObjectIndirectObjectReference) {
                status = objectContext.GetInDirectObjectsRegistry().DeleteObject(formId->mObjectID);
                if (status != eSuccess) {
                    std::cerr << "failed to mark acroform for deletion" << std::endl;
                    break;
                }                
            }
        }
        
        status = writer.EndPDF();


    }while(false);

    return status == eSuccess ? 0:1;	

}