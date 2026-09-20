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

#ifndef PDFHUMMUS_NO_OPENSSL

#include <openssl/rand.h>

using namespace PDFHummus;

static int (*sRandBytesFunc)(unsigned char*, int) = RAND_bytes;

void RandomGenerator::SetRandBytesFunc(int (*inRandBytesFunc)(unsigned char*, int)) {
	sRandBytesFunc = inRandBytesFunc ? inRandBytesFunc : RAND_bytes;
}

EStatusCode RandomGenerator::FillBytes(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	if (sRandBytesFunc(outBuffer, (int)inSize) == 1)
		return eSuccess;

	TRACE_LOG("RandomGenerator::FillBytes, RAND_bytes failed");
	return eFailure;
}

#else // PDFHUMMUS_NO_OPENSSL

#if defined(_WIN32) && !defined(PDFHUMMUS_NO_BCRYPT)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#include <stdlib.h> // arc4random_buf
#elif defined(__unix__) || defined(__unix) || defined(unix)
#include <unistd.h>
#ifndef PDFHUMMUS_HAVE_GETENTROPY
#include <errno.h>
#include <fcntl.h>
#endif
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

#ifdef PDFHUMMUS_HAVE_GETENTROPY

static bool FillWithPlatformCSPRNG(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	// getentropy() caps at 256 bytes per call - fine here, callers only ever ask for a
	// handful of bytes (AES IVs, PDF 2.0 key material).
	return getentropy(outBuffer, inSize) == 0;
}

#else // PDFHUMMUS_HAVE_GETENTROPY

static const int scMaxRetries = 10;

static bool FillWithPlatformCSPRNG(IOBasicTypes::Byte* outBuffer, size_t inSize)
{
	int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		return false;

	size_t totalRead = 0;
	int retries = 0;
	bool success = true;
	while (totalRead < inSize)
	{
		ssize_t readSize = read(fd, outBuffer + totalRead, inSize - totalRead);
		if (readSize < 0 && errno == EINTR)
		{
			if (++retries > scMaxRetries)
			{
				TRACE_LOG("RandomGenerator::FillWithPlatformCSPRNG, exceeded max retries after repeated EINTR on /dev/urandom read");
				success = false;
				break;
			}
			continue; // interrupted by a signal, not an actual failure - retry
		}

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

#endif // PDFHUMMUS_HAVE_GETENTROPY

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

#endif // PDFHUMMUS_NO_OPENSSL
