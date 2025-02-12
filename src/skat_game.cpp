/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code

*/

#include "skat_game.h"
#include <cstring>

using namespace std;

Skat_Game::Skat_Game( const char *  p1,  const char *  p2,  const char *  p3)
{
   number_of_players = 3;        /// Hardcoded for initial version
   strcpy(player_name[0], p1);   /// Hardcoded for initial version
   strcpy(player_name[1], p2);   /// Hardcoded for initial version
   strcpy(player_name[2], p3);   /// Hardcoded for initial version
   number_of_hands = 36;
   current_hand = 0;
}

void Skat_Game::calculate_hand_score(int hand) {

}

void Skat_Game::calculate_game_score() {

}

void Skat_Game::print_game_status() {

}