/*
   Source File : AbstractGradientShadingPatternWritingTask.cpp


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


#include "AbstractGradientShadingPatternWritingTask.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFTiledPattern.h"
#include "PDFFormXObject.h"
#include "TiledPatternContentContext.h"
#include "XObjectContentContext.h"

using namespace PDFHummus;

AbstractGradientShadingPatternWritingTask::AbstractGradientShadingPatternWritingTask(
    InterpretedGradientStopList inColorLine,
    PDFRectangle inBounds,
    PDFMatrix inMatrix,
    ObjectIDType inPatternObjectId
) {
   colorLine = inColorLine;
   bounds = inBounds;
   matrix = inMatrix;
   patternObjectId = inPatternObjectId;

    // help with edges. if there's no 0...1 pad with repeats of the nearest stop (this probably fits only with pad)   
    InterpretedGradientStop aStop = colorLine.front();
    if(aStop.stopOffset != 0) {
        aStop.stopOffset = 0;
        colorLine.push_front(aStop);
    }
    // same with 1, if last stop is smaller than 1...complete to 1
    aStop = colorLine.back();
    if(aStop.stopOffset < 1) {
        aStop.stopOffset = 1;
        colorLine.push_back(aStop);
    }
}

AbstractGradientShadingPatternWritingTask::~AbstractGradientShadingPatternWritingTask(){
}

class FromTransparencyGroupWriter: public DocumentContextExtenderAdapter {
    public:
        FromTransparencyGroupWriter(){};
        virtual ~FromTransparencyGroupWriter(){};

        virtual PDFHummus::EStatusCode OnFormXObjectWrite(
                        ObjectIDType inFormXObjectID,
                        ObjectIDType inFormXObjectResourcesDictionaryID,
                        DictionaryContext* inFormDictionaryContext,
                        ObjectsContext* inPDFWriterObjectContext,
                        PDFHummus::DocumentContext* inDocumentContext){
            inFormDictionaryContext->WriteKey("Group");
            DictionaryContext* transparencyGroup = inPDFWriterObjectContext->StartDictionary();
            transparencyGroup->WriteKey("Type");
            transparencyGroup->WriteNameValue("Group");
            transparencyGroup->WriteKey("CS");
            transparencyGroup->WriteNameValue("DeviceRGB");
            transparencyGroup->WriteKey("S");
            transparencyGroup->WriteNameValue("Transparency");
            transparencyGroup->WriteKey("I");
            transparencyGroup->WriteBooleanValue(true);
            return inPDFWriterObjectContext->EndDictionary(transparencyGroup);
        }
};

EStatusCode AbstractGradientShadingPatternWritingTask::WriteAlphaSoftMaskForm(ObjectIDType inObjectID, 
                                    ObjectsContext* inObjectsContext, 
                                    DocumentContext* inDocumentContext) {

    EStatusCode status = eSuccess;

    PDFRectangle formBounds = matrix.Transform(bounds);

    do {
        ObjectIDType alphaShadingPatternObjectId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID(); 

        // adding a small extender to allow writing full transparency group data
        FromTransparencyGroupWriter groupWriter;
        inDocumentContext->AddDocumentContextExtender(&groupWriter);
        PDFFormXObject* form = inDocumentContext->StartFormXObject(formBounds,inObjectID,NULL,false);
        inDocumentContext->RemoveDocumentContextExtender(&groupWriter);

        XObjectContentContext* xobjectContentContext = form->GetContentContext();

        xobjectContentContext->cs("Pattern");
        xobjectContentContext->scn(form->GetResourcesDictionary().AddPatternMapping(alphaShadingPatternObjectId));
        xobjectContentContext->re(formBounds.LowerLeftX,formBounds.LowerLeftY,formBounds.GetWidth(),formBounds.GetHeight());
        xobjectContentContext->fStar();

        status = inDocumentContext->EndFormXObjectAndRelease(form);
        if(status != eSuccess)
            break;

        // create "color line" to reuse the shading pattern code for RGB for the softmask
        InterpretedGradientStopList alphaAsRGBColorLine;
        InterpretedGradientStopList::const_iterator it = colorLine.begin();
        for(; it != colorLine.end();++it) {
            FT_Byte alphaColor = FT_Byte(floor(it->alpha * it->color.alpha));
            FT_Color color = {
                alphaColor,
                alphaColor,
                alphaColor
            };

            InterpretedGradientStop stop = {
                it->stopOffset,
                1,
                color  
            };

            alphaAsRGBColorLine.push_back(stop);
        }


        status = WriteRGBShadingPatternObject(alphaAsRGBColorLine, alphaShadingPatternObjectId, inObjectsContext, inDocumentContext);
    } while(false);


    return status;
}

EStatusCode AbstractGradientShadingPatternWritingTask::WriteAlphaSoftMaskExtGState(ObjectIDType inObjectID,
                                                                                 ObjectsContext* inObjectsContext,
                                                                                 DocumentContext* inDocumentContext) {
    EStatusCode status = eSuccess;

    do {
        ObjectIDType smaskFormObjectId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();

        // write ExtGState with soft mask reference
        inObjectsContext->StartNewIndirectObject(inObjectID);
        DictionaryContext* extGStateDict = inObjectsContext->StartDictionary();

        extGStateDict->WriteKey("Type");
        extGStateDict->WriteNameValue("ExtGState");
        extGStateDict->WriteKey("SMask");
        DictionaryContext* smaskDict = inObjectsContext->StartDictionary();
        
        smaskDict->WriteKey("Type");
        smaskDict->WriteNameValue("Mask");
        smaskDict->WriteKey("S");
        smaskDict->WriteNameValue("Luminosity");
        smaskDict->WriteKey("G");
        smaskDict->WriteNewObjectReferenceValue(smaskFormObjectId);
        status = inObjectsContext->EndDictionary(smaskDict);
        if(status != eSuccess)
            break;
        status = inObjectsContext->EndDictionary(extGStateDict);
        if(status != eSuccess)
            break;
        inObjectsContext->EndIndirectObject();

        status = WriteAlphaSoftMaskForm(smaskFormObjectId, inObjectsContext, inDocumentContext);

    } while(false);


    return status;
}

EStatusCode AbstractGradientShadingPatternWritingTask::WriteRGBATiledPatternObject(
    ObjectIDType inObjectID, 
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext
) {
    EStatusCode status = eSuccess;

    do {
        ObjectIDType transparencySmaskExtGStateObjectId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
        ObjectIDType rgbShadingPatternObjectId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();

        PDFRectangle tiledBounds = matrix.Transform(bounds);

        PDFTiledPattern* tiledPattern = inDocumentContext->StartTiledPattern(
                                        1,
                                        1,
                                        tiledBounds,
                                        tiledBounds.GetWidth(),
                                        tiledBounds.GetHeight(),
                                        inObjectID,
                                        NULL);

        TiledPatternContentContext* patternContentContext = tiledPattern->GetContentContext();

        patternContentContext->gs(tiledPattern->GetResourcesDictionary().AddExtGStateMapping(transparencySmaskExtGStateObjectId));
        patternContentContext->cs("Pattern");
        patternContentContext->scn(tiledPattern->GetResourcesDictionary().AddPatternMapping(rgbShadingPatternObjectId));
        patternContentContext->re(tiledBounds.LowerLeftX,tiledBounds.LowerLeftY,tiledBounds.GetWidth(),tiledBounds.GetHeight());
        patternContentContext->fStar();

        status = inDocumentContext->EndTiledPatternAndRelease(tiledPattern);
        if(status != eSuccess)
            break;

        status = WriteAlphaSoftMaskExtGState(transparencySmaskExtGStateObjectId, inObjectsContext, inDocumentContext);    
        if(status != eSuccess)
            break;

        status = WriteRGBShadingPatternObject(colorLine, rgbShadingPatternObjectId, inObjectsContext, inDocumentContext);
    } while(false);


    return status;
}

bool AbstractGradientShadingPatternWritingTask::ColorLineHasTransparency() {
    bool no = true;

    InterpretedGradientStopList::iterator it = colorLine.begin();

    for(; it != colorLine.end() & no;++it) {
        no = (it->alpha == 1.0) && (it->color.alpha == 255);
    }

    return !no;
}

EStatusCode AbstractGradientShadingPatternWritingTask::Write(
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {
        if(ColorLineHasTransparency())
            return WriteRGBATiledPatternObject(patternObjectId, inObjectsContext, inDocumentContext);
        else
            return WriteRGBShadingPatternObject(colorLine, patternObjectId, inObjectsContext, inDocumentContext);
}
