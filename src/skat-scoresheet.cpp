/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code

*/

const char* VERSION = "v0.0.1";

#include "skat_game.h"
#include <iostream>

using namespace std;

enum State {INIT, START_GAME, NEW_HAND, SCORE_HAND, END_GAME, EDIT_GAME, GAME_COMPLETED};

State state = INIT;
Skat_Game game("AT", "RT", "MT");
int winning_player;
int tie_game[4]; 
int number_of_ties;
int play_first_card;

int main(int argc, char** argv) {

    /// Next 3 lines are for initial testing
    cout << "Player 1: " << game.player_name[0] << endl;
    cout << "Player 2: " << game.player_name[1] << endl;
    cout << "Player 3: " << game.player_name[2] << endl;

    while (state != GAME_COMPLETED) {

        switch (state) {
            case INIT:
                cout << "Welcome to the Skat Scoresheet!" << endl;
                cout << "Enter number of players:" << endl;
                /// Input integer
                /// Eventually add code to read from config file if it exists
                game.number_of_players = 3;  /// update 
                cout << "Enter Player 1 name (dealer): " << endl;
                /// Input string
                /// Eventually add code to read from config file if it exists
                /// strcpy(game.player_name[0], c-string); /// Should really use strncpy
                cout << "Enter Player 2 name: " << endl;
                /// Input string
                /// Eventually add code to read from config file if it exists
                /// strcpy(game.player_name[1], c-string); /// Should really use strncpy
                cout << "Enter Player 3 name: " << endl;
                /// Input string
                /// Eventually add code to read from config file if it exists
                /// strcpy(game.player_name[2], c-string); /// Should really use strncpy
                if (game.number_of_players >3) {
                    cout << "Enter Player 2 name: " << endl;
                    /// Input string
                    /// Eventually add code to read from config file if it exists
                    /// strcpy(game.player_name[3], c-string); /// Should really use strncpy
                }
                cout << "How many hands will you be plaiying in today's game? " << endl;
                /// Input integer
                game.number_of_hands = 36;  /// Update with entered value
                state = START_GAME;
                break;

            case START_GAME:
                /// Set up the game data structure
                /// For now, I think that means setting all the scores in each hand to zero:
                ///   current_hand, hand[0..number_of_hands-1].score[0..3] all zeroed
                /// Does C++ initialze? 
                for (int i = 0; i < game.number_of_players; i++ ) tie_game[i] = -1;
                number_of_ties = 0;
///                cout << "Hand 0 scores: " << game.hand[0].score[0] << " " << game.hand[0].score[1] << " " << game.hand[0].score[2] << endl;
                state = NEW_HAND;
                break;

            case NEW_HAND:
                cout << "Starting hand number " << game.current_hand +1 << endl;
                cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer." << endl;
                /// Add who is gaben and sagen
                cout << "Enter winning bid (enter 0 for Ramsch): " << endl; 
                game.hand[game.current_hand].bid = 22; /// Update with input value
                /// if (bid == 0), then handle Ramsch
                /// if Ramsch, then don't need to ask about contract
                cout << "Enter contract (C, D, H, S, G, N)" << endl; 
                game.hand[game.current_hand].contract = game.SPADES;  /// Update with input value 
                /// If not Ramsch 
                cout << "Who won the bid? " << endl; /// Need some logic to make it easy to enter bidder: print name and then the index, just ask for index?
                game.hand[game.current_hand].bidder = (game.current_hand % game.number_of_players);
                cout << "Will it be played Hand? " << endl; /// game.hand[current_hand].multipliers = HAND
                /// If hand, then can play open. 
                /// If open, the automatic announce Schneider and Schwarz
                /// If hand but not open, then can announce Schneider or Schwarz
                cout << "Announce Schneider? " << endl; /// logical OR the multiplier
                cout << "Annoucne Schwarz? " << endl; /// logical OR the multipler
                cout << "Hand summary: " << endl; /// Print out the hand summary and ask for comformation before continuing.
                play_first_card = (game.current_hand % game.number_of_players) + 1;
                if (play_first_card == game.number_of_players) play_first_card = 0;
                cout << game.player_name[play_first_card] << " leads." << endl;
                state = SCORE_HAND;
                break;

            case SCORE_HAND:
                cout << "Scoring the hand..." << endl; ///
                if (game.hand[game.current_hand].contract == game.RAMSCH) {
                    cout << "How many players lost the Ramsch?" << endl;
                    /// Assume 1 for now
                    cout << "Who lost?" << endl; /// Print player names and index
                    /// Assume player 1 lost
                    cout << "Was there a Jungfrau?" << endl;
                } else { // Regular hand, not Ramsch
                    cout << "With or without how many? " << endl;
                    game.hand[game.current_hand].matadors = game.current_hand % 5 + 1;
                    cout << "Was there a Kontra? " << endl;
                    cout << " Was there a Re? " << endl;  /// Only ask if there was a Kontra
                    game.hand[game.current_hand].kontrare = game.SINGLE; /// Update per input
                    cout << "Did " << game.player_name[game.hand[game.current_hand].bidder] << " win the hand?" << endl;
                    game.hand[game.current_hand].winlose = game.current_hand % 2 ? game.WIN : game.LOSE; /// Replace 
                    cout << "Was it Schneider?" << endl;
                    cout << "Was it Schwarz?" << endl;
                    /// Need a handler for player lost
                }

                game.calculate_hand_score(game.current_hand);
                game.calculate_game_score();
                game.print_game_status();
                game.current_hand++;
                if (game.current_hand < game.number_of_hands) state = NEW_HAND;
                else state = END_GAME;
                break;
            
            case END_GAME:
                cout << "Final Scoring..." << endl;
                for (int i = 0; i < game.number_of_players; i++) {
                    cout << game.player_name[i] << ": " << game.total_score[i] << endl;
                }
                if (game.total_score[0] > game.total_score[1]) winning_player = 0;
                else winning_player = 1;
                if (game.total_score[winning_player] < game.total_score[2]) winning_player = 2; 
                if (game.number_of_players == 4) {
                    if (game.total_score[winning_player] < game.total_score[3]) winning_player = 3;
                }
                // Check for ties
                for (int i = 0; i < game.number_of_players; i++) {
                    if (game.total_score[i] == game.total_score[winning_player]) tie_game[i] = 1;
                }
                for (int i = 0; i < game.number_of_players; i++) {
                    if (tie_game[i] == 1) number_of_ties++;
                }
                if (number_of_ties > 1) {
                    cout << "There was a tie for first place. " << endl;
                    cout << "Players ";
                    for (int i = 0; i < game.number_of_players; i++) {
                        if (tie_game[i] == 1) cout << game.player_name[i];
                        if (--number_of_ties > 0) cout << " and ";
                    }
                    cout << " won." << endl;
                } else {
                    cout << "Player " << game.player_name[winning_player] << " won." << endl;
                }
                cout << "Congratulations -- game has completed." << endl;
                return 9;
                break;
            
            case EDIT_GAME:
                break;

            default:  /// Need better error handling
                cout << "Invalid state reached. Exit." << endl;
                return 1;
                break;
        }
    }
   
    return 0;
}
