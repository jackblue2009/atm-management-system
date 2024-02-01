#include <ctype.h>
#include <sqlite3.h>
#include "header.h"
#include "system.c"
#include "config.c"
#include <gtk-3.0/gtk/gtk.h>

void regMenu()
{   
    system("clear");
    struct User usr, usrNum[MAXUSERS];
    char uName[50], pass1[50], pass2[50];
    const char* tableName = "USERS";
    int numUsers = 0;
    printf("\x1b[1;35m");
    printf("\n\n\t\t=======           Register New User         =======\n");
    printf("\x1b[0m");
    printf("\x1b[35m");
    printf("\n\t\tFill in the proper details below to create new user account:\n");
    printf("\x1b[0m");
    printf("\n\t\t\x1b[1;35mEnter your new username: \x1b[0m");
    scanf("%s", uName);

    if (checkUserName(global_db, toLower(uName))) {
        printf("\nUsername already exists!\n\t\tEnter 1 to go back to initial menu: ");
        int op1;
        scanf("%d", &op1);
        switch (op1)
        {
        case 1:
            initMenu();
            break;
        default:
            ExitProgram();
            break;
        }
    }
    
    printf("\n\t\t\x1b[1;35mEnter your new passowrd: \x1b[0m");
    scanf("%s", pass1);
    printf("\n\t\t\x1b[1;35mEnter your new passowrd again: \x1b[0m");
    scanf("%s", pass2);
    if (strcmp(pass1, pass2) == 0)
    {
        printf("Password Match Accepted!\n");
        setUserName(&usr, toLower(uName));
        setPassword(&usr, pass1);
    }
    else
    {
        PrintError("\nPassword does not match! System Aborted");
        printf("\n\t\tEnter 1 to go back to initial menu: ");
        int op1;
        scanf("%d", &op1);
        switch (op1)
        {
        case 1:
            initMenu();
            break;
        default:
            ExitProgram();
            break;
        }
    }
    // printf("USERNAME: [%s]\nPASSWORD: [%s]\n", usr.name, usr.password);
    // printf("DECRYPTED PASSWORD: [%s]\n", decryptPass(usr.password, HASHKEYPASS));
    if (insertData(global_db, usr.name, usr.password))
    {
        printf("\n\t\tRegistration Successful\n");
    }
    else
    {
        printf("\n\t\tRegistration Failed!.\n");
    }
    printf("\n\t\tEnter 1 to go back to initial menu: ");
    int op1;
    scanf("%d", &op1);
    switch (op1)
    {
    case 1:
        initMenu();
        break;
    
    default:
        ExitProgram();
        break;
    }
}

void logMenu()
{
    system("clear");
    char uName[50], pass[50];
    printf("\x1b[1;32m");
    printf("\n\n\t\t=======           UNITED BANK ATM - LOGIN         =======\n");
    printf("\x1b[0m");
    printf("\x1b[32m");
    printf("\n\t\t-->> If you are already registered, then proceed below with your credentials to login:\n");
    printf("\n\t\tEnter your username: ");
    printf("\x1b[0m");
    scanf("%s", uName);
    //printf("NAME: %s", uName);
    printf("\x1b[32m");
    printf("\n\t\tEnter your password: ");
    printf("\x1b[0m");
    scanf("%s", pass);
    int isLogged = getLoginInfo(global_db, toLower(uName), encryptPass(pass, HASHKEYPASS));
    if (isLogged == 1) {
        setUserLogged(toLower(uName));
        dashboard();
    } else {
        printf("\nIncorrect Credentials!\n");
    }
}

void dashboard()
{
    system("clear");
    int r = 0;
    int option;
    printf("\x1b[1;34m");
    printf("\n\n\t\t=========================================================\n");
    printf("\n\t\tWelcome %s, Your Dashboard:\n", getUserLogged());
    printf("\x1b[0m");
    printf("\x1b[34m");
    printf("\n\t\tChoose one of the options below to get started:\n");
    printf("\n\t\t[1]- Create New Account\n");
    printf("\n\t\t[2]- Update Account Details\n");
    printf("\n\t\t[3]- Check Account Details\n");
    printf("\n\t\t[4]- Display list of all available Accounts\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- DELETE EXISTING ACCOUNT\n");
    printf("\n\t\t[7]- TRANSFER OWNERSHIP\n");
    printf("\n\t\t[8]- Exit\n");
    printf("\x1b[0m");
    while (!r)
    {
        printf("\x1b[4;34m");
        printf("\n\t\t[%s]: ", getUserLogged());
        printf("\x1b[0m");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            createNewAcc();
            r = 1;
            break;
        case 2:
            updateAccInfo();
            r = 1;
            break;
        case 3:
            checkAllAccounts();
            r = 1;
            break;
        case 4:
            displayAllAccounts();
            r = 1;
            break;
        case 5:
            makeTransaction();
            r = 1;
            break;
        case 6:
            removeExistAcc();
            r = 1;
            break;
        case 7:
            transferOwnership();
            r = 1;
            break;
        case 8:
            ExitProgram();
            r = 1;
            break;
        default:
            printf("\n\t\tInvalid response, please type a valid option.\n");
            r = 1;
            break;
        }
    }
}

void initMenu()
{//struct User *u
    system("clear");
    int r = 0;
    int option;
    //sqlite3* db = getConnection();
    printf("\x1b[1;36m");
    printf("\n\n\t\t=======           UNITED BANK - ATM         =======\n");
    printf("\x1b[0m");
    printf("\x1b[36m");
    printf("\n\t\t-->> Welcome to the regional united bank ATM system!\n\t\tChoose one of the options below to get you started:\n");
    printf("\n\t\t[1]- Login as existing user\n");
    printf("\n\t\t[2]- Register a new user\n");
    printf("\n\t\t[3]- Exit\n");
    printf("\x1b[0m");
    while (!r)
    {
        printf("\n\t\t\x1b[4;44;35mUser Input:");
        printf("\x1b[0m");
        printf(" ");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            logMenu();
            r = 1;
            break;
        case 2:
            // student TODO : add your **Registration** function
            // here
            regMenu();
            r = 1;
            break;
        case 3:
            ExitProgram();
            break;
        default:
            printf("Insert a valid operation!\n\t\t");
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int rc = openDB();
    if (rc != SQLITE_OK) {
        printf("\nDatabase already open\n");
    }
    global_db = getGlobalDB();
    
    initMenu();

    return 0;
}
