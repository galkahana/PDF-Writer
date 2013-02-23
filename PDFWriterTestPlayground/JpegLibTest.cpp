/*
 Source File : JpegLibTest
 
 
 Copyright 2012 Gal Kahana PDFWriter
 
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
#ifndef PDFHUMMUS_NO_DCT

#include "JpegLibTest.h"
#include "TestsRunner.h"
#include "Trace.h"
#include "InputFile.h"

#include "jpeglib.h"

using namespace PDFHummus;

JpegLibTest::JpegLibTest(void)
{
    
}

JpegLibTest::~JpegLibTest(void)
{
    
}

class HummusJPGException
{
public:
    HummusJPGException(){}
    ~HummusJPGException(){}
    
};

METHODDEF(void) HummusJPGErrorExit (j_common_ptr cinfo)
{
    (*cinfo->err->output_message) (cinfo);
    throw HummusJPGException();
}

METHODDEF(void) HummusJPGOutputMessage(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    
    (*cinfo->err->format_message) (cinfo, buffer);
    TRACE_LOG1("HummusJPGOutputMessage, error from jpg library: %s",buffer);
}

struct HummusSourceManager
{
    struct jpeg_source_mgr pub;	/* public fields */
    
    IByteReader *mReader;	/* source stream */
    JOCTET * buffer;		/* start of buffer */
};

#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */

METHODDEF(boolean) HummusFillInputBuffer (j_decompress_ptr cinfo)
{
    HummusSourceManager* src = (HummusSourceManager*) cinfo->src;
    size_t nbytes;
    
    nbytes =  src->mReader->Read((Byte*)(src->buffer),INPUT_BUF_SIZE);
    
    if (nbytes <= 0) {
        /* Insert a fake EOI marker */
        src->buffer[0] = (JOCTET) 0xFF;
        src->buffer[1] = (JOCTET) JPEG_EOI;
        nbytes = 2;
    }
    
    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    
    return TRUE;
}

METHODDEF(void) HummusSkipInputData (j_decompress_ptr cinfo, long num_bytes)
{
    
    struct jpeg_source_mgr * src = cinfo->src;
    
     if (num_bytes > 0) {
        while (num_bytes > (long) src->bytes_in_buffer) {
            num_bytes -= (long) src->bytes_in_buffer;
            (void) (*src->fill_input_buffer) (cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
             * so suspension need not be handled.
             */
        }
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

METHODDEF(void) HummusNoOp (j_decompress_ptr cinfo)
{
    /* no work necessary here */
}

GLOBAL(void) HummusJPGSourceInitialization (j_decompress_ptr cinfo, IByteReader * inSourceStream)
{
    HummusSourceManager* src;
    
    /* The source object and input buffer are made permanent so that a series
     * of JPEG images can be read from the same file by calling jpeg_stdio_src
     * only before the first one.  (If we discarded the buffer at the end of
     * one image, we'd likely lose the start of the next one.)
     * This makes it unsafe to use this manager and a different source
     * manager serially with the same JPEG object.  Caveat programmer.
     */
    if (cinfo->src == NULL) {	/* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                    sizeof(HummusSourceManager));
        src = (HummusSourceManager*) cinfo->src;
        src->buffer = (JOCTET *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                    INPUT_BUF_SIZE * sizeof(JOCTET));
    }
    
    src = (HummusSourceManager*) cinfo->src;
    src->pub.init_source = HummusNoOp;
    src->pub.fill_input_buffer = HummusFillInputBuffer;
    src->pub.skip_input_data = HummusSkipInputData;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = HummusNoOp;
    src->mReader = inSourceStream;
    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    src->pub.next_input_byte = NULL; /* until buffer loaded */
}


EStatusCode JpegLibTest::Run(const TestConfiguration& inTestConfiguration)
{
    
    EStatusCode status = eSuccess;
    jpeg_decompress_struct cinfo;

    do {
        InputFile aFile;
        
        aFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/soundcloud_logo.jpg"));
        
        jpeg_error_mgr jerr;
        
        cinfo.err = jpeg_std_error(&jerr);
        jerr.error_exit = HummusJPGErrorExit;
        jerr.output_message = HummusJPGOutputMessage;
        
        try {
            jpeg_create_decompress(&cinfo);
            
            HummusJPGSourceInitialization(&cinfo,aFile.GetInputStream());
            
            jpeg_read_header(&cinfo, TRUE);
            jpeg_start_decompress(&cinfo);
            
            int row_stride = cinfo.output_width * cinfo.output_components;
            JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.rec_outbuf_height);
            
            while (cinfo.output_scanline < cinfo.output_height) {
                jpeg_read_scanlines(&cinfo, buffer, cinfo.rec_outbuf_height);
                //put_scanline_someplace(buffer[0], row_stride);
            }
            
            jpeg_finish_decompress(&cinfo);
        } catch (HummusJPGException ex) {
            status = eFailure;
            TRACE_LOG("JpegLibTest::Run, caught jpg library exception");
        }
        
        jpeg_destroy_decompress(&cinfo);
    } while (false);
    
    return status;
}



ADD_CATEGORIZED_TEST(JpegLibTest,"DCTDecode")

#endif