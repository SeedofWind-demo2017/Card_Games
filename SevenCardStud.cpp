
#include "stdafx.h"
#include "SevenCardStud.h"
#include "functions.h"
#include "Game.h"
#include "Deck.h"
#include <Math.h>

using namespace std;
SevenCardStud::SevenCardStud(){

	//initialize to zero
	current_bet_value = 0;
	dealer_position = 0;
	num_player_fold = 0;
	all_player_check = false;
	current_position = 0;
	//intialize common pot of chips
	chips_pot = 0;

	most_recent_bet_position = -1;

	//generate 52 combinations of cards and add them to main deck
	Card curr_card;
	for (int suitIdx = Card::Suit::clubs; suitIdx != Card::Suit::spades; ++suitIdx){
		for (int rankIdx = Card::Rank::two; rankIdx != Card::Rank::ace; ++rankIdx){
			curr_card.rank = static_cast<Card::Rank>(rankIdx);
			curr_card.suit = static_cast<Card::Suit>(suitIdx);
			main_deck.add_card(curr_card);
		}
	}

}


//first turn
int SevenCardStud::first_turn(Player & player){
	Card temp;

	//draw two faceup cards
	for (unsigned int i  = 0; i < 2; i++){
		temp = main_deck.remove();
		player.add_card(temp);
	}

	//draw one facedown card
	temp = main_deck.remove();
	temp.faceup = false;
	player.add_card(temp);

	return Success;
}


//second, third, fourth turn
int SevenCardStud:: middle_turns(Player & player){
	//draw one faceup card
	Card temp = main_deck.remove();
	player.add_card(temp);
	return Success;
}


//fifth turn
int SevenCardStud::final_turn(Player & player){

	//draw one facedown card
	Card temp = main_deck.remove();
	temp.faceup = false;
	player.add_card(temp);
	return Success;

}

//betting phase
int SevenCardStud:: participate_betting(){

	//one chip deducted from each player and added to chips_pot
	for (size_t count = 0; count < num_players_in_game(); count++){
		chips_pot++;
		(*players[count]).decrease_chips(CHIP_UNIT);
		if ((*players[count]).get_num_chips() == 0){
			(*players[count]).status = chip_run_out;
		}
	}

	return Success;
}



//before betting phase
int SevenCardStud::before_betting(size_t position){
	
	string response;
	//betting starts with the player at the (dealer position + 1)
	for (size_t i = 1; i <= num_players_in_game(); ++i){

		cout << "-------------------Players's cards ---------------------------" << endl;
		print_players_cards(players[(position + i) % num_players_in_game()]);
		cout << "--------------------------------------------------------------" << endl;

		//ignore players that are fold or run out of chips
		if ((*players[(position + i) % num_players_in_game()]).status == out_of_round || (*players[(position + i) % num_players_in_game()]).status == chip_run_out){
			cout << (*players[(position + i) % num_players_in_game()]).player_name << ": folded or chip run out so Ignored " << endl;
			continue;
		}


		cout << (*players[(position + i) % num_players_in_game()]).player_name << ": do you want to check or bet ? " << endl;
		getline(cin, response);

		//keep prompting if invalid input
		if (response.compare("bet") != 0 && response.compare("check") != 0){
			before_betting((position + i - 1) % num_players_in_game());
		    return 0;
		}

		//set status to check
		if (response.compare("check") == 0){
			(*players[(position + i) % num_players_in_game()]).status = check;
		}

		//player bets
		if (response.compare("bet") == 0){
			cout << "Do you want to bet 1 or 2 ? " << endl;
			getline(cin, response);

			//can only bet 1 or 2
			while ((!is_anumber(response)) || !(stoi(response) == 1 || stoi(response) == 2)){
				cout << "Sorry, you can only bet 1 or 2" << endl;
				cout << "Do you want to bet 1 or 2 ?" << endl;
				getline(cin, response);
			}

			//update current bet value
			//determine position of most recent bet
			current_bet_value = stoi(response);
			most_recent_bet_position = (position + i) % num_players_in_game();


			//update player's bet so far
			(*players[(position + i) % num_players_in_game()]).set_bet_so_far(current_bet_value);


			//print info
			cout << "---------- Someone bets ---------------" << endl;
			cout << "Chips in pot: " << chips_pot << endl;
			cout << "Current bet value " << current_bet_value << endl;
		

			(*players[(position + i) % num_players_in_game()]).status = raise;
			return 0;
		}
		
	}

	return 0;
}


