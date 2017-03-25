
#ifndef FiveCardDraw_H
#define FiveCardDraw_H

#include <algorithm>
#include "Game.h"




#define CHIP_UNIT 1


using namespace std;

//public inheritance from Game base class
class FiveCardDraw : public Game{

protected:
	unsigned int most_recent_bet_position;
	unsigned int chips_pot;     //common pot of chips that winner wins
	unsigned int current_bet_value = 0;
	unsigned int current_position = 0;
	bool all_player_check = false;
	unsigned int num_player_fold = 0;

	size_t dealer_position;    //current dealer position, rotating during the game
	Deck discard_deck = Deck();         //deck to store discarded cards
public:
	FiveCardDraw();            //default constructor

	//player's turn
	virtual int before_turn(Player &player);
	virtual int turn(Player &player);
	virtual int after_turn(Player &player);

	//game's round
	virtual int before_round();
	virtual int round();
	virtual int after_round();

	//betting phase
	int participate_betting();
	int before_betting(size_t position);
	int betting();
	int after_betting(vector<std::string> winners);

	//betting for each player
	int player_bet(std::shared_ptr<Player> &player);

	//utility function for betting
	bool is_betting_end();
	void reset_betting_status(size_t);
	void reset_all_status();
	int reset_afte_firstBetting();
	void print_betting_stats();


	//helper methods for after_round()
protected:
	vector<std::string> print_sorted_player();
	void reset();


	//Helper Function for extra credit
	vector<int> auto_valid_positions(Player &player);
	void highest_automated_leave(string automated_name);
	void lowest_automated_leave(string automated_name);
	void other_automated_leave(string automated_name);
	void terminate();


};







#endif