
#ifndef CARD_H

#define CARD_H
#include <string>


struct Card{
	//enumeration for different suits
	enum Suit{
		clubs,
		diamonds,
		hearts,
		spades
	};

	//enumeration for different ranks
	enum Rank{
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		ten,
		jack,
		queen,
		king,
		ace
	};

	//define less than operator
	bool operator< (const Card &) const;
	bool operator== (const Card &) const;
	std::string asString() const;
	std::string visibleAsString() const;

	//member variables
	enum Suit suit;
	enum Rank rank;
	bool faceup = true;     //indicate if card is faceup
	bool isShared = false;    //indicate if card is shared by all players

};

#endif
