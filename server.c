#include "server.h"

// FUNCTION TO READ INFO FROM FILE, ADD IT INTO ARRAY
void readAccountsFile()
{
    uint8_t str[255][25];
    uint8_t* token;
    uint8_t i = 0;

    FILE* filePointer = fopen("bankAccountsDB.txt", "r");

    while (fgets(str, 30, filePointer) != NULL)
    {
        token = strtok(str, " ");
        strcpy(bankAccounts[i].primaryAccountNumber, token);
        token = strtok(NULL, " ");
        bankAccounts[i].balance = atof(token);
        i++;
    }

    ACCOUNTS_NUMBER = i;
    fclose(filePointer);
}

void modifyNewBalance(uint8_t* cardPAN) // TO MODIFY BALANCE IN FILE    
{
    FILE* mainFile = fopen("bankAccountsDB.txt", "r");
    FILE* tempFile = fopen("bankAccountsDB_TEMP.txt", "w"); // temporary file
   
    uint8_t str[255][30]; // STRING TAKEN
    uint8_t* stringToPut = malloc(30); // STRING PUT
    
    uint8_t count = 0;
    uint16_t lineToModify = searchAccountsDB(cardPAN);


    while (count < ACCOUNTS_NUMBER)
    {
        if (count == lineToModify)
        {
            if (count == ACCOUNTS_NUMBER-1) // If last line, do not add an extra new line
                sprintf(stringToPut, "%s %.2f", cardPAN, bankAccounts[lineToModify].balance);
            else
                sprintf(stringToPut, "%s %.2f\n", cardPAN, bankAccounts[lineToModify].balance);

            fputs(stringToPut, tempFile);
            fgets(str, 30, mainFile);
        }
        else
        {
            fgets(str, 30, mainFile);
            fputs(str, tempFile);
        }
        count++;
    }

    fclose(mainFile);
    fclose(tempFile);

    remove("bankAccountsDB.txt");
    rename("bankAccountsDB_TEMP.txt", "bankAccountsDB.txt");
}

uint16_t getLastTransNumber()
{
    uint8_t str[255][60];
    uint8_t i = 0;
    uint16_t lastChar;

    FILE* filePointer = fopen("transactionsDB.txt", "r");

    while (fgets(str[i], 60, filePointer) != NULL)
        i++;

    TRANSACTIONS_NUMBER = i - 2;

    fclose(filePointer);

    char* token = strtok(str[i-1], "\t");

    return (atoi(token));
}

uint8_t* enumToStr_transState(EN_transState_t transState)
{
    switch (transState)
    {
    case 0: 
        return "APPROVED";
        break;
    case 1:
        return "DECLINED_INSUFFICIENT_FUND";
        break;
    case 2:
        return "DECLINED_STOLEN_CARD";
        break;
    case 3:
        return "INTERNAL_SERVER_ERROR";
        break;
    }
}

void writeTransactionsFile(uint8_t * cardPAN, float transAmount, EN_transState_t transState, uint32_t transactionNumber)
{
    FILE* filePointer = fopen("transactionsDB.txt", "a+");
    uint8_t *toPrint = malloc(60);


    sprintf(toPrint, "\n%d\t%s\t%.2f\t%d\t%s", transactionNumber, removeWhiteSpaces(cardPAN), transAmount, transState, enumToStr_transState(transState));
    fputs(toPrint, filePointer);

    fclose(filePointer);
}

uint8_t* removeWhiteSpaces(uint8_t* str)
{
    uint8_t* newStr = (uint8_t*)malloc(strlen(str));
    strcpy(newStr, str);

    uint8_t i = 0, j = 0;

    while (newStr[i])
    {
        if (newStr[i] != ' ')
            newStr[j++] = newStr[i];
        i++;
    }
    newStr[j] = '\0';

    return newStr;
}

