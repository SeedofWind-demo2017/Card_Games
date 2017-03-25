#include "stdafx.h"
#include "Hand.h"


static const std::string suits[] = { "C", "D", "H", "S" };
static const std::string ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
static const std::string handRanks[] = { "no_rank", "one_pair", "two_pairs", "three_kind", "straight", "flush", "full_house", "four_kind", "straight_flush" };

//constructor
Hand::Hand(const Hand &anotherHand){
	cards = anotherHand.cards;
	handRank = anotherHand.handRank;
}

//assignment operator
Hand& Hand::operator=(const Hand &other){
	//check for self-assignment
	if (&other == this){
		return *this;
	}
	else{
		cards.clear();
		cards = other.cards;
		handRank = other.handRank;
		return *this;
	}
}

//deconstructor
Hand::~Hand(){

}

//determine rank of hand
void Hand::setHandRank(){
	if (cards.size() < VALID_HAND_SIZE){
		handRank = Rank::lowest_rank;
		return;
	}

	//variable to record rank results
	int straight = 0, flush = 0, four = 0, three = 0, fullhouse = 0;

	//index used for the comparison
	std::vector<Card>::size_type k = 0;
	int offset = 0;

	//check if cards in hand are consecutive in rank
	//if so, set straight to 1
	while (k<cards.size() - 1 && cards[k + 1].rank - cards[k].rank == 1){
		k++;
	}
	if (k == cards.size() - 1){
		straight = 1;
	}


	//check if cards in hand are of the same suit
	//if so, set flush to 1
	k = 0;
	while (k<cards.size() - 1 && cards[k].suit == cards[k + 1].suit){
		k++;
	}
	if (k == cards.size() - 1){
		flush = 1;
	}

	//determine case of straight flush
	if (straight && flush){
		handRank = Rank::straight_flush;
		return;
	}


	//check fours
	//since the cards are already sorted by rank, if four of a kind exists,
	//the only card with different rank must either be the first card or last card in hand
	k = 0;
	offset = 0;
	while (k < cards.size() - 3){
		while (offset<3 && cards[k + offset].rank == cards[k + offset + 1].rank){
			offset++;
		}
		if (offset == 3){
			four = 1;
		}
		offset = 0;
		k++;
	}

	//deterime case of four of a kind
	if (four){
		handRank = Rank::four_kind;
		return;
	}


	//check case of threes and full house
	k = 0;
	offset = 0;
	while (k < cards.size() - 2){
		while (offset<2 && cards[k + offset].rank == cards[k + offset + 1].rank){
			offset++;
		}
		if (offset == 2){
			//three of a kind
			three = 1;
			//continue to check if full house exists
			if (k == 0 && cards[k + 3].rank == cards[k + 4].rank){
				fullhouse = 1;
			}
			else if (k == 2 && cards[k - 2].rank == cards[k - 1].rank){
				fullhouse = 1;
			}
		}
		offset = 0;
		k++;
	}

	//deterime case of fullhouse, flush, straight and three of a kind
	if (fullhouse){
		handRank = Rank::full_house;
		return;
	}
	else if (flush){
		handRank = Rank::flush;
		return;
	}
	else if (straight){
		handRank = Rank::straight;
		return;
	}
	else if (three){
		handRank = Rank::three_kind;
		return;
	}

	//check case of two or one pairs
	k = 0;
	int pairs = 0;
	while (k < cards.size() - 1){
		if (cards[k].rank == cards[k + 1].rank){
			pairs++;
		}
		k++;
	}

	//determine case of one, two pairs or no rank
	if (pairs == 2){
		handRank = Rank::two_pairs;
		return;
	}
	else if (pairs == 1){
		handRank = Rank::one_pair;
		return;
	}
	else{
		handRank = Rank::no_rank;
		return;
	}
}


//size of a hand
int Hand::size() const{
	return (const int)cards.size();
}

//get rank of hand
int Hand::getRank() const{
	return handRank;
}


