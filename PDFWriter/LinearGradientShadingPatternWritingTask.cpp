/*
   Source File : LinearGradientShadingPatternWritingTask.cpp


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


#include "LinearGradientShadingPatternWritingTask.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFTiledPattern.h"
#include "PDFFormXObject.h"
#include "TiledPatternContentContext.h"
#include "XObjectContentContext.h"

using namespace PDFHummus;

LinearGradientShadingPatternWritingTask::LinearGradientShadingPatternWritingTask(
    double inX0,
    double inY0,
    double inX1,
    double inY1,
    InterpretedGradientStopList inColorLine,
    PDFRectangle inBounds,
    PDFMatrix inMatrix,
    ObjectIDType inPatternObjectId
):AbstractGradientShadingPatternWritingTask(inColorLine, inBounds, inMatrix, inPatternObjectId) {
   x0 = inX0;
   y0 = inY0;
   x1 = inX1;
   y1 = inY1;
}

LinearGradientShadingPatternWritingTask::~LinearGradientShadingPatternWritingTask(){
}

void LinearGradientShadingPatternWritingTask::WriteShadingSpecifics(DictionaryContext* inShadingDict, ObjectsContext* inObjectsContext) {
    inShadingDict->WriteKey("ShadingType");
    inShadingDict->WriteIntegerValue(2);
    inShadingDict->WriteKey("Coords");
    inObjectsContext->StartArray();
    inObjectsContext->WriteDouble(x0);
    inObjectsContext->WriteDouble(y0);
    inObjectsContext->WriteDouble(x1);
    inObjectsContext->WriteDouble(y1);
    inObjectsContext->EndArray(eTokenSeparatorEndLine);
}

