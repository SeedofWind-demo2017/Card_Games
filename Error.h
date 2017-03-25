//define different error levels


#ifndef ERROR_H
#define ERROR_H

enum Error{
	Success,
	only_shuffle = 1,
	neither_shuffle,
	no_arg,
	too_many_arg,
	load_fail,
	not_enough_cards,
	index_out_of_bound,
	file_not_open,
	instance_not_available,
	game_already_started,
	unknown_game,
	no_game_in_progress,
	already_playing,
	before_turn_remove_fail,
	no_more_card_for_this_game_go_home,
	too_many_players,
	before_turn_fail,
	player_not_join,


	/////////////////////////////////
	//Main function error
	two_few_arguments,
	too_many_folds,
	terminator



};

#endif