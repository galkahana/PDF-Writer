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


void AbstractGradientShadingPatternWritingTask::WriteColorLineStepsProgram(const InterpretedGradientStopList& inRGBColorLine) {
    // write postscript code to define the part of the function that draws interpolated colors. thank you Skia :)

    double firstStop = inRGBColorLine.front().stopOffset;
    double lastStop = inRGBColorLine.back().stopOffset;

    // padding unto first stop
    WriteStreamText("dup ");
    mPrimitiveWriter.WriteDouble(firstStop);
    WriteStreamText("le {pop ");
    mPrimitiveWriter.WriteDouble(inRGBColorLine.front().color.red / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.front().color.green / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.front().color.blue / 255.0);
    WriteStreamText("0} if\n");        

    // wrapper for in between stops code
    WriteStreamText("dup dup ");
    mPrimitiveWriter.WriteDouble(firstStop);
    WriteStreamText("gt exch ");
    mPrimitiveWriter.WriteDouble(lastStop);
    WriteStreamText("le and ");

    // k. interim stops, lets prep those cursers
    InterpretedGradientStopList::const_iterator itStartRange = inRGBColorLine.begin();
    InterpretedGradientStopList::const_iterator itEndRange = inRGBColorLine.begin();
    ++itEndRange;

    // k. write stops, and interpolate.
    for(; itEndRange != inRGBColorLine.end(); ++itStartRange,++itEndRange) {
        if(itStartRange->stopOffset > firstStop) {
            // all but first, close previous if of ifelse statement
            WriteStreamText("}");
        }
        WriteStreamText("{\n");
        if(itEndRange->stopOffset < lastStop) {
            // all but last check value (last will enjoy the last else of ifelse)
            WriteStreamText("dup ");
            mPrimitiveWriter.WriteDouble(itEndRange->stopOffset);
            WriteStreamText("le ");
            WriteStreamText("{\n");
        }
        mPrimitiveWriter.WriteDouble(itStartRange->stopOffset);
        WriteStreamText("sub ");

        // if all colors are the same, just write as is

        // otherwise, interpolate
        if(itStartRange->color.red == itEndRange->color.red &&
            itStartRange->color.green == itEndRange->color.green &&
            itStartRange->color.blue == itEndRange->color.blue) {

            WriteStreamText("pop ");
            mPrimitiveWriter.WriteDouble(itEndRange->color.red / 255.0);
            mPrimitiveWriter.WriteDouble(itEndRange->color.green / 255.0);
            mPrimitiveWriter.WriteDouble(itEndRange->color.blue / 255.0);
        } else {
            double rangeDiff = itEndRange->stopOffset - itStartRange->stopOffset;
            WriteColorInterpolation(itStartRange->color.red, itEndRange->color.red, rangeDiff);
            WriteColorInterpolation(itStartRange->color.green, itEndRange->color.green, rangeDiff);
            WriteColorInterpolation(itStartRange->color.blue, itEndRange->color.blue, rangeDiff);
            WriteStreamText("pop ");
        }
    }

    // "ifselse" (basically size -2, cause only all but first, and then always dual)
    for(int i=0;i<inRGBColorLine.size()-2;++i) {
        WriteStreamText("} ifelse\n");
    }

    WriteStreamText("0} if\n");

    // padding after last stop
    WriteStreamText("0 gt {");
    mPrimitiveWriter.WriteDouble(inRGBColorLine.back().color.red / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.back().color.green / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.back().color.blue / 255.0);
    WriteStreamText("} if\n");


    // function end
    WriteStreamText("}");        
}

void AbstractGradientShadingPatternWritingTask::WriteColorInterpolation(FT_Byte inColorStart, FT_Byte inColorEnd, double inStopDiff) {
    if(inColorEnd == inColorStart) {
        // same color...so just put it there
        mPrimitiveWriter.WriteDouble(inColorEnd/255.0);
    } else {
        // interpolate
        WriteStreamText("dup ");
        mPrimitiveWriter.WriteDouble((inColorEnd - inColorStart)/(inStopDiff*255.0));
        WriteStreamText("mul ");
        mPrimitiveWriter.WriteDouble(inColorStart/255.0);
        WriteStreamText("add ");
    }
    WriteStreamText("exch ");
}

void AbstractGradientShadingPatternWritingTask::WriteStreamText(const std::string& inString) {
    mWriteStream->Write((const Byte*)(inString.c_str()),inString.size());
}