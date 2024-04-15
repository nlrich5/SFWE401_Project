#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h> 


<<<<<<< HEAD

=======
//#include <windows.h>


#define NUM_RANDOM_BYTES 16 // Adjust as needed for your application
>>>>>>> 31880e84a3ffa05133b5425cafd95bc7d8262363
#define DECK_SIZE 108 // Assuming DECK_SIZE is defined somewhere





// structure to define a new variable of type card
typedef struct card_s {
	char color[7];
	int value;
	char action[15];
	struct card_s* pt;
}card;

// where the deck of cards are stored
card deck[108];
int currCard = 0;
int currDiscard = 0;
int playerNum, currPlayer = 1;

//where colors are stored
char red[7] = "red";
char yellow[7] = "yellow";
char green[7] = "green";
char blue[7] = "blue";

//bool for direction (reverse)
bool reverse = false;

// function to create all the cards of color c for the game deck
void setup_color(char* c) {
	// creates cards from 1-9 of color c twice.
	for (int i = 1; i < 10; i++) {
		for (int j = 0; j < 2; j++) {
			card num;
			strcpy(num.color, c);
			num.value = i;
			strcpy(num.action, "none");
			deck[currCard] = num;
			currCard++;
		}
	}
	// creates skip, reverse and plus two card twice
	for (int i = 0; i < 2; i++) {
		card skip;
		strcpy(skip.color, c);
		skip.value = -1; // not a regular number
		strcpy(skip.action, "skip");
		deck[currCard] = skip;
		currCard++;

		card rev;
		strcpy(rev.color, c);
		rev.value = -2;
		strcpy(rev.action, "reverse");
		deck[currCard] = rev;
		currCard++;

		card two;
		strcpy(two.color, c);
		two.value = -3;
		strcpy(two.action, "plus two");
		deck[currCard] = two;
		currCard++;
	}

	card zero;
	strcpy(zero.color, c);
	zero.value = 0;
	strcpy(zero.action, "none");
	deck[currCard] = zero;
	currCard++;
}

// function to create the special cards (wild and plus four) for the game deck
void setup_special() {
	for (int i = 0; i < 4; i++) {
		card wild;
		strcpy(wild.color, "any");
		wild.value = -4;
		strcpy(wild.action, "wild");
		deck[currCard] = wild;
		currCard++;

		card four;
		strcpy(four.color, "any");
		four.value = -5;
		strcpy(four.action, "wild plus four");
		deck[currCard] = four;
		currCard++;
	}
}


void randomize_deck(){
    _Atomic unsigned int seed = (unsigned int)time(NULL); // Atomic seed variable for thread safety
    card temp; // Temporary variable for swapping

    // Fisher-Yates shuffle algorithm
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        unsigned int random_index;
        // Generate secure random index using atomic operations and system time
        seed = seed * 1103515245 + 12345; // Linear congruential generator (LCG)
        random_index = seed % (i + 1);

        // Swap deck[i] and deck[random_index]
        temp = deck[random_index];
        deck[random_index] = deck[i];
        deck[i] = temp;
    }
}

//if/else for our deck or file
void shuffleOrLoad(int input) {
	if (input == 1) {
		// Code for when a new deck is shuffled instead of loaded
		setup_color(red);
		setup_color(yellow);
		setup_color(green);
		setup_color(blue);

		setup_special();
		randomize_deck();
		currCard = 0; // resets back to the top of the deck
		printf("The deck is shuffled.\n");
	}
	else if (input == 2) {
		FILE* input = NULL;
		char userFile[50];
		printf("Enter the file name: ");
		scanf("%s", userFile);
		fopen(userFile, "r");
		for (int i = 0; i < 108; i++) {
			fscanf(userFile, "%d %s %s", &deck[i].value, deck[i].color, deck[i].action);
			fscanf(userFile, "%*c");
		}
	}
	else {
		printf("Invalid input\n");
		return;
	}
}

//distributes the cards into each player's hands
void distributeHands(int numPlayers, card hand[][50]) {
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < numPlayers; j++) {
			hand[j][i] = deck[currCard];
			currCard++;
		}
	}
}