int16_t searchAccountsDB(uint8_t* cardPAN) // FUNCTION RETURNS INDEX OF ARRAY, IF DOESN'T EXIST RETURN -1
{
    for (uint8_t i = 0; i < ACCOUNTS_NUMBER; i++) // TO BE MODIFIED
    {
        if (strcmp(removeWhiteSpaces(bankAccounts[i].primaryAccountNumber), removeWhiteSpaces(cardPAN)) == 0)
            return i;
    }
    return -1;
}

EN_serverError_t isValidAccount(ST_cardData_t* cardData)
{
    uint8_t* cardPAN = cardData->primaryAccountNumber;

    if (searchAccountsDB(cardPAN) != -1) // IF IT IS FOUND
    {
        return SERVER_OK;
    }

    return DECLINED_STOLEN_CARD;
}

EN_serverError_t isAmountAvailable(ST_transaction_t* transData)
{
    float transactionAmount = transData->terminalData.transAmount;
    uint8_t* cardPAN = transData->cardHolderData.primaryAccountNumber;
    uint8_t index = searchAccountsDB(cardPAN);

    float currentBalance = bankAccounts[index].balance;

    if (transactionAmount > currentBalance)
    {
        return LOW_BALANCE;
    }
    else
    {
        return SERVER_OK;
    }
}
EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
    float transactionAmount = transData->terminalData.transAmount;
    uint8_t* cardPAN = transData->cardHolderData.primaryAccountNumber;
    uint8_t index = searchAccountsDB(cardPAN);

    if (isValidAccount(&(transData->cardHolderData)) == SERVER_OK)
    {
        if (isAmountAvailable(transData) == SERVER_OK)
        {
            bankAccounts[index].balance = bankAccounts[index].balance - transactionAmount;
            transData->transState = APPROVED;
            
            if (saveTransaction(transData) == SERVER_OK)
                return APPROVED;
            else
            {
                transData->transState = INTERNAL_SERVER_ERROR;
                saveTransaction(transData);
                return INTERNAL_SERVER_ERROR;
            }
        }
        else
        {
            transData->transState = DECLINED_INSUFFECIENT_FUND;
            saveTransaction(transData);
            return DECLINED_INSUFFECIENT_FUND;
        }
    }
    else
    {
        transData->transState = DECLINED_STOLEN_CARD;
        saveTransaction(transData);
        return DECLINED_STOLEN_CARD;
    }
}

EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
    printf("SAVING TRANSACTION.......\n");

    uint16_t transNumber = getLastTransNumber();

    if (TRANSACTIONS_NUMBER == 0);
    else
        transNumber++;

    transData->transactionSequenceNumber = transNumber;
    writeTransactionsFile(transData->cardHolderData.primaryAccountNumber, transData->terminalData.transAmount, transData->transState, transNumber);
    modifyNewBalance(transData->cardHolderData.primaryAccountNumber); // TO MODIFY BALANCE IN FILE    

    return SERVER_OK;
}

EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t* transData)
{
    uint8_t str[255][60];
    uint8_t i = 0;
    FILE* filePointer = fopen("transactionsDB.txt", "r");

    uint8_t* token = malloc(3);
    uint8_t* tempStr = malloc(60);

    while (fgets(str, 60, filePointer) != NULL)
    {
        if (i > 1) // After the first 2 lines in the file
        {
            strcpy(tempStr, str);
            token = strtok(str, "\t");
            if (atoi(token) == transactionSequenceNumber)
            {
                transData->transactionSequenceNumber = atoi(token);
                strcpy(transData->cardHolderData.primaryAccountNumber, strtok(NULL, "\t"));
                transData->terminalData.transAmount = atof(strtok(NULL, "\t"));
                transData->transState = atoi(strtok(NULL, "\t"));

                fclose(filePointer);
                return SERVER_OK;
            }
        }
        i++;
    }
    fclose(filePointer);
    return TRANSACTION_NOT_FOUND;
}