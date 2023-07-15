#ifndef SERVER_H
#define SERVER_H

#include "dataTypes.h"
#include "card.h"
#include "terminal.h"

typedef enum EN_transState_t
{
    APPROVED, 
    DECLINED_INSUFFECIENT_FUND, 
    DECLINED_STOLEN_CARD, 
    INTERNAL_SERVER_ERROR
}EN_transState_t;

typedef enum EN_serverError_t
{
    SERVER_OK,
    SAVING_FAILED,
    TRANSACTION_NOT_FOUND,
    ACCOUNT_NOT_FOUND,
    LOW_BALANCE
}EN_serverError_t;

typedef struct ST_transaction_t
{
    ST_cardData_t cardHolderData;
    ST_terminalData_t terminalData;
    EN_transState_t transState;
    uint32_t transactionSequenceNumber;
}ST_transaction_t;

typedef struct ST_accountsDB_t
{
    float balance;
    uint8_t primaryAccountNumber[20];
}ST_accountsDB_t;

ST_accountsDB_t bankAccounts[255];
ST_transaction_t transactionData[255];

uint8_t ACCOUNTS_NUMBER;
uint8_t TRANSACTIONS_NUMBER;

EN_transState_t  recieveTransactionData(ST_transaction_t* transData);
EN_serverError_t isValidAccount(ST_cardData_t* cardData);
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData);
EN_serverError_t saveTransaction(ST_transaction_t* transData);
EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t* transData);

uint8_t* removeWhiteSpaces(uint8_t* string);
uint8_t* enumToStr_transState(EN_transState_t transState);
void modifyNewBalance(uint8_t* cardPAN);
uint16_t getLastTransNumber();
void readAccountsFile();
void writeTransactionsFile();

#endif