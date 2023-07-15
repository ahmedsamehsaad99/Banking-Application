#include "card.h"

EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
    uint8_t* cardOwner = (uint8_t*)malloc(30);

    puts("Enter the card holder name: ");
    gets(cardOwner);

    if (strlen(cardOwner) == 0 || strlen(cardOwner) < 20 || strlen(cardOwner) > 24)
        return WRONG_NAME;
    else
    {
        strcpy_s(cardData->cardHolderName, 25, cardOwner);
        return CARD_OK;
    }
}

EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{    // 
    uint8_t* expiryDate = malloc(5);

    puts("Enter the card expiry date: ");
    gets(expiryDate);

    if (expiryDate == NULL || strlen(expiryDate) < 5 || strlen(expiryDate) > 5)
        return WRONG_EXP_DATE;
    else
    {
        strcpy_s(cardData->cardExpirationDate, 6, expiryDate);
        return CARD_OK;
    }
}


EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
    uint8_t* PAN = (uint8_t*)malloc(19);

    puts("Enter the PAN: ");
    gets(PAN);

    if (PAN == NULL || strlen(PAN) < 16 || strlen(PAN) > 19)
        return WRONG_PAN;
    else
    {
        strcpy_s(cardData->primaryAccountNumber, 20, PAN);
        return CARD_OK;
    }
}