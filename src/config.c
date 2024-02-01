#include "header.h"
#include <sqlite3.h>

#ifndef bool
typedef int bool;
#define true 1
#define false 0

char* user_name;
char* user_country;
char* user_phone;
char* account_type;
char* account_number;
char* deposit_amount;

float calculateInterest(float amount, float rate, char* accType) {
    //return amount * (1 + rate / 100);
    //float monthlyRate = rate / 100 / 12;
    //return amount * monthlyRate;
    if (strcmp(accType, "fixed01") == 0)
        return amount * (rate / 100);
    else if (strcmp(accType, "fixed02") == 0)
        return (amount * (rate / 100)) * 2;
    else if (strcmp(accType, "fixed03") == 0)
        return (amount * (rate / 100)) * 3;
    else if (strcmp(accType, "saving") == 0)
        return amount  * (rate / 100 / 12);
}

void displayInterest(double amount, float rate, char* accType, int dayOfMonth, char* month, int year) {
    float interest = calculateInterest(amount, rate, accType) + 0.01;
    if (strcmp(accType, "saving") == 0)
        printf("\nYou will get a $%.2f as interest on day %d of every month.\n", interest, dayOfMonth);
    else if (strcmp(accType, "fixed01") == 0)
        printf("\nYou will get a $%.2f as interest on %d-%s-%d.\n", interest, dayOfMonth, month, year+1);
    else if (strcmp(accType, "fixed02") == 0)
        printf("\nYou will get a $%.2f as interest on %d-%s-%d.\n", interest, dayOfMonth, month, year+2);
    else if (strcmp(accType, "fixed03") == 0)
        printf("\nYou will get a $%.2f as interest on %d-%s-%d.\n", interest, dayOfMonth, month, year+3);
}

void setDepositAmount(char* dep) {
    float amount;
    char *endptr;
    amount = strtof(dep, &endptr);
    if (*endptr != '\0') {
        PrintError("Invalid data type entered!");
    }
    deposit_amount = dep;
}

char* getDepositAmount() {
    return deposit_amount;
}

void setAccNum(char* num) {
    if (!isNumber(num)) {
        PrintError("Invalid data type entered!");
    } else {
        if (atoi(num) > 999999999 && atoi(num) < 0) {
            PrintError("Inavlid account number entered!");
        }
    }
    account_number = num;
}

char* getAccNum() {
    return account_number;
}

void setUserCountry(char* c) {
    if (strlen(c) > 50) {
        PrintError("Country name entered too long!");
    }
    user_country = c;
}

char* getUserCountry() {
    return user_country;
}

void setUserPhone(char* p) {
    if (!isNumber(p)) {
        PrintError("Invalid data type entered!");
    }
    int phNum = atoi(p);
    if (countDigits(phNum) < 5 && countDigits(phNum) > 10) {
        PrintError("Inavlid phone number entered!");
    }
    user_phone = p;
}

char* getUserPhone() {
    return user_phone;
}

void setAccType(char* acc) {
    account_type = acc;
}

char* getAccType() {
    return account_type;
}

void setUserLogged(char* name) {
    user_name = name;
}

char* getUserLogged() {
    return user_name;
}

float parseFloat(char* str) {
    float amount;
    char *endptr;
    amount = strtof(str, &endptr);
    if (*endptr != '\0') {
        PrintError("Failed to parse float value");
    }
    return amount;
}

void stringToLower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void returnMenu() {
    printf("\n\t\tEnter 1 to go to main menu and 0 to exit: ");
    int aOp;
    scanf("%d", &aOp);
    //sqlite3_close();
    switch (aOp)
    {
    case 1:
        dashboard();
        break;
    default:
        ExitProgram();
        break;
    }
}

char* toLower(char* str) {
    for (char *i=str; *i; i++) *i = tolower(*i);
    return str;
}

bool isDateValid(int yyyy, int mm, int dd) {
    if (yyyy >= 1900 && yyyy <= 9999) {
        //check month
        if(mm>=1 && mm<=12)
        {
            //check days
            if((dd>=1 && dd<=31) && (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12))
                return true;
            else if((dd>=1 && dd<=30) && (mm==4 || mm==6 || mm==9 || mm==11))
                return true;
            else if((dd>=1 && dd<=28) && (mm==2))
                return true;
            else if(dd==29 && mm==2 && (yyyy%400==0 ||(yyyy%4==0 && yyyy%100!=0)))
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    } else {
        return false;
    }
}

bool isNumber(char* nbr) {
    for (int i = 0; i < strlen(nbr); i++) {
        if (!isdigit(nbr[i])) {
            return false;
        }
    }
    return true;
}

#endif