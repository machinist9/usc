#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility> // std::pair, std::make_pair
#define MIN -999999
#define MAX 999999
#define SIZE 8
using namespace std;

int get_inputs();
void initialize_scores();
void print_inputs();

char ip_board[8][8], my_player;
int cutoff_depth;
string traverse_log[MAX];
int log_counter = 0;
int scores[8][8] = {
    {99, -8, 8, 6, 6, 8, -8, 99},
    {-8, -24, -4, -3, -3, -4, -24, -8},
    {8, -4, 7, 4, 4, 7, -4, 8},
    {6, -3, 4, 0, 0, 4, -3, 6},
    {6, -3, 4, 0, 0, 4, -3, 6},
    {8, -4, 7, 4, 4, 7, -4, 8},
    {-8, -24, -4, -3, -3, -4, -24, -8},
    {99, -8, 8, 6, 6, 8, -8, 99}
  };

char int2char[8] = {'a', 'b', 'c','d', 'e', 'f', 'g', 'h'};

// Structure for Minimax as we need both position (X,Y) and value for getting the best move.
struct move {
    pair <int, int> cell;
    int val;
};

// Structure for alpha-beta (AB).
// We need - position coordinates, value and 'AB' for getting the best move.
struct move_ab {
    pair <int, int> cell;
    int val;
    int alpha;
    int beta;
};

// Output file
ofstream fout("output.txt");

// Returns the opponent marker.
char get_opponent(char player) {
	if (player == 'X') {
		return 'O';
	} else {
		return 'X';
	}
}

// Checks if the present position is valid by
// looking at the same column upwards.
bool has_valid_up_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int row;
	for (row = x-1; row >= 0; row--) {
		// Keep going upwards and make sure there is opponent marker.
		if (board[row][y] == opponent) {
			opponent_marker = true;
		} else {
			break;
		}
	}
	if (opponent_marker && row >=0) {
		if (board[row][y] == player) {
			return true;
		}
	}
	return false;
}

// Checks if the present position is valid by
// looking at the same column downwards.
bool has_valid_down_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int row;
	for (row = x+1; row < 8; row++) {
		// Keep going downwards and make sure there is opponent marker.
		if (board[row][y] == opponent) {
			opponent_marker = true;
		} else {
			break;
		}
	}
	if (opponent_marker && row < 8) {
		if (board[row][y] == player) {
			return true;
		}
	}
	return false;
}

// Checks if the present position is valid by
// looking at the same row leftwards.
bool has_valid_left_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int col;
	for (col = y-1; col >= 0; col--) {
		// Keep going leftwards and make sure there is opponent marker.
		if (board[x][col] == opponent) {
			opponent_marker = true;
		} else {
			break;
		}
	}
	if (opponent_marker && col >= 0) {
		if (board[x][col] == player) {
			return true;
		}
	}
	return false;
}

// Checks if the present position is valid by
// looking at the same row rightwards.
bool has_valid_right_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int col;
	for (col = y+1; col < 8; col++) {
		// Keep going rightwards and make sure there is opponent marker.
		if (board[x][col] == opponent) {
		opponent_marker = true;
		} else {
		break;
		}
	}
	if (opponent_marker && col < 8) {
		if (board[x][col] == player) {
			return true;
		}
	}
	return false;
}

// Checks if the present position is valid by
// looking at the up-left diagonal.
bool has_valid_up_left_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int col, row;
	for (row = x-1, col = y-1; row >= 0 && col >= 0; row--, col--) {
		// Keep going upwards and leftwards to make sure there is opponent marker.
		if (board[row][col] == opponent) {
		opponent_marker = true;
		} else {
		break;
		}
	}
	if (opponent_marker && row >= 0 && col >= 0) {
		if (board[row][col] == player) {
			return true;
		}
	}
	return false;
}

