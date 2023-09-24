/*
   Source File : PaintedGlyphsDrawingContext.cpp


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


#include "RadialGradientShadingPatternWritingTask.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFTiledPattern.h"
#include "PDFFormXObject.h"
#include "TiledPatternContentContext.h"
#include "XObjectContentContext.h"

using namespace PDFHummus;

RadialGradientShadingPatternWritingTask::RadialGradientShadingPatternWritingTask(
    double inX0,
    double inY0,
    double inR0,
    double inX1,
    double inY1,
    double inR1,
    InterpretedGradientStopList inColorLine,
    PDFRectangle inBounds,
    PDFMatrix inMatrix,
    ObjectIDType inPatternObjectId
) {
   x0 = inX0;
   y0 = inY0;
   r0 = inR0;
   x1 = inX1;
   y1 = inY1;
   r1 = inR1;
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

RadialGradientShadingPatternWritingTask::~RadialGradientShadingPatternWritingTask(){
}

EStatusCode RadialGradientShadingPatternWritingTask::WriteRGBShadingPatternObject(ObjectIDType inObjectID, ObjectsContext* inObjectsContext) {
        EStatusCode status = eSuccess;

        do {
            inObjectsContext->StartNewIndirectObject(inObjectID);
            DictionaryContext* patternDict = inObjectsContext->StartDictionary();
            patternDict->WriteKey("Type");
            patternDict->WriteNameValue("Pattern");
            patternDict->WriteKey("PatternType");
            patternDict->WriteIntegerValue(2);
            patternDict->WriteKey("Matrix");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(matrix.a);
            inObjectsContext->WriteDouble(matrix.b);
            inObjectsContext->WriteDouble(matrix.c);
            inObjectsContext->WriteDouble(matrix.d);
            inObjectsContext->WriteDouble(matrix.e);
            inObjectsContext->WriteDouble(matrix.f);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            patternDict->WriteKey("Shading");
            DictionaryContext* shadingDict = inObjectsContext->StartDictionary();
            shadingDict->WriteKey("ShadingType");
            shadingDict->WriteIntegerValue(3);
            shadingDict->WriteKey("ColorSpace");
            shadingDict->WriteNameValue("DeviceRGB");
            shadingDict->WriteKey("Extend");
            inObjectsContext->StartArray();
            inObjectsContext->WriteBoolean(true);
            inObjectsContext->WriteBoolean(true);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            shadingDict->WriteKey("Coords");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(x0);
            inObjectsContext->WriteDouble(y0);
            inObjectsContext->WriteDouble(r0);
            inObjectsContext->WriteDouble(x1);
            inObjectsContext->WriteDouble(y1);
            inObjectsContext->WriteDouble(r1);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            shadingDict->WriteKey("Function");
            DictionaryContext* functionDict = inObjectsContext->StartDictionary();
            functionDict->WriteKey("FunctionType");
            functionDict->WriteIntegerValue(3);
            functionDict->WriteKey("Bounds");
            inObjectsContext->StartArray();
            // bound contains whats within the edges, so should skip
            // first and last (which are expected to be 0 and 1)
            InterpretedGradientStopList::iterator it1 = colorLine.begin();
            InterpretedGradientStopList::iterator it2 = colorLine.begin();
            double firstStop = it1->stopOffset;
            ++it1;
            ++it2;
            ++it1;
            for(; it1 != colorLine.end();++it1,++it2) {
                inObjectsContext->WriteDouble(it2->stopOffset);
            }
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            // last stop, which should probably be 1, is saved cause will be used in domains
            double lastStop = it2->stopOffset;
            functionDict->WriteKey("Domain");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(firstStop);
            inObjectsContext->WriteDouble(lastStop);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            functionDict->WriteKey("Encode");
            inObjectsContext->StartArray();
            // write encodes for size-1 functions
            for(int i=0; i<colorLine.size()-1;++i) {
                inObjectsContext->WriteDouble(firstStop);
                inObjectsContext->WriteDouble(lastStop);
            }
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            functionDict->WriteKey("Functions");
            // draw all stops, each in its own function. note that there should be size-1 functions!
            inObjectsContext->StartArray();
            it1 = colorLine.begin();
            it2 = colorLine.begin();
            ++it1;
            for(; it1 != colorLine.end() && eSuccess == status;++it1,++it2) {
                DictionaryContext* colorStopDict = inObjectsContext->StartDictionary();
                colorStopDict->WriteKey("FunctionType");
                colorStopDict->WriteIntegerValue(2);
                colorStopDict->WriteKey("N");
                colorStopDict->WriteIntegerValue(1);
                colorStopDict->WriteKey("Domain");
                inObjectsContext->StartArray();
                inObjectsContext->WriteDouble(0);
                inObjectsContext->WriteDouble(lastStop);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                colorStopDict->WriteKey("C0");
                inObjectsContext->StartArray();
                inObjectsContext->WriteDouble(it2->color.red/255.0);
                inObjectsContext->WriteDouble(it2->color.green/255.0);
                inObjectsContext->WriteDouble(it2->color.blue/255.0);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                colorStopDict->WriteKey("C1");
                inObjectsContext->StartArray();
                inObjectsContext->WriteDouble(it1->color.red/255.0);
                inObjectsContext->WriteDouble(it1->color.green/255.0);
                inObjectsContext->WriteDouble(it1->color.blue/255.0);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                status = inObjectsContext->EndDictionary(colorStopDict);
                if(status != eSuccess)
                    break;
            } 
            if(status != eSuccess)
                break;

            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            status = inObjectsContext->EndDictionary(functionDict);
            if(status != eSuccess)
                break;
            status = inObjectsContext->EndDictionary(shadingDict);
            if(status != eSuccess)
                break;
            status = inObjectsContext->EndDictionary(patternDict);
            if(status != eSuccess)
                break;
            inObjectsContext->EndIndirectObject();
        }while(false);
        return status;
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

EStatusCode RadialGradientShadingPatternWritingTask::WriteAlphaShadingPatternObject(ObjectIDType inObjectID, ObjectsContext* inObjectsContext) {
        EStatusCode status = eSuccess;

        do {
            inObjectsContext->StartNewIndirectObject(inObjectID);
            DictionaryContext* patternDict = inObjectsContext->StartDictionary();
            patternDict->WriteKey("Type");
            patternDict->WriteNameValue("Pattern");
            patternDict->WriteKey("PatternType");
            patternDict->WriteIntegerValue(2);
            patternDict->WriteKey("Matrix");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(matrix.a);
            inObjectsContext->WriteDouble(matrix.b);
            inObjectsContext->WriteDouble(matrix.c);
            inObjectsContext->WriteDouble(matrix.d);
            inObjectsContext->WriteDouble(matrix.e);
            inObjectsContext->WriteDouble(matrix.f);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            patternDict->WriteKey("Shading");
            DictionaryContext* shadingDict = inObjectsContext->StartDictionary();
            shadingDict->WriteKey("ShadingType");
            shadingDict->WriteIntegerValue(3);
            shadingDict->WriteKey("ColorSpace");
            shadingDict->WriteNameValue("DeviceRGB");
            shadingDict->WriteKey("Extend");
            inObjectsContext->StartArray();
            inObjectsContext->WriteBoolean(true);
            inObjectsContext->WriteBoolean(true);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            shadingDict->WriteKey("Coords");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(x0);
            inObjectsContext->WriteDouble(y0);
            inObjectsContext->WriteDouble(r0);
            inObjectsContext->WriteDouble(x1);
            inObjectsContext->WriteDouble(y1);
            inObjectsContext->WriteDouble(r1);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            shadingDict->WriteKey("Function");
            DictionaryContext* functionDict = inObjectsContext->StartDictionary();
            functionDict->WriteKey("FunctionType");
            functionDict->WriteIntegerValue(3);
            functionDict->WriteKey("Bounds");
            inObjectsContext->StartArray();
            // bound contains whats within the edges, so should skip
            // first and last (which are expected to be 0 and 1)
            InterpretedGradientStopList::iterator it1 = colorLine.begin();
            InterpretedGradientStopList::iterator it2 = colorLine.begin();
            double firstStop = it1->stopOffset;
            ++it1;
            ++it2;
            ++it1;
            for(; it1 != colorLine.end();++it1,++it2) {
                inObjectsContext->WriteDouble(it2->stopOffset);
            }
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            // last stop, which should probably be 1, is saved cause will be used in domains
            double lastStop = it2->stopOffset;
            functionDict->WriteKey("Domain");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(firstStop);
            inObjectsContext->WriteDouble(lastStop);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            functionDict->WriteKey("Encode");
            inObjectsContext->StartArray();
            // write encodes for size-1 functions
            for(int i=0; i<colorLine.size()-1;++i) {
                inObjectsContext->WriteDouble(firstStop);
                inObjectsContext->WriteDouble(lastStop);
            }
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            functionDict->WriteKey("Functions");
            // draw all stops, each in its own function. note that there should be size-1 functions!
            inObjectsContext->StartArray();
            it1 = colorLine.begin();
            it2 = colorLine.begin();
            ++it1;
            for(; it1 != colorLine.end() && eSuccess == status;++it1,++it2) {
                DictionaryContext* colorStopDict = inObjectsContext->StartDictionary();
                colorStopDict->WriteKey("FunctionType");
                colorStopDict->WriteIntegerValue(2);
                colorStopDict->WriteKey("N");
                colorStopDict->WriteIntegerValue(1);
                colorStopDict->WriteKey("Domain");
                inObjectsContext->StartArray();
                inObjectsContext->WriteDouble(0);
                inObjectsContext->WriteDouble(lastStop);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                colorStopDict->WriteKey("C0");
                inObjectsContext->StartArray();
                double startAlphaValue = it2->alpha * it2->color.alpha/255.0;
                inObjectsContext->WriteDouble(startAlphaValue);
                inObjectsContext->WriteDouble(startAlphaValue);
                inObjectsContext->WriteDouble(startAlphaValue);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                colorStopDict->WriteKey("C1");
                inObjectsContext->StartArray();
                double endAlphaValue = it1->alpha * it1->color.alpha/255.0;
                inObjectsContext->WriteDouble(endAlphaValue);
                inObjectsContext->WriteDouble(endAlphaValue);
                inObjectsContext->WriteDouble(endAlphaValue);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                status = inObjectsContext->EndDictionary(colorStopDict);
                if(status != eSuccess)
                    break;
            } 
            if(status != eSuccess)
                break;

            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            status = inObjectsContext->EndDictionary(functionDict);
            if(status != eSuccess)
                break;
            status = inObjectsContext->EndDictionary(shadingDict);
            if(status != eSuccess)
                break;
            status = inObjectsContext->EndDictionary(patternDict);
            if(status != eSuccess)
                break;
            inObjectsContext->EndIndirectObject();
        }while(false);
        return status;    
}


EStatusCode RadialGradientShadingPatternWritingTask::WriteAlphaSoftMaskForm(ObjectIDType inObjectID, 
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

        status = WriteAlphaShadingPatternObject(alphaShadingPatternObjectId, inObjectsContext);
    } while(false);


    return status;
}

EStatusCode RadialGradientShadingPatternWritingTask::WriteAlphaSoftMaskExtGState(ObjectIDType inObjectID,
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

EStatusCode RadialGradientShadingPatternWritingTask::WriteRGBATiledPatternObject(
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

        status = WriteRGBShadingPatternObject(rgbShadingPatternObjectId, inObjectsContext);
    } while(false);


    return status;
}

bool RadialGradientShadingPatternWritingTask::ColorLineHasTransparency() {
    bool no = true;

    InterpretedGradientStopList::iterator it = colorLine.begin();

    for(; it != colorLine.end() & no;++it) {
        no = (it->alpha == 1.0) && (it->color.alpha == 255);
    }

    return !no;
}

EStatusCode RadialGradientShadingPatternWritingTask::Write(
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {
        if(ColorLineHasTransparency())
            return WriteRGBATiledPatternObject(patternObjectId, inObjectsContext, inDocumentContext);
        else
            return WriteRGBShadingPatternObject(patternObjectId, inObjectsContext);
}