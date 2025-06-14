/*
   Source File : XCryptionCommon2_0.cpp


   Copyright 2025 Gal Kahana PDFWriter

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
#ifndef PDFHUMMUS_NO_OPENSSL
#include "XCryptionCommon2_0.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "aescpp.h"

#include <utility>
#include <vector>

#define MAX_PASSWORD_LENGTH 127


using namespace IOBasicTypes;
using namespace std;


XCryptionCommon2_0::XCryptionCommon2_0(void)
{
}

XCryptionCommon2_0::~XCryptionCommon2_0(void)
{
}

static ByteList getOUHashValue(const ByteList& inKey) {
    return substr(inKey, 0, 32);
}

static ByteList getOUValidationSalt(const ByteList& inKey) {
    return substr(inKey, 32, 8);
}

static ByteList getOUKeySalt(const ByteList& inKey) {
    return substr(inKey, 40, 8);
}

static Byte* byteListToNewByteArray(const ByteList& inList) {
    Byte* buffer = new Byte[inList.size()];
    ByteList::const_iterator it = inList.begin();
    for(int i = 0; it != inList.end(); ++i, ++it) {
        buffer[i] = *it;
    }
    return buffer;
}

static ByteList byteArrayToByteList(const Byte* inArray, size_t inSize) {
    ByteList result;
    for(size_t i = 0; i < inSize; ++i) {
        result.push_back(inArray[i]);
    }
    return result;
}



static ByteList createSHA256(const ByteList& inKey) {
    ByteList result;

    Byte* buffer = byteListToNewByteArray(inKey);
    Byte bufferResult[SHA256_DIGEST_LENGTH];


    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, buffer, inKey.size());
    SHA256_Final(bufferResult, &ctx);

    delete[] buffer;
    return byteArrayToByteList(bufferResult, SHA256_DIGEST_LENGTH);
}


static ByteList createSHA384(const ByteList& inKey) {
    ByteList result;

    Byte* buffer = byteListToNewByteArray(inKey);
    Byte bufferResult[SHA384_DIGEST_LENGTH];

    SHA512_CTX ctx;
    SHA384_Init(&ctx);
    SHA384_Update(&ctx, buffer, inKey.size());
    SHA384_Final(bufferResult, &ctx);

    delete[] buffer;
    return byteArrayToByteList(bufferResult, SHA384_DIGEST_LENGTH);
}

static ByteList createSHA512(const ByteList& inKey) {
    ByteList result;

    Byte* buffer = byteListToNewByteArray(inKey);
    Byte bufferResult[SHA512_DIGEST_LENGTH];

    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, buffer, inKey.size());
    SHA512_Final(bufferResult, &ctx);

    delete[] buffer;
    return byteArrayToByteList(bufferResult, SHA512_DIGEST_LENGTH);

    return result;
}

static ByteList encryptKeyCBC(const ByteList& inKey, const ByteList& inData, Byte* inIV) {
    AESencrypt encrypt;
    Byte* keyBuffer = byteListToNewByteArray(inKey);
    Byte* dataBuffer = byteListToNewByteArray(inData);
    Byte* outBuffer = new Byte[inData.size()];

    encrypt.key(keyBuffer, inKey.size());
    encrypt.cbc_encrypt(dataBuffer, outBuffer, inData.size(), inIV);

    ByteList result = byteArrayToByteList(outBuffer, inData.size());

    delete[] keyBuffer;
    delete[] dataBuffer;
    delete[] outBuffer;
    
    return result;
}

static ByteList encryptKeyCBCZeroIV(const ByteList& inKey, const ByteList& inData) {
    Byte zeroIV[AES_BLOCK_SIZE] = { // this is NOT a const. IV is used internally by AESencrypt, so it needs to be mutable
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
    };

    return encryptKeyCBC(inKey, inData, zeroIV);
}

static ByteList encryptKeyCBC(const ByteList& inKey, const ByteList& inData, const ByteList& inIV) {
    Byte* ivBuffer = byteListToNewByteArray(inIV);
    ByteList result = encryptKeyCBC(inKey, inData, ivBuffer);
    delete[] ivBuffer;
    return result;
}

static ByteList encryptKeyECB(const ByteList& inKey, const ByteList& inData) {
    AESencrypt encrypt;
    Byte* keyBuffer = byteListToNewByteArray(inKey);
    Byte* dataBuffer = byteListToNewByteArray(inData);
    Byte* outBuffer = new Byte[inData.size()];

    encrypt.key(keyBuffer, inKey.size());
    encrypt.ecb_encrypt(dataBuffer, outBuffer, inData.size());

    ByteList result = byteArrayToByteList(outBuffer, inData.size());

    delete[] keyBuffer;
    delete[] dataBuffer;
    delete[] outBuffer;

    return result;
}

static ByteListPair createHashRoundKey(const ByteList& inK, const ByteList& inTrimmedPassword, const ByteList& inAdditionalKey) {
    ByteList K1;  

    ByteList sequence = concat(inTrimmedPassword, inK);
    if(inAdditionalKey.size() > 0) {
        sequence = concat(sequence, inAdditionalKey);
    }
    for(int i = 0; i< 64; ++i) {
        append(K1, sequence);
    }

    ByteList E = encryptKeyCBC(substr(inK,0,16), K1, substr(inK, 16, 16));
    // the specs here wants to use the first 16 bytes of E as a big endian number and go mod 3. 
    // problem is that 16 bytes of a number is hugh and kinda doesn't fit into long long, or any of the simple int types.
    // fortunately (as is pointed out in QPDF and others...but originally i read it there - https://github.com/qpdf/qpdf/blob/main/libqpdf/QPDF_encryption.cc#L286)
    // If you just want to get the mod 3, then just sum all the bytes as ints and mod that and you got the same result. 
    // here's why:
    // x = a1*256^15 + a2*256^14 + ... + a16*256^0
    // (1) (y + z) mod n = ((y mod n) + (z mod n)) mod n
    // [why? (k1*n+rem1 + k2*n + rem2) mod n = (rem1 + rem2) mod n, because k1*n and k2*n don't contribute to the mod, and so only the remainders do]
    // so x mod 3 can become:
    // (a1*256^15 mod 3 + a2*256^14 mod 3 + ... + a16*256^0 mod 3) mod 3
    //
    // now 256 mod 3 = 1, so 256^n mod 3 = 1 for any n. this means ai*256^n mod 3 = ai mod 3. here's why:
    // 256^n is some 3k+1. ai*(3k+1) = ai*3k + ai. let's consider mod 3 of that. ai*3K is dividable by 3, so it doesn't contribute to mod 3, and so it's just ai mod 3. 
    // Summing all of them items up with this in mind we get to (a1*256^15 mod 3 + a2*256^14 mod 3 + ... + a16*256^0 mod 3) = (a1 mod 3 + a2 mod 3 + ... + a16 mod 3).
    //
    // obviously if x = y then also x mod 3 = y mod 3, so add them mods to each side.
    // so (a1 mod 3 + a2 mod 3 + ... + a16 mod 3) mod 3 = (a1*256^15 mod 3 + a2*256^14 mod 3 + ... + a16*256^0 mod 3) mod 3 = x mod 3.
    // as to why we can just sum and mod 3 and don't need to mod each item and finally mod 3 like the left side here does, that's back to (1) above.
    // which means (a1 + a2 + ... + a16) mod 3 = x mod 3.
    long long result = 0;
    ByteList::const_iterator itE = E.begin();
    for (int i=0; i<16;++i, ++itE) {
        result += (*itE);
    }
    Byte eMod3 = result % 3;

    ByteList newK;
    switch(eMod3) {
        case 0:
            newK = createSHA256(E);
            break;
        case 1:
            newK = createSHA384(E);
            break;
        default:
            // case 2
            newK = createSHA512(E);
    }

    return ByteListPair(newK, E);
}


static ByteList createHash(const ByteList& inKey, const ByteList& inTrimmedPassword, const ByteList& inAdditionalKey) {
    // Per algorithm 2.B (section 7.6.4.3.4)
    ByteList K = createSHA256(inKey);
    ByteList E;
    ByteListPair roundResult;

    // minimal rounds
    int i = 0;
    for(; i < 64; ++i) {
        roundResult = createHashRoundKey(K, inTrimmedPassword, inAdditionalKey);
        K = roundResult.first;
        E = roundResult.second;
    }

    // starting from round 64, each round end comes with a validation (i just love iterations which end condition is based on fairly random data)
    while(E.back() > i - 32) {
        roundResult = createHashRoundKey(K, inTrimmedPassword, inAdditionalKey);
        K = roundResult.first;
        E = roundResult.second;        
        ++i;
    }

    return substr(K, 0, 32);  // done here (hurrah!)
}



static ByteList decryptKeyCBCZeroIV(const ByteList& inKey, const ByteList& inEncryptedKey) {
    AESdecrypt decryptor;
    unsigned char iv[AES_BLOCK_SIZE] = { // this is NOT a const. IV is used internally by AESdecrypt, so it needs to be mutable
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
    };
    Byte* keyBuffer = byteListToNewByteArray(inKey);
    Byte* encryptedKeyBuffer = byteListToNewByteArray(inEncryptedKey);
    Byte* outputBuffer = new Byte[inEncryptedKey.size()];

    decryptor.key(keyBuffer, inKey.size());
    decryptor.cbc_decrypt(encryptedKeyBuffer, outputBuffer, inEncryptedKey.size(), iv);
    ByteList result = byteArrayToByteList(outputBuffer, inEncryptedKey.size());

    delete[] keyBuffer;
    delete[] encryptedKeyBuffer;
    delete[] outputBuffer;

    return result;
}

static ByteList decryptKeyECB(const ByteList& inKey, const ByteList& inEncryptedKey) {
    AESdecrypt decryptor;

    Byte* keyBuffer = byteListToNewByteArray(inKey);
    Byte* encryptedKeyBuffer = byteListToNewByteArray(inEncryptedKey);
    Byte* outputBuffer = new Byte[inEncryptedKey.size()];

    decryptor.key(keyBuffer, inKey.size());
    decryptor.ecb_decrypt(encryptedKeyBuffer, outputBuffer, inEncryptedKey.size());
    ByteList result = byteArrayToByteList(outputBuffer, inEncryptedKey.size());

    delete[] keyBuffer;
    delete[] encryptedKeyBuffer;
    delete[] outputBuffer;

    return result;
}

static ByteList trimPassword(const ByteList& inPassword) {
    // trim password to MAX_PASSWORD_LENGTH
    return inPassword.size() > MAX_PASSWORD_LENGTH ? substr(inPassword, 0, MAX_PASSWORD_LENGTH) : inPassword;
}

static bool authenticateOwnerPassword(
    const ByteList& inTrimmedPassword,
    const ByteList& inO,
    const ByteList& inU
) {
    // password here is assumed to be already trimmed
    ByteList keyToHash = concat(concat(inTrimmedPassword, getOUValidationSalt(inO)), inU);
    ByteList hashedOwnerTest = createHash(keyToHash, inTrimmedPassword, inU);
    return hashedOwnerTest == getOUHashValue(inO);
}


template <size_t N>
static ByteList generateRandomBytes() {
    unsigned char buffer[N];

    // trying to use openssl ran bytes
    if (RAND_bytes(buffer, N) == 1) {
        // awesome. return as a result
        return ByteList(buffer, buffer + N);
    } else {
        // fallback (hopefully rare) using simple rand
        ByteList result;
        for (size_t i = 0; i < N; ++i) {
            result.push_back(static_cast<Byte>(rand() % 256));
        }
        return result;
    }
}
static ByteList generate32RandomBytes() {
    return generateRandomBytes<32>();
}
static ByteList generate16RandomBytes() {
    return generateRandomBytes<16>();
}
static ByteList generate4RandomBytes() {
    return generateRandomBytes<4>();
}

static const ByteList scEmptyByteList;

/**
 * V5/PDF2.0 creating file encryption key based on input password and document encryption parameters.
 * Thie is Algorithm 2.A (section 7.6.4.3.3), excluding the part verifying Perms, which seems optional. 
 * Use AuthenticatePerms for that.
 */
