#include <termios.h>
#include "header.h"
#include <sqlite3.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <crypt.h>
#include <time.h>
#include <stdbool.h>
#include "config.c"

#define MAX_LENGTH_USERNAME 50
#define MAX_LENGTH_PASSWORD 50
#define MAX_STRING_LENGTH 50
#define SHIFT 3
#define HASHKEYPASS "Bucheeri1994Encryption"
#define MAXUSERS 100
#define MAX_COLUMNS

// #define MAX_CONNECTIONS 10

// typedef struct {
//     sqlite3* db;
//     bool in_use;
// } DBConnection;

// DBConnection connections[MAX_CONNECTIONS];

// void initConnectionPool() {
//     for (int i = 0; i < MAX_CONNECTIONS; i++) {
//         connections[i].db = NULL;
//         connections[i].in_use = false;
//     }
// }

// sqlite3* getConnection() {
//     for (int i = 0; i < MAX_CONNECTIONS; i++) {
//         if (!connections[i].in_use) {
//             if (sqlite3_open(dbName, &connections[i].db) == SQLITE_OK) {
//                 connections[i].in_use = true;
//                 return connections[i].db;
//             } else {
//                 return NULL;
//             }
//         }
//     }
//     //No Available connections
//     return NULL;
// }

// void releaseConnection(sqlite3* db) {
//     for (int i = 0; i < MAX_CONNECTIONS; i++) {
//         if (connections[i].db == db) {
//             sqlite3_close(db);
//             connections[i].in_use = false;
//             break;
//         }
//     }
// }

const char* dbName = "src/atm-db.db";
sqlite3* global_db = NULL;

int openDB()
{
    if (global_db != NULL) {
        fprintf(stderr, "Database connection is already open!\n");
        return SQLITE_OK;
    }
    int rc = sqlite3_open(dbName, &global_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database!\n");
        return rc;
    }
    return SQLITE_OK;
}

int closeDB() {
    if (global_db == NULL) {
        fprintf(stderr, "Database connection is already closed!\n");
        return SQLITE_OK;
    }
    int rc = sqlite3_close(global_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error closing database!\n");
        return rc;
    }
    global_db = NULL;
    return SQLITE_OK;
}

sqlite3* getGlobalDB() {
    if (global_db == NULL) {
        fprintf(stderr, "Database connection is not open!\n");
        return NULL;
    }
    return global_db;
}
//int insertNewRecordData(sqlite3* db, char* uId, char* name, char* accNum, char* coun, char* ph, char* depo, char* accType)
int insertNewRecordData(sqlite3* db, struct Record *rcrd) {
    char sql[1000];
    snprintf(sql, sizeof(sql), "INSERT INTO RECORDS (r_id, u_id, u_name, account_num, country, phone, balance, type_account, date_created) VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?);");

    char* errMsg = 0;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL Record Insertion Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_int(stmt, 1, atoi(rcrd->userId));
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL uId Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 2, rcrd->name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Name Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_int(stmt, 3, atoi(rcrd->accountNbr));
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Account Number Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 4, rcrd->country, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Country Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_int(stmt, 5, atoi(rcrd->phone));
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Phone Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    double depAmount;
    char *endptr;
    depAmount = strtof(rcrd->deposit, &endptr);
    if (*endptr != '\0') {
        PrintError("Error converting deposit amount to double");
    }
    rc = sqlite3_bind_double(stmt, 6, depAmount);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Deposit Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 7, rcrd->accountType, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Account Type Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 8, rcrd->dateCreated, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Date Binding Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "SQL Insertion Step Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_finalize(stmt);

    //printf("Data Inserted Successfully.\n");
    return 1;
}

int insertData(sqlite3* db, const char* name, const char* pass)
{
    if (db == NULL || name == NULL || pass == NULL)
    {
        fprintf(stderr, "error: Invalid input parameters.\n");
        return 0;
    }
    char sql[1000];

    snprintf(sql, sizeof(sql), "INSERT INTO USERS (u_id, u_name, u_password) VALUES(NULL, ?, ?);");

    char* errMsg = 0;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        //sqlite3_free(errMsg);
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return 0;
    }

    rc = sqlite3_finalize(stmt);

    //printf("Data Inserted Successfully.\n");
    return 1;
}