std::string Hand::rankAsString() const{
	std::string res = handRanks[handRank];
	return res;
}


//get cards in the hand
std::vector<Card> Hand::getCards() const{
	return cards;
}

//equivalence operator
bool Hand::operator==(const Hand &anotherHand) const{
	if (cards.size() != anotherHand.cards.size()){
		return false;
	}
	for (std::vector<Card>::size_type i = 0; i != cards.size(); i++){
		if (cards[i].rank != anotherHand.cards[i].rank || cards[i].suit != anotherHand.cards[i].suit){
			return false;
		}
	}
	return true;

}


//less than operator to compare with another hand
bool Hand::operator<(const Hand &anotherHand) const{

	std::cout << "in here" <<std:: endl;

	std::cout << cards.size() << std::endl;
	std::cout << anotherHand.size() << std::endl;
	for (std::vector<Card>::size_type i = 0; i != cards.size() || i != anotherHand.cards.size(); i++){
		if (cards[i].rank != anotherHand.cards[i].rank){
			return cards[i].rank < anotherHand.cards[i].rank;
		}
		else{
			if (cards[i].suit != anotherHand.cards[i].suit){
				return cards[i].suit < anotherHand.cards[i].suit;
			}
		}
	}
	return cards.size() <= anotherHand.cards.size();
}


//build card strings in the hand
std::string Hand::asString() const{
	std::string res = "";
	Card currCard;
	//traverse the vector to print each card
	for (std::vector<Card>::size_type i = 0; i != cards.size(); i++){
		currCard = cards[i];
		//build a string and print out
		std::string cardInfo = currCard.asString();
		res += cardInfo;
		res += " ";
	}
	return res;
}



std::string Hand::visibleAsString() const{
	std::string res = "";
	Card currCard;
	//traverse the vector to print each card
	for (std::vector<Card>::size_type i = 0; i != cards.size(); i++){
		currCard = cards[i];
		//build a string and print out
		std::string cardInfo = currCard.visibleAsString();
		res += cardInfo;
		res += " ";
	}
	return res;
}

std::string Hand::noShareAsString() const{
	std::string res = "";
	Card currCard;
	//traverse the vector to print each card
	for (std::vector<Card>::size_type i = 0; i != cards.size(); i++){
		currCard = cards[i];
		//build a string and print out
		std::string cardInfo = currCard.isShared ? "" : currCard.visibleAsString();
		res += cardInfo;
		res += " ";
	}
	return res;
}



//add card strings into ostream
std::ostream& operator<< (std::ostream &os, const Hand &hand){
	//empty card deck
	if (hand.cards.size() < 1){
		std::cout << "Empty card deck" << std::endl;
		return os;
	}

	Card currCard;

	//traverse the vector to print each card
	for (std::vector<Card>::size_type i = 0; i != hand.cards.size(); i++){
		currCard = hand.cards[i];
		//build a string and print out
		std::string cardInfo = ranks[currCard.rank] + suits[currCard.suit];
		os << cardInfo << " ";
	}
	os << std::endl;
	return os;
}


//insertion operator to draw card from deck into hand
Hand& operator<< (Hand &hand, Deck &deck){
	Card c = deck.remove();
	hand.cards.push_back(c);
	sort(hand.cards.begin(), hand.cards.end());
	return hand;
}



//indexing operator
Card Hand::operator[](size_t index){
	//index out of bound exception
	if (index >= cards.size() || index < 0){
		throw (int)Error::index_out_of_bound;
	}
	//return card at index
	else{
		return cards[index];
	}
}

void Hand::add_card(Card card){

	cards.push_back(card);
}

//remove card at given index
void Hand::remove_card(size_t index){
	if (index >= cards.size() || index < 0){
		throw (int)Error::index_out_of_bound;
	}
	else{
		cards.erase(cards.begin() + index);
	}
}

void Hand::sethand(std::vector<Card> cards){

(*this).cards = cards;
}