ByteList XCryptionCommon2_0::RetrieveFileEncryptionKey(
    const ByteList& inPassword,
    const ByteList& inO,
    const ByteList& inU,
    const ByteList& inOE,
    const ByteList& inUE
) {
    ByteList trimmedPassword = trimPassword(inPassword);
    
    if (authenticateOwnerPassword(trimmedPassword, inO, inU)) {
        // owner password, compute owner encryption key
        ByteList intermediateOWnerKey = createHash(concat(concat(trimmedPassword, getOUKeySalt(inO)), inU), trimmedPassword, inU);
        return decryptKeyCBCZeroIV(intermediateOWnerKey, inOE);
    } else {
        // user password, compute user encryption key
        ByteList intermediateUserKey = createHash(concat(trimmedPassword, getOUKeySalt(inU)), trimmedPassword, scEmptyByteList);
        return decryptKeyCBCZeroIV(intermediateUserKey, inUE);

    }
}


static const Byte scA = 'a';
static const Byte scD = 'd';
static const Byte scB = 'b';

bool XCryptionCommon2_0::AuthenticaePerms(
    const ByteList& inFileEncryptionKey,
    const ByteList& inPerms,
    long long inP
) {
    ByteList decryptedPerms = decryptKeyECB(inFileEncryptionKey, inPerms);
    ByteList::const_iterator it = decryptedPerms.begin();

    // move forward till placement 9
    for (int i = 0; i < 9 && it != decryptedPerms.end(); ++i, ++it);

    // now validate bytes 9 to 11 against 'a', 'd', 'b'
    if (it == decryptedPerms.end() || *it != scA) {
        return false;
    }
    ++it;
    if (it == decryptedPerms.end() || *it != scD) {
        return false;
    }
    ++it;
    if (it == decryptedPerms.end() || *it != scB) {
        return false;
    }
    

    // now verify the P value against 0..4 bytes representing little endian long long
    long long permsP = 0;
    it = decryptedPerms.begin();
    for(int i = 0; i < 4; ++i, ++it) {
         permsP |= ((long long)(*it)) << (8 * i);
    }

    if (permsP != inP) {
        return false;
    }


    return true;

}