// Checks if the present position is valid by
// looking at the down-right diagonal.
bool has_valid_down_right_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int col, row;
	for (row = x+1, col = y+1; row < 8 && col < 8; row++, col++) {
		// Keep going downwards and rightwards to make sure there is opponent marker.
		if (board[row][col] == opponent) {
			opponent_marker = true;
		} else {
			break;
		}
	}
	if (opponent_marker && row < 8 && col < 8) {
		if (board[row][col] == player) {
			return true;
		}
	}
	return false;
}

// Checks if the present position is valid by
// looking at the up-right diagonal.
bool has_valid_up_right_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int col, row;
	for (row = x-1, col = y+1; row >= 0 && col < 8; row--, col++) {
		// Keep going upwards and rightwards to make sure there is an opponent marker.
		if (board[row][col] == opponent) {
			opponent_marker = true;
		} else {
			break;
		}
	}
	if (opponent_marker && row >= 0 && col < 8) {
		if (board[row][col] == player) {
			return true;
		}
	}
	return false;
}

// Checks if the present position is a valid move by
// looking at the down-left diagonal.
bool has_valid_down_left_move(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	bool opponent_marker = false;
	int col, row;
	for (row = x+1, col = y-1; row < 8 && col >= 0; row++, col--) {
		// Keep going downwards and leftwards to make sure there is an opponent marker.
		if (board[row][col] == opponent) {
			opponent_marker = true;
		} else {
			break;
		}
	}
	if (opponent_marker && row < 8 && col >= 0) {
		if (board[row][col] == player) {
			return true;
		}
	}
	return false;
}


// Code to change the state of the board.
// Following 'occupy_*_markers' function assumes that the given
// position is valid and occupies all the opponent markers.
// It is the job of the caller to make sure that the position is a valid one.
void occupy_upward_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int row;
	for (row = x-1; row >= 0; row--) {
		if (board[row][y] == opponent) {
			board[row][y] = player;
		} else {
			break;
		}
	}
}

void occupy_downward_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int row;
	for (row = x+1; row < 8; row++) {
		if (board[row][y] == opponent) {
			board[row][y] = player;
		} else {
			break;
		}
	}
}

void occupy_leftward_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int col;
	for (col = y-1; col >= 0; col--) {
		// Keep going leftwards and make sure its opponent marker.
		if (board[x][col] == opponent) {
			board[x][col] = player;
		} else {
			break;
		}
	}
}

void occupy_rightward_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int col;
	for (col = y+1; col < 8; col++) {
		// Keep going rightwards and make sure its opponent marker.
		if (board[x][col] == opponent) {
			board[x][col] = player;
		} else {
			break;
		}
	}
}

void occupy_up_left_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int col, row;
	for (row = x-1, col = y-1; row >= 0 && col >= 0; row--, col--) {
		// Keep going upwards and leftwards, and change opponent marker.
		if (board[row][col] == opponent) {
			board[row][col] = player;
		} else {
			break;
		}
	}
}

void occupy_down_right_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int col, row;
	for (row = x+1, col = y+1; row < 8 && col < 8; row++, col++) {
		// Keep going downwards and rightwards, and change opponent marker.
		if (board[row][col] == opponent) {
			board[row][col] = player;
		} else {
			break;
		}
	}
}

void occupy_up_right_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int col, row;
	for (row = x-1, col = y+1; row >= 0 && col < 8; row--, col++) {
		// Keep going upwards and rightwards, and change opponent marker.
		if (board[row][col] == opponent) {
			board[row][col] = player;
		} else {
			break;
		}
	}
}

void occupy_down_left_markers(char board[][8], int x, int y, char player) {
	char opponent = get_opponent(player);
	int col, row;
	for (row = x+1, col = y-1; row < 8 && col >= 0; row++, col--) {
		// Keep going downwards and leftwards, and change opponent marker.
		if (board[row][col] == opponent) {
			board[row][col] = player;
		} else {
			break;
		}
	}
}