bool checkUserName(sqlite3* db, const char* uName) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char* sql = "SELECT u_name FROM USERS WHERE u_name = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_text(res, 1, uName, -1, SQLITE_STATIC);
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }

    int step = sqlite3_step(res);

    sqlite3_finalize(res);
    return step == SQLITE_ROW;
}

int getUserId(sqlite3* db, const char* uName) {
    char* err_msg = 0;
    sqlite3_stmt *res;
    int usrId = 0;

    char* sql = "SELECT u_id FROM USERS WHERE u_name = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_text(res, 1, uName, -1, SQLITE_STATIC);

        int step = sqlite3_step(res);

        if (step == SQLITE_ROW) {
            //printf("UserID: %d\n", sqlite3_column_int(res, 0));
            usrId = sqlite3_column_int(res, 0);
        } else {
            PrintError("No such user found in database.");
        }
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
    return usrId;
}

int getAccountNumber(sqlite3* db, char* accId) {
    if (!isNumber(accId)) {
        PrintError("Incorrect data type detected");
    }
    char* err_msg = 0;
    sqlite3_stmt *res;
    int accountId = -1;

    char* sql = "SELECT account_num FROM RECORDS WHERE account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, atoi(accId));

        int step = sqlite3_step(res);

        if (step == SQLITE_ROW) {
            //printf("\nACCOUNT NUMBER: %d\n", sqlite3_column_int(res, 0));
            accountId = sqlite3_column_int(res, 0);
        } else {
            return -1;
            //fprintf(stderr, "No data found: %s\n", sqlite3_errmsg(db));
        }
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
    return accountId;
}

bool isAccountOwnedByUsr(sqlite3* db, int usrId, int accId) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char* sql = "SELECT u_id, account_num FROM RECORDS WHERE u_id = ? AND account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, usrId);
        sqlite3_bind_int(res, 2, accId);
        int step = sqlite3_step(res);

        if (step == SQLITE_ROW) {
            //printf("\nUSER ID: %d\nACCOUNT NUMBER: %d\n", sqlite3_column_int(res, 0), sqlite3_column_int(res, 1));
            return true;
        } else {
            return false;
        }
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);
    return false;
}

void getAccRowByNum(sqlite3* db, int accId) {
    char* err_msg = 0;
    sqlite3_stmt *stmt;

    char* sql = "SELECT * FROM RECORDS WHERE account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, accId);
        int step = sqlite3_step(stmt);

        if (step == SQLITE_ROW) {
            printf("\nAccount Number: %d\n", sqlite3_column_int(stmt, 3));
            printf("Country: %s\n", sqlite3_column_text(stmt, 4));
            printf("Phone Number: %d\n", sqlite3_column_int(stmt, 5));
            printf("Amount Deposited: %.2f\n", sqlite3_column_double(stmt, 6));
            printf("Account Type: %s\n", sqlite3_column_text(stmt, 7));
            printf("Deposit Date: %s\n", sqlite3_column_text(stmt, 8));
        }
    } else {
        fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
}

//Function to check if account number exist in database
bool isAccountNumExist(sqlite3* db, char* acc) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char* sql = "SELECT account_num FROM RECORDS WHERE account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, atoi(acc));

        int step = sqlite3_step(res);

        if (step == SQLITE_ROW) {
            //printf("\nACCOUNT NUMBER: %d\n", sqlite3_column_int(res, 0));
            //accountId = sqlite3_column_int(res, 0);
            return true;
        } else {
            //fprintf(stderr, "No data found: %s\n", sqlite3_errmsg(db));
            return false;
        }
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
    return false;
}

void transferAccUsr(sqlite3* db, char* usrName, int accId) {
    int usrId = getUserId(db, usrName);
    if (usrId == -1) {
        PrintError("Entered username not found in our database.");
        return;
    }
    char* err_msg = 0;
    sqlite3_stmt *stmt;
    char sql[100];
    snprintf(sql, sizeof(sql), "%s", "UPDATE RECORDS set u_id = ?, u_name = ? WHERE account_num = ?");
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Update Error: %s.\n", sqlite3_errmsg(db));
        return;
    }
    rc = sqlite3_bind_int(stmt, 1, usrId);
    rc = sqlite3_bind_text(stmt, 2, usrName, -1, SQLITE_STATIC);
    rc = sqlite3_bind_int(stmt, 3, accId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return;
    } else {
        printf("\n\t\tAccount has been transferred!\n");
    }

    sqlite3_finalize(stmt);
}

