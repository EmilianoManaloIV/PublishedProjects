#ifndef DECK_H
#define DECK_H
#include "Card.h"
#include <list>
#include <iostream>
#include <deque>
#include <map>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;

class Deck
{
private:
    deque<Card> Cards;              //Used to store all the card objects
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

    // Helper function to get a time-based seed
    unsigned int getTimeBasedSeed() {
        auto now = chrono::high_resolution_clock::now();
        auto time = now.time_since_epoch();
        auto count = chrono::duration_cast<chrono::nanoseconds>(time).count();
        return static_cast<unsigned int>(count);
    }

public:
    Deck() = default;
    Deck(int);                      //Creates a deck with 0 cards in it
    Deck(Card);                     //Creates a deck with one card in it
    Deck(list<Card>);               //Creates a deck out of a given amount of cards
    ~Deck() = default;              //Destroys the deck
    
    //---SPECIAL FUNCTIONS---
    void shuffleDeck();
    void sortDeck();
    void displayDeck();

    Card takeTopFromDeck();
    Card takeBottomFromDeck();
    Card takeRandomFromDeck();
    Card takeCardFromDeck(int);
    list<Card> selectCardsAndTakeFromDeck(list<int>);
    list<Card> getCards() const;
    int amountOfCards();
    void placeCardIntoDeck(Card);
    void placeCardsIntoDeck(list<Card>);
    void removeCard(const Card& card);
    int size() const;
};

//Creates a 52 deck without jokers at instantiation
Deck::Deck(int i)
{
    if(i == 52) {
        // Create all cards in order
        for(int i=1; i <= 4; i++) {
            for(int j=1; j <= 13; j++) {
                Cards.push_back(Card(j,i));
            }
        }
        // Shuffle immediately using time-based seed
        shuffleDeck();
    }
}

void Deck::shuffleDeck()
{
    // Get a time-based seed
    unsigned int seed = getTimeBasedSeed();
    
    // Combine with random device for additional entropy
    random_device rd;
    seed ^= rd();
    
    // Use the combined seed to initialize the Mersenne Twister
    mt19937 g(seed);
    
    // Shuffle the deck using STL's shuffle
    shuffle(Cards.begin(), Cards.end(), g);
}

Card Deck::takeRandomFromDeck()
{
    if (Cards.empty()) {
        throw runtime_error("Deck is empty");
    }
    
    // Get a time-based seed
    unsigned int seed = getTimeBasedSeed();
    
    // Combine with random device for additional entropy
    random_device rd;
    seed ^= rd();
    
    // Use the combined seed to initialize the Mersenne Twister
    mt19937 g(seed);
    
    // Get a random index
    uniform_int_distribution<size_t> dist(0, Cards.size() - 1);
    size_t randomIndex = dist(g);
    
    // Get the card and remove it
    auto it = Cards.begin();
    advance(it, randomIndex);
    Card C = *it;
    Cards.erase(it);
    return C;
}

Card Deck::takeCardFromDeck(int I)
{
    if (Cards.empty()) {
        throw runtime_error("Deck is empty");
    }
    if (I < 0 || I >= Cards.size()) {
        throw runtime_error("Invalid card index: " + to_string(I) + " (deck size: " + to_string(Cards.size()) + ")");
    }
    
    // Get the card at the specified index
    auto it = Cards.begin();
    advance(it, I);
    Card C = *it;
    
    // Remove the card from the deck
    Cards.erase(it);
    
    return C;
}

//Sort the deck to be neat again
void Deck::sortDeck()
{
    sort(Cards.begin(), Cards.end(),
    [](const Card& a, const Card& b) {
        if (a.getSuit() != b.getSuit()) {
            return a.getSuit() < b.getSuit();  // Sort by suit first: 1=&, 2=^, etc.
        }
        return a.getCard() < b.getCard();      // Then by rank within suit: 1=3 ... 13=2
    });
}
//Uses maps to display the character & string oriented with numbered index
void Deck::displayDeck()
{
    for(const auto& card : Cards)
    {
        // Use find() instead of at() to safely check if the key exists
        auto rankIt = cardRankRef.find(card.getCard());
        auto suitIt = cardSuitesRef.find(card.getSuit());
        
        if (rankIt != cardRankRef.end() && suitIt != cardSuitesRef.end()) {
            cout << rankIt->second << " of " << suitIt->second << endl;
        } else {
            cout << "Invalid card: Rank=" << card.getCard() << ", Suit=" << card.getSuit() << endl;
        }
    }
}
//Takes a card from the top of the deck
Card Deck::takeTopFromDeck()
{
    if (Cards.empty()) {
        throw runtime_error("Deck is empty");
    }
    Card C = Cards.front();
    Cards.pop_front();
    return C;
}
//Takes a card from the bottom of the deck
Card Deck::takeBottomFromDeck()
{
    if (Cards.empty()) {
        throw runtime_error("Deck is empty");
    }
    Card C = Cards.back();
    Cards.pop_back();
    return C;
}
//Returns the amount of cards this deck has
int Deck::amountOfCards()
{
    return Cards.size();
}
//Places a card into the deck
void Deck::placeCardIntoDeck(Card C)
{
    Cards.push_back(C);
}
//Places a list of cards into the deck
void Deck::placeCardsIntoDeck(list<Card> Cs)
{
    Cards.insert(Cards.end(),Cs.begin(),Cs.end());
}
//Selects cards then take them out of the deck
list<Card> Deck::selectCardsAndTakeFromDeck(list<int> selection)
{
    std::list<Card> temp;

    // Convert selection of indices to actual Card objects
    auto it = Cards.begin();
    for (const auto& i : selection)
    {
        std::advance(it, i);  // Move iterator to the desired index
        temp.push_back(*it);  // Add the selected card to temp
        it = Cards.begin();  // Reset iterator to beginning for next selection
    }

    // Now remove selected cards from the original deck
    for (const auto& card : temp)
    {
        auto removeIt = std::find(Cards.begin(), Cards.end(), card);
        if (removeIt != Cards.end()) {
            Cards.erase(removeIt);  // Remove the card from the deck
        }
    }

    return temp;  // Return the list of selected cards
}
list<Card> Deck::getCards() const
{
    return list<Card>(Cards.begin(), Cards.end());
}
// Removes a specific card from the deck
void Deck::removeCard(const Card& card) {
    auto it = std::find(Cards.begin(), Cards.end(), card);
    if (it != Cards.end()) {
        Cards.erase(it);
    }
}
   int Deck::size() const
   {
        return Cards.size();
   }
#endif 