/**
  A position is a valid move if there exists atleast one occupied opponent,
  between this position and another previously occupied position.
  This occupied opponent can be either aligned diagonally or
  hozirontally or vertically.
*/
bool is_valid_move(char board[][8], int x, int y, char player){
  // If already occupied, then its not a valid move.
  if (board[x][y] == 'X' || board[x][y] == 'O') {
    //cout << "Occupied "<< x << "," << y <<endl;
    return false;
  }
  // Check vertically up.
  if (has_valid_up_move(board, x, y, player)) {
    return true;
  }
  // Check vertically down.
  if (has_valid_down_move(board, x, y, player)) {
    return true;
  }
  // Check horizontally left.
  if (has_valid_left_move(board, x, y, player)) {
    return true;
  }
  // Check horizontally right.
  if (has_valid_right_move(board, x,y, player)) {
    return true;
  }
  // Check diagonally up left.
  if (has_valid_up_left_move(board, x, y, player)) {
    return true;
  }
  // Check diagonally down right.
  if (has_valid_down_right_move(board, x, y, player)) {
    return true;
  }
  // Check diagonally up right.
  if (has_valid_up_right_move(board, x, y, player)) {
    return true;
  }
  // Check diagonally down left.
  if (has_valid_down_left_move(board, x, y, player)) {
    return true;
  }
  return false;
}

/** 
	Takes the board as input and returns the list of valid moves.
	The valid moves are saved as a vector of pair <X, Y>
**/
vector<pair<int, int> > get_valid_moves(char board[][8], char player) {
	// Save points as pairs in this vector.
	vector<pair<int, int> > valid_positions;
	for(int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col ++) {
			// For every position on board, check if its valid.
			if (is_valid_move(board, row, col, player)) {
				// If valid, generate a pair and save them.
				//cout << "Valid move = "<< row << ","<< col <<endl;
				pair <int, int> pos(row, col);
				valid_positions.push_back(pos);
			}
		}
	}
	// Add a special "pass" move (-2,-2) if there are no valid moves for a player.
	if (valid_positions.size() == 0) {
		pair <int,int> p(-2,-2);
		valid_positions.push_back(p);
	}
	return valid_positions;
}

void change_board_state(char board[][8], int x, int y, char player) {
  // Positions should be valid.
  if (x < 0 || y < 0 || x >=8 || y >= 8) {
  	return;
  }
  // Check vertically up.
  if (has_valid_up_move(board, x, y, player)) {
    occupy_upward_markers(board, x,y, player);
  }
  // Check vertically down.
  if (has_valid_down_move(board, x, y, player)) {
    occupy_downward_markers(board, x,y, player);
  }
  // Check horizontally left.
  if (has_valid_left_move(board, x, y, player)) {
    occupy_leftward_markers(board, x,y, player);
  }
  // Check horizontally right.
  if (has_valid_right_move(board, x,y, player)) {
    occupy_rightward_markers(board, x,y, player);
  }
  // Check diagonally up left.
  if (has_valid_up_left_move(board, x, y, player)) {
    occupy_up_left_markers(board, x,y, player);
  }
  // Check diagonally down right.
  if (has_valid_down_right_move(board, x, y, player)) {
    occupy_down_right_markers(board, x,y, player);
  }
  // Check diagonally up right.
  if (has_valid_up_right_move(board, x, y, player)) {
    occupy_up_right_markers(board, x,y, player);
  }
  // Check diagonally down left.
  if (has_valid_down_left_move(board, x, y, player)) {
    occupy_down_left_markers(board, x,y, player);
  }
  // Set the original passed position now.
  board[x][y] = player;
}

int get_eval_score(char board[][8], char player) {
	int player_score = 0, opponent_score = 0;
	char opponent_marker = get_opponent(player);
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
      		if (board[row][col] == player) {
      			player_score += scores[row][col];
      		} else if (board[row][col] == opponent_marker) {
      			opponent_score += scores[row][col];
      		}
    	}
	}
	return (player_score - opponent_score);
}

void copy_board(char board[][8], char new_board[][8], int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			new_board[i][j] = board[i][j];
		}
	}
}

void print_state(char board[][8]) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      cout << board[row][col];
    }
    cout << endl;
  }
}

