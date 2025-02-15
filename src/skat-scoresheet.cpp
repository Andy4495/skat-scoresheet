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
    char c; 
    int h;

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
                cout << "Enter Player 2 name: " << endl;
                cin >> name;
                strncpy(game.player_name[1], name.c_str(), MAX_NAME_SIZE);
                cout << "Enter Player 3 name: " << endl;
                cin >> name;
                strncpy(game.player_name[2], name.c_str(), MAX_NAME_SIZE);
                if (game.number_of_players > 3) {
                    cout << "Enter Player 4 name: " << endl;
                    cin >> name;
                    strncpy(game.player_name[3], name.c_str(), MAX_NAME_SIZE);
                }
                cout << "How many hands will you be playing in today's game (3 to 36)?" << endl;
                game.number_of_hands = input_and_validate(3, 36);
                cout << "Game will consist of " << game.number_of_players << " players named: " << endl;
                for (int i = 0; i < game.number_of_players; i++) cout << game.player_name[i] << endl;
                cout << "Game will end after " << game.number_of_hands << " hands." << endl;
                cout << "Is this correct? " << endl;
                if (yes()) state = START_GAME;
                else state = INIT;
                break;

            case START_GAME:  /// This state may not be needed. The code so far could be part of INIT.
                for (int i = 0; i < game.number_of_players; i++ ) tie_game[i] = -1;
                state = NEW_HAND;
                break;

            case NEW_HAND:
                cout << "Starting hand number " << game.current_hand +1 << endl;
                /// This may not be correct for 4 player games
                cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer." << endl;
                cout << game.player_name[(game.current_hand + 1) % game.number_of_players] << " listens." << endl;
                cout << game.player_name[(game.current_hand + 2) % game.number_of_players] << " speaks."  << endl;
                cout << "Enter winning bid (enter 0 for Ramsch): " << endl; 
                game.hand[game.current_hand].bid = input_and_validate(0, 168);
                // if Ramsch, then don't need to ask about contract
                if (game.hand[game.current_hand].bid == 0) {  // RAMSCH
                    game.hand[game.current_hand].contract = Skat_Game::RAMSCH;
                }
                else {
                    cout << "Enter contract (C, S, H, D, G, N): " << endl; 
                    cin >> c;
                    switch (c) {
                        case 'c':
                        case 'C': 
                        case 'k': // Kreuz
                        case 'K': 
                          game.hand[game.current_hand].contract = Skat_Game::CLUBS;
                          break;
                        case 's':
                        case 'S': 
                        case 'p': // Pik
                        case 'P':
                          game.hand[game.current_hand].contract = Skat_Game::SPADES;
                          break;
                        case 'h': // Herz
                        case 'H': 
                          game.hand[game.current_hand].contract = Skat_Game::HEARTS;
                          break;
                        case 'd':
                        case 'D': 
                          game.hand[game.current_hand].contract = Skat_Game::DIAMONDS;
                          break;
                        case 'g':
                        case 'G': 
                          game.hand[game.current_hand].contract = Skat_Game::GRAND;
                          break;
                        case 'n':
                        case 'N': 
                          game.hand[game.current_hand].contract = Skat_Game::NULLL;
                          break;
                        default:
                          cout << "Invalid contract. Defaulting to Clubs." << endl;
                          game.hand[game.current_hand].contract = Skat_Game::CLUBS;
                          break;
                    }
                    if (game.hand[game.current_hand].contract != Skat_Game::RAMSCH) {
                        /// Need logic for 4-player game -- dealer should not be listed
                        cout << "Who is the declarer? " << endl; /// Need some logic to make it easy to enter declarer: print name and then the index, just ask for index?
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
                                game.hand[game.current_hand].ramschpoints = input_and_validate(40, 120); 
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
                                game.hand[game.current_hand].ramschpoints = input_and_validate(40, 120); 
                                break;

                            case 3: 
                                cout << "40/40/40 Ramch." << endl;
                                game.hand[game.current_hand].winlose = Skat_Game::LOSE;
                                game.hand[game.current_hand].number_of_losers = 3;
                                game.hand[game.current_hand].ramschpoints = 40;
                                /// Need to add a handler for 4 players
                                for (int i = 0; i < 3; i++) {
                                    game.hand[game.current_hand].loser[i] = i;
                                }
                                break;

                            default:
                                cout << "Invalid number of Ramsch losers." << endl;
                                break;
                        }
                        cout << "Was there a Jungfrau?" << endl;
                        if (yes()) game.hand[game.current_hand].ramsch = Skat_Game::JUNGFRAU;
                        else game.hand[game.current_hand].ramsch = Skat_Game::PLAIN;
                    }
                } else { // not Ramsch
                    cout << "Did " << game.player_name[game.hand[game.current_hand].declarer] << " win the hand? " << endl;
                    if (yes()) game.hand[game.current_hand].winlose = Skat_Game::WIN;
                    else game.hand[game.current_hand].winlose = Skat_Game::LOSE;
                    if (game.hand[game.current_hand].contract == Skat_Game::NULLL){

                    } else {
                        cout << "With or without how many? " << endl;
                        game.hand[game.current_hand].matadors = input_and_validate(1, 8);
                        cout << "Was it Schneider?" << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::SCHNEIDER;
                            cout << "Was it Schwarz?" << endl;
                            if (yes()) game.hand[game.current_hand].multipliers |= Skat_Game::SCHWARZ;
                        }
                    }
                    cout << "Was there a Kontra? " << endl;
                    if (yes()) {
                        game.hand[game.current_hand].kontrare = Skat_Game::KONTRA;
                        cout << " Was there a Re? " << endl;  /// Only ask about Re if there was a Kontra
                        if (yes()) game.hand[game.current_hand].kontrare = Skat_Game::RE;
                    } else game.hand[game.current_hand].kontrare = game.SINGLE;

                    /// Add a handler to check if player lost (e.g., called Schneider but didn't)
                    /// Add Bock handler
                }

                game.calculate_hand_score(game.current_hand);
                game.calculate_game_score();
                game.print_game_status();
                cout << "Was this hand scored correctly? " << endl;
                if (yes()) {
                    if (game.current_hand < game.number_of_hands - 1) {
                        state = NEW_HAND;
                        game.current_hand++;
                    }
                    else state = END_GAME;
                } 
                else state = SCORE_HAND;
                break;
            
            case END_GAME:
                number_of_ties = 0;
                cout << "All hands have been played." << endl;
                cout << "Are there any hands that need to be corrected? " << endl;
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
                cout << "Which hand # needs to be udpated? " << endl;
                h = input_and_validate(1, game.number_of_hands) - 1;
                /// Print the data for this hand one by one and ask for correction
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