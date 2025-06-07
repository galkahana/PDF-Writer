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
    encrypt.cbc_encrypt(dataBuffer, dataBuffer, inData.size(), inIV);

    ByteList result = byteArrayToByteList(dataBuffer, inData.size());

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

long long byteListToLittleEndian(const ByteList& inList) {
    long long result = 0;
    ByteList::const_iterator it = inList.begin();
    for (int i = 0; i < 8 && it != inList.end(); ++i, ++it) {
        result |= ((long long)(*it)) << (8 * i);
    }
    return result;
}

long long byteListToBigEndian(const ByteList& inList) {
    long long result = 0;
    ByteList::const_iterator it = inList.begin();
    for (int i = 0; i < 8 && it != inList.end(); ++i, ++it) {
        result = (result << 8) | ((long long)(*it));
    }
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
    // determine which sha algorithm to use, based on big endian representaiton of E first 16 bytes
    long long eAsNumber = byteListToLittleEndian(substr(E, 0, 16));
    ByteList newK;
    switch(eAsNumber % 3) {
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
    

    // now verify the P value against 0..4 bytes
    long long permsP = byteListToLittleEndian(substr(decryptedPerms, 0, 4));

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



    