//betting
int SevenCardStud::betting(){
	//check if all players check
	all_player_check = true;
	for (size_t i = 0; i < num_players_in_game(); ++i){
		if ((*players[i]).status != chip_run_out && (*players[i]).status != check) {
			all_player_check = false;
			break;
		}
	}


	//no need to continue if all players check
	if (all_player_check){
		return 0;
	}

	//one player bets
	else{
		current_position = (most_recent_bet_position + 1) % num_players_in_game();
		string response;

		//every player decides to fold, call or raise
		while (!is_betting_end()){

			//player will not participate the betting phase
			//if he runs out of chips or he is already out of round
			if ((*players[current_position]).status != out_of_round && (*players[current_position]).status != chip_run_out){
			
				player_bet(players[current_position]);
			}
			current_position++;
			current_position %= num_players_in_game();
		}


	}
	return Success;
}



//after betting
int SevenCardStud::after_betting(vector<string> winners){
	
	//put player's bet into pot
	for (size_t i = 0; i < num_players_in_game(); ++i){
		if ((*players[i]).status != out_of_round){
			unsigned int bet = (*players[i]).get_bet_so_far();
			chips_pot += bet;
			(*players[i]).decrease_chips(bet);
		}
	}

	//if all players except oen decide to fold, entire spot is awarded to the remaining player
	if (num_player_fold + 1 == num_players_in_game()){
		size_t i = 0;
		for (; i < num_players_in_game(); ++i){
			if ((*players[i]).status != out_of_round){
				break;
			}
		}

		//award entire pot to the player
		(*players[i]).increase_chips(chips_pot);
		chips_pot = 0;

		cout << "Result: all players except one decide to fold; entire pot is awarded to the remaining player" << endl;
		return 0;
	}



	
	//determine the number of players with equal highest rank
	unsigned int highest_rank_players = winners.size();
	


	//compute the number of chips to split
	unsigned int split = (unsigned int)floor(chips_pot / highest_rank_players);
	cout << "split value " << split << endl;
	cout << "chips in pot: " << chips_pot << endl;

	//split chips to winners
	for (size_t i = 0; i < highest_rank_players; ++i){
		if ((*players[i]).status != out_of_round && std::find(winners.begin(), winners.end(), (*players[i]).player_name) != winners.end()){
			(*players[i]).increase_chips(split);
		}
	}

	chips_pot = 0;

	
	return 0;

}






//betting for each player
int SevenCardStud:: player_bet(std::shared_ptr<Player> &player){
	cout << "----------------- Player " << (*player).player_name << " Start--------------------" << endl;

	//print card
	cout << "-------------------Players's cards ---------------------------" << endl;
	print_players_cards(player);
	cout << "--------------------------------------------------------------" << endl;

	//print player info before making decision
	cout << "-------------------Game status before making decision ---------------------------" << endl;
	cout << "Current chips in the pot: " << chips_pot << endl;
	cout << "Current bet value: " << current_bet_value << endl;
	cout << "Player's bet so far:  " << (*player).get_bet_so_far() << endl;
	cout << "your status " << player->status << endl;



	//player runs out of chips, no obligation to call, bet, or raise
	if ((*player).get_bet_so_far() == (*player).get_num_chips()){
		cout << "NOTE: " << (*player).player_name << " runs out of chip, so no obligation to call, bet, or raise" << endl;
		return 0;

	}


	string response;
	unsigned int bet_sofar = (*player).get_bet_so_far();
	cout << (*player).player_name << ": Do you want to fold, call or raise ? " << endl;
	getline(cin, response);

	//player folds
	if (response.compare("fold") == 0){
		//player is out of round
		(*player).status = out_of_round;

		//push his bet so far to the bot
		chips_pot += bet_sofar;
		(*player).decrease_chips(bet_sofar);

		//update counter
		num_player_fold++;

	}

	//player calls
	else if (response.compare("call") == 0){
		//update player's bet so far
		(*player).set_bet_so_far(current_bet_value);
		(*player).status = call;
	}

	//player raises
	else if (response.compare("raise") == 0){
		cout << "Amount to raiase: " << endl;
		getline(cin, response);

		//can only raise 1 or 2
		while ((!is_anumber(response)) || !(stoi(response) == 1 || stoi(response) == 2)){
			cout << "Sorry, you can only raise 1 or 2" << endl;
			cout << "Do you want to raise 1 or 2 ?" << endl;
			getline(cin, response);
		}

		//check if player has enough chips for the raise
		int remaining_chips = (*player).get_num_chips() - ((*player).get_bet_so_far() + stoi(response));
		if (remaining_chips < 0){
			cout << "Sorry, you do not have enough chips to raise" << endl;

			//not enough to raise 2, but OK to raise 1
			if (stoi(response) == 2 && remaining_chips == -1){
				cout << "TIP: You do not have enough chips to raise 2, but you can raise 1" << endl;
				cout << "Please choose again" << endl;
				return player_bet(player);
			}
		}


		//update player's bet so far
		(*player).set_bet_so_far(current_bet_value + stoi(response));
		current_bet_value += stoi(response);
		(*player).status = raise;

		//updaate position of most recent bet/raise
		most_recent_bet_position = find(players.begin(), players.end(), player) - players.begin();

		//reset status of remaining players
		reset_betting_status(most_recent_bet_position);

	}

	//command not recognized
	else{
		cout << "Sorry, your command is not recognized" << endl;
		return player_bet(player);
	}



	//print player info after making decision
	cout << "-------------------Game status after making decision ---------------------------" << endl;
	cout << "Current chips in the pot: " << chips_pot << endl;
	cout << "Current bet value: " << current_bet_value << endl;
	cout << "Player's bet so far:  " << (*player).get_bet_so_far() << endl;
	cout << "Player's chip count: " << (*player).get_num_chips() << endl;
	cout << "----------------- Player " << (*player).player_name << " End --------------------" << endl;

	cout << endl;

	return 0;

}

