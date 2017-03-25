#include "stdafx.h"
#include "functions.h"


using namespace std;
//different hand ranks
static const std::string handRanks[] = { "no_rank", "one_pair", "two_pairs", "three_kind", "straight", "flush", "full_house", "four_kind", "straight_flush" };

//parse response and return a vector of valid positions for discarding player's cards
//NOTE: vector[0] used to store number of invalid positions that user entered
bool is_anumber(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	//traverse the string until a character is not a digit
	while (it != s.end() && isdigit(*it)) {
		++it;
	}
	return !s.empty() && it == s.end();
}


vector<int> valid_input_positions(string response, unsigned int hand_size){

	istringstream ss(response);
	vector<int> user_inputs;
	user_inputs.push_back(0);
	string temp;

	while (ss >> temp){
		//convert string to numerical value
		if (!is_anumber(temp)){ cout << "Detecting non-numeric position value from user input" << endl; ++user_inputs[0]; continue; }
		size_t check = stoi(temp);

		//valid positions
		if (check >= 0 && check < hand_size){
			user_inputs.push_back(check);
		}
		else{
			cout << "detecting invliad position from user" << ": " << check << endl;
			//increment the counter for invalid positions
			++user_inputs[0];
		}
	}



	return user_inputs;
}



//get information if hand rank is two pairs
std::vector<int> getTwoPairsInfo(const std::vector<Card> &cardsInHand){
	//twoPairsInfo[0] = rank of highest pair
	//twoPairsInfo[1] = rank of second highest pair
	//twoPairsInfo[2] = rank of fifth card
	std::vector<int> twoPairsInfo;
	if (cardsInHand[3].rank == cardsInHand[4].rank){
		twoPairsInfo.push_back(cardsInHand[4].rank);
		if (cardsInHand[1].rank == cardsInHand[2].rank){
			twoPairsInfo.push_back(cardsInHand[2].rank);
			twoPairsInfo.push_back(cardsInHand[0].rank);
		}
		else{
			twoPairsInfo.push_back(cardsInHand[0].rank);
			twoPairsInfo.push_back(cardsInHand[2].rank);
		}
	}
	else{
		twoPairsInfo.push_back(cardsInHand[2].rank);
		twoPairsInfo.push_back(cardsInHand[0].rank);
		twoPairsInfo.push_back(cardsInHand[4].rank);
	}
	return twoPairsInfo;
}

//get information if hand rank is one pair
std::vector<int> getOnePairInfo(const std::vector<Card> &cardsInHand){
	//onePairInfo[0] = rank of the pair
	//onePairInfo[1] = highest rank of unpaired card
	//onePairInfo[2] = next highest rank of unpaired card
	//onePairInfo[3] = next highest rank of unpaired card
	std::vector<int> onePairInfo;
	if (cardsInHand[0].rank == cardsInHand[1].rank){
		onePairInfo.push_back(cardsInHand[0].rank);
		onePairInfo.push_back(cardsInHand[4].rank);
		onePairInfo.push_back(cardsInHand[3].rank);
		onePairInfo.push_back(cardsInHand[2].rank);
	}
	else if (cardsInHand[1].rank == cardsInHand[2].rank){
		onePairInfo.push_back(cardsInHand[1].rank);
		onePairInfo.push_back(cardsInHand[4].rank);
		onePairInfo.push_back(cardsInHand[3].rank);
		onePairInfo.push_back(cardsInHand[0].rank);
	}
	else if (cardsInHand[2].rank == cardsInHand[3].rank){
		onePairInfo.push_back(cardsInHand[2].rank);
		onePairInfo.push_back(cardsInHand[4].rank);
		onePairInfo.push_back(cardsInHand[1].rank);
		onePairInfo.push_back(cardsInHand[0].rank);
	}
	else{
		onePairInfo.push_back(cardsInHand[4].rank);
		onePairInfo.push_back(cardsInHand[2].rank);
		onePairInfo.push_back(cardsInHand[1].rank);
		onePairInfo.push_back(cardsInHand[0].rank);
	}
	return onePairInfo;
}