void print_state_in_file(char board[][8]) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      fout << board[row][col];
    }
    fout << endl;
  }
}

void greedy() {
	vector<pair<int, int> > valid_moves = get_valid_moves(ip_board, my_player);
	int max_score = MIN;
	pair<int, int> best_pos(-1, -1);
	for(vector<pair<int, int> >::iterator it = valid_moves.begin();
		it != valid_moves.end(); ++it) {
		pair<int, int> pos = *it;
		// Skip the special moves.
		if (pos.first == -1 || pos.first == -2) {
			continue;
		}
		char new_board[8][8];
		copy_board(ip_board, new_board, SIZE, SIZE);
		change_board_state(new_board, pos.first, pos.second, my_player);
		int eval_score = get_eval_score(new_board, my_player);
		//cout << pos.first << "," << pos.second << "|| score = " << scores[pos.first][pos.second] << "|| max score = " << max_score<<endl;
		// Choose the position with max score.
		if (eval_score > max_score) {
			best_pos = pos;
			max_score  = eval_score;
		}
	}
	if (best_pos.first != -1) {
		change_board_state(ip_board, best_pos.first, best_pos.second, my_player);
	}
	print_state_in_file(ip_board);
}

// This function converts the int value to string taking care of extreme values.
string get_str_val(int value) {
	string val;
	if (value == MIN) {
	    val = "-Infinity";
	} else if (value == MAX) {
		val = "Infinity";
	} else {
		char str_val[100];
		sprintf(str_val, "%d", value);
		val = string (str_val);
	}
	return val;
}

void print_minmax_node (pair<int, int> pos, int depth, int value) {
	// Print traverse log.
	string node_info;
	if (depth == 0) {
		node_info = "root,";
	} else if (pos.first == -2) {
		node_info = "pass,";
	} else {
		int row = pos.first + 1;
		char row_str[100];
		sprintf(row_str, "%d", row);
		node_info = int2char[pos.second] +  string(row_str) + ",";
	}
	char depth_str[100];
	sprintf(depth_str, "%d", depth);
	// Append depth
	node_info = node_info + string(depth_str) + ",";
	// Append value
	node_info = node_info + get_str_val(value);
	//// cout << node_info << endl;
	traverse_log[log_counter++] = node_info;
}

void print_traverse_log () {
	for (int i = 0; i < log_counter; i++) {
		cout << traverse_log[i] << endl;
	}
}

void print_traverse_log_in_file () {
	for (int i = 0; i < log_counter; i++) {
		fout << traverse_log[i] << endl;
	}
}

/**
	This method updates the best move, if the temp move is better
	than the available best move.
	Returns the best move structure.
	*/
move update_best_move(move best, move temp, int depth, pair<int, int> pos) {
	if (depth % 2 == 0) {
		if (temp.val > best.val) {
			best.cell = pos;
			best.val = temp.val;
		}
	} else {
		if (temp.val < best.val) {
			best.cell = pos;
			best.val = temp.val;
		}
	}
	return best;
}

bool is_end_state(char board[][8]) {
	int player1 = 0, player2 = 0;
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (board[r][c] == 'X') {
				player1++;
			} else if (board[r][c] == 'O') {
				player2++;
			}
		}
	}
	if (player1 != 0 && player2 != 0) {
		return false;
	}
	return true;
}