//prints a card
void printCard(card inputCard) {
	if (inputCard.value >= 0) { // number card
		printf("%s %d", inputCard.color, inputCard.value);
	}
	else if (inputCard.value > -4) { // wild
		printf("%s %s", inputCard.color, inputCard.action);
	}
	else { // plus four
		printf("%s %s", inputCard.color, inputCard.action);
	}
}

//checks a player's hand to see if they have UNO
bool checkUno(int cardCount[]) {
	if (cardCount[currPlayer - 1] == 1) {
		return true;
	}
	return false;
}

//checks a player's hand to see if they have won
bool checkWin(int cardCount[]) {
	if (cardCount[currPlayer - 1] == 0) {
		return true;
	}
	return false;
}

//makes sure the move a player wants to do is valid
bool checkValidMove(card discardCard, card playerCard) {
	if (playerCard.value < -3) {
		return true;
	}
	else if (strcmp(discardCard.color, playerCard.color) == 0) {
		return true;
	}
	else if (discardCard.value >= 0) {
		if (discardCard.value == playerCard.value) {
			return true;
		}
	}
	else if (strcmp(discardCard.action, playerCard.action) == 0) {
		return true;
	}
	else {
		return false;
	}
}

//makes sure that currPlayer is not greater than the # of players
void check_currPlayer() {
	if (currPlayer > playerNum) {
		currPlayer = currPlayer - playerNum;
	}
	else if (currPlayer <= 0) {
		currPlayer = playerNum + currPlayer;
	}
}

//applies the effect of the card
void cardEffect(card input, card hand[][50], int cardCount[]) {
	char temp[7];

	if (input.value >= 0) {					//for number cards
		return;
	}
	else if (input.value == -1) {			//skips
		if (reverse == false) {
			currPlayer += 1;
		}
		else {
			currPlayer -= 1;
		}
	}
	else if (input.value == -2) {			//reverse
		if (reverse == true) {
			reverse = false;
		}
		else {
			reverse = true;
		}
	}
	else if (input.value == -3) {			//plus two
		for (int i = 0; i < 2; i++) {
			if (currPlayer == playerNum) {
				hand[0][cardCount[0]] = deck[currCard];
				cardCount[0]++;
				currCard++;
			}
			else {
				hand[currPlayer][cardCount[currPlayer]] = deck[currCard];
				cardCount[currPlayer]++;
				currCard++;
			}
		}
		currPlayer += 1;
	}
	else if (input.value == -4) {			//wilds
		printf("Enter the color you would like the wild to be: ");
		scanf("%s", temp);
		strcpy(deck[currDiscard].color, temp);
	}
	else {									//wild plus four
		printf("Enter the color you would like the wild to be: ");
		scanf("%s", temp);
		strcpy(deck[currDiscard].color, temp);
		int challenger = currPlayer + 1;
		char challenge;
		if (challenger > playerNum) {
			challenger = challenger - playerNum;
		}

		//challenge portion
		printf("Player %d, whould you like to challenge (y/n)? ", challenger);
		scanf(" %c", &challenge);

		if (challenge == 'n') {						//applies the plus four if the player does not challenge
			for (int i = 0; i < 4; i++) {
				if (currPlayer == playerNum) {
					hand[0][cardCount[0]] = deck[currCard];
					cardCount[0]++;
					currCard++;
				}
				else {
					hand[currPlayer][cardCount[currPlayer]] = deck[currCard];
					cardCount[currPlayer]++;
					currCard++;
				}
			}
			currPlayer++;
		}
		else {										//if the player does challenge
			bool challengeSuccess = false;
			for (int i = 0; i < cardCount[currPlayer - 1]; i++) {	//checks if the person playing the wild could have played another card
				if (checkValidMove(deck[currDiscard - 1], hand[currPlayer - 1][i]) == true) {
					challengeSuccess = true;
					break;
				}
			}
			if (challengeSuccess == true) {				//if the challenge is successful
				for (int i = 0; i < 4; i++) {
					hand[currPlayer - 1][cardCount[currPlayer - 1]] = deck[currCard];
					cardCount[currPlayer - 1]++;
					currCard++;
				}
			}
			else {									//if the challenge fails
				for (int i = 0; i < 6; i++) {
					if (currPlayer == playerNum) {
						hand[0][cardCount[0]] = deck[currCard];
						cardCount[0]++;
						currCard++;
					}
					else {
						hand[currPlayer][cardCount[currPlayer]] = deck[currCard];
						cardCount[currPlayer]++;
						currCard++;
					}
				}
				currPlayer++;
			}
		}
	}

}