//compare two hands
bool poker_rank(const Hand &hand2, const Hand &hand1){

	if (hand1.getRank() == hand2.getRank() && hand2.getRank() == Rank::lowest_rank){
		return false;
	}


	//use hand rank to decide if two hand ranks are different
	if (hand1.getRank() != hand2.getRank()){
		return hand1.getRank() < hand2.getRank();
	}
	else{
		std::vector<Card> cardsInHand = hand1.getCards();
		std::vector<Card> cardsInHand2 = hand2.getCards();
		//get vectors that contain information in the case of two pairs and one pair
		std::vector<int> info1 = getTwoPairsInfo(cardsInHand);
		std::vector<int> info2 = getTwoPairsInfo(cardsInHand2);
		std::vector<int> info3 = getOnePairInfo(cardsInHand);
		std::vector<int> info4 = getOnePairInfo(cardsInHand2);

		switch (hand1.getRank()){
		case Rank::straight_flush:
			//sorted by highest card in each hand
			return cardsInHand[4].operator<(cardsInHand2[4]);

		case Rank::four_kind:
			//sorted by rank of 4 cards that have the same rank
			return cardsInHand[2].rank < cardsInHand2[2].rank;

		case Rank::full_house:
			//sorted by rank of 3 cards that have the same rank
			//then by rank of 2 cards that have the same rank
			if (cardsInHand[2].rank == cardsInHand2[2].rank){
				return 1;
			}
			else{
				return cardsInHand[2].rank < cardsInHand2[2].rank;
			}

		case Rank::flush:
			//sorted by highest ranking card, then by next highest ranking card, etc
			for (std::vector<Card>::size_type i = cardsInHand.size() - 1; i != 0; i--){
				if (cardsInHand[i].rank != cardsInHand2[i].rank){
					return cardsInHand[i].rank < cardsInHand2[i].rank;
				}
			}
			return cardsInHand[0].rank < cardsInHand2[0].rank;

		case Rank::straight:
			//sorted by highest card in each hand
			return cardsInHand[4].operator<(cardsInHand2[4]);

		case Rank::three_kind:
			//sorted by the rank of the 3 cards that have the same rank
			return cardsInHand[2].rank < cardsInHand2[2].rank;

		case Rank::two_pairs:
			//sorted by the rank of higher pair, then by lower pair, then  by 5th card
			if (info1[0] != info2[0]){
				return info1[0] < info2[0];
			}
			else{
				if (info1[1] != info2[1]){
					return info1[1] < info2[1];
				}
				else{
					return info1[2] < info2[2];
				}
			}


		case Rank::one_pair:
			//sorted by the rank of the pair, then by the highest unpaired card
			//then next highest card, etc
			if (info3[0] != info4[0]){
				return info3[0] < info4[0];
			}
			else{
				if (info3[1] != info4[1]){
					return info3[1] < info4[1];
				}
				else{
					if (info3[2] != info4[2]){
						return info3[2] < info4[2];
					}
					else{
						return info3[3] < info4[3];
					}
				}
			}

		case Rank::no_rank:
			//sorted by highest ranking card, then by next highest ranking card
			for (std::vector<Card>::size_type i = cardsInHand.size() - 1; i != 0; i--){
				if (cardsInHand[i].rank != cardsInHand2[i].rank){
					return cardsInHand[i].rank < cardsInHand2[i].rank;
				}
			}
			return cardsInHand[0].rank < cardsInHand2[0].rank;

		default:
			return false;

		}
	}
}

bool player_rank(const std::shared_ptr<Player> & p1, const std::shared_ptr<Player> &p2){
	//return false if first pointer is singluar
	if (p1 == 0){
		return false;
	}

	//return true if first pointer is non-singluar but second pointer is singular
	if (p1 != 0 && p2 == 0){
		return true;
	}

	//otherwise, return normal comparison result based on their hands
	return poker_rank((*p1).getPlayerHand(), (*p2).getPlayerHand());
}