//utility function for betting
bool SevenCardStud::is_betting_end(){

	//no need to continue if all players except one fold
	if (num_player_fold + 1 == num_players_in_game()){

		return true;
	}


	for (size_t i = 1; i < num_players_in_game(); ++i){
		//no need to consider when player is out of round or runs out of chips
		if ((*players[(most_recent_bet_position + i) % num_players_in_game()]).status == out_of_round || (*players[(most_recent_bet_position + i) % num_players_in_game()]).status == chip_run_out){
			continue;
		}

		//betting ends if most recent bet or raise is called by all remaining players
		if ((*players[(most_recent_bet_position + i) % num_players_in_game()]).status == raise || (*players[(most_recent_bet_position + i) % num_players_in_game()]).status == not_set){
			return false;
		}
	}

	return true;

}



// Reset player status after every raise
void SevenCardStud::reset_betting_status(size_t raise_index){
	for (size_t i = 1; i < num_players_in_game(); ++i){

		//skip invalid player
		if ((*players[(raise_index + i) % num_players_in_game()]).status == out_of_round || (*players[(raise_index + i) % num_players_in_game()]).status == chip_run_out){
			continue;
		}

		if ((raise_index + i) % num_players_in_game() != raise_index){
			(*players[(raise_index + i) % num_players_in_game()]).status = not_set;
		}
	}

}



//reset status after betting
void SevenCardStud:: reset_all_status(){
	for (size_t i = 0; i < num_players_in_game(); ++i){
		(*players[i]).status = not_set;
		(*players[i]).bet_so_far = 0;
	}

	//reset game state
	current_bet_value = 0;
	current_position = 0;
	all_player_check = false;
	most_recent_bet_position = -1;
	num_player_fold = 0;
}



//reset status after a betting
int SevenCardStud:: reset_afte_firstBetting(){

	for (size_t i = 0; i < num_players_in_game(); ++i){

		//skip invalid player
		if ((*players[i]).status == out_of_round || (*players[i]).status == chip_run_out){
			continue;
		}

		if ((*players[i]).status != out_of_round){
			unsigned int bet = (*players[i]).get_bet_so_far();
			chips_pot += bet;
			(*players[i]).decrease_chips(bet);
		}


		//reset player's bet so far and status
		(*players[i]).bet_so_far = 0;
		(*players[i]).status = not_set;
	}


	if (num_player_fold + 1 == num_players_in_game()){
		size_t i = 0;
		for (; i < num_players_in_game(); ++i){
			if ((*players[i]).status != out_of_round){
				break;
			}
		}

		//award entire pot to the player
		(*players[i]).increase_chips(chips_pot);
		chips_pot = 0;

		cout << "Result: all players except one decide to fold; entire pot is awarded to the remaining player" << endl;

		return too_many_folds;
	}


	//reset bet value for the phase
	current_bet_value = 0;
	return Success;

}



