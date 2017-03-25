#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>
#include <algorithm>

#include "Deck.h"
#include "Error.h"
#include "Player.h"

class Game{
protected:
	static std::shared_ptr<Game> staPtr_to_game;    //shared_ptr to Game, initialized to singular
	Deck main_deck = Deck();                        //main deck of the game
	std::vector <std::shared_ptr<Player>> players;  //shared_ptrs to players in the game
public:
	static std::shared_ptr<Game>instance();        //return a copy of pointer member variable
	unsigned int num_players_in_game();       //number of current players in the game

	static void start_game(const std::string &);     //start the game by allocating FiveCardDraw instance
	static void stop_game();                         //stop the game, reset pointer member variable to singular
	void add_player(const std::string &);             //add a player into the game with given player's name
	std::shared_ptr<Player> find_player(const std::string &);      //return pointer to the matching player

	//declarations of pure virtual methods
	virtual int before_turn(Player &) = 0;
	virtual int turn(Player &) = 0;
	virtual int after_turn(Player &) = 0;
	virtual int before_round() = 0;
	virtual int round() = 0;
	virtual int after_round() = 0;
	

};



#endif