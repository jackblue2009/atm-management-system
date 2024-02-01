#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HEADER_H
#define HEADER_H
#define MAX_LENGTH 50
#define MAX_DIGITS_PHONE 8
#define MAX_DIGITS 12
#ifndef bool
typedef int bool;
#define true 1
#define false 0

// // all fields for each record of an account
struct Record
{
    int id;
    char userId[20];
    char name[MAX_LENGTH];
    char accountNbr[MAX_LENGTH];
    char country[MAX_LENGTH];
    char phone[MAX_DIGITS_PHONE];
    char deposit[MAX_DIGITS];
    char withdraw[MAX_DIGITS];
    //double amount;
    char accountType[10];
    char dateCreated[MAX_LENGTH];
};

struct User
{
    int id;
    char name[MAX_LENGTH];
    char password[MAX_LENGTH];
};

sqlite3* getDB();
void initMenu();
void logMenu();
// system function
void createNewAcc();
void regMenu();
void dashboard();
void checkAllAccounts();
void updateAccountElement(char* col, int accId);

int getUserId(sqlite3* db, const char* uName);

void PrintError(char* errMsg);
void ExitProgram();
int countDigits(int num);
void stringToLower(char *str);
bool isNumber(char* nbr);
const char* getLastValue(char* str, char deli);
char* getUserLogged();
char* toLower(char* str);

void displayInterest(double amount, float rate, char* accType, int dayOfMonth, char* month, int year);
float parseFloat(char* str);
void returnMenu();
bool isDateValid(int yyyy, int mm, int dd);

#endif
#endif