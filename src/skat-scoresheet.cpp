/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code
   1.0.0 21-Feb-2025 Andy4495 Version 1.0
   1.1.0 02-Jun-2025 Andy4495 Version 1.1

*/

#include "version.h"
#include "skat_game.h"
#include <iostream>
#include <string.h>
#include <limits>

enum CRMBE  {CONTINUEGAME, RESCOREHAND, MANUALSCORE, NEWBOCK, ENDTHEGAME};
enum State {INIT, NEW_HAND_BID, HAND_CONTRACT, HAND_SUMMARY, HAND_THROWN,
            SCORE_HAND, MANUALLY_SCORE_HAND, END_GAME, EDIT_GAME, 
            PRINT_GAME_STATUS, GAME_COMPLETED};
enum YESNOTYPE {YESNO, GRANDRAMSCH};

int   yes(YESNOTYPE t = YESNO);
CRMBE crmbe();
int   input_and_validate(int min, int max);
void  score_ramsch(int h);
void  manually_score_hand(int h);
void  list_player_names_this_hand(int h);


State       state = INIT;
Skat_Game   game;
int         winning_player;
int         tie_game[4]; 
int         number_of_ties;

using namespace std;

int main(int argc, char** argv) {
    string name;
    int h;
    int i;  // For use in for loops
    CRMBE crmbe_result;

    while (state != GAME_COMPLETED) {
        switch (state) {
            case INIT:
                cout << "Welcome to the Skat Scoresheet " << VERSION << "!" << endl;
                cout << "The scoresheet supports up to " << MAX_NUMBER_OF_HANDS <<" hands in a game" << endl;
                cout << "Will this be a Ramsch-only game? (y/n) " << endl;
                if (yes()) {
                    game.ramschonly = true;
                }
                else {
                    game.ramschonly = false;
                    game.ramsch_count = 0;
                }
                cout << "Enter number of players (3 or 4): " << endl;
                game.number_of_players = input_and_validate(3, 4);
                cout << "Enter Player 1 name (this is the first dealer): " << endl;
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
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any extra characters
                if (game.number_of_players > 3) {
                    cout << "Enter Player 4 name: " << endl;
                    cin >> name;
                    strncpy(game.player_name[3], name.c_str(), MAX_NAME_SIZE);
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any extra characters
                }
                // Set up House Rules
                cout << "Configure house rules for this game." << endl;
                if (game.ramschonly == true) {
                    cout << "Is a bid of Grand Hand allowed? (y/n) " << endl;
                    if (yes()) game.rules.grand_hand_during_ramschround = true;
                    else       game.rules.grand_hand_during_ramschround = false;
                } else {  // Regular game
                    cout << "Are Kontra and Rekontra allowed? (y/n) " << endl;
                    if (yes()) game.rules.kontrare_allowed = true;
                    else       game.rules.kontrare_allowed = false;
                    cout << "Play a Ramsch hand if nobody bids 18? (y/n) " << endl;
                    if (yes()) game.rules.nobody_bids_18_play_ramsch = true;
                    else       game.rules.nobody_bids_18_play_ramsch = false;
                    cout << "Play a Ramsch round after a Bock round? (y/n) " << endl;
                    if (yes()) {
                        game.rules.ramschround_after_bockround = true;
                        cout << "Allow Grand Hand bid during Ramsch round? (y/n) " << endl;
                        if (yes()) game.rules.grand_hand_during_ramschround = true;
                        else       game.rules.grand_hand_during_ramschround = false;
                    } else { // No Ramsch round after Bock round
                        game.rules.ramschround_after_bockround = false;
                        game.rules.grand_hand_during_ramschround = false;
                    }
                    cout << "Create Bock round for 60/60 tie? (y/n) " << endl;
                    if (yes()) game.rules.bockround_for_60_60_tie = true;
                    else       game.rules.bockround_for_60_60_tie = false;
                    cout << "Create Bock round for 120 raw hand score? (y/n) " << endl;
                    if (yes()) game.rules.bockround_for_120_hand_score = true;
                    else       game.rules.bockround_for_120_hand_score = false;
                    cout << "Create Bock round for lost Kontra/Rekontra? (y/n) " << endl;
                    if (yes()) game.rules.bockround_for_lost_contra_or_rekontra = true;
                    else       game.rules.bockround_for_lost_contra_or_rekontra = false;
                    cout << "Create Bock round for Schneider? (y/n) " << endl;
                    if (yes()) {
                        game.rules.bockround_for_schneider = true;
                        cout << "Only have Schneider create Bock round if there are currently no Bock? (y/n) " << endl;
                        if (yes()) game.rules.bockround_for_schneider_only_if_not_bockround = true;
                        else       game.rules.bockround_for_schneider_only_if_not_bockround = false;
                    } else {
                        game.rules.bockround_for_schneider = false;
                        game.rules.bockround_for_schneider_only_if_not_bockround = false;
                    }  // Next question only applies if there are Bock round rules enabled
                    cout << "Create Bock round if there are two full rounds starting with Player 1 without Bock? (y/n) " << endl;
                    if (yes()) game.rules.bockround_if_two_rounds_without_bock = true;
                    else       game.rules.bockround_if_two_rounds_without_bock = false;
                    if (game.rules.bockround_for_60_60_tie || game.rules.bockround_for_120_hand_score ||
                        game.rules.bockround_for_lost_contra_or_rekontra || game.rules.bockround_for_schneider) {
                            cout << "Limit each hand to create at most one Bock round? (y/n) " << endl;
                            if (yes()) game.rules.bockround_max_one_per_hand = true;
                            else       game.rules.bockround_max_one_per_hand = false;
                    }
                }
                game.number_of_hands = MAX_NUMBER_OF_HANDS;
                cout << "Game will consist of " << game.number_of_players << " players named: " << endl;
                for (i = 0; i < game.number_of_players; i++) cout << game.player_name[i] << endl;
                if (game.ramschonly) {
                    cout << "This game will be all Ramsch." << endl;
                    cout << "with house rule of "; 
                    if (game.rules.grand_hand_during_ramschround) cout << "Grand Hand bid allowed." << endl;
                    else cout << "Grand Hand bid is not allowed." << endl;
                } else {
                    if (game.rules.kontrare_allowed                                ||
                        game.rules.nobody_bids_18_play_ramsch                      ||
                        game.rules.ramschround_after_bockround                     ||
                        game.rules.grand_hand_during_ramschround                   ||
                        game.rules.bockround_for_60_60_tie                         ||
                        game.rules.bockround_for_120_hand_score                    ||
                        game.rules.bockround_for_lost_contra_or_rekontra           ||
                        game.rules.bockround_for_schneider                         ||
                        game.rules.bockround_for_schneider_only_if_not_bockround   ||
                        game.rules.bockround_max_one_per_hand                      ||
                        game.rules.bockround_can_be_created_in_grand_during_ramsch ||
                        game.rules.bockround_if_two_rounds_without_bock   ) {
                            cout << "This will be a regular game with house rules of: " << endl;
                            if (game.rules.kontrare_allowed) {
                                cout << "Kontra and Rekontra are allowed." << endl;
                            }
                            if (game.rules.nobody_bids_18_play_ramsch) {
                                cout << "Play a Ramsch hand if nobody bids 18." << endl;
                            }
                            if (game.rules.ramschround_after_bockround) {
                                cout << "Ramsch round is played after a Bock round." << endl;
                                if (game.rules.grand_hand_during_ramschround) {
                                    cout << "  - Grand Hand bid allowed during Ramsch Round." << endl;
                                } else cout << "  - Grand Hand bid is not allowed during Ramsch Round." << endl;
                            }
                            if (game.rules.bockround_for_60_60_tie) cout << "Bock round created for 60/60 card points tie." << endl;
                            if (game.rules.bockround_for_120_hand_score) cout << "Bock round created for 120 raw hand score." << endl;
                            if (game.rules.bockround_for_lost_contra_or_rekontra) cout << "Bock round created for lost Kontra/Rekontra." << endl;
                            if (game.rules.bockround_for_schneider) {
                                cout << "Bock round created for Schneider"; 
                                if (game.rules.bockround_for_schneider_only_if_not_bockround) cout << ", but only if no Bocks currently." << endl;
                                else cout << "." << endl;
                            }
                            if (game.rules.bockround_if_two_rounds_without_bock) 
                            cout << "Bock round created if there are two full rounds without Bock." << endl;
                            if (game.rules.bockround_max_one_per_hand) cout << "Maximum one Bock round created per hand." << endl;
                    } else {
                        cout << "This will be a regular game with no configured house rules." << endl;
                    }
                }
                cout << "Is this correct? (y/n) " << endl;
                if (yes()) state = NEW_HAND_BID;
                else state = INIT;
                for (i = 0; i < game.number_of_players; i++ ) tie_game[i] = -1;
                break;

            case NEW_HAND_BID:
                game.hand[game.current_hand].grand_during_ramsch = false;
                cout << "======================";
                if (game.current_hand + 1 > 9) cout << "=";
                cout << endl;
                cout << "Starting hand number " << game.current_hand + 1 << endl;
                cout << "======================";
                if (game.current_hand + 1 > 9) cout << "=";
                cout << endl;
                if (game.ramschonly || game.ramsch_count) {
                    cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer" << endl;
                    if (game.rules.grand_hand_during_ramschround) {
                        cout << "Will this be Ramsch or Grand Hand? (r/g) " << endl;
                        if (yes(GRANDRAMSCH)) {  // Grand Hand during Ramsch
                            game.hand[game.current_hand].contract = Skat_Game::GRAND;
                            game.hand[game.current_hand].multipliers = Skat_Game::HAND;
                            game.hand[game.current_hand].grand_during_ramsch = true;
                            cout << "Who is the declarer? " << endl;
                            list_player_names_this_hand(game.current_hand);
                            game.hand[game.current_hand].declarer = input_and_validate(1, game.number_of_players) - 1;
                            game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                            state = HAND_SUMMARY;
                        } else {  // Play Ramsch
                            game.hand[game.current_hand].bid = 0;
                            game.hand[game.current_hand].contract = Skat_Game::RAMSCH;
                            game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                            state = HAND_SUMMARY;    
                        }
                    } else {  // Play Ramsch
                            game.hand[game.current_hand].bid = 0;
                            game.hand[game.current_hand].contract = Skat_Game::RAMSCH;
                            game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                            state = HAND_SUMMARY;    
                    }
                } else {  // Not an automatic Ramsch hand
                    if (game.bock_count > 0) {
                        cout << "This hand is Bock. Scores double." << endl;
                        game.hand[game.current_hand].bock = Skat_Game::BOCK;
                    } else game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                    // In 4 player games, geben-hoeren-sagen-weitersagen still applies: 
                    // - Player to left of dealer is "hoeren" and player to right of dealer is "weitersagen" ("continue saying")
                    // - weitersagen continues bid after either gaben or hoeren passes
                    cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer" << endl;
                    cout << game.player_name[(game.current_hand + 1) % game.number_of_players] << " listens and will lead" << endl;
                    cout << game.player_name[(game.current_hand + 2) % game.number_of_players] << " speaks"  << endl;
                    if (game.number_of_players == 4) {
                        cout << game.player_name[(game.current_hand + 3) % game.number_of_players] << " continues"  << endl;
                    }
                    if (game.rules.nobody_bids_18_play_ramsch) {
                        cout << "Enter winning bid (0 for Ramsch): " << endl; 
                    } else {
                        cout << "Enter winning bid (0 if nobody bids 18): " << endl; 
                    }
                    game.hand[game.current_hand].bid = input_and_validate(0, 264);
                    if (game.hand[game.current_hand].bid == 0) {
                        if (game.rules.nobody_bids_18_play_ramsch) {
                            game.hand[game.current_hand].contract = Skat_Game::RAMSCH;
                            state = HAND_SUMMARY;
                        } else {
                            state = HAND_THROWN;
                        }
                    } else {
                        state = HAND_CONTRACT;
                    }
                }
                break;
            
            case HAND_CONTRACT:
                cout << "Who is the declarer? " << endl;
                list_player_names_this_hand(game.current_hand);
                game.hand[game.current_hand].declarer = input_and_validate(1, game.number_of_players) - 1;
                game.set_contract(game.current_hand);
                if (game.hand[game.current_hand].contract == Skat_Game::Skat_Game::NULLL) {
                    if (game.hand[game.current_hand].bid <= game.SCORE_NULL_PLAIN) {
                        cout << "Will it be played Hand? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::HAND;
                        }
                        cout << "Will it be played Open? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::OPEN;
                        }
                        state = HAND_SUMMARY;
                        break;
                    }
                    if (game.hand[game.current_hand].bid <= game.SCORE_NULL_HAND) {
                        cout << "Will it be played Hand? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::HAND;
                        }
                        cout << "Will it be played Open? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::OPEN;
                        }
                        // Bid greater than SCORE_NULL_PLAIN to SCORE_NULL_HAND needs to be Hand or Open
                        if ( (game.hand[game.current_hand].multipliers & Skat_Game::HAND) ||
                             (game.hand[game.current_hand].multipliers & Skat_Game::OPEN) ) {
                                state = HAND_SUMMARY;
                                break;
                        } else {
                            cout << "Overbid. Need to play Hand or Open with Null bid of "
                                 << game.hand[game.current_hand].bid 
                                 << ".  Re-enter bid and contract." << endl;
                            state = NEW_HAND_BID;
                            break;
                        }
                    }
                    if (game.hand[game.current_hand].bid <= game.SCORE_NULL_OPEN) {
                        cout << "Null bid of " << game.hand[game.current_hand].bid
                             << " must be played Open." << endl;
                        game.hand[game.current_hand].multipliers |= Skat_Game::OPEN;
                        cout << "Will it be played Hand? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers |= Skat_Game::HAND;
                        }
                        state = HAND_SUMMARY;
                        break;
                    }
                    if (game.hand[game.current_hand].bid <= game.SCORE_NULL_HAND_OPEN) {
                        cout << "Null bid of " << game.hand[game.current_hand].bid
                             << " must be played Hand and Open." << endl;
                        game.hand[game.current_hand].multipliers |= Skat_Game::HAND;
                        game.hand[game.current_hand].multipliers |= Skat_Game::OPEN;
                        state = HAND_SUMMARY;
                        break;
                    }
                    if (game.hand[game.current_hand].bid > game.SCORE_NULL_HAND_OPEN) {
                        cout << "Cannot play Null with a bid > " << game.SCORE_NULL_HAND_OPEN 
                             << ". Re-enter bid and contract." << endl; 
                        state = NEW_HAND_BID;
                        break;
                    }
                } else { // Suit or Grand
                    cout << "Will it be played Hand? (y/n) " << endl;
                    if (yes()) game.hand[game.current_hand].multipliers = Skat_Game::HAND;
                    // If hand, then can play open. 
                    if (game.hand[game.current_hand].multipliers & Skat_Game::HAND) {
                        cout << "Will it be played Open -- must take all tricks? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers = 
                            (Skat_Game::HAND | Skat_Game::OPEN | Skat_Game::SCHNEIDER |  
                            Skat_Game::SCHN_ANNC | Skat_Game::SCHW_ANNC);
                            cout << "Playing Open implies announcing Schneider and Schwarz." << endl;
                        } else {
                            // If hand but not open, then can announce Schneider or Schwarz
                            cout << "Announce Schneider? (y/n) " << endl;
                            if (yes()) {
                                game.hand[game.current_hand].multipliers |= Skat_Game::SCHN_ANNC;
                                cout << "Announce Schwarz? (y/n) " << endl; 
                                if (yes()) game.hand[game.current_hand].multipliers |= Skat_Game::SCHW_ANNC;
                            }
                        }
                    }
                }
                state = HAND_SUMMARY;
                break;             

            case HAND_SUMMARY:
                cout << "Hand summary: " << endl; 
                // Grand Hand during Ramsch
                if (game.hand[game.current_hand].grand_during_ramsch) {
                    cout << game.player_name[game.hand[game.current_hand].declarer] 
                        << " plays Grand Hand during Ramsch." << endl;
                } else if (game.hand[game.current_hand].contract == Skat_Game::RAMSCH) {
                        cout << "This hand will be played Ramsch." << endl;
                } else { // Normal hand
                    cout << game.player_name[game.hand[game.current_hand].declarer]
                         << " plays " << game.get_contract_name(game.hand[game.current_hand].contract) << " ";
                    if (game.hand[game.current_hand].multipliers & Skat_Game::HAND) cout << "Hand ";
                    if (game.hand[game.current_hand].multipliers & Skat_Game::OPEN) cout << "Open ";
                    if (game.hand[game.current_hand].multipliers & Skat_Game::SCHN_ANNC) cout << "announce Schneider ";
                    if (game.hand[game.current_hand].multipliers & Skat_Game::SCHW_ANNC) cout << "announce Schwarz ";
                    cout << "on a bid of " << game.hand[game.current_hand].bid << endl;
                }
                cout << "Is this correct? (y/n) " << endl;
                if (yes()) {
                    if (game.hand[game.current_hand].grand_during_ramsch) {
                        cout << game.player_name[game.hand[game.current_hand].declarer] << " leads." << endl;
                    } else {
                        cout << ">>> " << game.player_name[(game.current_hand + 1) % game.number_of_players] 
                             << " leads. <<<" << endl;
                    }
                    state = SCORE_HAND;
                } else state = NEW_HAND_BID;
                break;

            case HAND_THROWN:
                cout << "Nobody bid 18. Hand will be thrown out (everyone scores zero) and next player deals." << endl;
                cout << "Is this correct? (y/n) " << endl;
                if (yes()) {
                    /// Update state variables as needed
                    state = PRINT_GAME_STATUS;
                } else {
                    state = NEW_HAND_BID;
                }
                break;

            case SCORE_HAND:
                cout << "Scoring the hand after play..." << endl;
                if (game.hand[game.current_hand].contract == game.RAMSCH) {
                    score_ramsch(game.current_hand);
                } else { // not Ramsch
                    if (game.hand[game.current_hand].contract == Skat_Game::NULLL) {
                        cout << "Did " << game.player_name[game.hand[game.current_hand].declarer] 
                             << " win the Null? (y/n) " << endl;
                        if (yes()) game.hand[game.current_hand].winlose = Skat_Game::WIN;
                        else game.hand[game.current_hand].winlose = Skat_Game::LOSE;
                        game.hand[game.current_hand].cardpoints = 0;
                    } else {
                        cout << "How many card points did " << game.player_name[game.hand[game.current_hand].declarer] 
                             << " collect? " << endl;
                        game.hand[game.current_hand].cardpoints = input_and_validate(0, 120);
                        game.hand[game.current_hand].takealltricks = false;
                        if (game.hand[game.current_hand].cardpoints == 120) {
                            cout << "Did "<< game.player_name[game.hand[game.current_hand].declarer] 
                                 << " take all the tricks? " << endl;
                            // Can only be Schwarz if you took all the tricks
                            if (yes()) game.hand[game.current_hand].takealltricks = true;
                        }
                        cout << "With or without how many? " << endl;
                        game.hand[game.current_hand].matadors = input_and_validate(1, 8);
                        game.calculate_win_lose(game.current_hand);
                        if (game.hand[game.current_hand].winlose == Skat_Game::WIN) {
                            cout << "+++ " << game.player_name[game.hand[game.current_hand].declarer]; 
                            cout << " won the hand. +++" << endl;
                        } else {
                            cout << "--- " << game.player_name[game.hand[game.current_hand].declarer]; 
                            cout << " lost the hand. ---" << endl;
                        }
                    }
                    game.hand[game.current_hand].kontrare = game.SINGLE;
                    if (game.rules.kontrare_allowed && !game.hand[game.current_hand].grand_during_ramsch) {
                        cout << "Was there a Kontra? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].kontrare = Skat_Game::KONTRA;
                            cout << "Did declarer Rekontra? (y/n) " << endl;  // Only ask about Re if there was a Kontra
                            if (yes()) game.hand[game.current_hand].kontrare = Skat_Game::RE;
                        }
                    }
                }
                game.calculate_hand_score(game.current_hand);
                state = PRINT_GAME_STATUS;
                break;

            case PRINT_GAME_STATUS:
                game.calculate_game_score();
                game.print_game_status();
                if ((game.bock_count > 0) && (game.ramsch_count == 0)) { 
                    cout << "This hand was Bock. " << endl;
                }
                cout << "Enter 'c' to continue game, 'r' to re-score the hand, 'm' to manually enter hand info, 'b' to add a Bock round and continue, or 'e' to end game (c/r/m/b/e): " << endl;
                crmbe_result = crmbe();
                switch (crmbe_result) {
                    case CONTINUEGAME:
                    case NEWBOCK:
                        if (game.current_hand < game.number_of_hands - 1) {
                                state = NEW_HAND_BID;
                                if (game.ramsch_count > 0) {  // Check if this is a ramschround hand
                                    if (--game.ramsch_count == 0) {
                                        cout << "End of Ramsch round." << endl;
                                    } else {
                                            cout << "There ";
                                            if (game.ramsch_count == 1) {
                                                cout << "is 1 Ramsch hand ";
                                            } else {
                                                cout << "are " << game.ramsch_count << " Ramsch hands ";
                                            }
                                            cout << "remaining." << endl;
                                    }
                                } else {  // Not a ramschround hand
                                    // Track the "no Bock" hands, starting with Player 1
                                    if (game.bock_count == 0) {
                                        if ((game.current_hand % game.number_of_players == 0) && (game.no_bock_count == 0)) {
                                            game.no_bock_count = 1;
                                        }
                                        else game.no_bock_count++;
                                    }
                                    h = game.calculate_new_bocks(game.current_hand);
                                    if (game.bock_count > 0) game.bock_count--;
                                    // If we completed a bockround, then start a ramschround
                                    if ( (game.hand[game.current_hand].bock == Skat_Game::BOCK) &&
                                        (game.bock_count % game.number_of_players == 0) &&
                                        (game.rules.ramschround_after_bockround)) {
                                            cout << "End of a Bock round. Next hand starts a Ramsch round." << endl;
                                            game.ramsch_count = game.number_of_players;
                                    }
                                    if (h > 0) {
                                        cout << "This hand created new Bocks." << endl;
                                        game.no_bock_count = 0;
                                        game.bock_count += h;
                                    }
                                    if (game.bock_count > 0) {
                                        cout << "There ";
                                        if (game.bock_count == 1) {
                                            cout << "is 1 Bock ";
                                        } else {
                                            cout << "are " << game.bock_count << " Bocks ";
                                        }
                                        cout << "remaining." << endl;
                                    }
                                }
                                game.current_hand++;
                        } else { // We have exceeded MAX_NUMBER_OF_HANDS
                            state = END_GAME;
                        }
                        if (crmbe_result == NEWBOCK) {
                            cout << "Manually added a Bock round." << endl;
                            game.bock_count += game.number_of_players;
                            game.no_bock_count = 0;
                        }
                        break;

                    case MANUALSCORE:
                        state = MANUALLY_SCORE_HAND;
                        break;
                        
                    case ENDTHEGAME:
                        state = END_GAME;
                        break;

                    case RESCOREHAND:
                        state = SCORE_HAND;
                        break;
                }
                break;
            
            case END_GAME:
                number_of_ties = 0;
                cout << "***************************" << endl;
                cout << "All hands have been played." << endl;
                cout << "Are there any hand scores that need to be corrected? (y/n) " << endl;
                if (yes()) state = EDIT_GAME;
                else {
                    cout << "Congratulations -- game has completed." << endl;
                    state = GAME_COMPLETED;
                    cout << "Final Scoring..." << endl;
                    for (i = 0; i < game.number_of_players; i++) {
                        cout << game.player_name[i] << ": " << game.total_score[i] << endl;
                    }
                    if (game.total_score[0] > game.total_score[1]) winning_player = 0;
                    else winning_player = 1;
                    if (game.total_score[winning_player] < game.total_score[2]) winning_player = 2; 
                    if (game.number_of_players == 4) {
                        if (game.total_score[winning_player] < game.total_score[3]) winning_player = 3;
                    }
                    // Check for ties
                    for (i = 0; i < game.number_of_players; i++) {
                        if (game.total_score[i] == game.total_score[winning_player]) tie_game[i] = 1;
                    }
                    for (i = 0; i < game.number_of_players; i++) {
                        if (tie_game[i] == 1) number_of_ties++;
                    }
                    if (number_of_ties > 1) {
                        cout << "There was a tie for first place. " << endl;
                        cout << "Players ";
                        for (i = 0; i < game.number_of_players; i++) {
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
                // This only lets you edit player scores for the selected hands
                // It does not change any other setting for that hand (can't change declarer or contract, etc)
                // It does not re-calculate the hand score (although game total score will be recalculated)
                // A set of three asterisks will be printed at the end of the line of any edited score
                cout << "Which hand # needs to be udpated? " << endl;
                h = input_and_validate(1, game.number_of_hands) - 1;
                for (i = 0; i < game.number_of_players; i++) {
                    cout << "Is " << game.player_name[i] << " score of " << game.hand[h].score[i] << " correct? (y/n)" << endl;
                    if (!yes()) {
                        cout << "Enter corrected score: " << endl;
                        game.hand[h].score[i] = input_and_validate(-3000, 3000);
                        game.hand[h].edited = 1;
                    }
                }
                game.calculate_game_score();
                game.print_game_status();
                cout << "Any more hands to update? (y/n) " << endl;
                if (yes()) state = EDIT_GAME;
                else state = END_GAME;
                break;

            case MANUALLY_SCORE_HAND:
                // Edit every hand data value manually; nothing is computed
                // Also edit bockround and ramschround counts
                manually_score_hand(game.current_hand);
                state = PRINT_GAME_STATUS;
                break;

            default:  /// Need better error handling
                cout << "Invalid state reached. Exit." << endl;
                return 1;
                break;
        }
    }
    return 0;
}

