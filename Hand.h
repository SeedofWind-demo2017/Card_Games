
#ifndef HAND_H
#define HAND_H

#include <vector>
#include "Card.h"
#include "Deck.h"
#include "Error.h"
#include <algorithm>
#include <vector>
#include <iostream>


#define VALID_HAND_SIZE 5

#define VALID_SEVEN_CARDSTUD_HAND_SIZE 7

enum Rank{
	lowest_rank,
	no_rank,
	one_pair,
	two_pairs,
	three_kind,
	straight,
	flush,
	full_house,
	four_kind,
	straight_flush
};

class Hand{
	friend std::ostream& operator<< (std::ostream &, const Hand &);   //passed card strings to ostream
	friend Hand& operator<<(Hand &, Deck &);     //deal card from deck into hand, keep in sorted order

private:
	std::vector<Card> cards;
	enum Rank handRank;
public:
	void sethand(std::vector<Card> cards);
	Hand(){};   //default constructor
	Hand(const Hand &);    //copy constructor
	virtual ~Hand();      //deconstructor
	void setHandRank();    //decide rank for hand
	Hand& operator=(const Hand &);   //assignment operator
	int size() const;       //size of a hand
	std::vector<Card> getCards() const;   //get cards in the hand
	int getRank() const;    //get rank of a hand
	std::string rankAsString() const;
	bool operator==(const Hand &) const;   //equivalence operator
	bool operator<(const Hand &) const;    //less than operator to compare with another hand
	std::string asString() const;      //card strings in the hand
	std::string visibleAsString() const;
	std::string noShareAsString() const;
	Card operator[](size_t);    //indexing operator
	void remove_card(size_t);   //remove card at given index
	void add_card(Card card); // add one card to current hand
};





#endif