//print betting stats
void SevenCardStud::print_betting_stats(){
	cout << "////////////////////////////////////////////////" << endl;
	cout << "--------------Betting Stats ----------------------" << endl;
	for (size_t i = 0; i < num_players_in_game(); ++i){
		cout << "----------------------" << endl;
		cout << "Player Name: " << (*players[i]).player_name << endl;
		cout << "Win: " << (*players[i]).hands_won << endl;
		cout << "Loss: " << (*players[i]).hands_lost << endl;
		cout << "Chip Count: " << (*players[i]).get_num_chips() << endl;
		cout << "Chip Bet: " << (*players[i]).get_bet_so_far() << endl;

		if ((*players[i]).status == out_of_round){
			cout << "Player Hand: player folded without showing down their hand" << endl;
		}
		else{
			cout << "Player Hand: "  << endl;
			cout << (*players[i]).getPlayerHand().visibleAsString() << endl;
			
		}


		cout << "////////////////////////////////////////////////" << endl;
	}
}






//game's round
int SevenCardStud::before_round(){
	
	//too many players to distribute 5 cards to each player
	if ((short)(VALID_SEVEN_CARDSTUD_HAND_SIZE * players.size()) > (main_deck.size() + discard_deck.size())){
		return too_many_players;
	}
	main_deck.shuffle();
	return Success;

}


int SevenCardStud::firstphase(){
	cout << "Begin First Phase--Calling first turn on players-----" << endl;
	for (size_t count = 1; count <= num_players_in_game(); count++){
		//call turn on the player
		 first_turn(*players[(dealer_position + count) % num_players_in_game()]);

	}
	cout << "End First Phase--Calling first turn on players-----" << endl;


	cout << "// First betting phase  //" << endl;

	cout << "-------------- Betting Start -----------" << endl;

	//betting phase
	participate_betting();
	before_betting(dealer_position);
	betting();
	int reset_after_FirstBetting_result = reset_afte_firstBetting();



	if (reset_after_FirstBetting_result != Success){

		throw too_many_folds;
	}



	cout << "-------------- Betting End -----------" << endl;


	return 0;



}


//middle phase
int SevenCardStud::middle_phase(){

	cout << "Begin Second Phase--Calling Second turn on players-----" << endl;
	for (size_t count = 1; count <= num_players_in_game(); count++){
		//call turn on the player
	middle_turns(*players[(dealer_position + count) % num_players_in_game()]);

	}
	cout << "End Second Phase--Calling Second turn on players-----" << endl;


	cout << "// Second betting phase  //" << endl;
	


	cout << "-------------- Betting Start -----------" << endl;
	
	//betting phase
	before_betting(most_recent_bet_position - 1);
	betting();
	int reset_after_FirstBetting_result = reset_afte_firstBetting();


	if (reset_after_FirstBetting_result != Success){

		throw too_many_folds;
	}



	cout << "-------------- Betting End -----------" << endl;

	return 0;
}



//final betting phase
int SevenCardStud::final_phase(){
	cout << "Begin Final Phase--Calling Second turn on players-----" << endl;
	for (size_t count = 1; count <= num_players_in_game(); count++){
		//call turn on the player
		final_turn(*players[(dealer_position + count) % num_players_in_game()]);

	}

	
	cout << "// Final betting phase  //" << endl;
	


	cout << "-------------- Betting Start -----------" << endl;
	//betting phase
	before_betting(most_recent_bet_position - 1);
	betting();
	int reset_after_FirstBetting_result = reset_afte_firstBetting();


	if (reset_after_FirstBetting_result != Success){
		throw too_many_folds;
	}


	cout << "-------------- Betting End -----------" << endl;
	return Success;
}


//game round
int SevenCardStud:: round(){
	try{

		firstphase();
		for (size_t i = 0; i < 3; i++){
			middle_phase();
		}

		final_phase();

	}

	catch (Error e){
		return e;
	}
	catch (...){

		cout << "getting unknown exceptions" << endl;
		return -1;
	}


	return 0;

}