//Function to update account detail phone and country
void updateAccountDetail(sqlite3* db, char* col, char* value, int accId) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char sql[100];
    if (strcmp(col, "Phone Number") == 0) {
        snprintf(sql, sizeof(sql), "%s", "UPDATE RECORDS set phone = ? WHERE account_num = ?");
    } else if (strcmp(col, "Country") == 0) {
        snprintf(sql, sizeof(sql), "%s", "UPDATE RECORDS set country = ? WHERE account_num = ?");
    }

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Update Error: %s.\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Update Error: %s.\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_bind_int(res, 2, accId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_step(res);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return;
    } else {
        printf("\n\t\tRecord has been updated successfully!\n");
    }

    sqlite3_finalize(res);
}

//Function to return true if user name and  password exist in database,
//Otherwise false
bool getLoginInfo(sqlite3* db, const char* uName, const char* pass) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char* sql = "SELECT u_name, u_password FROM USERS WHERE u_name = ? AND u_password = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_text(res, 1, uName, -1, SQLITE_STATIC);
        sqlite3_bind_text(res, 2, pass, -1, SQLITE_STATIC);
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }

    int step = sqlite3_step(res);

    // if (step == SQLITE_ROW) {
    //     printf("Name: %s\n", sqlite3_column_text(res, 0));
    //     printf("Pass: %s\n", sqlite3_column_text(res, 1));
    // }

    sqlite3_finalize(res);
    return step == SQLITE_ROW;
}

void deleteAccountNum(sqlite3* db, int accId) {
    char* err_msg = 0;
    sqlite3_stmt *stmt;

    char* sql = "SELECT * FROM RECORDS WHERE account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, accId);
        int step = sqlite3_step(stmt);

        if (step == SQLITE_ROW) {
            printf("\nAccount Number: %d\n", sqlite3_column_int(stmt, 3));
            printf("Country: %s\n", sqlite3_column_text(stmt, 4));
            printf("Phone Number: %d\n", sqlite3_column_int(stmt, 5));
            printf("Amount Deposited: %.2f\n", sqlite3_column_double(stmt, 6));
            printf("Account Type: %s\n", sqlite3_column_text(stmt, 7));
            printf("Deposit Date: %s\n", sqlite3_column_text(stmt, 8));
        }
    } else {
        fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);

    char* errMsg = 0;
    sqlite3_stmt *res;

    char *query = "DELETE FROM RECORDS WHERE account_num = ?";
    int rv = sqlite3_prepare_v2(db, query, -1, &res, 0);

    if (rv == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, accId);
    } else {
        fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(db));
    }

    rv = sqlite3_step(res);
    if (rv != SQLITE_DONE)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return;
    } else {
        printf("\n\t\tAccount has been deleted successfully!\n");
    }
    sqlite3_finalize(res);
}

