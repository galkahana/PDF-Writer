#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "TIFFUsageParameters.h"

#include <string>
#include <list>

using namespace std;


struct T2P;
struct T2P_BOX;
class DictionaryContext;
class PDFImageXObject;
class PDFStream;
class PDFFormXObject;
class DocumentContext;
class ObjectsContext;

typedef list<ObjectIDType> ObjectIDTypeList;
typedef list<PDFImageXObject*> PDFImageXObjectList;
typedef list<string> StringList;

//  tiff.h extracted defs [prefer to avoid including too much here]
typedef	unsigned short uint16;	/* sizeof (uint16) must == 2 */
typedef	unsigned int uint32;	/* sizeof (uint32) must == 4 */
typedef	int int32;
typedef int32 tsize_t;          /* i/o size in bytes */
typedef void* tdata_t;          /* image data ref */

typedef	tsize_t (*ImageSizeProc)(T2P* inT2p);

class TIFFImageHandler
{
public:
	TIFFImageHandler();
	virtual ~TIFFImageHandler(void);

	// create a form XObject from an image (using form for 1. tiled images 2. to setup matrix, set color space...and leave you with just placing the image object
	PDFFormXObject* CreateFormXObjectFromTIFFFile(	const wstring& inTIFFFilePath,
													const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);

	void SetOperationsContexts(DocumentContext* inContainerDocumentContext,ObjectsContext* inObjectsContext);

private:
	DocumentContext* mContainerDocumentContext;
	ObjectsContext* mObjectsContext;
	T2P* mT2p; // state for tiff->pdf
	TIFFUsageParameters mUserParameters;


	void InitializeConversionState();
	void DestroyConversionState();
	PDFFormXObject* ConvertTiff2PDF();
	EStatusCode ReadTopLevelTiffInformation();
	EStatusCode ReadTIFFPageInformation();
	EStatusCode ReadPhotometricPalette();
	EStatusCode ReadPhotometricPaletteCMYK();
	void ComposePDFPage();
	void ComposePDFPageOrient(T2P_BOX*, uint16);
	void ComposePDFPageOrientFlip(T2P_BOX*, uint16);
	ObjectIDType WriteTransferFunction(int i);
	ObjectIDType WriteTransferFunctionsExtGState(const ObjectIDTypeList& inTransferFunctions);
	ObjectIDType WritePaletteCS();
	ObjectIDType WriteICCCS();
	void WriteXObjectCS(DictionaryContext* inContainerDictionary);
	void WriteXObjectCALCS();
	PDFImageXObject* WriteTileImageXObject(int inTileIndex);
	bool TileIsRightEdge(int inTileIndex);
	bool TileIsBottomEdge(int inTileIndex);
	void WriteImageXObjectDecode(DictionaryContext* inImageDictionary);
	void WriteImageXObjectFilter(DictionaryContext* inImageDictionary,int inTileIndex);
	void CalculateTiffTileSize(int inTileIndex);
	EStatusCode WriteImageTileData(PDFStream* inImageStream,int inTileIndex);
	void SamplePlanarSeparateToContig(unsigned char* inBuffer, 
									  unsigned char* inSamplebuffer, 
									  tsize_t inSamplebuffersize);
	tsize_t SampleRGBAToRGB(tdata_t inData, uint32 inSampleCount);
	tsize_t SampleRGBAAToRGB(tdata_t inData, uint32 inSampleCount);
	tsize_t	SampleLABSignedToUnsigned(tdata_t inBuffer, uint32 inSampleCount);
	void TileCollapseLeft(tdata_t inBuffer, tsize_t inScanWidth, 
							uint32 inTileWidth, uint32 inEdgeTileWidth, 
							uint32 inTileLength);
	PDFImageXObject* WriteUntiledImageXObject();
	void WriteCommonImageDictionaryProperties(DictionaryContext* inImageContext);
	EStatusCode WriteImageData(PDFStream* inImageStream);
	void CalculateTiffSizeNoTiles();
	void SampleRealizePalette(unsigned char* inBuffer);
	tsize_t SampleABGRToRGB(tdata_t inData, uint32 inSampleCount);
	EStatusCode WriteImageBufferToStream(	PDFStream* inPDFStream,
											uint32 inImageWidth,
											uint32 inImageLength,
											unsigned char* inBuffer,
											ImageSizeProc inBufferSizeFunction);
	PDFFormXObject* WriteImagesFormXObject(const PDFImageXObjectList& inImages);
	void AddImagesProcsets(PDFImageXObject* inImageXObject);
	void WriteIndexedCSForBiLevelColorMap();
};