bool XCryptionCommon2_0::AuthenticateUserPassword(
    const ByteList& inPassword,
    const ByteList& inU
) {
    ByteList trimmedPassword = trimPassword(inPassword);
    ByteList keyToHash = concat(trimmedPassword, getOUValidationSalt(inU));
    ByteList hashedUserTest = createHash(keyToHash, trimmedPassword, scEmptyByteList);
    return hashedUserTest == getOUHashValue(inU);
}

bool XCryptionCommon2_0::AuthenticateOwnerPassword(
    const ByteList& inPassword,
    const ByteList& inO,
    const ByteList& inU
) {
    return authenticateOwnerPassword(trimPassword(inPassword), inO, inU);
}


ByteList XCryptionCommon2_0::GenerateFileEncryptionKey() {
    // just a 32 byte randomly generated sequance of bytes
    return generate32RandomBytes();   
}

ByteListPair XCryptionCommon2_0::CreateUandUEValues(
    const ByteList& inUserPassword,
    const ByteList& inFileEncryptionKey    
) {
    // per algoritm 8 (section 7.6.4.4.7)

    ByteList trimmedPassword = trimPassword(inUserPassword);

    // generate U
    ByteList randomBytes = generate16RandomBytes();
    ByteList userValidationSalt = substr(randomBytes, 0, 8);
    ByteList userKeySalt = substr(randomBytes, 8, 8);
    ByteList u = createHash(concat(trimmedPassword, userValidationSalt), trimmedPassword, scEmptyByteList);
    append(u, userValidationSalt);
    append(u, userKeySalt);

    // encrypt encryption key to get UE
    ByteList intermediateUserKey = createHash(concat(trimmedPassword, userKeySalt), trimmedPassword, scEmptyByteList);
    ByteList ue = encryptKeyCBCZeroIV(intermediateUserKey, inFileEncryptionKey);

    return ByteListPair(u, ue);


}

