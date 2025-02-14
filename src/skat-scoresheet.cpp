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

int yes(char c);

using namespace std;

enum State {INIT, START_GAME, NEW_HAND, SCORE_HAND, END_GAME, EDIT_GAME, GAME_COMPLETED};

State state = INIT;
Skat_Game game("AT", "RT", "MT");
int winning_player;
int tie_game[4]; 
int number_of_ties;
int play_first_card;

int main(int argc, char** argv) {
    string name;
    char c; 
    int d;

    /// Add support for a Ramsch-only game

    while (state != GAME_COMPLETED) {
        switch (state) {
            case INIT:
                cout << "Welcome to the Skat Scoresheet!" << endl;
                cout << "Enter number of players (3 or 4): " << endl;
                cin >> game.number_of_players;
                if (game.number_of_players > 4) game.number_of_players = 4;
                if (game.number_of_players < 3) game.number_of_players = 3;
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
                cin >> game.number_of_hands;
                if (game.number_of_hands > 36) game.number_of_hands = 36;
                if (game.number_of_hands <  3) game.number_of_hands =  3;
                cout << "Game will consist of " << game.number_of_players << " players named: " << endl;
                for (int i = 0; i < game.number_of_players; i++) cout << game.player_name[i] << endl;
                cout << "Game will end after " << game.number_of_hands << " hands." << endl;
                state = START_GAME;
                break;

            case START_GAME:  /// This state may not be needed. The code so far could be part of INIT.
                for (int i = 0; i < game.number_of_players; i++ ) tie_game[i] = -1;
                number_of_ties = 0;
                state = NEW_HAND;
                break;

            case NEW_HAND:
                cout << "Starting hand number " << game.current_hand +1 << endl;
                /// This may not be correct for 4 player games
                cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer." << endl;
                cout << game.player_name[(game.current_hand + 1) % game.number_of_players] << " listens." << endl;
                cout << game.player_name[(game.current_hand + 2) % game.number_of_players] << " speaks."  << endl;
                cout << "Enter winning bid (enter 0 for Ramsch): " << endl; 
                cin  >> game.hand[game.current_hand].bid;
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
                        cin >> d;
                        if (d < 1) d = 1;
                        if (d > game.number_of_players) d = game.number_of_players;
                        game.hand[game.current_hand].declarer = d - 1;
                        if (game.hand[game.current_hand].contract == Skat_Game::Skat_Game::NULLL) {
                            cout << "Will it be played Hand (Y/N)? " << endl;
                            cin >> c;
                            if (yes(c)) {
                                game.hand[game.current_hand].multipliers |= Skat_Game::HAND;
                            }
                            cout << "Will it be played Open (Y/N)? " << endl;
                            cin >> c;
                            if (yes(c)) {
                                game.hand[game.current_hand].multipliers |= Skat_Game::OPEN;
                            }
                        } else { // Suit or Grand
                            cout << "Will it be played Hand (Y/N)? " << endl;
                            cin >> c; 
                            if (yes(c)) game.hand[game.current_hand].multipliers = Skat_Game::HAND;
                            // If hand, then can play open. 
                             if (game.hand[game.current_hand].multipliers & Skat_Game::HAND) {
                                cout << "Will it be played Open -- must take all tricks (Y/N)? " << endl;
                                cin >> c; 
                                if (yes(c)) {
                                    game.hand[game.current_hand].multipliers = 
                                    (Skat_Game::HAND | Skat_Game::OPEN | Skat_Game::SCHNEIDER |  
                                     Skat_Game::SCHN_ANNC | Skat_Game::SCHWARZ | Skat_Game::SCHW_ANNC);
                                    cout << "Playing Open implies announcing Schneider and Schwarz." << endl;
                                } else {
                                    // If hand but not open, then can announce Schneider or Schwarz
                                    cout << "Announce Schneider? " << endl;
                                    cin >> c;
                                    if (yes(c)) {
                                        game.hand[game.current_hand].multipliers |= 
                                          (Skat_Game::SCHN_ANNC | Skat_Game::SCHNEIDER);
                                        cout << "Announce Schwarz? " << endl; 
                                        cin >> c;
                                        if (yes(c)) game.hand[game.current_hand].multipliers |= 
                                          (Skat_Game::SCHW_ANNC | Skat_Game::SCHWARZ); 
                                    }
                                }

                            }
                        }                    
                    }
                }

                cout << "Hand summary: " << endl; 
                if (game.hand[game.current_hand].contract == game.RAMSCH) {
                    cout << "THis hand will be played Ramsch." << endl;
                } else {
                    cout << game.player_name[game.hand[game.current_hand].declarer]
                         << " plays " << game.get_contract_name(game.hand[game.current_hand].contract);
                    if (game.hand[game.current_hand].multipliers & game.HAND) cout << " HAND";
                    cout << " on a bid of " << game.hand[game.current_hand].bid << endl;
                    if (game.hand[game.current_hand].multipliers & game.SCHN_ANNC) cout << "Schneider announced." << endl;
                    if (game.hand[game.current_hand].multipliers & game.SCHW_ANNC) cout << "Schwarz announced." << endl;
                }
                /// Ask for comformation of game summary before continuing.
                play_first_card = (game.current_hand + 1) % game.number_of_players;
                cout << game.player_name[play_first_card] << " leads." << endl;
                state = SCORE_HAND;
                break;

            case SCORE_HAND:
                cout << "Scoring the hand after play..." << endl;
                if (game.hand[game.current_hand].contract == game.RAMSCH) {
                    cout << "How many players lost the Ramsch?" << endl;
                    /// Assume 1 for now
                    cout << "Who lost?" << endl; /// Print player names and index
                    /// Assume player 1 lost
                    cout << "Was there a Jungfrau?" << endl;
                } else { // Regular hand, not Ramsch
                    cout << "With or without how many? " << endl;
                    cin >> game.hand[game.current_hand].matadors;
                    cout << "Was there a Kontra? " << endl;
                    cin >> c;
                    if (yes(c)) {
                        game.hand[game.current_hand].kontrare = Skat_Game::KONTRA;
                        cout << " Was there a Re? " << endl;  /// Only ask if there was a Kontra
                        cin >> c;
                        if (yes(c)) game.hand[game.current_hand].kontrare = Skat_Game::RE;
                    } else game.hand[game.current_hand].kontrare = game.SINGLE;
                    cout << "Did " << game.player_name[game.hand[game.current_hand].declarer] << " win the hand?" << endl;
                    cin >> c;
                    if (yes(c)) game.hand[game.current_hand].winlose = Skat_Game::WIN;
                    else game.hand[game.current_hand].winlose = Skat_Game::LOSE;
                    cout << "Was it Schneider?" << endl;
                    cin >> c;
                    if (yes(c)) {
                        game.hand[game.current_hand].multipliers |= Skat_Game::SCHNEIDER;
                        cout << "Was it Schwarz?" << endl;
                        cin >> c;
                        if (yes(c)) game.hand[game.current_hand].multipliers |= Skat_Game::SCHWARZ;
                    }
                    /// Add a handler to check if player lost (e.g., called Schneider but didn't)
                    /// Add Bock handler
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
                        if (tie_game[i] == 1) {
                            cout << game.player_name[i];
                            if (--number_of_ties > 0) cout << " and ";
                        }
                    }
                    cout << " won." << endl;
                } else {
                    cout << "Player " << game.player_name[winning_player] << " won." << endl;
                }
                cout << "Congratulations -- game has completed." << endl;
                state = GAME_COMPLETED;
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

int yes(char c) {
    int result = 0;

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