int yes(YESNOTYPE t) {
    // result of 1 indicates the first option
    // 1 -> Yes for Yes/No
    // 1 -> Grand for Grand/Ramsch
    int result = 0;
    char c;
    bool valid = false;

    while (valid == false) {
        cin >> c;
        cin.clear(); // Clear the error flags
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input

        if (t == YESNO) {
            switch (c) {
                case 'Y':
                case 'y':
                    result = 1;
                    valid = true;
                    break;
                
                case 'N':
                case 'n': 
                    valid = true;
                    break;
        
                default:
                    cout << "Invalid response. Enter y or n: " << endl;
                    break;
            } 
        } else {  // GRANDRAMSCH
            switch (c) {
                case 'G':
                case 'g': 
                    result = 1;
                    valid = true;
                    break;
                    
                case 'R':
                case 'r':
                    valid = true;
                    break;

                default:
                    cout << "Invalid response. Enter r or g: " << endl;
                    break;
            }             
        }
    }

    return result;
}

CRMBE crmbe() {
    CRMBE result;
    char c;
    bool valid = false;

    while (valid == false) {
        cin >> c;
        switch (c) {
            case 'c':
            case 'C':
                result = CONTINUEGAME; 
                valid = true;
                break;

            case 'R':
            case 'r':
                result = RESCOREHAND;
                valid = true;
                break;
            
            case 'm':
            case 'M':
                result = MANUALSCORE;
                valid = true;
                break;

            case 'b':
            case 'B':
                result = NEWBOCK;
                valid = true;
                break;

            case 'e':
            case 'E': 
                result = ENDTHEGAME;
                valid = true;
                break;

            default:
                cout << "Invalid response. Enter c, r, m, b, or e: " << endl;
                break;
        } 
        cin.clear(); // Clear the error flags
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
    }
    return result;
}