ByteListPair XCryptionCommon2_0::CreateOandOEValues(
    const ByteList& inOwnerPassword,
    const ByteList& inFileEncryptionKey,
    const ByteList& inU
) {
    // per algoritm 9 (section 7.6.4.8)
    ByteList trimmedPassword = trimPassword(inOwnerPassword);

    // generate O
    ByteList randomBytes = generate16RandomBytes();
    ByteList ownerValidationSalt = substr(randomBytes, 0, 8);
    ByteList ownerKeySalt = substr(randomBytes, 8, 8);
    ByteList o = createHash(concat(concat(trimmedPassword, ownerValidationSalt), inU), trimmedPassword, inU);

    append(o, ownerValidationSalt);
    append(o, ownerKeySalt);

    // encrypt encryption key to get OE
    ByteList intermediateOwnerKey = createHash(concat(concat(trimmedPassword, ownerKeySalt), inU), trimmedPassword, inU);
    ByteList oe = encryptKeyCBCZeroIV(intermediateOwnerKey, inFileEncryptionKey);

    return ByteListPair(o, oe);
}

static const Byte scT = 'T';
static const Byte scF = 'F';

ByteList XCryptionCommon2_0::CreatePerms(
    const ByteList& inFileEncryptionKey,
    long long inP,
    bool inEncryptMetadata
) {
    // per algoritm 10 (section 7.6.4.9)
    ByteList perms;

    // start with the 4 bytes of P, little endian (so low order first)
    for(int i = 0; i < 4; ++i) {
        perms.push_back(Byte(inP & 0xFF));
        inP >>= 8; // we can "mutate" inP, will not be used later
    }

    // fill the next 4 bytes with 0xFF
    for(int i = 0; i < 4; ++i) {
        perms.push_back(0xFF);
    }

    // now "T" or "F", based on inEncryptMetadata
    perms.push_back(inEncryptMetadata ? scT : scF);

    // 'a', 'd', 'b' bytes
    perms.push_back(scA);
    perms.push_back(scD);
    perms.push_back(scB);

    // 12-15 are intended to be "random data, which will be ignored".
    // i was thinking of using "hmms" but then figured let's leave less traces, so random bytes it is.
    ByteList randomBytes = generate4RandomBytes();
    append(perms, randomBytes);


    return encryptKeyECB(inFileEncryptionKey, perms);
}

#endif // PDFHUMMUS_NO_OPENSSL