#ifndef PLAYER_H
#define PLAYER_H
#include "Deck.h"
#include "PlayingHand.h"
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>

// AI Behavior Configuration
const int AGGRESSIVE_CARD_COUNT = 3;     // Be aggressive when cards <= this
const int VERY_STRONG_HAND_TYPE = 8;     // Four of a kind and above
const int MODERATE_HAND_TYPE = 5;        // Straight and above

class Player
{
private:
    map<int,string> handType =
    {
        {0, "Skip"},
        {1,"High Card"},
        {2, "Pair"},
        {3, "Two Pair"},
        {4, "Three Of A Kind"},
        {5, "Straight"},
        {6, "Flush"}, 
        {7, "Full House"},
        {8, "Four Of A Kind"},
        {9, "Straight Flush"},
        {10,"Royal Flush"}
    };
    map<int, string> cardRankRef = { 
        {1, "3"},
        {2, "4"},
        {3, "5"},
        {4, "6"},
        {5, "7"},
        {6, "8"},
        {7, "9"},
        {8, "10"},
        {9, "J"},
        {10, "Q"},
        {11, "K"},
        {12, "A"},
        {13, "2"}}; //Map for card rank
    map<int, char> cardSuitesRef = 
    {{1, '&'},
    {2, '^'},
    {3, 'V'},
    {4, 'O'}};  //Map for suit rank

    string playerName;
    bool isAi;
    Deck playerDeck;
    PlayingHand playerHand;

    list<int> handSelection();
    PlayingHand findBestHand(PlayingHand hand);
    bool shouldPass(PlayingHand currentHand);
    PlayingHand tryHandCombination(const std::list<Card>& cards, 
                                 std::list<Card>::const_iterator start,
                                 size_t count,
                                 int requiredType,
                                 PlayingHand currentHand);
    bool isValidPlay(PlayingHand selectedHand, PlayingHand currentHand);

public:
    void addToPlayerHand(list<Card>);
    void addToPlayerHand(Card);
    //---Instantiation Routines---
    Player();
    Player(bool);
    ~Player();
    //---Operator Overloads---
    bool operator==(const Player&) const;
    bool operator!=(const Player&) const;
    bool operator<(const Player&) const;
    bool operator>(const Player&) const;
    bool operator<=(const Player&) const;
    bool operator>=(const Player&) const;

    //---Turn Routines---
    void playerTurn(PlayingHand);
    void aiTurn(PlayingHand);
    //---Player Object Decision---
    PlayingHand decision(PlayingHand);
    //---Get amount of cards the player has
    int getAmountOfCards();
    const Deck& getPlayerDeck() const { return playerDeck; }
};
Player::Player()
{
    //Make sure this is a player
    isAi = false;
}
Player::Player(bool b)
{
    if(b == true)
    {
        isAi = true;
    }
}
Player::~Player()
{
    //Does nothing because the STL automatically deletes itself
}
void Player::addToPlayerHand(list<Card> Cs)
{
    playerDeck.placeCardsIntoDeck(Cs);
}
void Player::addToPlayerHand(Card C)
{
    playerDeck.placeCardIntoDeck(C);
}
bool Player::operator==(const Player& other) const {
    return playerHand == other.playerHand;
}

bool Player::operator!=(const Player& other) const {
    return !(*this == other);
}

bool Player::operator<(const Player& other) const {
    return playerHand < other.playerHand;
}

bool Player::operator>(const Player& other) const {
    return playerHand > other.playerHand;
}

bool Player::operator<=(const Player& other) const {
    return !(*this > other);
}

