/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code

*/

#include "skat_game.h"
#include <cstring>
#include <iostream>
#include <iomanip>

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
   cout << "|  # | Bid | Contract | HOSAZA | " << setw(8) << player_name[0] << " | "
   << setw(8) << player_name[1] << " | " << setw(8) << player_name[2]; 
   if (number_of_players == 4) cout << " | " << setw(8) << player_name[3];
   cout << " | Bock |" << endl;
   cout << "| -- | --- | -------- | ------ | -------- | -------- | -------- | ---- |" << endl;
   for (int i = 0; i <= current_hand; i++ ) {
      cout << "| " << setw(2) << i + 1 << " | " << setw(3) << hand[i].bid << " | ";
      switch(hand[i].contract) {
         case CLUBS:
           cout << setw(8) << "Clubs"; 
           break;
         case SPADES:
           cout << setw(8) << "Spades";
           break;
         case HEARTS: 
           cout << setw(8) << "Hearts"; 
           break;
         case DIAMONDS:
           cout << setw(8) << "Diamonds";
           break;
         case NULLL: 
           cout << setw(8) << "Null";
           break;
         case GRAND: 
           cout << setw(8) << "Grand";
           break;
         case RAMSCH:
           cout << setw(8) << "Ramsch";
           break;
         default:
           cout << "Invalid contract type.";
           break;
      }
      cout << " | "; // Multiplers column
      if (hand[current_hand].multipliers & HAND) cout << "H";
      else cout << " ";
      if (hand[current_hand].multipliers & OPEN) cout << "O";
      else cout << " ";
      if (hand[current_hand].multipliers & SCHNEIDER) cout << "S";
      else cout << " ";
      if (hand[current_hand].multipliers & SCHN_ANNC) cout << "A";
      else cout << " ";
      if (hand[current_hand].multipliers & SCHWARZ) cout << "Z";
      else cout << " ";
      if (hand[current_hand].multipliers & SCHWARZ_ANNC) cout << "A";
      else cout << " ";
      cout << " | ";
      // Player score columns
      cout << setw(8) << hand[current_hand].score[0] << " | " 
           << setw(8) << hand[current_hand].score[1] << " | " 
           << setw(8) << hand[current_hand].score[2];
      if (number_of_players == 4) cout << " | " << setw(8) << hand[current_hand].score[3];
      cout << " | "; // Bock column
      if (hand[current_hand].bock == 1) cout << " B  ";
      else cout << "    ";
      cout << " |" << endl;
   }

}