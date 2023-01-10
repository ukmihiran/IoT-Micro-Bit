#ifndef AES_H
#define AES_H
#include <string>
#include <vector>

using namespace std;

string AES_Encrypt(string m, string key);
string AES_Decrypt(string c, string key);


#endif