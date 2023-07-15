#include "terminal.h"

EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	// GET PC TIME, ALWAYS OK
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	// SET THE PC TIME AS TRANSACTION DATE AS MM/YY
	sprintf(termData->transactionDate, "%02d/%02d", tm.tm_mon + 1, (tm.tm_year + 1900)%100);
	return TERMINAL_OK;
}

EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData)
{
	uint8_t* currentDate = malloc(7);
	uint8_t* cardExpiryDate = malloc(7);
	
	strcpy(currentDate, termData->transactionDate);
	strcpy(cardExpiryDate, cardData->cardExpirationDate);

	uint8_t currentDateArray[2];
	uint8_t expiredDateArray[2];
	

	// FILLING ARRAYS WITH INTEGER VALUES OF MONTH AND YEAR
	expiredDateArray[0] = (cardExpiryDate[0] - 48) * 10 + (cardExpiryDate[1] - 48);
	expiredDateArray[1] = (cardExpiryDate[3] - 48) * 10 + (cardExpiryDate[4] - 48);

	currentDateArray[0] = (currentDate[0] - 48) * 10 + (currentDate[1] - 48);
	currentDateArray[1] = (currentDate[3] - 48) * 10 + (currentDate[4] - 48);

	if (expiredDateArray[1] > currentDateArray[1] || ((expiredDateArray[1] == currentDateArray[1]) && (expiredDateArray[0] > currentDateArray[0])))
		return TERMINAL_OK;
	else
		return EXPIRED_CARD;
}


EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
	// USING LUHN'S ALGORITHM
	uint8_t * cardPAN = cardData->primaryAccountNumber;
	uint8_t PANarr[25]; // Integer array for calculations
	uint8_t digitsSummation = 0, i = 0, j = 0;

	// 5527 1797 8742 9765
	// 1+5+4+7+ 2+7+9+7+ 7+7+8+2+ 9+7+3+5
	for (; i < strlen(cardPAN); i++, j++)
	{
		if (cardPAN[i] == ' ') // TO SKIP SPACES
			i++;

		PANarr[j] = cardPAN[i] - 48; // CONVERTING CHARS TO THEIR NUMBER FORM USING ASCII

		if (j % 2 == 0)	// LUHN'S ALGORITHM, CHECKING EVEN INDICES
		{
			PANarr[j] = PANarr[j] * 2; // DOUBLING THEIR VALUES
			if (PANarr[j] / 10 != 0) // IF TWO DIGITS
				PANarr[j] = (PANarr[j] % 10) + (PANarr[j] / 10); // ADD THE TWO DIGITS
		}

		digitsSummation += PANarr[j]; // TO GET THE FINAL SUMMATION
	}

	if (digitsSummation % 10 == 0) // IF SUM IS DIVISIBLE BY 10, THEN IT'S A VALID PAN
		return TERMINAL_OK;
	else
		return INVALID_CARD;
}

EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{
	float maxAmount;

	puts("Enter the terminal max amount: ");
	scanf("%f", &maxAmount);

	if (maxAmount <= 0)
		return INVALID_MAX_AMOUNT;
	else
	{
		termData->maxTransAmount = maxAmount;
		return TERMINAL_OK;
	}
}

EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	float transactionAmount;

	puts("Enter the transaction amount: ");
	scanf("%f", &transactionAmount);

	if (transactionAmount <= 0)
		return INVALID_AMOUNT;
	else
	{
		termData->transAmount = transactionAmount;
		return TERMINAL_OK;
	}
}

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	float transactionAmount = termData->transAmount, maxAmount = termData->maxTransAmount;

	if (transactionAmount <= maxAmount)
		return TERMINAL_OK;
	else
	{
		return EXCEED_MAX_AMOUNT;
	}
}