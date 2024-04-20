#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define DECK_SIZE 108

typedef struct card_s {
    char color[7];
    int value;
    char action[15];
    struct card_s* pt;
} card;

card deck[DECK_SIZE];
int currCard = 0;
int currDiscard = 0;
int numCardsInHands = 0;
int playerNum = -1;
int currPlayer = 1;

char red[7] = "red";
char yellow[7] = "yellow";
char green[7] = "green";
char blue[7] = "blue";

bool reverse = false;

void setup_color(char* c) {
    for (int i = 1; i < 10; i++) {
        for (int j = 0; j < 2; j++) {
            card num;
            strcpy(num.color, c);
            num.value = i;
            strcpy(num.action, "none");
            deck[currCard++] = num;
        }
    }
    for (int i = 0; i < 2; i++) {
        card actions[] = {
            { .value = -1, .action = "skip" },
            { .value = -2, .action = "reverse" },
            { .value = -3, .action = "plus two" }
        };
        for (int j = 0; j < 3; j++) {
            strcpy(actions[j].color, c);
            deck[currCard++] = actions[j];
        }
    }
    card zero = {.value = 0, .action = "none"};
    strcpy(zero.color, c);
    deck[currCard++] = zero;
}

void setup_special() {
    for (int i = 0; i < 4; i++) {
        card wild = {.value = -4, .action = "wild", .color = "any"};
        deck[currCard++] = wild;
        card four = {.value = -5, .action = "wild plus four", .color = "any"};
        deck[currCard++] = four;
    }
}

void randomize_deck() {
    unsigned int seed = (unsigned int)time(NULL); // Generate a seed based on current time
    srand(seed); // Seed the random number generator

    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}


void shuffleOrLoad(int input) {
    if (input == 2) {
        FILE* filePtr = NULL;
        char userFile[50];
        printf("Enter the file name: ");
        scanf("%s", userFile);
        filePtr = fopen(userFile, "r");
        while (filePtr == NULL) {
            printf("Failed to open the file. Please re-enter the file name: ");
            scanf("%s", userFile);
            filePtr = fopen(userFile, "r");
        }
        for (int i = 0; i < DECK_SIZE; i++) {
            fscanf(filePtr, "%d %s %s", &deck[i].value, deck[i].color, deck[i].action);
            fscanf(filePtr, "%*c");  // Read and ignore newline characters
        }
        fclose(filePtr);
    } else {
        setup_color(red);
        setup_color(yellow);
        setup_color(green);
        setup_color(blue);
        setup_special();
        randomize_deck();
        currCard = 0;
        printf("The deck is shuffled.\n");
    }
}

void reshuffle(int discardCount) {
    for (int i = 0; i < discardCount; i++) {
        int random1 = rand() % discardCount;
        int random2 = rand() % discardCount;
        card temp = deck[random1];
        deck[random1] = deck[random2];
        deck[random2] = temp;
    }
}

void distributeHands(int numPlayers, card hand[][50]) {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < numPlayers; j++) {
            hand[j][i] = deck[currCard++];
        }
    }
}

void printCard(card inputCard) {
    if (inputCard.value >= 0) {
        printf("%s %d", inputCard.color, inputCard.value);
    } else {
        printf("%s %s", inputCard.color, inputCard.action);
    }
}

bool checkUno(int cardCount[]) {
    return cardCount[currPlayer - 1] == 1;
}

bool checkWin(int cardCount[]) {
    return cardCount[currPlayer - 1] == 0;
}

bool checkValidMove(card discardCard, card playerCard) {
    if (playerCard.value < -3) {
        return true;
    } else if (strcmp(discardCard.color, playerCard.color) == 0) {
        return true;
    } else if (discardCard.value == playerCard.value) {
        return true;
    } else {
        return strcmp(discardCard.action, playerCard.action) == 0;
    }
}

void check_currPlayer() {
    if (currPlayer > playerNum) {
        currPlayer -= playerNum;
    } else if (currPlayer <= 0) {
        currPlayer += playerNum;
    }
}

void cardEffect(card input, card hand[][50], int cardCount[]) {
    if (input.value >= 0) {
        return;
    } else if (input.value == -1) {
        currPlayer += (reverse ? -1 : 1);
    } else if (input.value == -2) {
        reverse = !reverse;
    } else if (input.value == -3) {
        int nextPlayer = (currPlayer % playerNum);
        for (int i = 0; i < 2; i++) {
            hand[nextPlayer][cardCount[nextPlayer]++] = deck[currCard++];
        }
        currPlayer += (reverse ? -1 : 1);
    } else if (input.value == -4 || input.value == -5) {
        char color[7];
        printf("Enter the color you would like the wild to be: ");
        scanf("%s", color);
        strcpy(deck[currDiscard].color, color);
        if (input.value == -5) {
            int nextPlayer = (currPlayer % playerNum);
            for (int i = 0; i < 4; i++) {
                hand[nextPlayer][cardCount[nextPlayer]++] = deck[currCard++];
            }
            currPlayer += (reverse ? -1 : 1);
        }
    }
}

void playerTurn(card hand[][50], int cardCount[]) {
    int userInput = -1;
    printf("\nDiscard pile: ");
    printCard(deck[currDiscard]);
    printf("\nPlayer %d's hand: \n", currPlayer);
    for (int i = 0; i < cardCount[currPlayer - 1]; i++) {
        printf("%d: ", i + 1);
        printCard(hand[currPlayer - 1][i]);
        printf("\n");
    }

    printf("\nPress 1-%d to play any card from your hand, or 0 to draw a card: ", cardCount[currPlayer - 1]);
    scanf("%d", &userInput);

    while (userInput != -1) {
        if (userInput == 0) {
            hand[currPlayer - 1][cardCount[currPlayer - 1]++] = deck[currCard++];
            userInput = -1;
        } else if (userInput > 0 && userInput <= cardCount[currPlayer - 1] &&
                   checkValidMove(deck[currDiscard], hand[currPlayer - 1][userInput - 1])) {
            currDiscard++;
            deck[currDiscard] = hand[currPlayer - 1][userInput - 1];
            for (int i = userInput - 1; i < cardCount[currPlayer - 1] - 1; i++) {
                hand[currPlayer - 1][i] = hand[currPlayer - 1][i + 1];
            }
            cardCount[currPlayer - 1]--;
            cardEffect(deck[currDiscard], hand, cardCount);
            userInput = -1;
        } else {
            printf("Invalid selection. Try again: ");
            scanf("%d", &userInput);
        }
    }
}

int main(void) {
    int userShuffle = 0;
    printf("Let's play a game of UNO\n");
    printf("Press 1 to shuffle the deck or 2 to load a deck: ");
    scanf("%d", &userShuffle);

    shuffleOrLoad(userShuffle);

    while (playerNum > 6 || playerNum < 2) {
        printf("Enter the number of players (2-6): ");
        scanf("%d", &playerNum);
    }

    card hands[6][50];
    int cardCount[6] = {7, 7, 7, 7, 7, 7};

    distributeHands(playerNum, hands);
    deck[0] = deck[currCard];
    numCardsInHands = playerNum * 7;

    printf("First card: ");
    printCard(deck[0]);
    printf("\n");

    while (!checkWin(cardCount)) {
        system("cls");
        currPlayer = reverse ? currPlayer - 1 : currPlayer + 1;
        check_currPlayer();
        playerTurn(hands, cardCount);

        if (currDiscard == (107 - numCardsInHands)) {
            reshuffle(currDiscard);
            currCard = 0;
        }
    }

    printf("Player %d has won\n", currPlayer);
    return 0;
}
