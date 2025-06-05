/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code
   1.0.0 21-Feb-2025 Andy4495 Version 1.0
   1.1.0 02-Jun-2025 Andy4495 Version 1.1

*/

#include "skat_game.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <bitset>

using namespace std;

const char* const Skat_Game::contract_name[]={"Clubs", "Spades", "Hearts", "Diamonds", "Null", "Grand", "Ramsch", "Thrown"};

Skat_Game::Skat_Game()
{
   current_hand   = 0;
   ramsch_count   = 0;
   bock_count     = 0;
   no_bock_count  = 0;
   rules.nobody_bids_18_play_ramsch                      = false;
   rules.ramschround_after_bockround                     = false;
   rules.grand_hand_during_ramschround                   = false;
   rules.bockround_for_60_60_tie                         = false;
   rules.bockround_for_120_hand_score                    = false;
   rules.bockround_for_lost_contra_or_rekontra           = false;
   rules.bockround_for_schneider                         = false;
   rules.bockround_for_schneider_only_if_not_bockround   = false;
   rules.bockround_max_one_per_hand                      = false;
   rules.bockround_can_be_created_in_grand_during_ramsch = false;
   rules.bockround_if_two_rounds_without_bock            = false;

}

void Skat_Game::calculate_hand_score(int h) {
   // Calculate Null contract score
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
      // Calculate Suit and Grand contracts score
      if (hand[h].contract != RAMSCH) { 
         hand[h].score[hand[h].declarer] = calculate_suit_grand_score(h) * hand[h].winlose * hand[h].bock * hand[h].kontrare;
      } else { 
         // Calculate Ramsch contract score
         for (int i = 0; i < number_of_players; i++) hand[h].score[i] = 0;
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
   // This returns a value before Kontra/Re, lose, and bock multipliers
   bitset<32> m(hand[h].multipliers);

   return hand[h].contract * (hand[h].matadors + hand[h].overbid + m.count() + 1);
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
   if (number_of_players == 4) cout  << " | " << setw(8) << player_name[3];
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
      // Multipliers field (Hand, Open, Schneider, Announce, Schwarz, Announce)
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
      if (hand[i].bock == BOCK) cout << " B  ";
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
   cout << "                                  Totals: ";
   cout << setw(8) << total_score[0] << " | " 
        << setw(8) << total_score[1] << " | " 
        << setw(8) << total_score[2];
   if (number_of_players == 4) cout  << " | " << setw(8) << total_score[3];
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
      case THROWN:
        index = 7;
        break;
      default:
        cout << "Invalid contract type.";
        index = 0;
        break;
   }
   return(contract_name[index]);
}

int Skat_Game::calculate_new_bocks(int h) {
   // Did this hand create more Bocks?
   int new_bocks = 0;

   // Check if bocks allowed from Grand Hand during Ramsch
   if (!hand[h].grand_during_ramsch || rules.bockround_can_be_created_in_grand_during_ramsch) {

      // Raw points > 120 (before loss/bock/Kontra/Re)
      if ( (hand[h].contract != RAMSCH) && (hand[h].contract != NULLL) && rules.bockround_for_120_hand_score) {
         if ((hand[h].matadors + 1) * hand[h].contract >= 120)
            new_bocks += number_of_players;
      }

      // 60/60 tie
      if ( (hand[h].contract != RAMSCH) && (hand[h].contract != NULLL) && rules.bockround_for_60_60_tie) {
            if (hand[h].cardpoints == 60)
               new_bocks += number_of_players;
      }

      // If bockround for Kontra/Re is enabled, then calculate bockround
      if (rules.bockround_for_lost_contra_or_rekontra) {
         // Any Rekontra --> Either opponents lose Kontra or declarer loses Re, so Re always creates a bock
         if (hand[h].kontrare == RE) {
               new_bocks += number_of_players;
         } else {
            // Successful Kontra (opponents win)
            if ( (hand[h].kontrare == KONTRA) && (hand[h].winlose == LOSE) ) {
                  new_bocks += number_of_players; 
            }
         }
      }

      // Schneider
      if (rules.bockround_for_schneider) {
         if (rules.bockround_for_schneider_only_if_not_bockround) {
            if (bock_count == 0 && (hand[current_hand].multipliers & SCHNEIDER) ) {
               new_bocks += number_of_players;
            }
         } else {  // Bockround created for all Schneiders, even if there is already a Schneider
            if ( (hand[current_hand].multipliers & SCHNEIDER)) {
                  new_bocks += number_of_players;
            }
         }
      }

      // Two rounds without Bock, but only if no bocks were created with this hand
      if (rules.bockround_if_two_rounds_without_bock) {
         if (no_bock_count == number_of_players * 2) {
            if (new_bocks == 0) new_bocks = number_of_players;
         }
      }

      // Check if we created more than one bock round
      if (rules.bockround_max_one_per_hand) {
         if (new_bocks > number_of_players) new_bocks = number_of_players;
      }
   }  // End of if for grand hand during ramsch

   return new_bocks;
}

