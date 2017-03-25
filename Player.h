#ifndef PLAYER_H

#define PLAYER_H

#include <string>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "Hand.h"
#include "Error.h"
#include "PlayerState.h"

//number of input file arguments: name, hands_won, hands_lost, num_chips
#define NUM_ARGS_INPUT_FILE 4

//default number of chips
#define DEFAULT_NUM_CHIPS 20

struct Player{
	friend class FiveCardDraw;
	friend class SevenCardStud;
	
	//extra credit
	friend class TexasHoldEm;

private:
	std::string player_name;    //player's name
	Hand player_hand;           //a hand for player's cards
	unsigned int hands_won;     //number of winning
	unsigned int hands_lost;    //number of losing
	unsigned int num_chips;     //number of chips 
	player_status status;       //player status
	unsigned int bet_so_far;      //current bet of chips

	
public:
	Hand player_original_hand;     //original seven cards in hand
	Player(const char *);      //constructor
	std::string get_player_name() const;   //get player's name
	Hand getPlayerHand() const;    //get player's hand
	unsigned int get_num_chips();          //get num_chips
	
	
	void decrease_chips(unsigned int);     //decrease num_chips by the given amount
	void increase_chips(unsigned int);     //increase num_chips by the given amount
	void reset_chips();           //reset num_chips = 20


	void set_bet_so_far(unsigned int);      //update current chip bet
	unsigned int get_bet_so_far();          //get current chip bet
	
	void add_card(Card card);              //add cards to player's hand

	friend std::ostream& operator<< (std::ostream &, const Player &);    //non-member insertion operator

};


#endif