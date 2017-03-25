#include "stdafx.h"
#include "Game.h"
#include "FiveCardDraw.h"
#include "SevenCardStud.h"
#include "TexasHoldEm.h"

//intialize pointer member variable to singluar
std::shared_ptr<Game> Game::staPtr_to_game = 0;

//return a copy of pointer member variable
std::shared_ptr<Game> Game::instance(){
	//pointer member variable is singular
	if (staPtr_to_game == NULL){
		throw Error::instance_not_available;
	}
	else{
		return std::shared_ptr<Game>(staPtr_to_game);
	}
}

//start the game by allocating instance of FiveCardDraw
void Game::start_game(const std::string &str){
	//game already started
	if (Game::staPtr_to_game != NULL){
		throw Error::game_already_started;
	}
	//check if game name contains "FiveCardDraw"
	if (str.find(std::string("FiveCardDraw")) == std::string::npos && str.find(std::string("SevenCardStud")) == std::string::npos && str.find(std::string("TexasHoldEm")) == std::string::npos){
		cout << "unknown game" << endl;
		throw Error::unknown_game;
	}

	//set pointer member variable to allocated FiveCardDraw, SevenCardStud or TexasHoldEm instance
	if (!(str.find(std::string("FiveCardDraw")) == std::string::npos)){
		Game::staPtr_to_game = std::make_shared<FiveCardDraw>(FiveCardDraw());
	}
	else if (!(str.find(std::string("SevenCardStud")) == std::string::npos)){
		Game::staPtr_to_game = std::make_shared<SevenCardStud>(SevenCardStud());
	}
	else{
		Game::staPtr_to_game = std::make_shared<TexasHoldEm>(TexasHoldEm());
	}
}

//stop the game
void Game::stop_game(){
	//no game in progress
	if (Game::staPtr_to_game == NULL){
		throw Error::no_game_in_progress;
	}

	//reset pointer member variable to singular
	Game::staPtr_to_game = NULL;
}


//add a player to the game
void Game::add_player(const std::string &player_name){
	//check if the given player already exists
	for (std::vector<std::shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it){
		Player current_player = **it;

		//player already exists
		if ((current_player.get_player_name()).compare(player_name) == 0){
			throw Error::already_playing;
		}
	}


	//create a new player and put it into the game
	try{
		Player p = Player(player_name.c_str());
		std::shared_ptr<Player> new_player = std::make_shared<Player>(p);
		players.push_back(new_player);
	}
	catch (Error e){
		cout << "Error caught with error code: " << e << endl;
		return;

	}

}


//return the pointer to the matching player
std::shared_ptr<Player> Game::find_player(const std::string &player_name){
	for (std::vector<std::shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it){
		Player current_player = **it;
		//matching player found
		if ((current_player.get_player_name()).compare(player_name) == 0){
			return *it;
		}
	}

	//return singluar pointer if no matching player
	return 0;
}


//return number of current players in the game
unsigned int Game::num_players_in_game(){
	return players.size();
}