void Skat_Game::calculate_win_lose(int h) {
   // This function is only useful for suit and grand bids. Ramsch and Null are calculated separately.

   // First, set or clear SCHNEIDER and SCHWARZ bits based on the cardpoints and take-all-tricks flag
   if (hand[h].takealltricks) hand[h].multipliers |= SCHWARZ;
   else hand[h].multipliers &= ~SCHWARZ;
   if (hand[h].cardpoints > 89) hand[h].multipliers |= SCHNEIDER;
   else hand[h].multipliers &= ~SCHNEIDER;

   // Next check if SCHNEIDER or SCHWARZ were announced
   if ((hand[h].multipliers & SCHW_ANNC)) {
      if (hand[h].takealltricks) hand[h].winlose = WIN;
      else {
         hand[h].winlose = LOSE;
         // Losing an announced hand also incurs the Schneider/Schwarz multipliers, regardless of points taken
         hand[h].multipliers |= SCHWARZ;
         hand[h].multipliers |= SCHNEIDER;
      }
   }
   else if ((hand[h].multipliers & SCHN_ANNC)) {
      if (hand[h].cardpoints > 89) hand[h].winlose = WIN;
      else {
         hand[h].winlose = LOSE;
         // Losing an announced hand also incurs the Schneider multiplier, regardless of points taken
         hand[h].multipliers |= SCHNEIDER;
      }
   }

   // Next check if non-announced hand won
   if ( !(hand[h].multipliers & (SCHW_ANNC | SCHN_ANNC)) ) {
      if (hand[h].cardpoints > 60) hand[h].winlose = WIN;
      else hand[h].winlose = LOSE;
   }

   // Finally check for declarer losing Schneider (takes less than 31 points) (unusual, but not unheard of)
   if ( !(hand[h].multipliers & (SCHW_ANNC | SCHN_ANNC)) ) {
      if (hand[h].cardpoints < 31) hand[h].multipliers |= SCHNEIDER;
   }   

   /// There is a final case to check: declarer loses all tricks
   /// This would incur a Schneider and Schwarz multiplier against the declarer
   /// This currently UNIMPLEMENTED, but should be extremely rare

   // Check for overbid
   // Overbid checks the score before Kontra/Re
   hand[h].overbid = 0;
   if (calculate_suit_grand_score(h) < hand[h].bid) {
      cout << "Overbid. Adding multiplier(s) to match bid." << endl;
      while (calculate_suit_grand_score(h) < hand[h].bid) {
         hand[h].overbid++;
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
       case 'z': // kreuZ (last letter abbreviation)
       case 'Z':
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
       case 'd': // Don't have first letter "Karo" or "Caro" because it clashes with Clubs/Kreuz
       case 'D': 
       case 'o':// carO (last letter abbreviation)
       case 'O': 
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