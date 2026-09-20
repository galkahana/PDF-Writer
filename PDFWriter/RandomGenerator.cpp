/*
   Source File : RandomGenerator.cpp


   Copyright 2026 Gal Kahana PDFWriter

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
#include "RandomGenerator.h"
#include "Trace.h"

#if defined(_WIN32) && !defined(PDFHUMMUS_NO_BCRYPT)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#include <stdlib.h> // arc4random_buf
#elif defined(__unix__) || defined(__unix) || defined(unix)
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace PDFHummus;

#if defined(_WIN32) && !defined(PDFHUMMUS_NO_BCRYPT)

static bool FillWithPlatformCSPRNG(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	NTSTATUS status = BCryptGenRandom(NULL, (PUCHAR)outBuffer, (ULONG)inSize, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	return BCRYPT_SUCCESS(status);
}

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)

static bool FillWithPlatformCSPRNG(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	arc4random_buf(outBuffer, inSize);
	return true;
}

#elif defined(__unix__) || defined(__unix) || defined(unix)

static bool FillWithPlatformCSPRNG(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return false;

	size_t totalRead = 0;
	bool success = true;
	while (totalRead < inSize)
	{
		ssize_t readSize = read(fd, outBuffer + totalRead, inSize - totalRead);
		if (readSize <= 0)
		{
			success = false;
			break;
		}
		totalRead += (size_t)readSize;
	}

	close(fd);
	return success;
}

#else

static bool FillWithPlatformCSPRNG(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	(void)outBuffer;
	(void)inSize;
	return false;
}

#endif

EStatusCode RandomGenerator::FillBytes(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	if (FillWithPlatformCSPRNG(outBuffer, inSize))
		return eSuccess;

	// Failing if CSPRNG is NOT available. If this is a problem please report an issue, and it'll be considered
	// to add a softer pseudo random path option.
	TRACE_LOG("RandomGenerator::FillBytes, no platform CSPRNG available, cannot create true random bytes, failing");
	return eFailure;
}
