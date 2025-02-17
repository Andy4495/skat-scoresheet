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

Skat_Game::Skat_Game()
{
   current_hand = 0;
}

void Skat_Game::calculate_hand_score(int h) {
   // It is up to the players to declare an overbid on NULL, since it is obvious before the hand starts play
   if (hand[h].contract == NULLL) {
      if (hand[h].multipliers == OPEN) 
        hand[h].score[hand[h].declarer] = hand[h].bock * hand[h].kontrare * hand[h].winlose * 46;
      else if (hand[h].multipliers == HAND) 
        hand[h].score[hand[h].declarer] = hand[h].bock * hand[h].kontrare * hand[h].winlose * 35;
      else if (hand[h].multipliers == (OPEN | HAND)) 
        hand[h].score[hand[h].declarer] = hand[h].bock * hand[h].kontrare * hand[h].winlose * 59;
      else 
        hand[h].score[hand[h].declarer] = hand[h].bock * hand[h].kontrare * hand[h].winlose * 23;
   } else {
      if (hand[h].contract != RAMSCH) {
         hand[h].score[hand[h].declarer] = calculate_suit_grand_score(h) * hand[h].bock * hand[h].kontrare;
      } else { // RAMSCH
         if (hand[h].ramsch == DURCHMARSCH) {
            hand[h].score[hand[h].declarer] = hand[h].bock * 120;
         } else {
            for (int i = 0; i < hand[h].number_of_losers; i++) {
               hand[h].score[hand[h].loser[i]] = hand[h].bock * hand[h].cardpoints * hand[h].ramsch;
            }
         }
      }
   }
}

int Skat_Game::calculate_suit_grand_score(int h) {
   // This returns a value before Kontra/Re and bock multipliers
   bitset<32> m(hand[h].multipliers);

   return  hand[h].winlose * hand[h].contract * (hand[h].matadors + m.count() + 1);
}

void Skat_Game::calculate_game_score() {
   int i, j; 
   for (i = 0; i < number_of_players; i++) total_score[i] = 0;
   for (i = 0; i <= current_hand; i++) {
      for (j=0; j < number_of_players; j++) total_score[j] += hand[i].score[j];
   }
}

