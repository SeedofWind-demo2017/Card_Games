// CardGame.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include "Game.h"
#include "Error.h"
#include"SevenCardStud.h"
#include "functions.h"
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

#define MIN_NUM_ARGS 4


//game control
int game_process(string gameName, vector<string> players){

	shared_ptr<Game>game_holder;
	try{
		//start the game with the given name
		Game::start_game(gameName);
		//obtain a pointer to the game
		shared_ptr<Game> ptr_newg = Game::instance();
		game_holder = ptr_newg;


		//add players to the game
		for (size_t i = 0; i < players.size(); i++){
			(*ptr_newg).add_player(players[i]);

		}

		//continue the game if at least two players
		while ((*ptr_newg).num_players_in_game() >= 2){
			int before_round_result = (*ptr_newg).before_round();
			(*ptr_newg).round();
			try{
				(*ptr_newg).after_round();
			}
			catch (Error e){

				//if not enough player, stop the game
				cout << "Error caught with error code: " << e << endl;
				cout << "Only one player in the game" << endl;
				cout << "Stopping current game" << endl;
				try{
					(*game_holder).stop_game();
				}
				catch (Error k){
					return k;
				}
				break;

			}
		}


		//ask if user wants to quit the program
		cout << "----------------------------" << endl;
		cout << "Do you want to quit the program? " << endl;
		cout << "Enter q to quit the program or c to continue the program" << endl;
		string response;
		getline(cin, response);

		//keep prompting if invalid input
		while (response.compare("q") != 0 && response.compare("c") != 0){
			cout << "Do you want to quit the program? " << endl;
			cout << "Enter q to quit the program or c to continue the program" << endl;
			getline(cin, response);
		}

		//quit the program
		if (response.compare("q") == 0){
			return Error::Success;
		}


		//ask user if want to start a new game
		cout << "----------------------------" << endl;
		cout << "Do you want to start another game?" << endl;
		cout << "Enter y to start another game and n to quit program" << endl;
		getline(cin, response);

		//keep prompting if invalid input
		while (response.compare("y") != 0 && response.compare("n") != 0){
			cout << "Do you want to start another game?" << endl;
			cout << "Enter y to start another game and n to continue current game" << endl;
			getline(cin, response);
		}

		//start a new game
		if (response.compare("y") == 0){
			try{
				cout << "Please type the command to start a new game" << endl;
				cout << "Here are your options: " << endl;
				cout << "Usage: CardGame <Game name> <player1's name> <player2's name> ... " << endl;
				string command;
				getline(cin, command);

				//parse user input
				istringstream iss(command);
				vector<string> tokens;
				copy(istream_iterator<string>(iss),
					istream_iterator<string>(),
					back_inserter(tokens));

				//too few arguments
				if (tokens.size() < MIN_NUM_ARGS){
					cout << "Too few arguments" << endl;
					cout << "Usage: CardGame <Game name> <player1's name> <player2's name> ... " << endl;
					return Error::two_few_arguments;
				}
				//start a new game
				else{
					string game_name = tokens[1];
					vector<string> game_players;
					for (size_t i = 2; i < tokens.size(); ++i){
						game_players.push_back(tokens[i]);
					}
					return game_process(game_name, game_players);

				}

			}
			catch (Error k){
				return k;
			}
		}
		else{
			return Error::Success;
		}

	}
	//catch exception and stop the game
	catch (Error i){
		cout << "Exception caught, stop the game" << endl;
		try{
			(*game_holder).stop_game();
		}
		catch (Error j){
			return j;
		}
		return i;
	}

	//stop the game in the end
	try{
		(*game_holder).stop_game();
		return Error::Success;
	}
	catch (Error k){
		return k;
	}
}


int main(int argc, char* argv[]){
	//number of arguments not enough
	if (argc < MIN_NUM_ARGS){
		cout << "Too few arguments" << endl;
		cout << "Usage: CardGame <Game name> <player1's name> <player2's name> ... " << endl;
		return Error::two_few_arguments;
	}

	//parse user command
	string gameName(argv[1]);
	vector<string> players;
	for (int i = 2; i < argc; i++){
		string s(argv[i]);
		players.push_back(s);
	}

	return game_process(gameName, players);
	
}