move run_min_max(int depth, char board[][8], pair <int, int> pos,
	pair<int, int> parent) {
	// Get the player's turn
	char present_player;
	move best_move;
	if (depth % 2 == 0) {
		present_player = my_player;
		best_move.val = MIN;
	} else {
		present_player = get_opponent(my_player);
		best_move.val = MAX;
	}
	if (depth == cutoff_depth || (pos.first == -2 && (parent).first == -2)
			|| is_end_state(board)) {
		///// cout << " ############ END STATE ############" << endl;
		///// print_state(board);
	    int eval_score = get_eval_score(board, my_player);
	    print_minmax_node (pos, depth, eval_score);
	    move m;
	    m.cell = pos;
	    m.val = eval_score;
	    return m;
	}
	print_minmax_node (pos, depth, best_move.val);
	vector<pair <int,int> > valid_moves = get_valid_moves(board, present_player);
	// Iterate through each valid move, change the state and return the score.
	for(vector<pair<int, int> >::iterator it = valid_moves.begin();
		it != valid_moves.end(); ++it) {
		char new_board[8][8];
		copy_board(board, new_board, SIZE, SIZE);
		change_board_state(new_board, (*it).first, (*it).second, present_player);
		/// print_state(new_board);
		move temp_move = run_min_max(depth + 1, new_board, *it, pos);
		// Update the best_move depending upon the depth.
		// Get the max score for MAX level and min score for MIN level.
		best_move = update_best_move(best_move, temp_move, depth, *it);
		print_minmax_node (pos, depth, best_move.val);
	}
	return best_move;
}

void minimax() {
	// Dummy position used for root node.
	pair<int, int> p(-1,-1);
	move minimax_move = run_min_max(0, ip_board, p, p);
	// Make final change in the main board.
	change_board_state(ip_board, minimax_move.cell.first,
		minimax_move.cell.second, my_player);
	// Print the next state.
	///// print_state(ip_board);
	print_state_in_file(ip_board);
	///// print_traverse_log();
	fout << "Node,Depth,Value" << endl;
	print_traverse_log_in_file();
}

void print_ab_node(pair <int, int> pos, int depth, int value, int alpha, int beta) {
	string node_info;
	if (depth == 0) {
	    node_info = "root,";
	} else if (pos.first == -2) {
		node_info = "pass,";
	}  else {
		int row = pos.first + 1;
		char row_str[100];
		sprintf(row_str, "%d", row);
		node_info = int2char[pos.second] +  string(row_str) + ",";
	}
	char depth_str[100];
	sprintf(depth_str, "%d", depth);
	node_info = node_info + string(depth_str) + ",";
	node_info = node_info + get_str_val(value);
	// Appending Alpha and Beta.
	node_info = node_info + "," + get_str_val(alpha);
	node_info = node_info + "," + get_str_val(beta);
	///// cout << node_info << endl;
	traverse_log[log_counter++] = node_info;
}

move_ab update_ab_best_move_pos_val (move_ab best, move_ab temp, int depth,
	pair<int,int> pos) {
	if (depth % 2 == 0) { // MAX player
		if (temp.val > best.val ) {
			best.val = temp.val;
			best.cell = pos;
		}
	} else { // MIN player
		if (temp.val < best.val ) {
			best.val = temp.val;
			best.cell = pos;
		}
	}
	return best;
}

bool loop_return_check (move_ab best, int depth, int alpha, int beta) {
	if (depth % 2 == 0) { // MAX player
		if (best.val >= beta) {
			return true;
		}
	} else { // MIN player.
		if (best.val <= alpha) {
			return true;
		}
	}
	return false;
}

move_ab update_ab_values(move_ab best, int depth, int alpha, int beta) {
	if (depth % 2 == 0) { // MAX player
		if (best.val > alpha) {
			best.alpha = best.val;
		}
	} else { // MIN player
		if (best.val < beta) {
			best.beta = best.val;
		}
	}
	return best;
}

