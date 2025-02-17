/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code

*/

const char* VERSION = "v0.0.1";

#include "skat_game.h"
#include <iostream>
#include <string.h>
#include <limits>

int yes();
int input_and_validate(int min, int max);

using namespace std;

enum State {INIT, START_GAME, NEW_HAND, SCORE_HAND, END_GAME, EDIT_GAME, GAME_COMPLETED};

State state = INIT;
Skat_Game game;
int winning_player;
int tie_game[4]; 
int number_of_ties;

int main(int argc, char** argv) {
    string name;
    int h;
    int bock = 1;  /// Change to 0 after testing

    /// Add support for a Ramsch-only game

    while (state != GAME_COMPLETED) {
        switch (state) {
            case INIT:
                cout << "Welcome to the Skat Scoresheet!" << endl;
                cout << "Enter number of players (3 or 4): " << endl;
                game.number_of_players = input_and_validate(3, 4);
                cout << "Enter Player 1 name: " << endl;
                cin >> name;
                strncpy(game.player_name[0], name.c_str(), MAX_NAME_SIZE);
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any extra characters
                cout << "Enter Player 2 name: " << endl;
                cin >> name;
                strncpy(game.player_name[1], name.c_str(), MAX_NAME_SIZE);
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any extra characters
                cout << "Enter Player 3 name: " << endl;
                cin >> name;
                strncpy(game.player_name[2], name.c_str(), MAX_NAME_SIZE);
                if (game.number_of_players > 3) {
                    cout << "Enter Player 4 name: " << endl;
                    cin >> name;
                    strncpy(game.player_name[3], name.c_str(), MAX_NAME_SIZE);
                }
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any extra characters
                cout << "How many hands will you be playing in today's game (3 to 36)?" << endl;
                game.number_of_hands = input_and_validate(3, 36);
                cout << "Game will consist of " << game.number_of_players << " players named: " << endl;
                for (int i = 0; i < game.number_of_players; i++) cout << game.player_name[i] << endl;
                cout << "Game will end after " << game.number_of_hands << " hands." << endl;
                cout << "Is this correct? " << endl;
                if (yes()) state = START_GAME;
                else state = INIT;
                break;

            case START_GAME:  /// This state may not be needed; merge code into INIT state
                for (int i = 0; i < game.number_of_players; i++ ) tie_game[i] = -1;
                state = NEW_HAND;
                break;

            case NEW_HAND:
                cout << "Starting hand number " << game.current_hand +1 << endl;
                if (bock > 0) {
                    cout << "This hand is a Bockrund. Scores double." << endl;
                    game.hand[game.current_hand].bock = Skat_Game::BOCKRUND;
                } else game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                /// This may not be correct for 4 player games
                cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer." << endl;
                cout << game.player_name[(game.current_hand + 1) % game.number_of_players] << " listens." << endl;
                cout << game.player_name[(game.current_hand + 2) % game.number_of_players] << " speaks."  << endl;
                cout << "Enter winning bid (enter 0 for Ramsch): " << endl; 
                game.hand[game.current_hand].bid = input_and_validate(0, 168);
                // if Ramsch, then don't need to ask about contract
                if (game.hand[game.current_hand].bid == 0) game.hand[game.current_hand].contract = Skat_Game::RAMSCH;
                else game.set_contract(game.current_hand);
                if (game.hand[game.current_hand].contract != Skat_Game::RAMSCH) {
                    /// Need logic for 4-player game -- dealer should not be listed
                    cout << "Who is the declarer? " << endl;
                    for (int i = 0; i < game.number_of_players; i++) {
                        cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                    }
                    game.hand[game.current_hand].declarer = input_and_validate(1, game.number_of_players) - 1;
                    if (game.hand[game.current_hand].contract == Skat_Game::Skat_Game::NULLL) {
                        cout << "Will it be played Hand (Y/N)? " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::HAND;
                        }
                        cout << "Will it be played Open (Y/N)? " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::OPEN;
                        }
                    } else { // Suit or Grand
                        cout << "Will it be played Hand (Y/N)? " << endl;
                        if (yes()) game.hand[game.current_hand].multipliers = Skat_Game::HAND;
                        // If hand, then can play open. 
                        if (game.hand[game.current_hand].multipliers & Skat_Game::HAND) {
                            cout << "Will it be played Open -- must take all tricks (Y/N)? " << endl;
                            if (yes()) {
                                game.hand[game.current_hand].multipliers = 
                                (Skat_Game::HAND | Skat_Game::OPEN | Skat_Game::SCHNEIDER |  
                                Skat_Game::SCHN_ANNC | Skat_Game::SCHWARZ | Skat_Game::SCHW_ANNC);
                                cout << "Playing Open implies announcing Schneider and Schwarz." << endl;
                            } else {
                                // If hand but not open, then can announce Schneider or Schwarz
                                cout << "Announce Schneider? " << endl;
                                if (yes()) {
                                    game.hand[game.current_hand].multipliers |= 
                                    (Skat_Game::SCHN_ANNC | Skat_Game::SCHNEIDER);
                                    cout << "Announce Schwarz? " << endl; 
                                    if (yes()) game.hand[game.current_hand].multipliers |= 
                                    (Skat_Game::SCHW_ANNC | Skat_Game::SCHWARZ); 
                                }
                            }
                        }
                    }                    
                }
                cout << "Hand summary: " << endl; 
                if (game.hand[game.current_hand].contract == game.RAMSCH) {
                    cout << "This hand will be played Ramsch." << endl;
                } else {
                    cout << game.player_name[game.hand[game.current_hand].declarer]
                         << " plays " << game.get_contract_name(game.hand[game.current_hand].contract);
                    if (game.hand[game.current_hand].multipliers & game.HAND) cout << " HAND";
                    cout << " on a bid of " << game.hand[game.current_hand].bid << endl;
                    if (game.hand[game.current_hand].multipliers & game.SCHN_ANNC) cout << "Schneider announced." << endl;
                    if (game.hand[game.current_hand].multipliers & game.SCHW_ANNC) cout << "Schwarz announced." << endl;
                }
                cout << "Is this correct: " << endl;
                if (yes()) {
                    cout << game.player_name[(game.current_hand + 1) % game.number_of_players] << " leads." << endl;
                    state = SCORE_HAND;
                 } else state = NEW_HAND;
                break;

            case SCORE_HAND:
                cout << "Scoring the hand after play..." << endl;
                if (game.hand[game.current_hand].contract == game.RAMSCH) {
                    cout << "Did a player win Durchmarsch? " << endl;
                    if (yes()) {
                        game.hand[game.current_hand].ramsch = Skat_Game::DURCHMARSCH;
                        game.hand[game.current_hand].winlose = Skat_Game::WIN;
                        game.hand[game.current_hand].number_of_losers = 0;
                        cout << "Who won the Durchmarsch? " << endl;
                        /// Need handler for 4 players
                        for (int i = 0; i < game.number_of_players; i++) {
                            cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                        }
                        game.hand[game.current_hand].declarer = input_and_validate(1, game.number_of_players) - 1;
                    } else { // Not a Durchmarsch
                        cout << "How many players lost the Ramsch?" << endl;
                        switch (input_and_validate(1, 3)) {
                            case 1: 
                                game.hand[game.current_hand].winlose = Skat_Game::LOSE;
                                game.hand[game.current_hand].number_of_losers = 1;
                                cout << "Who lost the Ramsch? " << endl;
                                /// Need handler for 4 players
                                for (int i = 0; i < game.number_of_players; i++) {
                                    cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                                }
                                game.hand[game.current_hand].loser[0] = input_and_validate(1, game.number_of_players) - 1;
                                cout << "How many points taken? " << endl;
                                game.hand[game.current_hand].cardpoints = input_and_validate(40, 120); 
                                cout << "Was there a Jungf?" << endl;
                                if (yes()) game.hand[game.current_hand].ramsch = Skat_Game::JUNGF;
                                else game.hand[game.current_hand].ramsch = Skat_Game::PLAIN;                                
                                break;

                            case 2: 
                                game.hand[game.current_hand].winlose = Skat_Game::LOSE;
                                game.hand[game.current_hand].number_of_losers = 2;
                                cout << "First player to lose the Ramsch? " << endl;
                                /// Need handler for 4 players
                                for (int i = 0; i < game.number_of_players; i++) {
                                    cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                                }
                                game.hand[game.current_hand].loser[0] = input_and_validate(1, game.number_of_players) - 1;
                                cout << "Other player to lose the Ramsch? " << endl;
                                /// Need handler for 4 players
                                /// Need to remove player name chosen above
                                for (int i = 0; i < game.number_of_players; i++) {
                                    cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                                }
                                game.hand[game.current_hand].loser[1] = input_and_validate(1, game.number_of_players) - 1;
                                cout << "How many points taken? " << endl;
                                game.hand[game.current_hand].cardpoints = input_and_validate(40, 120); 
                                cout << "Was there a Jungf?" << endl;
                                if (yes()) game.hand[game.current_hand].ramsch = Skat_Game::JUNGF;
                                else game.hand[game.current_hand].ramsch = Skat_Game::PLAIN;                                
                                break;

                            case 3: 
                                cout << "40/40/40 Ramch." << endl;
                                game.hand[game.current_hand].winlose = Skat_Game::LOSE;
                                game.hand[game.current_hand].number_of_losers = 3;
                                game.hand[game.current_hand].cardpoints = 40;
                                /// Need to add a handler for 4 players
                                for (int i = 0; i < 3; i++) {
                                    game.hand[game.current_hand].loser[i] = i;
                                }
                                game.hand[game.current_hand].ramsch = Skat_Game::PLAIN;                                
                                break;

                            default:
                                cout << "Invalid number of Ramsch losers." << endl;
                                break;
                        }
                    }
                } else { // not Ramsch
                    if (game.hand[game.current_hand].contract == Skat_Game::NULLL) {
                        cout << "Did " << game.player_name[game.hand[game.current_hand].declarer] << " win the Null? " << endl;
                        if (yes()) game.hand[game.current_hand].winlose = Skat_Game::WIN;
                        else game.hand[game.current_hand].winlose = Skat_Game::WIN;
                        game.hand[game.current_hand].cardpoints = 0;
                    } else {
                        cout << "How many card points did " << game.player_name[game.hand[game.current_hand].declarer] << " collect? " << endl;
                        game.hand[game.current_hand].cardpoints = input_and_validate(0, 120);
                        if (game.hand[h].cardpoints == 120) {
                            cout << "Did "<< game.player_name[game.hand[game.current_hand].declarer] << "take all the tricks? " << endl;
                            if (yes()) game.hand[h].multipliers |= Skat_Game::SCHWARZ;
                        }
                        cout << "With or without how many? " << endl;
                        game.hand[game.current_hand].matadors = input_and_validate(1, 8);
                        game.calculate_win_lose(game.current_hand);
                        cout << "*** " << game.player_name[game.hand[game.current_hand].declarer]; 
                        if (game.hand[game.current_hand].winlose == Skat_Game::WIN) cout << " won ";
                        else cout << " lost ";
                        cout << "the hand. ***" << endl;
                    }
                    cout << "Was there a Kontra? " << endl;
                    if (yes()) {
                        game.hand[game.current_hand].kontrare = Skat_Game::KONTRA;
                        cout << " Was there a Re? " << endl;  // Only ask about Re if there was a Kontra
                        if (yes()) game.hand[game.current_hand].kontrare = Skat_Game::RE;
                    } else game.hand[game.current_hand].kontrare = game.SINGLE;
                }

                game.calculate_hand_score(game.current_hand);
                game.calculate_game_score();
                game.print_game_status();
                if (bock > 0) { 
                    cout << "This hand was a Bockrund. " << endl;
                }
                cout << "Was this hand scored correctly? " << endl;
                if (yes()) {
                    h = game.calculate_new_bocks(bock, game.current_hand);
                    if (h > 0) {
                        cout << "This hand created new bocks." << endl;
                        bock += h;
                    }
                    /*  /// 
                        The code below adjusts # of bocks to the # of hands remaining
                        It may be more informative to show all bocks, even beyond end of game,
                        in case players want to extend the game
                        if (game.number_of_hands - game.current_hand - 1 > bock) h = bock;
                        else h = game.number_of_hands - game.current_hand - 1;
                        cout << "There are " << h << " Bockrunds remaining." << endl;
                    */
                   if (game.current_hand < game.number_of_hands - 1) {
                        state = NEW_HAND;
                        if (bock > 0) cout << "There are " << --bock << " Bocks remaining." << endl;
                        game.current_hand++;
                    }
                    else state = END_GAME;
                } 
                else state = SCORE_HAND;
                break;
            
            case END_GAME:
                number_of_ties = 0;
                cout << "All hands have been played." << endl;
                cout << "Are there any hand scores that need to be corrected? " << endl;
                if (yes()) state = EDIT_GAME;
                else {
                    cout << "Congratulations -- game has completed." << endl;
                    state = GAME_COMPLETED;
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
                            if (tie_game[i] == 1) {
                                cout << game.player_name[i];
                                if (--number_of_ties > 0) cout << " and ";
                            }
                        }
                        cout << " won." << endl;
                    } else {
                        cout << "Player " << game.player_name[winning_player] << " won." << endl;
                    }
                }
                break;
            
            case EDIT_GAME:
                // This only lets you edit scores for selected hands
                // It does not change any other setting for that hand (can't change declarer or contract, etc)
                // It does not re-calculate the hand score (although game total score will be recalculated)
                // A set of three asterisks will be printed at the end of the line of any edited score
                cout << "Which hand # needs to be udpated? " << endl;
                h = input_and_validate(1, game.number_of_hands) - 1;
                for (int i = 0; i < game.number_of_players; i++) {
                    cout << game.player_name[i] << ": " << game.hand[h].score[i] << " Correct (y/n)" << endl;
                    if (!yes()) {
                        cout << "Enter corrected score: " << endl;
                        game.hand[h].score[i] = input_and_validate(-3000, 3000);
                        game.hand[h].edited = 1;
                    }
                }
                game.calculate_game_score();
                game.print_game_status();
                cout << "Any more hands to update? " << endl;
                if (yes()) state = EDIT_GAME;
                else state = END_GAME;
                break;

            default:  /// Need better error handling
                cout << "Invalid state reached. Exit." << endl;
                return 1;
                break;
        }
    }
    return 0;
}

int yes() {
    int result = 0;
    char c;

    cin >> c;
    switch (c) {
        case 'Y':
        case 'y':
          result = 1;
          break;
        
        case 'N':
        case 'n': 
          break;

        default:
          cout << "Invalid response. Assuming NO." << endl;
          break;
    } 

    return result;
}

int input_and_validate(int min, int max) {
    int i;
    cin >> i;
    if (cin.fail()) {
        i = min;
        cout << "Invalid input, setting to " << min << endl;
        cin.clear(); // Clear the error flags
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input

    }
    if (i < min) i = min;
    if (i > max) i = max;

    return i;
}
