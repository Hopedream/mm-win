#pragma once  

#include <string>

using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class CMD5Checksum    
{  
public:  
 static tstring GetMD5OfString(tstring strString);  
 //interface functions for the RSA MD5 calculation  
 static tstring GetMD5(const tstring& strFilePath);  
  
protected:  
 //constructor/destructor  
 CMD5Checksum();  
 virtual ~CMD5Checksum() {};  
  
 //RSA MD5 implementation  
 void Transform(char Block[64]);  
 void Update(char* Input, unsigned long nInputLen);  
 tstring Final();  
 inline unsigned int RotateLeft(unsigned int x, int n);  
 inline void FF( unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T);  
 inline void GG( unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T);  
 inline void HH( unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T);  
 inline void II( unsigned int& A, unsigned intB, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T);  
  
 //utility functions  
 inline void DWordToByte(char* Output, unsigned int* Input, unsigned int nLength);  
 inline void ByteToDWord(unsigned int* Output, char* Input, unsigned int nLength);  
  
private:  
 char  m_lpszBuffer[64];  //input buffer  
 unsigned int m_nCount[2];   //number of bits, modulo 2^64 (lsb first)  
 unsigned int m_lMD5[4];   //MD5 checksum  
};  