//choose five five cards
Hand SevenCardStud::best_five(Player & player){
	Hand hand = player.getPlayerHand();

	vector<Hand>allhands;

	//generate all posible combinations
	for (size_t i = 0; i < VALID_SEVEN_CARDSTUD_HAND_SIZE; i++){
		for (size_t j = i + 1; j < VALID_SEVEN_CARDSTUD_HAND_SIZE; j++){
			for (size_t k = j + 1; k < VALID_SEVEN_CARDSTUD_HAND_SIZE; k++){
				for (size_t m = k + 1; m < VALID_SEVEN_CARDSTUD_HAND_SIZE; m++){
					for (size_t n = m + 1; n < VALID_SEVEN_CARDSTUD_HAND_SIZE; n++){
						Hand temphand;
						temphand.add_card(hand[i]);
						temphand.add_card(hand[j]);
						temphand.add_card(hand[k]);
						temphand.add_card(hand[m]);
						temphand.add_card(hand[n]);
						vector<Card> temp_cards = temphand.getCards();
						sort(temp_cards.begin(), temp_cards.end());
						temphand.sethand(temp_cards);
						temphand.setHandRank();
						allhands.push_back(temphand);

					}
				}
			}

		}

	}

	//sort combinations
	sort(allhands.begin(), allhands.end(), poker_rank);

	//choose the best one
	return allhands[0];
}



//choose best five cards for all players
int SevenCardStud::best_five_forall(){
	for (size_t i = 0; i < num_players_in_game(); ++i){
		if ((*players[i]).player_hand.size() != VALID_SEVEN_CARDSTUD_HAND_SIZE){
			continue;
		}
		(*players[i]).player_original_hand = (*players[i]).getPlayerHand();
		Hand bestfive = best_five((*players[i]));
		(*players[i]).player_hand.sethand(bestfive.getCards());
	}

	return 0;
}


//after a game round
int SevenCardStud::after_round(){
	cout << "---------------Round Ends----------------" << endl;
	cout << endl;
	cout << endl;
	cout << "----------------------------Round Summary----------------------" << endl;

	//choose best five cards for players
	best_five_forall();
	print_original_vs_bestfive();
	//sort players based on their hand rank
	vector<string> winners = print_sorted_player();

	bool all_not_enough_cards = true;
	for (size_t i = 0; i < num_players_in_game(); ++i){
		if ((*players[i]).getPlayerHand().size() >= 5){
			all_not_enough_cards = false;
			break;
		}
	}

	//not enough cards
	if (all_not_enough_cards){
		shared_ptr<Player> temp = players[0];
		players[0] = players[most_recent_bet_position];
		players[most_recent_bet_position] = temp;
	}

    //print betting stats
	after_betting(winners);
	print_betting_stats();
	reset_betting_status(most_recent_bet_position);


	//move all cards into main deck
	reset();


	//if a player's chip count is 0, ask him if reset or leave
	string choice;
	for (size_t i = 0; i < num_players_in_game(); i++){
		//find a player with 0 chip count
		if ((*players[i]).get_num_chips() == 0){
			cout << (*players[i]).player_name << ": your chip count is 0" << endl;
			cout << "Do you want to quit the game or reset chip count to 20 and keep playing" << endl;
			cout << "Please enter quit or reset" << endl;
			getline(cin, choice);

			//keep prompting if input is invalid
			while (!(choice.compare("quit") == 0 || choice.compare("reset") == 0)){
				cout << "Do you want to quit the game or reset chip count to 20 and keep playing" << endl;
				cout << "Please enter quit or reset" << endl;
				getline(cin, choice);
			}

			//player quits the game
			if (choice.compare("quit") == 0){
				string s = (*players[i]).player_name;
				ofstream player_file(s + ".txt");
				player_file << *players[i];
				players.erase(players.begin() + i);
				player_file.close();
				terminate();
			}
			//reset player's chip count
			else{
				(*players[i]).increase_chips(20);
			}

		}
	}




	//promt the user asking if anyone wants to leave
	string response;
	cout << "Anyone wants to leave?" << endl;

	//parse user response
	getline(cin, response);


	//keep promting the user until saying no
	while (response.compare("no") != 0){

		istringstream player_names(response);
		vector<string> names;
		string temp;
		while (player_names >> temp){
			names.push_back(temp);

		}

		for (size_t i = 0; i < num_players_in_game(); i++)
		{
			for (string s : names){
				//matching player found
				if ((*players[i]).player_name.compare(s) == 0){
					//output statistics to file
					ofstream player_file(s + ".txt");
					player_file << *players[i];
					players.erase(players.begin() + i);
					player_file.close();
					terminate();
				}
			}

		}

		//repromt
		cout << "Anyone wants to leave?" << endl;
		getline(cin, response);

	}

	//promt the user and ask if anyone wants to join
	cout << "Anyone wants to join?" << endl;
	string join_response;
	getline(cin, join_response);





	//keep repromting until saying no
	while (join_response.compare("no") != 0){
		//parse user response
		istringstream player_names_join(join_response);
		vector<string> join_names;
		string join_temp;
		while (player_names_join >> join_temp){
			join_names.push_back(join_temp);

		}

		for (string s : join_names){
			//add player to the game
			try{
				add_player(s);
				//player already in the game
			}
			catch (Error i) {
				cout << "player: " << s << " already playing" << endl;
				cout << "Error code: " << i << endl;
			}
		}

		//reprompt
		cout << "Anyone wants to join?" << endl;
		getline(cin, join_response);

	}

	//increment dealer position
	dealer_position = (dealer_position + 1) % num_players_in_game();

	//reset all player status
	reset_all_status();

	return 0;
}