int input_and_validate(int min, int max) {
    int i;
    bool valid = false;

    while (valid == false) {
        cin >> i;
        if ( cin.fail() || (i < min) || (i > max) ) {
            cin.clear(); // Clear the error flags
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
            cout << "Invalid input. Enter value from " << min << " to " << max << ": " << endl;
        }
        else valid = true;
    }

    return i;
}

void score_ramsch(int h) {
    int i;

    game.hand[h].ramsch = Skat_Game::PLAIN;
    game.hand[h].number_of_losers = 1;
    cout << "Highest card points collected? " << endl;
    game.hand[h].cardpoints = input_and_validate(40, 120);
    if (game.hand[h].cardpoints == 120) {
        cout << "Did a player take all the tricks? (y/n) " << endl;
        if (yes()) {
            cout << "Durchmarsch" << endl;
            game.hand[h].ramsch = Skat_Game::DURCHMARSCH;
            game.hand[h].winlose = Skat_Game::WIN;
            game.hand[h].number_of_losers = 0;
            cout << "Who won the Durchmarsch? " << endl;
            list_player_names_this_hand(game.current_hand);
            game.hand[h].declarer = input_and_validate(1, game.number_of_players) - 1;
        } else {
            cout << "Was there a Jungf? (y/n) " << endl;
            if (yes()) game.hand[h].ramsch = Skat_Game::JUNGF;    
            cout << "Who lost the Ramsch? " << endl;
            list_player_names_this_hand(game.current_hand);
            game.hand[h].loser[0] = input_and_validate(1, game.number_of_players) - 1;
        }
    }
    // Can only have JUNGF if >= 60 points collected
    if ((game.hand[h].cardpoints > 60) && (game.hand[h].cardpoints < 120)) {
        cout << "Was there a Jungf? (y/n) " << endl;
        if (yes()) game.hand[h].ramsch = Skat_Game::JUNGF;    
        cout << "Who lost the Ramsch? " << endl;
        list_player_names_this_hand(game.current_hand);
        game.hand[h].loser[0] = input_and_validate(1, game.number_of_players) - 1;
    }
    if (game.hand[h].cardpoints == 60) {
        // If there was a JUNGF with 60 points, then there must be two losers with 60 points
        cout << "Was there a Jungf? (y/n) " << endl;
        if (yes()) {
            game.hand[h].ramsch = Skat_Game::JUNGF;
            game.hand[h].number_of_losers = 2;
        } else {
            cout << "How many players lost the Ramsch? (1 or 2) " << endl;
            game.hand[h].number_of_losers = input_and_validate(1, 2);
        }
        cout << "Who lost the Ramsch? " << endl;
        list_player_names_this_hand(game.current_hand);
        game.hand[h].loser[0] = input_and_validate(1, game.number_of_players) - 1;
        if (game.hand[h].number_of_losers == 2) {
            cout << "Who else lost the Ramsch? " << endl;
            game.hand[h].loser[1] = input_and_validate(1, game.number_of_players) - 1;
        }
    }
    // Between 41 and 59 points collected. No JUNGF or DURCH. Either 1 or 2 players lost.
    if ((game.hand[h].cardpoints > 40) && (game.hand[h].cardpoints < 60)) {
        cout << "How many players lost the Ramsch? (1 or 2) " << endl;
        game.hand[h].number_of_losers = input_and_validate(1, 2);
        game.hand[h].winlose = Skat_Game::LOSE;
        cout << "Who lost the Ramsch? " << endl;
        list_player_names_this_hand(game.current_hand);
        game.hand[h].loser[0] = input_and_validate(1, game.number_of_players) - 1;
        if (game.hand[h].number_of_losers == 2) {
            cout << "Who else lost the Ramsch? " << endl;
            game.hand[h].loser[1] = input_and_validate(1, game.number_of_players) - 1;
        } 
    }
    // 40 points collected mean that there were 3 losers, each at 40 points
    if (game.hand[h].cardpoints == 40) {
        cout << "40/40/40 Ramch." << endl;
        game.hand[h].winlose = Skat_Game::LOSE;
        game.hand[h].number_of_losers = 3;
        i = 0;
        for (int j = 0; j < 3; j++) {
            if (game.number_of_players == 4) {  // In 4 player game, dealer didn't play
                if (h % game.number_of_players == i) i++;
            }
            game.hand[h].loser[j] = i++;
        }
        game.hand[h].ramsch = Skat_Game::PLAIN;   
    }

} // score_ramsch()

