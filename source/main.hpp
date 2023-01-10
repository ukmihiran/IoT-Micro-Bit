#include "MicroBit.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "Sha256.h"
#include "AES.h"

using namespace std;
typedef unsigned char BYTE;

//Prototypes
MicroBit uBit;
int select_Mode();
void SaltGen(char *salt);
void CreateDPK(char *Secret, char *salt, char* dpk);
void SendMode();
void SendCommand(const char *command, char *salt, char *dpk);
void ReceivMode();
void data_split(char *s, int index, char *first, char *second);
void Fan_request();
void compass();
void led_bulb();


//Globle varibale
const int SaltLength = 6;
char Secret[9] = "UWECyber";
const char *command1 = "udesh";
const char *command2 = "pamoda";
const char *command3 = "thusitha";