bool Player::operator>=(const Player& other) const {
    return !(*this < other);
}
void Player::playerTurn(PlayingHand currentHand) {
    cout << "===---[[Your Turn]]---===" << endl;
    char selection;
    bool validPlay = false;

    do {
        // Display current hand if there is one
        if (!currentHand.getCards().empty()) {
            cout << "=-=LAST PLAYED HAND=-=" << endl;
            currentHand.evaluateHand();
            currentHand.displayHand();
        } else {
            cout << "You can play any valid hand combination!" << endl;
        }

        // Get player's selection
        list<int> selectedIndices = handSelection();
        
        // If player chose to skip
        if (selectedIndices.empty()) {
            cout << "You chose to skip your turn." << endl;
            playerHand = PlayingHand();  // Set empty hand to indicate skip
            return;
        }

        playerHand.addToHand(playerDeck.selectCardsAndTakeFromDeck(selectedIndices));
        playerHand.evaluateHand();

        // Check if the play is valid
        validPlay = isValidPlay(playerHand, currentHand);

        if (!validPlay) {
            cout << "Invalid play! ";
            if (currentHand.getCards().empty()) {
                cout << "Must be a valid hand combination." << endl;
            } else {
                cout << "Must match hand type and beat the current hand." << endl;
            }
            playerDeck.placeCardsIntoDeck(playerHand.discardHand());
            playerDeck.sortDeck();
            continue;
        }

        // Display the hand being played
        cout << "===HAND BEING PLAYED===" << endl;
        cout << "HAND: " << handType[playerHand.getHandType()] << endl;
        cout << "RANK: " << cardRankRef[playerHand.getHighestCardRank()] << endl;
        cout << "SUIT: " << cardSuitesRef[playerHand.getHighestHandSuit()] << endl;

        cout << "Type o to confirm, type anything else to reselect: ";
        cin >> selection;
        if (selection != 'o') {
            playerDeck.placeCardsIntoDeck(playerHand.discardHand());
            playerDeck.sortDeck();
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while (selection != 'o' || !validPlay);
}
list<int> Player::handSelection()
{
     // First display all cards
     cout << "---YOUR CARDS---" << endl;
     int i = 0;
     for (const auto& card : playerDeck.getCards())
     {
         cout << i << ". [ ] ";
         card.displayCard();
         cout << endl;
         ++i;
     }
 
     // Ask for input
     cout << "Select Cards By Typing Their Index (or press Enter to skip): ";
     string inputLine;
     getline(cin, inputLine);
 
     // If input is empty, return empty list to indicate skip
     if (inputLine.empty()) {
         return list<int>();
     }
 
     istringstream iss(inputLine);
     list<int> selectedIndices;
     int index;
     while (iss >> index) {
         selectedIndices.push_back(index); // Store selected indices
     }
 
     // Display selected cards again with [X] indicator
     cout << "---SELECTED CARDS---" << endl;
     i = 0;
     for (const auto& card : playerDeck.getCards())
     {
         // Check if current index is selected
         bool selected = (find(selectedIndices.begin(), selectedIndices.end(), i) != selectedIndices.end());
         cout << i << ". [" << (selected ? "X" : " ") << "] ";
         card.displayCard();
         cout << endl;
         ++i;
     }
     set<int> cleanedInput(selectedIndices.begin(), selectedIndices.end());
     list<int> output(cleanedInput.begin(), cleanedInput.end());
     return output;
}
void Player::aiTurn(PlayingHand currentHand) {
    cout << "===---[[AI Turn]]---===" << endl;
    
    // Convert player's deck to a PlayingHand for AI evaluation
    PlayingHand aiHand;
    for (const auto& card : playerDeck.getCards()) {
        aiHand.addToHand(card);
    }
    
    // If no hand is being played (after all players passed), find the best possible hand
    if (currentHand.getCards().empty()) {
        PlayingHand bestHand = findBestHand(aiHand);
        // Remove the cards from the deck that were used in the hand
        for (const auto& card : bestHand.getCards()) {
            playerDeck.removeCard(card);
        }
        playerHand = bestHand;
        cout << "===HAND BEING PLAYED===" << endl;
        cout << "HAND: " << handType[bestHand.getHandType()] << endl;
        cout << "RANK: " << cardRankRef[bestHand.getHighestCardRank()] << endl;
        cout << "SUIT: " << cardSuitesRef[bestHand.getHighestHandSuit()] << endl;
        return;
    }

    // Get the required hand type and card count
    int requiredType = currentHand.getHandType();
    int requiredCount = currentHand.getCards().size();
    
    // Try to find a valid hand to play that matches the current hand type
    std::list<Card> allCards = playerDeck.getCards();
    PlayingHand bestHand;
    int bestRank = -1;

    // Try each possible starting position
    for (auto start = allCards.begin(); start != allCards.end(); ++start) {
        PlayingHand tempHand = tryHandCombination(allCards, start, requiredCount, 
                                                requiredType, currentHand);
        if (!tempHand.getCards().empty() && 
            tempHand.getHighestCardRank() > bestRank) {
            bestHand = tempHand;
            bestRank = tempHand.getHighestCardRank();
        }
    }

    // If we found a valid hand to play
    if (bestRank > -1) {
        // Remove the cards from the deck that were used in the hand
        for (const auto& card : bestHand.getCards()) {
            playerDeck.removeCard(card);
        }
        playerHand = bestHand;
        cout << "===HAND BEING PLAYED===" << endl;
        cout << "HAND: " << handType[bestHand.getHandType()] << endl;
        cout << "RANK: " << cardRankRef[bestHand.getHighestCardRank()] << endl;
        cout << "SUIT: " << cardSuitesRef[bestHand.getHighestHandSuit()] << endl;
        return;
    }

    // Check if we should pass
    if (shouldPass(currentHand)) {
        cout << "AI passes" << endl;
        return;
    }

    // Try aggressive play if we have few cards
    if (playerDeck.getCards().size() <= AGGRESSIVE_CARD_COUNT) {
        PlayingHand aggressiveHand = findBestHand(aiHand);
        if (aggressiveHand.getHandType() == requiredType && 
            aggressiveHand.getCards().size() == requiredCount &&
            aggressiveHand.getHighestCardRank() > currentHand.getHighestCardRank()) {
            // Remove the cards from the deck that were used in the hand
            for (const auto& card : aggressiveHand.getCards()) {
                playerDeck.removeCard(card);
            }
            playerHand = aggressiveHand;
            cout << "===HAND BEING PLAYED===" << endl;
            cout << "HAND: " << handType[aggressiveHand.getHandType()] << endl;
            cout << "RANK: " << cardRankRef[aggressiveHand.getHighestCardRank()] << endl;
            cout << "SUIT: " << cardSuitesRef[aggressiveHand.getHighestHandSuit()] << endl;
            return;
        }
    }

    cout << "AI passes" << endl;
}

PlayingHand Player::findBestHand(PlayingHand hand) {
    std::list<Card> allCards = hand.getCards();
    PlayingHand bestHand;
    int bestHandType = -1;
    int bestCardRank = -1;

    // When starting a new round (no current hand), try to find the most efficient hand
    // that will help reduce the number of cards quickly
    if (hand.getCards().empty()) {
        // First try to find hands that use more cards
        for (int targetType = 8; targetType >= 1; --targetType) {  // Start with four of a kind
            for (size_t i = 1; i <= allCards.size(); ++i) {
                std::vector<bool> mask(allCards.size(), false);
                std::fill(mask.begin(), mask.begin() + i, true);
                
                do {
                    PlayingHand tempHand;
                    auto it = allCards.begin();
                    for (size_t j = 0; j < allCards.size(); ++j) {
                        if (mask[j]) {
                            tempHand.addToHand(*it);
                        }
                        ++it;
                    }
                    
                    tempHand.evaluateHand();
                    if (tempHand.getHandType() == targetType) {
                        // Prefer hands that use more cards
                        if (tempHand.getCards().size() > bestHand.getCards().size() ||
                            (tempHand.getCards().size() == bestHand.getCards().size() &&
                             tempHand.getHighestCardRank() > bestCardRank)) {
                            bestHandType = tempHand.getHandType();
                            bestCardRank = tempHand.getHighestCardRank();
                            bestHand = tempHand;
                        }
                    }
                } while (std::prev_permutation(mask.begin(), mask.end()));
            }
        }
        return bestHand;
    }

    // When trying to beat a current hand, find the highest possible hand type
    for (int targetType = 10; targetType >= 1; --targetType) {
        for (size_t i = 1; i <= allCards.size(); ++i) {
            std::vector<bool> mask(allCards.size(), false);
            std::fill(mask.begin(), mask.begin() + i, true);
            
            do {
                PlayingHand tempHand;
                auto it = allCards.begin();
                for (size_t j = 0; j < allCards.size(); ++j) {
                    if (mask[j]) {
                        tempHand.addToHand(*it);
                    }
                    ++it;
                }
                
                tempHand.evaluateHand();
                if (tempHand.getHandType() == targetType) {
                    if (tempHand.getHandType() > bestHandType || 
                        (tempHand.getHandType() == bestHandType && 
                         tempHand.getHighestCardRank() > bestCardRank)) {
                        bestHandType = tempHand.getHandType();
                        bestCardRank = tempHand.getHighestCardRank();
                        bestHand = tempHand;
                    }
                }
            } while (std::prev_permutation(mask.begin(), mask.end()));
        }
        
        if (bestHandType == targetType) {
            return bestHand;
        }
    }
    
    return bestHand;
}

bool Player::shouldPass(PlayingHand currentHand) {
    // Don't pass if we have very few cards
    if (playerDeck.getCards().size() <= AGGRESSIVE_CARD_COUNT) {
        return false;
    }

    // Pass if current hand is very strong
    if (currentHand.getHandType() >= VERY_STRONG_HAND_TYPE) {
        return true;
    }

    // Pass if we have many cards and current hand is moderate
    if (playerDeck.getCards().size() > 5 && 
        currentHand.getHandType() >= MODERATE_HAND_TYPE) {
        return true;
    }

    return false;
}

PlayingHand Player::tryHandCombination(const std::list<Card>& cards, 
                                     std::list<Card>::const_iterator start,
                                     size_t count,
                                     int requiredType,
                                     PlayingHand currentHand) {
    PlayingHand tempHand;
    auto it = start;
    for (size_t i = 0; i < count; ++i) {
        tempHand.addToHand(*it);
        ++it;
    }
    
    tempHand.evaluateHand();
    
    if (tempHand.getHandType() == requiredType && 
        tempHand.getHighestCardRank() > currentHand.getHighestCardRank()) {
        return tempHand;
    }
    return PlayingHand();
}

bool Player::isValidPlay(PlayingHand selectedHand, PlayingHand currentHand) {
    // If no hand is being played, validate the hand type and card count
    if (currentHand.getCards().empty()) {
        // Must be a valid hand type
        if (selectedHand.getHandType() <= 0) {
            return false;
        }
        
        // Validate card count based on hand type
        switch (selectedHand.getHandType()) {
            case 1: // High Card
                return selectedHand.getCards().size() == 1;
            case 2: // Pair
                return selectedHand.getCards().size() == 2;
            case 3: // Two Pair
                return selectedHand.getCards().size() == 4;
            case 4: // Three of a Kind
                return selectedHand.getCards().size() == 3;
            case 5: // Straight
            case 6: // Flush
            case 7: // Full House
            case 8: // Four of a Kind
            case 9: // Straight Flush
            case 10: // Royal Flush
                return selectedHand.getCards().size() == 5;
            default:
                return false;
        }
    }

    // Must match the hand type and card count
    if (selectedHand.getHandType() != currentHand.getHandType() ||
        selectedHand.getCards().size() != currentHand.getCards().size()) {
        return false;
    }

    // Must have a higher card rank
    return selectedHand.getHighestCardRank() > currentHand.getHighestCardRank();
}

PlayingHand Player::decision(PlayingHand currentHand)
{
    if (!currentHand.getCards().empty()) {
        cout << "=-=LAST PLAYED HAND=-=" << endl;
        currentHand.evaluateHand();  // Evaluate the hand first
        cout << "HAND: " << handType[currentHand.getHandType()] << endl;
        cout << "RANK: " << cardRankRef[currentHand.getHighestCardRank()] << endl;
        cout << "SUIT: " << cardSuitesRef[currentHand.getHighestHandSuit()] << endl;
        cout << "Cards: " << endl;
        currentHand.displayHand();
    }

    PlayingHand temp;
    if(isAi == true)
    {
        aiTurn(currentHand);
        return playerHand.discardHand();
    }
    else
    {
        playerTurn(currentHand);
        return playerHand.discardHand();
    }
}
  int Player::getAmountOfCards()
  {
    return playerDeck.size() + playerHand.size();
  }
#endif