//print sorted players
vector<string> SevenCardStud::print_sorted_player(){
	cout << "///////////////////////////////////////////////" << endl;
	cout << "--------------Game Results------------" << endl;
	//make a copy of vector member variable
	vector<shared_ptr<Player>> players_copy = players;
	vector<string> winners;

	for (unsigned int i = 0; i < num_players_in_game(); ++i){
		(*players_copy[i]).player_hand.setHandRank();
	}
	
	//sort
	std::sort(players_copy.begin(), players_copy.end(), player_rank);



	//update game statistics
	for (unsigned int i = 0; i < num_players_in_game(); ++i){
		//increment hands_won for players with highest rank
		//NOTE: we assume it is possible for several players to have the same highest rank
		if (player_rank(players[0], players[i]) == player_rank(players[i], players[0])){
			(*players_copy[i]).hands_won++;
			cout << "winner: " << (*players_copy[i]).player_name << endl;
			winners.push_back((*players_copy[i]).player_name);
		}
		//increment hands_lost for players not with highest rank
		else{
			(*players_copy[i]).hands_lost++;
		}
	}

	//print each player's name, hands_won, hands_lost and current hand
	for (unsigned int i = 0; i < players.size(); ++i){
		Player current_player = *players_copy[i];
		cout << "------------" << endl;
		cout << current_player << endl;
		cout << current_player.player_original_hand.visibleAsString() << endl;
		cout << "------------" << endl;
	}

	//handle automated player's leaving
	unsigned int game_size = num_players_in_game();

	for (unsigned int i = 0; i < num_players_in_game() - 1; ++i){
		char last = (*players_copy[i]).player_name[(*players_copy[i]).player_name.length() - 1];
		if (last == '*'){
			if (player_rank(players[0], players[i]) == player_rank(players[i], players[0])){
				highest_automated_leave((*players_copy[i]).player_name);
			}
			else{
				other_automated_leave((*players_copy[i]).player_name);
			}
		}
	}

	//check if automated player with lowest rank will leave
	if (num_players_in_game() > 1){
		char last = (*players_copy[num_players_in_game() - 1]).player_name[(*players_copy[num_players_in_game() - 1]).player_name.length() - 1];
		if (last == '*' && player_rank(players[num_players_in_game() - 1], players[num_players_in_game() - 2]) != player_rank(players[num_players_in_game() - 2], players[num_players_in_game() - 1])){
			lowest_automated_leave((*players_copy[num_players_in_game() - 1]).player_name);
		}
	}

	return winners;
	cout << "////////////////////////////////////////////" << endl;
}



//reset cards and deck
void SevenCardStud::reset(){
	//move all cards from players into main deck
	for (unsigned int i = 0; i < num_players_in_game(); ++i){
		int size = (*players[i]).player_hand.size();
		for (int j = 0; j < (size); ++j){
			main_deck.add_card((*players[i]).player_hand[0]);
			(*players[i]).player_hand.remove_card(0);
		}
	}

	//move all cards from discard deck into main deck
	while (discard_deck.size() != 0){
		main_deck.add_card(discard_deck.remove());
	}

}