void Skat_Game::print_game_status() {
   // Header row
   cout << "|  # | Bid | M | Contract | HOSAZA | KR | " << setw(8) << player_name[0] << " | "
   << setw(8) << player_name[1] << " | " << setw(8) << player_name[2]; 
   if (number_of_players == 4) cout << " | " << setw(8) << player_name[3];
   cout << " | Bock |" << endl;
   // Delimiter row
   cout << "| -- | --- | - | -------- | ------ | -- | -------- | -------- | -------- |";
   if (number_of_players == 4) cout << " -------- |";
   cout << " ---- |" << endl;
   // Hand-by-hand rows
   for (int i = 0; i <= current_hand; i++ ) {
      // Hand # and bid fields
      cout << "| " << setw(2) << i + 1 << " | " << setw(3) << hand[i].bid << " | ";
      // Matadors field
      if (hand[i].contract == NULLL || hand[i].contract == RAMSCH) cout << "  | "; // No matadors for Null or Ramsch
      else cout << hand[i].matadors << " | ";
      // Contract field
      cout << setw(8) << left << get_contract_name(hand[i].contract) << right;
      // Multipliers field (Hand, Open, Schneider, Schwarz, Announce)
      cout << " | ";
      if (hand[i].contract == RAMSCH) {
         if (hand[i].ramsch == JUNGF) cout << "JUNGF ";
         else if (hand[i].ramsch == DURCHMARSCH) cout << "DURCH ";
         else cout << "      ";
      } else {
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
      }
      // Kontra/Re field
      cout << " | ";
      if (hand[i].kontrare == RE) cout << "KR | ";
      else if (hand[i].kontrare == KONTRA) cout << "K  | ";
      else cout << "   | ";
      // Player score fields
      cout << setw(8) << hand[i].score[0] << " | " 
           << setw(8) << hand[i].score[1] << " | " 
           << setw(8) << hand[i].score[2];
      if (number_of_players == 4) cout << " | " << setw(8) << hand[i].score[3];
      // Bock field
      cout << " | ";
      if (hand[i].bock == BOCKRUND) cout << " B  ";
      else cout << "    ";
      cout << " |";
      // Score edited field
      if (hand[i].edited) {
         cout << " ***";
         edited_score_note = 1;
      }
      cout << endl;
   }
   cout << "| -- | --- | - | -------- | ------ | -- | -------- | -------- | -------- |";
   if (number_of_players == 4) cout << " -------- |";
   cout << " ---- |" << endl;
   if (number_of_players == 4) cout << "           ";
   cout << "                                  Totals: ";
   cout << setw(8) << total_score[0] << " | " 
   << setw(8) << total_score[1] << " | " 
   << setw(8) << total_score[2];
   if (number_of_players == 4) cout << " | " << setw(8) << total_score[3];
   cout << endl;
   if (edited_score_note) cout << "*** Indicates that a score was manually edited for that hand." << endl;
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

int Skat_Game::calculate_new_bocks(int b, int h) {
   // Did this hand create more Bocks?
   int new_bocks = 0;

   // - Raw points > 120 (before loss/bock/Kontra/Re)
   if ( (hand[h].contract != RAMSCH) && (hand[h].contract != NULLL) ) {
       if ((hand[h].matadors + 1) * hand[h].contract >= 120)
           new_bocks += 3;
   }

   // - 60/60 tie
   if ( (hand[h].contract != RAMSCH) && (hand[h].contract != NULLL) ) {
       if (hand[h].cardpoints == 60)
           new_bocks += 3;
   }

   // - successful Kontra (opponents win)
   if ( (hand[h].kontrare == KONTRA) && (hand[h].winlose == LOSE) ) {
       new_bocks += 3; 
   }

   // - successful Re (declarer wins) --> Somebody loses in Re, so Re always creates a bock
   if (hand[h].kontrare == RE) {
       new_bocks += 3;
   }

   // - Schneider (if there are currently no Bocks)
   if ( (b == 0) && (hand[current_hand].multipliers & SCHNEIDER) ) {
                           new_bocks += 3;
   }

   // Max 3 bocks generated per hand
   if (new_bocks > 3) new_bocks = 3;

   return new_bocks;
}

void Skat_Game::calculate_win_lose(int h) {
   // This function is only useful for suit and grand bids. Ramsch and Null are calculated separately.
   // Assumes that SCHWARZ bit is set before calling this method if player took all the tricks
   // Score of 120 wins unless SCHWARZ was announced, but player did not take all tricks
   if (hand[h].cardpoints == 120) {
      if (hand[h].multipliers & SCHW_ANNC) {
         if (hand[h].multipliers & SCHWARZ) hand[h].winlose = WIN;
         else hand[h].winlose = LOSE;
      } else {
         hand[h].multipliers |= SCHNEIDER;
         hand[h].winlose = WIN;
      }
   } else { // Check for Schneider
         if (hand[h].cardpoints > 89) {
            hand[h].multipliers |= SCHNEIDER;
            hand[h].winlose = WIN;
         } else { // Not Schneider or Schwarz, check if regular game win
            if (hand[h].cardpoints > 60) {// Between [61 and 89], win unless Schneider Announced
               if (hand[h].multipliers & SCHN_ANNC) hand[h].winlose = LOSE;
               else hand[h].winlose = WIN;
            } else  {  // Declarer lost. Check if delarer was out of Schneider (extra multiple)
               hand[h].winlose = LOSE;
               if (hand[h].cardpoints < 31) hand[h].multipliers |= SCHNEIDER;
            }
         }
   }
   // Check for overbid
   // Overbid checks the score before Kontra/Re
   if ( (hand[h].winlose == WIN) && (calculate_suit_grand_score(h) < hand[h].bid)) {
      cout << "Overbid. Adding a multiplier to match bid." << endl;
      while (calculate_suit_grand_score(h) < hand[h].bid) {
         hand[h].matadors++;
      }
      hand[h].winlose = LOSE;
   }
}

void Skat_Game::set_contract(int h) {
   char c;

   cout << "Enter contract (C, S, H, D, G, N): " << endl; 
   cin >> c;
   switch (c) {
       case 'c':
       case 'C': 
       case 'k': // Kreuz
       case 'K': 
         hand[h].contract = Skat_Game::CLUBS;
         break;
       case 's':
       case 'S': 
       case 'p': // Pik
       case 'P':
         hand[h].contract = Skat_Game::SPADES;
         break;
       case 'h': // Herz
       case 'H': 
         hand[h].contract = Skat_Game::HEARTS;
         break;
       case 'd': // Don't have one-letter "karo" or "caro" because it clashes with Clubs/Kreuz
       case 'D': 
         hand[h].contract = Skat_Game::DIAMONDS;
         break;
       case 'g':
       case 'G': 
         hand[h].contract = Skat_Game::GRAND;
         break;
       case 'n':
       case 'N': 
         hand[h].contract = Skat_Game::NULLL;
         break;
       default:
         cout << "Invalid contract. Defaulting to Clubs." << endl;
         hand[h].contract = Skat_Game::CLUBS;
         break;
   }
}