//Function to check account detail where account number = input number, 
//and prints interest rate of account if applicable
void checkAccDetail(sqlite3* db, int accId) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char* sql = "SELECT * FROM RECORDS WHERE account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, accId);

        int step = sqlite3_step(res);
        if (step == SQLITE_ROW) {
            printf("\nAccount Number: %d\n", sqlite3_column_int(res, 3));
            printf("Country: %s\n", sqlite3_column_text(res, 4));
            printf("Phone Number: %d\n", sqlite3_column_int(res, 5));
            printf("Amount Deposited: %.2f\n", sqlite3_column_double(res, 6));
            printf("Account Type: %s\n", sqlite3_column_text(res, 7));
            printf("Deposit Date: %s\n", sqlite3_column_text(res, 8));

            const char* accType = sqlite3_column_text(res, 7);
            char* dayColumn = (char*)sqlite3_column_text(res, 8);
            const char* day = getLastValue(dayColumn, '-');
            char year[5];
            strncpy(year, sqlite3_column_text(res, 8), 4);
            year[4] = '\0';
            char month[3];
            sscanf(dayColumn, "%*d-%2s", month);
            //printf("\nYEAR: %s\n", year);
            if (strcmp(accType, "current") == 0) {
                printf("\nYou will not get interests because the account is of type current.\n");
            } else if (strcmp(accType, "saving") == 0) {
                displayInterest(sqlite3_column_int(res, 6), 7.0, "saving", atoi(day), month, atoi(year));
            } else if (strcmp(accType, "fixed01") == 0) {
                displayInterest(sqlite3_column_int(res, 6), 4.0, "fixed01", atoi(day), month, atoi(year));
            } else if (strcmp(accType, "fixed02") == 0) {
                displayInterest(sqlite3_column_int(res, 6), 5.0, "fixed02", atoi(day), month, atoi(year));
            } else if (strcmp(accType, "fixed03") == 0) {
                displayInterest(sqlite3_column_int(res, 6), 8.0, "fixed03", atoi(day), month, atoi(year));
            } else {
                dashboard();
            }
        }
    } else {
        fprintf(stderr, "Failed To Execute CheckAccDetail Statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
}

//Function to get all columns data from RECORDS table where user id = current user logged id
void getAccInfo(sqlite3* db, const char* uId) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char* sql = "SELECT * FROM RECORDS WHERE u_id = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, atoi(uId));

        while (1) {
            int step = sqlite3_step(res);
            if (step == SQLITE_ROW) {
                printf("____________________________\n");
                printf("Account Number: %d\n", sqlite3_column_int(res, 3));
                printf("Country: %s\n", sqlite3_column_text(res, 4));
                printf("Phone Number: %d\n", sqlite3_column_int(res, 5));
                printf("Amount Deposited: %d\n", sqlite3_column_int(res, 6));
                printf("Account Type: %s\n", sqlite3_column_text(res, 7));
                printf("Deposit Date: %s\n", sqlite3_column_text(res, 8));
            } else if (step == SQLITE_DONE) {
                break;
            } else {
                fprintf(stderr, "No data found: %s\n", sqlite3_errmsg(db));
                break;
            }
        }
        
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
}

//Functions to return account type where accoun_num = ?
char* getAccountType(sqlite3* db, char* accId) {
    char* err_msg = 0;
    sqlite3_stmt *res;

    char* sql = "SELECT type_account FROM RECORDS WHERE account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, atoi(accId));
        int step = sqlite3_step(res);
        if (step == SQLITE_ROW) {
            //printf("\nACCOUNT TYPE: %s", sqlite3_column_text(res, 0));
            return (char*)sqlite3_column_text(res, 0);
        } else {
            return "NULL";
        }
    }
    return "NULL";
}

// Function to get the balance amount where account num = ?
double getBalanceAcc(sqlite3* db, char* accId) {
    char* err_msg = 0;
    sqlite3_stmt *res;
    double balance = 0;

    char* sql = "SELECT balance FROM RECORDS WHERE account_num = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, atoi(accId));

        int step = sqlite3_step(res);

        if (step == SQLITE_ROW) {
            //printf("\nACCOUNT NUMBER: %d\n", sqlite3_column_int(res, 0));
            balance = sqlite3_column_double(res, 0);
        } else {
            balance = 0;
        }
    } else {
        fprintf(stderr, "Failed To Execute Statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
    return balance;
}

// Function to deposit/ withdraw amount from account
void makeTransactionAccount(sqlite3* db, char* accId, char* method, double amount) {
    double currentBalance = getBalanceAcc(db, accId);

    if (strcmp(method, "withdraw") == 0 && currentBalance < amount) {
        printf("\nCannot withdraw.\nInsuficient balance amount!\n");
        returnMenu();
    }

    char* err_msg = 0;
    sqlite3_stmt *res;

    char sql[100];
    snprintf(sql, sizeof(sql), "%s", "UPDATE RECORDS set balance = ? WHERE account_num = ?");

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Update Error: %s.\n", sqlite3_errmsg(db));
        return;
    }

    if (strcmp(method, "withdraw") == 0) {
        rc = sqlite3_bind_double(res, 1, (currentBalance-amount));
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
            return;
        }
    } else if (strcmp(method, "deposit") == 0) {
        rc = sqlite3_bind_double(res, 1, (currentBalance+amount));
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
            return;
        }
    }

    rc = sqlite3_bind_int(res, 2, atoi(accId));
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_step(res);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "SQL Error: %s.\n", sqlite3_errmsg(db));
        return;
    } else {
        if (strcmp(method, "withdraw") == 0) {
            printf("\n\033[32mAmount withdrawn successfully!\033[0m\n");
        } else if (strcmp(method, "deposit") == 0) {
            printf("\n\033[32mAmount deposited successfully!\033[0m\n");
        }
    }

    sqlite3_finalize(res);
}

