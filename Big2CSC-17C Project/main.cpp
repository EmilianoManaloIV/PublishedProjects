#include <iostream>
#include "Card.h"
#include "Deck.h"
#include "PlayingHand.h"
#include "Player.h"
#include <stack>
#include <queue>
#include <random>
#include <numeric>
#include <algorithm>
using namespace std;

// Function to find player with three of clubs
Player* findFirstPlayer(Player* players[], int numPlayers) {
    for (int i = 0; i < numPlayers; i++) {
        for (const auto& card : players[i]->getPlayerDeck().getCards()) {
            if (card.getCard() == 1 && card.getSuit() == 1) {  // 3 of clubs
                return players[i];
            }
        }
    }
    return nullptr;
}

// Function to display all players' card counts
void displayCardCounts(Player* players[], int numPlayers) {
    cout << "\n=== Card Counts ===" << endl;
    for (int i = 0; i < numPlayers; i++) {
        cout << "Player " << (i + 1) << ": " << players[i]->getAmountOfCards() << " cards" << endl;
    }
    cout << "=================" << endl;
}

int main()
{
    const int amountOfPlayers = 4;
    Deck* TestDeck = new Deck(52);
    Player* TestPlayer = new Player();
    Player* AI1 = new Player(true);  // Set as AI
    Player* AI2 = new Player(true);  // Set as AI
    Player* AI3 = new Player(true);  // Set as AI

    // Array of players for easier iteration
    Player* players[amountOfPlayers] = {TestPlayer, AI1, AI2, AI3};

    // Stack to keep track of previous hands
    stack<PlayingHand> handHistory;
    
    // Queue to manage turn order
    queue<Player*> turnOrder;

    // Deal cards
    TestDeck->shuffleDeck();
    
    // Deal cards directly from the shuffled deck
    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < amountOfPlayers; j++) {
            if(!TestDeck->getCards().empty()) {
                Card dealtCard = TestDeck->takeTopFromDeck();
                players[j]->addToPlayerHand(dealtCard);
            }
        }
    }

    // Find player with three of clubs and set up turn order
    Player* firstPlayer = findFirstPlayer(players, amountOfPlayers);
    if (!firstPlayer) {
        cout << "Error: No player has the three of clubs!" << endl;
        return 1;
    }

    // Set up turn order starting with player who has three of clubs
    turnOrder.push(firstPlayer);
    for (int i = 0; i < amountOfPlayers; i++) {
        if (players[i] != firstPlayer) {
            turnOrder.push(players[i]);
        }
    }

    // Display initial card counts
    displayCardCounts(players, amountOfPlayers);

    // Game loop
    bool gameOver = false;
    int consecutivePasses = 0;
    Player* lastPlayer = nullptr;  // Track who played the last hand

    while (!gameOver) {
        Player* currentPlayer = turnOrder.front();
        cout << "\n=== Player " << (currentPlayer == TestPlayer ? "1" : 
                                  currentPlayer == AI1 ? "2" : 
                                  currentPlayer == AI2 ? "3" : "4") << "'s Turn ===" << endl;
        
        // Get the current hand to beat (if any)
        PlayingHand currentHand;
        if (!handHistory.empty()) {
            currentHand = handHistory.top();
        }

        // Player takes their turn
        PlayingHand playedHand = currentPlayer->decision(currentHand);

        // Check if player passed
        if (playedHand.getCards().empty()) {
            cout << "Player " << (currentPlayer == TestPlayer ? "1" : 
                                currentPlayer == AI1 ? "2" : 
                                currentPlayer == AI2 ? "3" : "4") << " passes" << endl;
            consecutivePasses++;
            // Move player to back of queue
            turnOrder.pop();
            turnOrder.push(currentPlayer);
        } else {
            // Player played a hand
            handHistory.push(playedHand);
            consecutivePasses = 0;
            lastPlayer = currentPlayer;
            // Move player to back of queue
            turnOrder.pop();
            turnOrder.push(currentPlayer);
        }

        // Display updated card counts after each turn
        displayCardCounts(players, amountOfPlayers);

        // Check for game over conditions
        if (currentPlayer->getAmountOfCards() == 0) {
            cout << "\nPlayer " << (currentPlayer == TestPlayer ? "1" : 
                                  currentPlayer == AI1 ? "2" : 
                                  currentPlayer == AI2 ? "3" : "4") << " wins!" << endl;
            gameOver = true;
            continue;
        }

        // Check if we should start a new round (3 consecutive passes)
        if (consecutivePasses >= 3) {
            cout << "\n=== New Round ===" << endl;
            handHistory = stack<PlayingHand>();  // Clear the hand history
            consecutivePasses = 0;
            
            // If someone played a hand, they start the next round
            if (lastPlayer) {
                // Reorder queue to start with last player
                while (turnOrder.front() != lastPlayer) {
                    turnOrder.push(turnOrder.front());
                    turnOrder.pop();
                }
            }
        }
    }

    // Cleanup
    delete TestDeck;
    delete TestPlayer;
    delete AI1;
    delete AI2;
    delete AI3;
    return 0;
}