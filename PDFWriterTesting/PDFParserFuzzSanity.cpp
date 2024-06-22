#include "PDFParser.h"
#include "InputFile.h"
#include "Trace.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

int PDFParserFuzzSanity(int argc, char* argv[]) {
    PDFParser parser;
    InputFile pdfFile;

    std::string path = argv[3];

    EStatusCode status = pdfFile.OpenFile(path);
    if(status != PDFHummus::eSuccess)
    {
        cout<<"unable to open file for reading, Path:"<<path.c_str()<<"\n";
        return 1;
    }

    // traces on
    Trace::DefaultTrace().SetLogSettings(BuildRelativeOutputPath(argv,  argv[4]), true, true);

    // checks that returns, status doesn't matter
    parser.StartPDFParsing(pdfFile.GetInputStream());

    return status;
}
