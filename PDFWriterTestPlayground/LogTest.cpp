#include "LogTest.h"
#include "Log.h"
#include "Trace.h"

LogTest::LogTest(void)
{
}

LogTest::~LogTest(void)
{
}

EStatusCode LogTest::Run()
{
	Log log(L"C:\\PDFLibTests\\logTest.txt");

	log.LogEntry(L"testing wide string input");

	wchar_t* aWideString = L"testing Byte input";

	log.LogEntry((const Byte*)aWideString,wcslen(aWideString)*2);


	Trace trace;

	trace.SetLogSettings(L"C:\\PDFLibTests\\traceTest.txt",false);
	trace.TraceToLog(L"Tracing number %d %d",10,20);
	trace.TraceToLog(L"Tracing some other items %s 0x%x",L"hello",20);

	return eSuccess;
}

ADD_CETEGORIZED_TEST(LogTest,"IO")