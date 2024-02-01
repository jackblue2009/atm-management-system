#include <sqlite3.h>
#include <time.h>
#include "header.h"
#include "config.c"
#include "auth.c"

void createNewAcc()
{
    system("clear");
    if (global_db != NULL) {
        time_t current_time;
        struct tm *time_info;
        char time_string[15];
        struct Record rcrd;
        char uId[20], accNum[50], country[50], phone[8], depo[12], accType[10];
        int usrId = getUserId(global_db, getUserLogged());
        char userID[20];
        snprintf(userID, sizeof(userID), "%d", usrId);

        printf("\n\n\t\t=========================================================\n");
        printf("\n\t\t-------------------> New Record: <-----------------------\n");
        int dateChoice;
        NEW_RECORD_LBL:
        printf("\n\t\tEnter 1 to get today's date by default\n\t\tOr 2 to enter new date: ");
        scanf("%d", &dateChoice);
        switch (dateChoice)
        {
        case 1:
            time(&current_time);
            time_info = localtime(&current_time);
            strftime(time_string, sizeof(time_string), "%Y-%m-%d", time_info);
            break;
        case 2:
            int dd, mm, yyyy;
            printf("\n\t\tEnter Date (YYYY-MM-DD): ");
            scanf("%d-%d-%d", &yyyy, &mm, &dd);
            if (!isDateValid(yyyy, mm, dd)) {
                printf("\nInvalid date format\n");
                sleep(2);
                goto NEW_RECORD_LBL;
            } else {
                snprintf(time_string, sizeof(time_string), "%d-%d-%d", yyyy, mm, dd);
            }
            break;
        default:
            goto NEW_RECORD_LBL;
            break;
        }
        
        //setUserLogged(uName);
        printf("\n\t\tToday's Date: %s\n", time_string);
        printf("\n\t\tAccount ID: %d\n", usrId);
        printf("\n\t\tAccount Owner: %s\n", getUserLogged());
        ACCOUNT_CREATION_START:
        printf("\n\t\tEnter Account Number: ");
        scanf("%s", accNum);
        if (isAccountNumExist(global_db, accNum)) {
            printf("\n\t\tAccount Number already exist!\n");
            sleep(2);
            goto ACCOUNT_CREATION_START;
        }
        setAccNum(accNum);
    
        printf("\n\t\tEnter Country: ");
        scanf("%s", country);
        setUserCountry(country);
    
        printf("\n\t\tEnter Phone Number: ");
        scanf("%s", phone);
        setUserPhone(phone);
    
        printf("\n\t\tEnter Amount To Deposit: $");
        scanf("%s", depo);
        setDepositAmount(depo);
    
        printf("\n\t\tChoose Type of Account:\n");
        printf("\t\t\t-> (1) saving\n");
        printf("\t\t\t-> (2) current\n");
        printf("\t\t\t-> (3) fixed01 (for 1 year)\n");
        printf("\t\t\t-> (4) fixed02 (for 2 years)\n");
        printf("\t\t\t-> (5) fixed03 (for 3 years)\n");
        printf("\t\tEnter Your Choice: ");
        int accOption = 0;
        scanf("%d", &accOption);
        //char accList[5][10] = {"saving", "current", "fixed01", "fixed02", "fixed03"};
        //stringToLower(accType);
        switch (accOption) {
            case 1:
                setAccType("saving");
                break;
            case 2:
                setAccType("current");
                break;
            case 3:
                setAccType("fixed01");
                break;
            case 4:
                setAccType("fixed02");
                break;
            case 5:
                setAccType("fixed03");
                break;
            default:
                printf("\nInvalid input\n");
                break;
        }
        setRecordId(&rcrd, userID);
        setRecordName(&rcrd, getUserLogged());
        setRecordAccNum(&rcrd, getAccNum());
        setRecordCountry(&rcrd, getUserCountry());
        setRecordPhone(&rcrd, getUserPhone());
        setRecordDeposit(&rcrd, getDepositAmount());
        setRecordAccType(&rcrd, getAccType());
        setRecordDate(&rcrd, time_string);

        // printf("\n\nNew Record Info:\n");
        // printf("USER ID: %s\n", rcrd.userId);
        // printf("USER NAME: %s\n", rcrd.name);
        // printf("ACCOUNT NUMBER: %s\n", rcrd.accountNbr);
        // printf("COUNTRY: %s\n", rcrd.country);
        // printf("PHONE: %s\n", rcrd.phone);
        // printf("DEPOSIT: %s\n", rcrd.deposit);
        // printf("WITHDRAW: %s\n", rcrd.withdraw);
        // printf("ACCOUNT TYPE: %s\n", rcrd.accountType);
        // printf("DATE CREATED: %s\n", rcrd.dateCreated);

        if (insertNewRecordData(global_db, &rcrd)) {
            printf("\nNew Record Inserted Successfully!\n");
        } else {
            printf("\nFailed To Insert New Record. Try Again Later.\n");
        }
    }
    returnMenu();
}

