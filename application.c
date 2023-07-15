#include "application.h"

void appStart(void)
{
	printf("========================================================\n");

	ST_cardData_t theCard;
	ST_terminalData_t theTerminal;
	ST_transaction_t theServer;

	//fflush(stdin); // TO CLEAR BUFFER
	readAccountsFile();

	EN_cardError_t cardHolderName = getCardHolderName(&theCard);
	EN_cardError_t cardExpiryDate = getCardExpiryDate(&theCard);
	EN_cardError_t cardPAN = getCardPAN(&theCard);
	
	EN_terminalError_t transactionDate = getTransactionDate(&theTerminal);
	EN_terminalError_t cardExpiryState = isCardExpired(&theCard, &theTerminal);
	EN_terminalError_t cardPANState = isValidCardPAN(&theCard);

	if (cardExpiryState == EXPIRED_CARD)
	{
		printf("\nERROR: THE CARD IS EXPIRED\n");
		return;
	}

	if (cardPANState == INVALID_CARD)
	{
		printf("\nERROR: THE CARD IS INVALID\n");
		return;
	}

	// SETTING TERMINAL MAX AMOUNT
	EN_terminalError_t settingMaxAmountState = setMaxAmount(&theTerminal);

	EN_terminalError_t transactionAmountState = getTransactionAmount(&theTerminal);

	EN_terminalError_t belowMaxAmountState = isBelowMaxAmount(&theTerminal);

	if (belowMaxAmountState == EXCEED_MAX_AMOUNT)
	{
		printf("\nERROR: EXCEEDED MAX AMOUNT\n");
		getchar(); // TO AVOID BUFFER
		return;
	}

	theServer.cardHolderData = theCard;
	theServer.terminalData = theTerminal;

	EN_transState_t transactionDataState = recieveTransactionData(&theServer);

	if (transactionDataState != APPROVED)
	{
			printf("\nERROR OCCURED NUMBER #%d - %s\n", transactionDataState, enumToStr_transState(transactionDataState));
			getchar(); // TO AVOID BUFFER
			return;
	}

	printf("\nTRANSACTION SUCESS!\n");
	printf("PAN:\t %s\n", theServer.cardHolderData.primaryAccountNumber);
	printf("TRANSACTION NUMBER:\t %d\n", theServer.transactionSequenceNumber);
	printf("AMOUNT:\t %.2fLE\n", theServer.terminalData.transAmount);
	
	uint16_t index = searchAccountsDB(theServer.cardHolderData.primaryAccountNumber);

	printf("NEW BALANCE: %.2fLE\n", bankAccounts[index].balance);

	getchar(); // TO AVOID BUFFER
}