#include "stdafx.h"

#include "Deck.h"


//card information
static const std::string suits[] = { "C", "D", "H", "S" };
static const std::string ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };

//default constructor (compiler supplied)
Deck::Deck(){

}

//constructor
//if load fails, throw exception
Deck::Deck(char *filename){
	try{
		load(filename);
	}
	catch (Error e){
		throw e;
	}
}

//deconstructor
Deck::~Deck(){

}


//determine if string represents a number
bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	//traverse the string until a character is not a digit
	while (it != s.end() && isdigit(*it)) {
		++it;
	}
	return !s.empty() && it == s.end();
}

//load cards from given file
void Deck::load(char *filename){
	//card definition string
	std::string cardDef;
	//suit and rank information of current card
	char cardSuit;
	std::string cardRank;
	//current card
	Card currCard;
	std::ifstream ifs;
	ifs.open(filename);
	//successfully open file
	if (ifs.is_open()){
		while (ifs >> cardDef){
			//card definition string should be length of 2 or 3
			if (cardDef.size() == 2 || cardDef.size() == 3){
				//retrieve suit and rank of current card
				cardSuit = cardDef[cardDef.size() - 1];
				cardRank = cardDef.substr(0, cardDef.size() - 1);

				//assign suit to card if valid
				switch (cardSuit){
				case 'c':
				case 'C':
					currCard.suit = Card::clubs;
					break;
				case 'd':
				case 'D':
					currCard.suit = Card::diamonds;
					break;
				case 'h':
				case 'H':
					currCard.suit = Card::hearts;
					break;
				case 's':
				case 'S':
					currCard.suit = Card::spades;
					break;
					//invalid suit information, process next string
				default:
					continue;
				}

				//assign rank to card if valid
				//case when rank info is numerical value
				if (is_number(cardRank)){
					//conver to numerical value
					std::istringstream argsin(cardRank);
					int idx;
					argsin >> idx;
					//consider 02c, 03S, 09d as invalid string
					if (idx >= 2 && idx <= 9 && cardRank.size() == 2){
						continue;
					}
					//check range
					if (idx >= 2 && idx <= 10){
						currCard.rank = Card::Rank(idx - 2);
					}
					//invalid rank information, process next string
					else{
						continue;
					}
				}
				//case when rank info is not numerical value
				else{
					//jack
					if (cardRank.compare("J") == 0 || cardRank.compare("j") == 0){
						currCard.rank = Card::jack;
					}
					//queen
					else if (cardRank.compare("Q") == 0 || cardRank.compare("q") == 0){
						currCard.rank = Card::queen;
					}
					//king
					else if (cardRank.compare("K") == 0 || cardRank.compare("k") == 0){
						currCard.rank = Card::king;
					}
					//ace
					else if (cardRank.compare("A") == 0 || cardRank.compare("a") == 0){
						currCard.rank = Card::ace;
					}
					//invalid rank information, process next string
					else{
						continue;
					}
				}
				//add card into vector if valid
				cards.push_back(currCard);
			}
			//invalid length for card definition string, process next string
			else{
				continue;
			}

		}
		ifs.close();
	}
	//return nonzero value if fail to open file
	else{
		std::cout << "ERROR: cannot open the given file" << std::endl;
		throw (int)Error::load_fail;
	}
}

//shuffle deck
void Deck::shuffle(){
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(cards.begin(), cards.end(), g);
}

//size of deck
int Deck::size() const{
	return (const int)cards.size();
}

//get cards in the deck
std::vector<Card> Deck::getCards() const{
	return cards;
}


//add card strings to ostream
std::ostream& operator<< (std::ostream &os, const Deck &deck){
	//empty card deck
	if (deck.cards.size() < 1){
		std::cout << "Empty card deck" << std::endl;
		return os;
	}

	Card currCard;

	//traverse the vector to print each card
	for (std::vector<Card>::size_type i = 0; i != deck.cards.size(); i++){
		currCard = deck.cards[i];
		//build a string and print out
		std::string cardInfo = ranks[currCard.rank] + " of " + suits[currCard.suit];
		os << cardInfo << std::endl;
	}
	return os;
}

//add a card to the deck
void Deck::add_card(const Card &card){
	cards.push_back(card);

}



//remove from back of deck
Card Deck::remove(){
	Card c = cards.back();
	cards.pop_back();
	return c;
}