void displayAllAccounts() {
    system("clear");
    if (global_db != NULL) {
        int usrId = getUserId(global_db, getUserLogged());
        char userID[20];
        snprintf(userID, sizeof(userID), "%d", usrId);

        printf("\n\n\t\t=========================================================\n");
        printf("\n\t\t-------------------> All accounts for user, %s <-----------------------\n", getUserLogged());
    
        getAccInfo(global_db, userID);

        printf("\nSuccess!\n");
    }
    returnMenu();
}

void updateAccInfo() {
    system("clear");
    if (global_db != NULL) {
        char accNum[50];

        printf("\n\n\t\t=========================================================\n");
        printf("\n\t\t-------------------> Update account details for user, %s <-----------------------\n", getUserLogged());

        printf("\n\t\tWhat is the account number you want to update: ");
        scanf("%s", accNum);
        int accountId = getAccountNumber(global_db, accNum);
        //printf("\n\t\tThis is your account number: %d\n", accountId);

        if (accountId == -1) {
            printf("\nAccount number entered is not registered in our database.\n");
            returnMenu();
        } else {
            UPDATE_INFO:
            printf("\n\t\tWhich information do you want to update?\n");
            printf("\t\t1-> Phone Number\n\t\t2-> Country\n");
            int aOp;
            printf("\n\t\tEnter a choice: ");
            scanf("%d", &aOp);
            switch (aOp)
            {
                case 1:
                    updateAccountElement("Phone Number", accountId);
                    break;
                case 2:
                    updateAccountElement("Country", accountId);
                    break;
                default:
                    printf("\nPlease input correct choice from menu above:\n");
                    sleep(1);
                goto UPDATE_INFO;
                break;
            }
        }
    }
}

void updateAccountElement(char* col, int accId) {
    if (global_db != NULL) {
        char val[12];
        printf("\n\t\tEnter the new %s: ", col);
        scanf("%s", val);
        //
        if (strcmp(col, "Phone Number") == 0) {
            int num = atoi(val);
            if (!isNumber(val) && countDigits(num) != 8) {
                PrintError("Inavlid phone number entered!");
            }
        } else if (strcmp(col, "Country") == 0) {
            for (int i = 0; i < strlen(val); i++) {
                if (!isalpha(val[i])) {
                    PrintError("Invalid country name entered!");
                }
            }
            if (strlen(val) > 12) {
            PrintError("Country name too long!");
            return;
            } else if (strlen(val) < 2) {
                PrintError("Country name too short!");
                return;
            }
        }
    
        updateAccountDetail(global_db, col, val, accId);
    }
    returnMenu();
}

void checkAllAccounts()
{
    if (global_db != NULL) {
        char val[12];

        printf("\n\t\tEnter account number: ");
        scanf("%s", val);

        if (!isNumber(val)) {
            PrintError("Invalid data type entered!");
        } else {
            if (atoi(val) > 999999999 && atoi(val) < 0) {
                PrintError("Inavlid account number entered!");
            }
        }
        system("clear");
        checkAccDetail(global_db, atoi(val));
    }
    returnMenu();
}