//Function to set username to struct usr
void setUserName(struct User *usr, const char* uName)
{
    if (strlen(uName) > MAX_LENGTH_USERNAME)
    {
        printf("Username characters too long!\n");
        return;
    }
    strcpy(usr->name, uName);
}

//Function to check if username is valid characters
int isUserNameValid(const char* uName)
{
    for (int i = 0; uName[i] != '\0'; i++)
    {
        if (!isprint(uName[i]))
        {
            return 0;
        }
    }
    return 1;
}

//Function to encrypt pass word characters
char* encryptPass(char* str, char* key)
{
    // char encrypted_str[strlen(str)+1];
    char* encrypted_str = malloc(strlen(str)+1);

    for (int i = 0; i < strlen(str); i++)
    {
        encrypted_str[i] = str[i] ^ key[i % strlen(key)];
    }

    encrypted_str[strlen(str)] = '\0';
    return encrypted_str;
}

//Function to decrypt pass word characters
char* decryptPass(char* str, char* key)
{
    char* decrypted_str = malloc(strlen(str)+1);

    for (int i = 0; i < strlen(str); i++)
    {
        decrypted_str[i] = str[i] ^ key[i % strlen(key)];
    }

    decrypted_str[strlen(str)] = '\0';
    return decrypted_str;
}

//Function to set pass word to struct usr
void setPassword(struct User *usr, char* pass)
{
    if (strlen(pass) > MAX_LENGTH_PASSWORD)
    {
        printf("Password characters too long!\n");
        return;
    }
    strcpy(usr->password, encryptPass(pass, HASHKEYPASS));
}

//Function to count digits of a number
int countDigits(int num) {
    int count = 0;

    if (num == 0) {
        return 1;
    }

    while (num != 0) {
        num /= 10;
        count++;
    }

    return count;
}
//BELOW ARE SET OF FUNCTIONS TO SET EACH VARIABLE TO STRUCT RCRD (RECORD)
//ALL BELOW
void setRecordId(struct Record *rcrd, char* uId) {
    strcpy(rcrd->userId, uId);
}

void setRecordName(struct Record *rcrd, const char* uName)
{
    if (strlen(uName) > MAX_LENGTH_USERNAME)
    {
        printf("Username characters too long!\n");
        return;
    }
    strcpy(rcrd->name, uName);
}

void setRecordAccNum(struct Record *rcrd, const char* accountNb)
{
    strcpy(rcrd->accountNbr, accountNb);
}

void setRecordCountry(struct Record *rcrd, const char* coun)
{
    strcpy(rcrd->country, coun);
}

void setRecordPhone(struct Record *rcrd, const char* phne)
{
    strncpy(rcrd->phone, phne, sizeof(rcrd->phone) - 1);
    rcrd->phone[sizeof(rcrd->phone) - 1] = '\0';
}

void setRecordDeposit(struct Record *rcrd, const char* depo)
{
    strcpy(rcrd->deposit, depo);
}

void setRecordWithdraw(struct Record *rcrd, const char* wdraw) {
    strcpy(rcrd->withdraw, wdraw);
}

void setRecordAccType(struct Record *rcrd, const char* accType)
{
    strcpy(rcrd->accountType, accType);
}

void setRecordDate(struct Record *rcrd, const char* date) {
    strcpy(rcrd->dateCreated, date);
}