//function for a player's turn
void playerTurn(card hand[][50], int cardCount[]) {
	int userInput, i;

	//printing the discard pile and the player's hand
	printf("\n");
	printf("Discard pile: ");
	if (deck[currDiscard].value)
		printCard(deck[currDiscard]);
	printf("\nPlayer %d's hand: \n", currPlayer);
	for (i = 0; i < cardCount[currPlayer - 1]; i++) {
		printf("%d: ", i + 1);
		printCard(hand[currPlayer - 1][i]);
		if (i < cardCount[currPlayer - 1] - 1) {
			printf("\n");
		}
	}

	//chooses the card to play or draw
	printf("\nPress 1-%d to play any card from your hand, or 0 to draw a card: ", cardCount[currPlayer - 1]);
	scanf("%d", &userInput);

	while (userInput > 0) {						//loop to ensure the player plays a valid card
		if (checkValidMove(deck[currDiscard], hand[currPlayer - 1][userInput - 1]) == true) {
			currDiscard++;
			
			deck[currDiscard] = hand[currPlayer - 1][userInput - 1];
			for (i = (userInput - 1); i < cardCount[currPlayer - 1]; i++) {
				hand[currPlayer - 1][i] = hand[currPlayer - 1][i + 1];
			}
			cardCount[currPlayer - 1]--;
			userInput = -1;
			cardEffect(deck[currDiscard], hand, cardCount);
		}
		else {
			printf("That card cannot be played.\n");
			printf("\nPlayer %d's hand: ", currPlayer);
			for (i = 0; i < cardCount[currPlayer - 1]; i++) {
				printCard(hand[currPlayer - 1][i]);
				if (i < cardCount[currPlayer - 1] - 1) {
					printf(", ");
				}
			}
			printf("\nPress 1-%d to play any card from your hand, or 0 to draw a card: ", cardCount[currPlayer - 1]);
			scanf("%d", &userInput);
		}
	}

	if (userInput == 0) {					//drawing a card
		hand[currPlayer - 1][cardCount[currPlayer - 1]] = deck[currCard];
		currCard++;
		cardCount[currPlayer - 1]++;
	}

	//checks at the end of each player's turn if they have UNO and prints it
	if (checkUno(cardCount) == true) {
		printf("\nPlayer %d has UNO\n", currPlayer);
	}
}



int main(void) {
	int userShuffle;			//variable for choice between using our deck or a file
	printf("Let's play a game of UNO\n");
	printf("Press 1 to shuffle the deck or 2 to load a deck: ");
	scanf("%d", &userShuffle);

	shuffleOrLoad(userShuffle);

	//setting up player counter
	printf("Enter the number of players: ");
	scanf("%d", &playerNum);

	//creating the hands
	card hands[10][50];

	//creating an array to store how many cards each player has
	int cardCount[10];
	for (int i = 0; i < playerNum; i++) {
		cardCount[i] = 7;
	}

	//distributing the cards and placing the first discard card
	distributeHands(playerNum, hands);
	deck[0] = deck[currCard];

	//prints the setup to the game
	printf("First card: ");
	printCard(deck[0]);
	printf("\n");

	//checks if the first card affects the first player
	//cardEffect(discard[0], hands, cardCount);

	//first player's turn
	playerTurn(hands, cardCount);


	while (checkWin(cardCount) == false) {			//allows play while no one has won

		//easiest way I could think to do the reverse
		system("cls"); //clears the console for each players when its their turn
		if (reverse == false) {
			currPlayer++;
		}
		else {
			currPlayer--;
		}

		check_currPlayer();
		playerTurn(hands, cardCount);

	}

	//final print for the winner
	printf("Player %d has won\n", currPlayer);
}