void makeTransaction() {
    if (global_db != NULL) {
        system("clear");
        printf("\n\n\t\t============================================================\n");
        printf("\n\t\t-------------------> MAKE TRANSACTION <-----------------------\n");
        char accId[12];

        printf("\n\t\tEnter account number: ");
        scanf("%s", accId);

        if (!isNumber(accId)) {
            PrintError("Invalid data type entered!");
        } else {
            if (atoi(accId) > 999999999 && atoi(accId) < 0) {
                PrintError("Iavlid account number enterd!");
            }
        }

        if (strcmp(getAccountType(global_db, accId), "fixed01") == 0) {
            PrintError("Fixed accounts are not elgible to make transactions.");
        } else if (strcmp(getAccountType(global_db, accId), "fixed02") == 0) {
            PrintError("Fixed accounts are not elgible to make transactions.");
        } else if (strcmp(getAccountType(global_db, accId), "fixed03") == 0) {
            PrintError("Fixed accounts are not elgible to make transactions.");
        }

        if (!isAccountNumExist(global_db, accId)) {
            printf("\nAccount number entered does not exist in our database.\n");
            returnMenu();
        } else {
            printf("\n\t\tChoose method of transaction:\n\t\t1-> Withdraw\n\t\t2-> Deposit\n");
            int op;
            double a, b;
            printf("\n\t\tEnter choice: ");
            scanf("%d", &op);
            switch (op)
            {
                case 1:
                    printf("\n\t\tEnter amount to withdraw: ");
                    scanf("%le", &a);
                    makeTransactionAccount(global_db, accId, "withdraw", a);
                    break;
                case 2:
                    printf("\n\t\tEnter amount to deposit: ");
                    scanf("%le", &b);
                    makeTransactionAccount(global_db, accId, "deposit", b);
                    break;
                default:
                    printf("\nInvalid response.\n");
                    break;
            }
        }
    }
    returnMenu();
}

void removeExistAcc() {

    if (global_db != NULL) {
        char accId[12];
        printf("\n\t\tEnter number of account you want to delete: ");
        scanf("%s", accId);
        if (!isNumber(accId)) {
            PrintError("Invalid data type entered!");
        } else {
            if (atoi(accId) > 999999999 && atoi(accId) < 0) {
                PrintError("Iavlid account number enterd!");
            }
        }

        if (!isAccountNumExist(global_db, accId)) {
            printf("\nAccount number entered does not exist in our database.\n");
            returnMenu();
        } else {
            system("clear");
            printf("\t\t\t\t===== Deleted Account =====\t\t\t\t");
            deleteAccountNum(global_db, atoi(accId));
        }
    }
    returnMenu();
}

void transferOwnership() {
    int currentUsrId = getUserId(global_db, getUserLogged());
    printf("Enter the account number you want to transfer ownership: ");
    int accId;
    scanf("%d", &accId);
    if (isAccountOwnedByUsr(global_db, currentUsrId, accId)) {
        getAccRowByNum(global_db, accId);

        printf("\n\t\tWhich user you want to transfer ownership to (username): ");
        char* usrName;
        scanf("%s", usrName);
        //int newUsrId = getUserId(db, usrName);
        transferAccUsr(global_db, toLower(usrName), accId);
        //
        returnMenu();
    }
}

const char* getLastValue(char* str, char deli) {
    const char* lastDeli = strrchr(str, deli);

    if (lastDeli != NULL) {
        return lastDeli + 1;
    } else {
        return str;
    }
}

void PrintError(char* errMsg)
{
    printf("\n\x1b[1;31m%s.\x1b[0m\n", errMsg);
    //exit(1);
    returnMenu();
}

void ExitProgram()
{
    printf("\x1b[1;31m");
    printf("System Terminated\n");
    printf("\x1b[0m");
    exit(1);
}