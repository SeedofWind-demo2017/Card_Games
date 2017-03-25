#ifndef TEXASHOLDEM_H
#define TEXASHOLDEM_H

#include <algorithm>
#include <set>
#include "Game.h"



#define CHIP_UNIT 1


using namespace std;

//public inheritance from Game base class
class TexasHoldEm : public Game{

protected:
	unsigned int most_recent_bet_position;   //most recent bet position
	unsigned int chips_pot;     //common pot of chips that winner wins
	unsigned int current_bet_value;       //current bet value in the game
	unsigned int current_position;
	bool all_player_check = false; // check and chip out
	unsigned int num_player_fold = 0;

	size_t dealer_position;    //current dealer position, rotating during the game
	Deck discard_deck = Deck();         //deck to store discarded cards

public:

	set<Card> sharedCards;
	TexasHoldEm();            //default constructor

	//unused base
	virtual int before_turn(Player &){ return Success; };
	virtual int turn(Player &){ return Success; };
	virtual int after_turn(Player &){ return Success; };

	//player's turn
	int first_turn(Player &);
	int middle_turn(Player &);
	int final_turns(Player &);

	//game's round
	int firstphase();
	int middle_phase();
	int final_phase();
	virtual int before_round();
	virtual int round();
	virtual int after_round();


	//betting phase
	int participate_betting();
	int before_betting(size_t position);
	int betting();
	int after_betting(vector<string> winners);

	//betting for each player
	int player_bet(std::shared_ptr<Player> &player);

	//utility function for betting
	bool is_betting_end();
	void reset_betting_status(size_t);
	void reset_all_status();
	int reset_afte_firstBetting();
	void print_betting_stats();


	//helper methods for after_round()
	Hand best_five(Player & player);
	int best_five_forall();
protected:
	vector<string> print_sorted_player();
	void reset();

	//Helper Function for extra credit
	vector<int> auto_valid_positions(Player &player);
	void highest_automated_leave(string automated_name);
	void lowest_automated_leave(string automated_name);
	void other_automated_leave(string automated_name);
	void terminate();


	//utility
	void print_players_cards(shared_ptr<Player> player);
	void print_original_vs_bestfive();
	void print_shared_cards();


};







#endif