move_ab run_alpha_beta (int depth, char board[][8], pair <int, int> pos,
	int alpha, int beta, pair<int, int> parent) {
	// Get the player's turn and, initialize alpha and beta.
	char present_player;
	move_ab best_move;
	best_move.alpha = alpha;
	best_move.beta = beta;
	if (depth % 2 == 0) {
		present_player = my_player;
		best_move.val = MIN;
	} else {
		present_player = get_opponent(my_player);
		best_move.val = MAX;
	}
	// Return if the cutoff depth is reached or none of the players have a move
	// or if end state has been reached.
	if (depth == cutoff_depth || (pos.first == -2 && (parent).first == -2)
			|| is_end_state(board)) {
		///// cout << " ############ END STATE ############" << endl;
		///// print_state(board);
	    int eval_score = get_eval_score(board, my_player);
	    print_ab_node (pos, depth, eval_score, alpha, beta);
	    best_move.cell = pos;
	    best_move.val = eval_score;
	    // Update the alpha value, if MAX level or beta value if the MIN level,
	    // as this value will be used in the parent level.
	    if (depth % 2 == 0) {
	    	best_move.alpha = eval_score;
	    } else {
	    	best_move.beta = eval_score;
	    }
	    return best_move;
	}
	print_ab_node (pos, depth, best_move.val, alpha, beta);
	vector<pair <int, int> > valid_moves = get_valid_moves(board, present_player);
	// Iterate through each valid move, change the state and return the score.
	for(vector<pair<int, int> >::iterator it = valid_moves.begin();
		it != valid_moves.end(); ++it) {
		char new_board[8][8];
		copy_board(board, new_board, SIZE, SIZE);
		change_board_state(new_board, (*it).first, (*it).second, present_player);
		/// print_state(new_board);
		move_ab temp_move = run_alpha_beta(depth + 1, new_board, *it,
			best_move.alpha, best_move.beta, pos);
		//
		// NOTE: According to AIMA 5.7, we need to return if the 'value' is
		// greater than beta for MAX player and lesser than alpha for
		// MIN player before updating the alpha/beta values.
		//
		// So we just update the position and value of the best move, later
		// we update the alpha beta values if we do not return.
		best_move = update_ab_best_move_pos_val (best_move, temp_move,
			depth, *it);
		if (loop_return_check (best_move, depth, alpha, beta)) {
			print_ab_node (pos, depth, best_move.val, best_move.alpha, best_move.beta);
			return best_move;
		}
		// Update the best_move alpha-beta values depending upon the depth.
		best_move = update_ab_values(best_move, depth, alpha, beta);
		print_ab_node (pos, depth, best_move.val, best_move.alpha, best_move.beta);
	}
	////cout << "^^^^ Returning best move at depth " << depth<< ":" << best_move.cell.first << "," << best_move.cell.second << " ^^^^^^^^" << endl;
	return best_move;
}

void alpha_beta() {
	// Dummy position used for root node.
	pair<int, int> p(-1,-1);
	// Pass depth (0 for root), present board state, dummy root position,
	// alpha (-Infinity) and beta (+Infinity) values.
	move_ab ab_move = run_alpha_beta(0, ip_board, p, MIN, MAX, p);
	// Make final change in the main board.
	change_board_state(ip_board, ab_move.cell.first, ab_move.cell.second, my_player);
	// Print the next state.
	///// print_state(ip_board);
	print_state_in_file(ip_board);
	// Print the traverse log.
	///// print_traverse_log();
	fout << "Node,Depth,Value,Alpha,Beta" << endl;
	print_traverse_log_in_file();
}

int main() {
  // Get inputs.
  int task = get_inputs();

  //Print inputs
  ///// print_inputs();

  // Do the job.
  if (task == 1) {
    greedy();
  } else if (task == 2) {
    minimax();
  } else {
    alpha_beta();
  }

  // All set, clsoe the file and return.
  fout.close();
  return 0;
}

/** Reads the input from input.txt file.
Format of the file is as follows:
<task> Greedy = 1, MiniMax = 2, Alpha-beta = 3, Competition =4
<your player: X or O>
<cutting off depth >
<current state as follows:>
  *: blank cell
  X: Black player
  O: White Player
*/
int get_inputs () {
  ifstream fin("input.txt");
  if (!fin.is_open()) {
      fout << "File not found."<< endl;
      fout.close();
  }
  int task;
  fin >> task >> my_player >> cutoff_depth;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      fin >> ip_board[row][col];
    }
  }
  // Done with reading inputs.Close the file.
  fin.close();
  return task;
}

void print_inputs() {
  cout << "player = " << my_player << endl;
  cout << "Depth = " << cutoff_depth <<endl;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      cout << ip_board[row][col];
    }
    cout << endl;
  }
}