//extra credit
void SevenCardStud::highest_automated_leave(string automated_name){
	unsigned int random_num = rand() % 100;

	if (random_num <= 9){
		for (size_t i = 0; i < num_players_in_game(); i++)
		{
			//matching player found
			if ((*players[i]).player_name.compare(automated_name) == 0){
				//output statistics to file
				ofstream player_file(automated_name.substr(0, automated_name.length() - 1) + "_automated.txt");
				player_file << *players[i];
				players.erase(players.begin() + i);
				player_file.close();


				cout << "Automated player " << automated_name << " leave the game with highest rank with 10%" << endl;
				terminate();
			}
		}
	}
	else{
		cout << "Automated player " << automated_name << " does not leave the game with highest rank with 90%" << endl;
	}
}


//extra credit
void SevenCardStud::lowest_automated_leave(string automated_name){
	unsigned int random_num = rand() % 100;

	if (random_num <= 89){
		for (size_t i = 0; i < num_players_in_game(); i++)
		{
			//matching player found
			if ((*players[i]).player_name.compare(automated_name) == 0){
				//output statistics to file
				ofstream player_file(automated_name.substr(0, automated_name.length() - 1) + "_automated.txt");
				player_file << *players[i];
				players.erase(players.begin() + i);
				player_file.close();

				cout << "Automated player " << automated_name << " leave the game with lowest rank with 90%" << endl;
				terminate();
			}
		}
	}
	else{
		cout << "Automated player " << automated_name << " does not leave the game with lowest rank with 10%" << endl;
	}

}


//extra credit
void SevenCardStud::other_automated_leave(string automated_name){
	unsigned int random_num = rand() % 100;
	if (random_num <= 49){
		for (size_t i = 0; i < num_players_in_game(); i++)
		{
			//matching player found
			if ((*players[i]).player_name.compare(automated_name) == 0){
				//output statistics to file
				ofstream player_file(automated_name.substr(0, automated_name.length() - 1) + "_automated.txt");
				player_file << *players[i];
				players.erase(players.begin() + i);
				player_file.close();

				cout << "Automated player " << automated_name << " leave the game with other rank with 50%" << endl;
				terminate();
			}
		}
	}
	else{
		cout << "Automated player " << automated_name << " does not leave the game with other rank with 50%" << endl;
	}

}

//check if game needs to terminate
void SevenCardStud::terminate(){
	if (num_players_in_game() == 1){
		char last = (*players[0]).player_name[(*players[0]).player_name.length() - 1];
		if (last == '*'){
			ofstream player_file((*players[0]).player_name.substr(0, (*players[0]).player_name.length() - 1) + "_automated.txt");
			player_file << *players[0];
			players.erase(players.begin());
			player_file.close();
		}
		else{
			ofstream player_file((*players[0]).player_name + ".txt");
			player_file << *players[0];
			players.erase(players.begin());
			player_file.close();
		}

		cout << "Only One player left. Stop the game" << endl;
		throw Error::terminator;
	}
}



//print player cards
void SevenCardStud::print_players_cards(shared_ptr<Player> player){
	for (size_t i = 0; i < num_players_in_game(); ++i){
		if (players[i] != player){
			cout << (*players[i]).player_name << "'s cards: " << (*players[i]).getPlayerHand().asString() << endl;
		}
	}

	cout << "Your Card: ^_^" << endl;
	cout << (*player).player_name << "'s cards: " << (*player).getPlayerHand().visibleAsString() << endl;
}

//print original cards vs. best five cards
void SevenCardStud::print_original_vs_bestfive(){
	cout << "/////////////////////////////////////////////" << endl;
	cout << "----------Player's original hands vs. best five cards ---------------" << endl;
	for (size_t i = 0; i < num_players_in_game(); ++i){
		cout << (*players[i]).player_name << "'s original seven cards: " << (*players[i]).player_original_hand.visibleAsString() << endl;
		cout << (*players[i]).player_name << "'s best five cards: " << (*players[i]).getPlayerHand().visibleAsString() << endl;
		cout << (*players[i]).player_name << "'s best five cards rank: " << (*players[i]).getPlayerHand().getRank() << endl;
	}
	cout << "/////////////////////////////////////////////////////" << endl;

}