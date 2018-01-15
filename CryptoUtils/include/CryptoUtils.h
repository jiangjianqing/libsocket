#ifndef CRYPTOUTILS_H
#define CRYPTOUTILS_H

#include <string>

using namespace std;

class CryptoUtils
{

public:
    CryptoUtils();

    static void md5(const string &srcStr, string &encodedStr, string &encodedHexStr);
    static void sha256(const std::string &srcStr, std::string &encodedStr, std::string &encodedHexStr);
};

#endif // CRYPTOUTILS_H
