/*
   Source File : PaintedGlyphsDrawingContext.h


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

#include "PaintedGlyphsDrawingContext.h"
#include "PDFUsedFont.h"
#include "Trace.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "IndirectObjectsReferenceRegistry.h"
#include "ResourcesDictionary.h"
#include "IObjectEndWritingTask.h"
#include "DictionaryContext.h"
#include "InterpretedGradientStop.h"
#include "RadialGradientShadingPatternWritingTask.h"
#include <math.h>

#include FT_COLOR_H

static const double scFix16Dot16Scale = (double(1<<16));
static const std::string scPattern = "Pattern";

PaintedGlyphsDrawingContext::PaintedGlyphsDrawingContext(AbstractContentContext* inContentContext, const AbstractContentContext::TextOptions& inOptions):mOptions(inOptions) {
    mContentContext = inContentContext;
    mCanDrawGlyph = false;
    mLatestAdvance = 0;
    mPalette = NULL;
}

void PaintedGlyphsDrawingContext::SetGlyph(const GlyphUnicodeMapping& inGlyph) {
    mGlyph = inGlyph;
    mBoundsStack.clear();
    mDrawnGlyphs.clear();

    mCurrentOpaquePaint.p = NULL;

    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    FT_Bool hasPaint = FT_Get_Color_Glyph_Paint(
        *freeTypeFace,
        mGlyph.mGlyphCode,
        FT_COLOR_NO_ROOT_TRANSFORM,
        &mCurrentOpaquePaint
    );

    mCanDrawGlyph = hasPaint;
    if(!hasPaint)
        return;

    FT_Error error = freeTypeFace->SelectDefaultPalette(&mPalette);
    if (error) {
        mPalette = NULL;
        mCanDrawGlyph = false;
    }

}

bool PaintedGlyphsDrawingContext::CanDraw() {
    return mCanDrawGlyph;
}

void PaintedGlyphsDrawingContext::Draw(double inX, double inY, bool inComputeAdvance) {
    // set initial bounds for drawing. 
    // with some tests getting the base glyph brings 0
    UIntList glyphs;
    glyphs.push_back(mGlyph.mGlyphCode);
    PDFUsedFont::TextMeasures measures = mContentContext->GetCurrentFont()->CalculateTextDimensions(glyphs, mOptions.fontSize);
    mBoundsStack.push_back(PDFRectangle(measures.xMin,measures.yMin,measures.xMax,measures.yMax));


    if(inComputeAdvance) {
        // for some reason this beings a good advance, even if dimensions bring nothing
		mLatestAdvance = mContentContext->GetCurrentFont()->CalculateTextAdvance(glyphs, mOptions.fontSize);
    }

    // rmmbr i got into this glyph already to avoid loops
    mDrawnGlyphs.push_back(mGlyph.mGlyphCode);
    

    mContentContext->q();
    // initialize a graphic context with x,y so we dont have to deal with them later
    if(inX != 0 || inY != 0)
        mContentContext->cm(1,0,0,1,inX,inY);

    // now draw the graph per it's root
    ExecuteOpaquePaint(mCurrentOpaquePaint);
    mContentContext->Q();
}

double PaintedGlyphsDrawingContext::GetLatestAdvance() {
	return mLatestAdvance;
}

bool PaintedGlyphsDrawingContext::ExecuteOpaquePaint(FT_OpaquePaint inOpaquePaint) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    FT_COLR_Paint paint;
    FT_Bool isOK = FT_Get_Paint(
        *freeTypeFace,
        inOpaquePaint,
        &paint
    );

    if(!isOK)
        return false;

    return ExecuteColrPaint(paint);
}

bool PaintedGlyphsDrawingContext::ExecuteColrPaint(FT_COLR_Paint inColrPaint) {
    bool result = true;

    switch(inColrPaint.format) {
        case FT_COLR_PAINTFORMAT_COLR_LAYERS:
            result = ExecutePaintColrLayers(inColrPaint.u.colr_layers);
            break;
        case FT_COLR_PAINTFORMAT_GLYPH:
            result = ExecutePaintGlyph(inColrPaint.u.glyph);
            break;
        case FT_COLR_PAINTFORMAT_SOLID:
            result = ExecutePaintSolid(inColrPaint.u.solid);
            break;
        case FT_COLR_PAINTFORMAT_TRANSFORM:
            result = ExceutePaintTransform(inColrPaint.u.transform);
            break;
        case FT_COLR_PAINTFORMAT_TRANSLATE:
            result = ExceutePaintTranslate(inColrPaint.u.translate);
            break;
        case FT_COLR_PAINTFORMAT_SCALE:
            result = ExecutePaintScale(inColrPaint.u.scale);
            break;
        case FT_COLR_PAINTFORMAT_ROTATE:
            result = ExceutePaintRotate(inColrPaint.u.rotate);
            break;
        case FT_COLR_PAINTFORMAT_SKEW:
            result = ExecutePaintSkew(inColrPaint.u.skew);
            break;
        case FT_COLR_PAINTFORMAT_COLR_GLYPH:
            result = ExecutePaintColrGlyph(inColrPaint.u.colr_glyph);
            break;
        case FT_COLR_PAINTFORMAT_RADIAL_GRADIENT:
            result = ExecutePaintRadialGradient(inColrPaint.u.radial_gradient);
            break;
        default:
            TRACE_LOG1(
				"PaintedGlyphsDrawingContext::ExecuteColrPaint, unsupported Colrv1 paint format %d. skipping.",
				inColrPaint.format);
    }

    return result;
}

bool PaintedGlyphsDrawingContext::ExecutePaintColrLayers(FT_PaintColrLayers inColrLayers) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();
    bool result = true;

    FT_OpaquePaint paint;
    paint.p = NULL;

    while (result &&
        FT_Get_Paint_Layers(*freeTypeFace,
                            &inColrLayers.layer_iterator,
                            &paint)
    ) {
        result = ExecuteOpaquePaint(paint);
    }

    return result;

}

bool PaintedGlyphsDrawingContext::ExecutePaintGlyph(FT_PaintGlyph inGlyph) {
    // now lets save this state so we can clip without later artifacts
    mContentContext->q();

    // clip per a regular glyph
    mContentContext->BT();
    // set the rendering mode, for clipping
    mContentContext->Tr(7);

    // add the path to the clip path
    mContentContext->SetCurrentFontSize(mOptions.fontSize);
    GlyphUnicodeMappingList text;
    text.push_back(GlyphUnicodeMapping(inGlyph.glyphID, mGlyph.mUnicodeValues)); 
    mContentContext->Tj(text);

    // finish text object so path is added to clip path
    mContentContext->ET();    

    // add glyph bounds to stack, so lower levels can fill to it
    UIntList glyphs;
    glyphs.push_back(inGlyph.glyphID);
    PDFUsedFont::TextMeasures measures = mContentContext->GetCurrentFont()->CalculateTextDimensions(glyphs, mOptions.fontSize);
    mBoundsStack.push_back(PDFRectangle(measures.xMin,measures.yMin,measures.xMax,measures.yMax));


    // draw the internal glyph graph node...which should be a certain fill
    bool result = ExecuteOpaquePaint(inGlyph.paint);

    // pop previous pushed bounds...as they are about to go
    mBoundsStack.pop_back();

    // restore graphic stay, cleanup.
    mContentContext->Q();

    return result;
}

void PaintedGlyphsDrawingContext::FillCurrentBounds() {
    PDFRectangle& currentBounds = mBoundsStack.back();
    mContentContext->re(currentBounds.LowerLeftX,currentBounds.LowerLeftY,currentBounds.UpperRightX - currentBounds.LowerLeftX, currentBounds.UpperRightY - currentBounds.LowerLeftY);
    mContentContext->f();
}


bool PaintedGlyphsDrawingContext::ExecutePaintSolid(FT_PaintSolid inSolid) {
    FT_UInt16 palette_index = inSolid.color.palette_index;
    FT_F2Dot14 alpha = inSolid.color.alpha;

    // setup the color, with multiplier considered
    if(0xFFFF == palette_index) {
        mContentContext->SetupColor(AbstractContentContext::eFill,mOptions.colorValue,mOptions.colorSpace,mOptions.opacity * ( alpha / (1<<14)));
    } else {
        FT_Color layer_color = mPalette[palette_index];

        mContentContext->SetOpacity((double(layer_color.alpha)/255.0) * (alpha / (1<<14)));
        mContentContext->rg(double(layer_color.red)/255.0,double(layer_color.green)/255.0,double(layer_color.blue)/255.0);
    }    

    // now fill a path. use current bounds
    FillCurrentBounds();
    return true;
}

double PaintedGlyphsDrawingContext::GetFontUnitMeasurementInPDF(FT_Fixed inFixedPos) {
    return (mContentContext->GetCurrentFont()->GetFreeTypeFont()->GetInPDFMeasurements(inFixedPos) / scFix16Dot16Scale) * (mOptions.fontSize / 1000);
}

bool PaintedGlyphsDrawingContext::ExceutePaintTransform(FT_PaintTransform inTransform) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    return ApplyTransformToPaint(
        inTransform.affine.xx / scFix16Dot16Scale,
        inTransform.affine.yx / scFix16Dot16Scale,
        inTransform.affine.xy / scFix16Dot16Scale,
        inTransform.affine.yy / scFix16Dot16Scale,
        // vectors are in font space, so adjust translation to pdf measurements by em and then by font size
        GetFontUnitMeasurementInPDF(inTransform.affine.dx),
        GetFontUnitMeasurementInPDF(inTransform.affine.dy),
        inTransform.paint
    );
}

bool PaintedGlyphsDrawingContext::ApplyTransformToPaint(
    double inA,
    double inB,
    double inC,
    double inD,
    double inE,
    double inF,
    FT_OpaquePaint inOpaquePaint) {

    mContentContext->q();
    mContentContext->cm(inA,inB,inC,inD,inE,inF);

    bool result = ExecuteOpaquePaint(inOpaquePaint);

    mContentContext->Q();

    return result;
}


bool PaintedGlyphsDrawingContext::ExceutePaintTranslate(FT_PaintTranslate inTranslate) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    return ApplyTransformToPaint(
        1,
        0,
        0,
        1,
        // see transform
        GetFontUnitMeasurementInPDF(inTranslate.dx),
        GetFontUnitMeasurementInPDF(inTranslate.dy),
        inTranslate.paint
    );

}

bool PaintedGlyphsDrawingContext::ExecutePaintScale(FT_PaintScale inScale) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    return ApplyTransformToPaint(
        inScale.scale_x / scFix16Dot16Scale,
        0,
        0,
        inScale.scale_y / scFix16Dot16Scale,
        // see transform
        GetFontUnitMeasurementInPDF(inScale.center_x),
        GetFontUnitMeasurementInPDF(inScale.center_y),
        inScale.paint
    );

}

bool PaintedGlyphsDrawingContext::ExceutePaintRotate(FT_PaintRotate inRotate) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    double radianAngle = inRotate.angle / scFix16Dot16Scale;

    return ApplyTransformToPaint(
        cos(radianAngle),
        sin(radianAngle),
        -sin(radianAngle),
        cos(radianAngle),
        // see transform
        GetFontUnitMeasurementInPDF(inRotate.center_x),
        GetFontUnitMeasurementInPDF(inRotate.center_y),
        inRotate.paint
    );
}

bool PaintedGlyphsDrawingContext::ExecutePaintSkew(FT_PaintSkew inSkew) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    return ApplyTransformToPaint(
        1,
        tan(inSkew.x_skew_angle / scFix16Dot16Scale),
        tan(inSkew.y_skew_angle / scFix16Dot16Scale),
        1,
        // see transform
        GetFontUnitMeasurementInPDF(inSkew.center_x),
        GetFontUnitMeasurementInPDF(inSkew.center_y),
        inSkew.paint
    );
}

bool PaintedGlyphsDrawingContext::ExecutePaintColrGlyph(FT_PaintColrGlyph inColrGlyph) {
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    FT_UInt glyphID = inColrGlyph.glyphID; 

    bool didVisitGlyph = false;
    UShortList::iterator itDrawnGlyphs = mDrawnGlyphs.begin();

    for(; itDrawnGlyphs != mDrawnGlyphs.end() && !didVisitGlyph;++itDrawnGlyphs) {
        didVisitGlyph = (*itDrawnGlyphs == glyphID);
    }
    if(didVisitGlyph) {
        // orcish mischief. there's a circle in the DAG!!!!!!11. ignore per the specs
        TRACE_LOG2(
        "PaintedGlyphsDrawingContext::ExecutePaintColrGlyph, sub glyph id not found - %d while drawing glyph %d",
        glyphID,
        mGlyph.mGlyphCode);        
        return true;
    }

    FT_OpaquePaint rootPaint;
    rootPaint.p = NULL;
    FT_Bool hasPaint = FT_Get_Color_Glyph_Paint(
        *freeTypeFace,
        glyphID,
        FT_COLOR_NO_ROOT_TRANSFORM,
        &rootPaint
    );

    if(!hasPaint) {
        TRACE_LOG2(
        "PaintedGlyphsDrawingContext::ExecutePaintColrGlyph, sub glyph id not found - %d while drawing glyph %d",
        glyphID,
        mGlyph.mGlyphCode);
 
        // skip this. ignoring.
        return true;
    }

    // ok let's do this
    mDrawnGlyphs.push_back(glyphID);
    mContentContext->q();
    // now draw the graph per it's root
    bool result = ExecuteOpaquePaint(rootPaint);
    mContentContext->Q(); 
    mDrawnGlyphs.pop_back();
    return result;   
}


bool PaintedGlyphsDrawingContext::ExecutePaintRadialGradient(FT_PaintRadialGradient inColrRadialGradient)
{
    FreeTypeFaceWrapper* freeTypeFace = mContentContext->GetCurrentFont()->GetFreeTypeFont();

    double x0 = GetFontUnitMeasurementInPDF(inColrRadialGradient.c0.x);
    double y0 = GetFontUnitMeasurementInPDF(inColrRadialGradient.c0.y);
    double r0 = GetFontUnitMeasurementInPDF(inColrRadialGradient.r0);
    double x1 = GetFontUnitMeasurementInPDF(inColrRadialGradient.c1.x);
    double y1 = GetFontUnitMeasurementInPDF(inColrRadialGradient.c1.x);
    double r1 = GetFontUnitMeasurementInPDF(inColrRadialGradient.r1);

    FT_PaintExtend gradientExtend = inColrRadialGradient.colorline.extend;
    FT_ColorStopIterator colorStopIterator = inColrRadialGradient.colorline.color_stop_iterator;
    FT_ColorStop colorStop;
    InterpretedGradientStopList colorLine;

    while(FT_Get_Colorline_Stops(
        *freeTypeFace,
        &colorStop,
        &colorStopIterator)) {
            InterpretedGradientStop gradientStop = {
                colorStop.stop_offset/scFix16Dot16Scale,
                double(colorStop.color.alpha) / double(1<<14),
                mPalette[colorStop.color.palette_index]
            };

            colorLine.push_back(
                gradientStop
            );
    };

    // apply shading pattern
    ObjectIDType patternObjectId = mContentContext->GetDocumentContext()->GetObjectsContext()->GetInDirectObjectsRegistry().AllocateNewObjectID();
    mContentContext->cs(scPattern);
    mContentContext->scn(mContentContext->GetResourcesDictionary()->AddPatternMapping(patternObjectId));

    // schedule task to create object for this shading pattern
    mContentContext->ScheduleObjectEndWriteTask(new RadialGradientShadingPatternWritingTask(
        x0,
        y0,
        r0,
        x1,
        y1,
        r1,
        colorLine,
        patternObjectId
    ));

    // now fill a path. use current bounds
    FillCurrentBounds();
    return true;
}