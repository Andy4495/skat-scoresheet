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
#include <bitset>

using namespace std;

const char* const Skat_Game::contract_name[]={"Clubs", "Spades", "Hearts", "Diamonds", "Null", "Grand", "Ramsch"};

Skat_Game::Skat_Game( const char *  p1,  const char *  p2,  const char *  p3)
{
   number_of_players = 3;        /// Hardcoded for initial version
   strcpy(player_name[0], p1);   /// Hardcoded for initial version
   strcpy(player_name[1], p2);   /// Hardcoded for initial version
   strcpy(player_name[2], p3);   /// Hardcoded for initial version
   number_of_hands = 36;
   current_hand = 0;
}

void Skat_Game::calculate_hand_score(int h) {
   /// Need to add a check for overbid
   if (hand[h].contract == NULLL) {
      if (hand[h].multipliers == OPEN) hand[h].score[hand[h].declarer] = hand[h].kontrare * hand[h].winlose * 46;
      if (hand[h].multipliers == HAND) hand[h].score[hand[h].declarer] = hand[h].kontrare * hand[h].winlose * 35;
      if (hand[h].multipliers == (OPEN | HAND)) hand[h].score[hand[h].declarer] = hand[h].kontrare * hand[h].winlose * 59;
      else hand[h].score[hand[h].declarer] = hand[h].kontrare * hand[h].winlose * 23;
   } else {
      if (hand[h].contract != RAMSCH) {
         bitset<32> m(hand[h].multipliers);
         hand[h].score[hand[h].declarer] =
         hand[h].kontrare * hand[h].winlose * hand[h].contract * 
         (hand[h].matadors + m.count() + 1);
      } else { // RAMSCH
      /// Fill in RAMSCH scoring 
      }
   }
}

void Skat_Game::calculate_game_score() {
   int i, j; 
   for (i = 0; i < number_of_players; i++) total_score[i] = 0;
   for (i = 0; i <= current_hand; i++) {
      for (j=0; j < number_of_players; j++) total_score[j] += hand[i].score[j];
      /// Following is for testing
      /// for (j=0; j < number_of_players; j++) total_score[j] += i + j;
   }
}

void Skat_Game::print_game_status() {
   cout << "|  # | Bid | Contract | HOSAZA | KR | " << setw(8) << player_name[0] << " | "
   << setw(8) << player_name[1] << " | " << setw(8) << player_name[2]; 
   if (number_of_players == 4) cout << " | " << setw(8) << player_name[3];
   cout << " | Bock |" << endl;
   cout << "| -- | --- | -------- | ------ | -- | -------- | -------- | -------- |";
   if (number_of_players == 4) cout << " -------- |";
   cout << " ---- |" << endl;
   for (int i = 0; i <= current_hand; i++ ) {
      cout << "| " << setw(2) << i + 1 << " | " << setw(3) << hand[i].bid << " | ";
      cout << setw(8) << get_contract_name(hand[i].contract);
      cout << " | "; // Multiplers column
      if (hand[i].multipliers & HAND) cout << "H";
      else cout << " ";
      if (hand[i].multipliers & OPEN) cout << "O";
      else cout << " ";
      if (hand[i].multipliers & SCHNEIDER) cout << "S";
      else cout << " ";
      if (hand[i].multipliers & SCHN_ANNC) cout << "A";
      else cout << " ";
      if (hand[i].multipliers & SCHWARZ) cout << "Z";
      else cout << " ";
      if (hand[i].multipliers & SCHW_ANNC) cout << "A";
      else cout << " ";
      cout << " | ";
      // Kontra/Re column
      if (hand[i].kontrare == RE) cout << "KR | ";
      else if (hand[i].kontrare == KONTRA) cout << "K  | ";
      else cout << "   | ";
      // Player score columns
      cout << setw(8) << hand[i].score[0] << " | " 
           << setw(8) << hand[i].score[1] << " | " 
           << setw(8) << hand[i].score[2];
      if (number_of_players == 4) cout << " | " << setw(8) << hand[i].score[3];
      cout << " | "; // Bock column
      if (hand[i].bock == 1) cout << " B  ";
      else cout << "    ";
      cout << " |" << endl;
   }
   cout << "| -- | --- | -------- | ------ | -- | -------- | -------- | -------- |";
   if (number_of_players == 4) cout << " -------- |";
   cout << " ---- |" << endl;
   cout << "                              Totals: ";
   cout << setw(8) << total_score[0] << " | " 
   << setw(8) << total_score[1] << " | " 
   << setw(8) << total_score[2];
   if (number_of_players == 4) cout << " | " << setw(8) << total_score[3];
   cout << endl;
}

const char* const Skat_Game::get_contract_name(int h) {
   int index;

   switch(h) {
      case CLUBS:
        index = 0;
        break;
      case SPADES:
        index = 1;
        break;
      case HEARTS: 
        index = 2;
        break;
      case DIAMONDS:
        index = 3;
        break;
      case NULLL: 
        index = 4;
        break;
      case GRAND: 
        index = 5;
        break;
      case RAMSCH:
        index = 6;
        break;
      default:
        cout << "Invalid contract type.";
        index = 0;
        break;
   }
   return(contract_name[index]);
}