void manually_score_hand(int h) {
    // Edit every hand data value manually; nothing is computed
    // Also edit bockround and ramschround counts
    cout << "This mode allows correction of all the hand data." << endl;
    cout << "However, this requires you to score the hand manually; it is not checked for correctness." << endl;
    cout << "Bock count at beginning of hand (current value: " << game.bock_count << "): " << endl;
    game.bock_count = input_and_validate(0, 99);
    cout << "Ramsch count at beginning of hand (current value: " << game.ramsch_count << "): " << endl;
    game.ramsch_count = input_and_validate(0, 99);
    cout << "What was the bid (enter 0 for Ramch): " << endl;
    game.hand[h].bid = input_and_validate(0, 264);
    if (game.hand[h].bid > 0) {
        game.hand[h].kontrare = Skat_Game::SINGLE;
        game.set_contract(h);  // This methods prompts, validates the imput, and assigns the member value
        cout << "Did " << game.player_name[game.hand[h].declarer] << " win? (y/n) " << endl;
        if (yes()) game.hand[h].winlose = Skat_Game::WIN;
        else game.hand[h].winlose = Skat_Game::LOSE;
        game.hand[h].multipliers = Skat_Game::NO_MULTIPLIERS;
        cout << "Was the hand played HAND? (y/n) " << endl;
        if (yes()) game.hand[h].multipliers = Skat_Game::HAND;
        cout << "Was the hand played OPEN? (y/n) " << endl;
        if (yes()) game.hand[h].multipliers += Skat_Game::OPEN;
        cout << "Was Schneider announced? (y/n) " << endl;
        if (yes()) game.hand[h].multipliers += Skat_Game::SCHN_ANNC;
        cout << "Was Schwarz announced? (y/n) " << endl;
        if (yes()) game.hand[h].multipliers += Skat_Game::SCHW_ANNC;
        cout << "Was the hand won Schneider? (y/n) " << endl;
        if (yes()) game.hand[h].multipliers += Skat_Game::SCHNEIDER;
        cout << "Was the hand won Schwarz? (y/n) " << endl;
        if (yes()) game.hand[h].multipliers += Skat_Game::SCHWARZ;
        cout << "With or without how many? (enter 1 for Ramsch or Null) " << endl;
        game.hand[h].matadors = input_and_validate(1, 8);
        cout << "Enter overbid correction: " << endl;
        game.hand[h].overbid = input_and_validate(0, 9);
        cout << "Enter cardpoints collected: " << endl;
        game.hand[h].cardpoints = input_and_validate(0, 120);
        if (game.rules.kontrare_allowed) {
            cout << "Was there a Kontra? (y/n) " << endl;
            if (yes()) {
                game.hand[h].kontrare = Skat_Game::KONTRA;
                cout << "was there a Rekontra? (y/n) " << endl;
                if (yes()) game.hand[h].kontrare = Skat_Game::RE;
            }
        }
    } else { // Ramsch
        game.hand[h].contract = Skat_Game::RAMSCH;
        game.hand[h].ramsch = Skat_Game::PLAIN;
        cout << "Was there a JungF? (y/n) " << endl;
        if (yes()) {
            game.hand[h].ramsch = Skat_Game::JUNGF;
        } else {
            cout << "Was there a Durchmarsch? (y/n) " << endl;
            if (yes()) game.hand[h].ramsch = Skat_Game::DURCHMARSCH;
        }
    }
    cout << "Was this hand played Bock? (y/n) " << endl;
    if (yes()) game.hand[h].bock = Skat_Game::BOCK;
    else game.hand[h].bock = Skat_Game::NOBOCK;
    for (int i = 0; i < game.number_of_players; i++) {
        cout << "Is " << game.player_name[i] << " hand score of " << game.hand[h].score[i] << " correct? (y/n)" << endl;
        if (!yes()) {
            cout << "Enter corrected hand score: " << endl;
            game.hand[h].score[i] = input_and_validate(-3000, 3000);
        }
    }  

    game.hand[h].edited = 1;

}

void  list_player_names_this_hand(int h) {
    int i;
    // In 4 player games, dealer doesn't play, so don't list the dealer name
    for (i = 0; i < game.number_of_players; i++) {
        if ( (h % game.number_of_players != i) || (game.number_of_players == 3) )
            cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
    }
}
