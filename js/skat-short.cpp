#include <iostream>
#include <string.h>
#include <limits>
#include <cstring>
#include <iomanip>
#include <bitset>
#define MAX_NAME_SIZE 8
#define MAX_NUMBER_OF_HANDS 99
class Skat_Game {
public:
    int     number_of_players;
    char    player_name[4][MAX_NAME_SIZE + 1];
    int     number_of_hands;
    int     current_hand;
    int     total_score[4];
    int     edited_score_note;
    bool    schieberamsch;
    int     ramsch_count;
    int     bock_count;
    static const char* const contract_name[7];
    enum    Contract_Type {CLUBS = 12, SPADES = 11, HEARTS = 10, DIAMONDS = 9, NULLL = 23, GRAND= 24, RAMSCH = 1};
    enum    Other_Multipliers {NO_MULTIPLIERS = 0, HAND = 1, OPEN = 2, SCHNEIDER = 4,SCHN_ANNC = 8, SCHWARZ = 16, SCHW_ANNC = 32};
    enum    Kontra_Re {SINGLE = 1, KONTRA = 2, RE = 4};
    enum    Ramsch_Special {PLAIN = -1, JUNGF = -2, DURCHMARSCH = 99};
    enum    Win_Lose {LOSE = -2, WIN = 1};
    enum    Bock_Type {NOBOCK = 1, BOCK = 2};
    struct  {
        int         declarer;
        int         bid;
        Contract_Type contract;
        Win_Lose    winlose;
        int         multipliers;
        int         matadors;
        int         overbid;
        int         loser[3];
        int         number_of_losers;
        int         cardpoints;
        Kontra_Re   kontrare;
        Ramsch_Special ramsch;
        Bock_Type   bock;
        int         score[4];
        int         edited;
        bool        grand_during_ramsch;
        bool        takealltricks;
    } hand[MAX_NUMBER_OF_HANDS];
    Skat_Game();
    void    calculate_hand_score(int hand);
    int     calculate_suit_grand_score(int hand);
    void    calculate_game_score();
    void    print_game_status();
    const char* const get_contract_name(int hand);
    int     calculate_new_bocks(int h);
    void    calculate_win_lose(int h);
    void    set_contract(int h);
};
using namespace std;
const char* const Skat_Game::contract_name[]={"Clubs", "Spades", "Hearts", "Diamonds", "Null", "Grand", "Ramsch"};
Skat_Game::Skat_Game()
{
   current_hand = 0;
   ramsch_count = 0;
   bock_count = 0;
}
void Skat_Game::calculate_hand_score(int h) {
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
         hand[h].score[hand[h].declarer] = calculate_suit_grand_score(h) * hand[h].winlose * hand[h].bock * hand[h].kontrare;
      } else {
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
   cout << "|  # | Bid | M | Contract | HOSAZA | KR | " << setw(8) << player_name[0] << " | "
        << setw(8) << player_name[1] << " | " << setw(8) << player_name[2];
   if (number_of_players == 4) cout  << " | " << setw(8) << player_name[3];
   cout << " | Bock |" << endl;
   cout << "| -- | --- | - | -------- | ------ | -- | -------- | -------- | -------- |";
   if (number_of_players == 4) cout << " -------- |";
   cout << " ---- |" << endl;
   for (int i = 0; i <= current_hand; i++ ) {
      cout << "| " << setw(2) << i + 1 << " | " << setw(3) << hand[i].bid << " | ";
      if (hand[i].contract == NULLL || hand[i].contract == RAMSCH) cout << "  | ";
      else cout << hand[i].matadors << " | ";
      cout << setw(8) << left << get_contract_name(hand[i].contract) << right;
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
      cout << " | ";
      if (hand[i].kontrare == RE) cout << "KR | ";
      else if (hand[i].kontrare == KONTRA) cout << "K  | ";
      else cout << "   | ";
      cout << setw(8) << hand[i].score[0] << " | " << setw(8) << hand[i].score[1] << " | " << setw(8) << hand[i].score[2];
      if (number_of_players == 4) cout << " | " << setw(8) << hand[i].score[3];
      cout << " | ";
      if (hand[i].bock == BOCK) cout << " B  ";
      else cout << "    ";
      cout << " |";
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
   cout << setw(8) << total_score[0] << " | " << setw(8) << total_score[1] << " | " << setw(8) << total_score[2];
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
      default:
        cout << "Invalid contract type.";
        index = 0;
        break;
   }
   return(contract_name[index]);
}
int Skat_Game::calculate_new_bocks(int h) {
   int new_bocks = 0;
   if (!hand[h].grand_during_ramsch) {
      if ( (hand[h].contract != RAMSCH) && (hand[h].contract != NULLL) ) {
         if ((hand[h].matadors + 1) * hand[h].contract >= 120)
            new_bocks = number_of_players;
      }
      if ( (hand[h].contract != RAMSCH) && (hand[h].contract != NULLL) ) {
            if (hand[h].cardpoints == 60)
               new_bocks = number_of_players;
      }
      if ( (hand[h].kontrare == KONTRA) && (hand[h].winlose == LOSE) ) {
            new_bocks = number_of_players;
      }
      if (hand[h].kontrare == RE) {
            new_bocks = number_of_players;
      }
      if ( (bock_count == 0) && (hand[current_hand].multipliers & SCHNEIDER) ) {
            new_bocks = number_of_players;
      }
   }
   return new_bocks;
}
void Skat_Game::calculate_win_lose(int h) {
   if (hand[h].takealltricks) hand[h].multipliers |= SCHWARZ;
   else hand[h].multipliers &= ~SCHWARZ;
   if (hand[h].cardpoints > 89) hand[h].multipliers |= SCHNEIDER;
   else hand[h].multipliers &= ~SCHNEIDER;
   if ((hand[h].multipliers & SCHW_ANNC)) {
      if (hand[h].takealltricks) hand[h].winlose = WIN;
      else {
         hand[h].winlose = LOSE;
         hand[h].multipliers |= SCHWARZ;
         hand[h].multipliers |= SCHNEIDER;
      }
   }
   else if ((hand[h].multipliers & SCHN_ANNC)) {
      if (hand[h].cardpoints > 89) hand[h].winlose = WIN;
      else {
         hand[h].winlose = LOSE;
         hand[h].multipliers |= SCHNEIDER;
      }
   }
   if ( !(hand[h].multipliers & (SCHW_ANNC | SCHN_ANNC)) ) {
      if (hand[h].cardpoints > 60) hand[h].winlose = WIN;
      else hand[h].winlose = LOSE;
   }
   if ( !(hand[h].multipliers & (SCHW_ANNC | SCHN_ANNC)) ) {
      if (hand[h].cardpoints < 31) hand[h].multipliers |= SCHNEIDER;
   }
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
       case 'k':
       case 'K':
       case 'z':
       case 'Z':
         hand[h].contract = Skat_Game::CLUBS;
         break;
       case 's':
       case 'S':
       case 'p':
       case 'P':
         hand[h].contract = Skat_Game::SPADES;
         break;
       case 'h':
       case 'H':
         hand[h].contract = Skat_Game::HEARTS;
         break;
       case 'd':
       case 'D':
       case 'o':
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
enum RCE {RESCOREGAME, CONTINUEGAME, ENDTHEGAME};
enum State {INIT, NEW_HAND_BID, HAND_CONTRACT, HAND_SUMMARY,
            SCORE_HAND, END_GAME, EDIT_GAME, GAME_COMPLETED};
int yes();
RCE rce();
int input_and_validate(int min, int max);
void score_ramsch(int h);
State       state = INIT;
Skat_Game   game;
int         winning_player;
int         tie_game[4];
int         number_of_ties;
const char* VERSION = "v1.0.0";
using namespace std;
int main(int argc, char** argv) {
    string name;
    int h;
    int i;
    while (state != GAME_COMPLETED) {
        switch (state) {
            case INIT:
                cout << "Welcome to the Skat Scoresheet " << VERSION << "!" << endl;
                cout << "The scoresheet supports up to " << MAX_NUMBER_OF_HANDS <<" hands in a game" << endl;
                cout << "Will this be a Ramsch-only (Schieberamsch) game? (y/n) " << endl;
                if (yes()) {
                    game.schieberamsch = true;
                }
                else {
                    game.schieberamsch = false;
                    game.ramsch_count = 0;
                }
                cout << "Enter number of players (3 or 4): " << endl;
                game.number_of_players = input_and_validate(3, 4);
                cout << "Enter Player 1 name: " << endl;
                cin >> name;
                strncpy(game.player_name[0], name.c_str(), MAX_NAME_SIZE);
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Enter Player 2 name: " << endl;
                cin >> name;
                strncpy(game.player_name[1], name.c_str(), MAX_NAME_SIZE);
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Enter Player 3 name: " << endl;
                cin >> name;
                strncpy(game.player_name[2], name.c_str(), MAX_NAME_SIZE);
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (game.number_of_players > 3) {
                    cout << "Enter Player 4 name: " << endl;
                    cin >> name;
                    strncpy(game.player_name[3], name.c_str(), MAX_NAME_SIZE);
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                game.number_of_hands = MAX_NUMBER_OF_HANDS;
                cout << "Game will consist of " << game.number_of_players << " players named: " << endl;
                for (i = 0; i < game.number_of_players; i++) cout << game.player_name[i] << endl;
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
                if (game.schieberamsch || game.ramsch_count) {
                    cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer" << endl;
                    cout << "This is normally a Ramsch hand. Does anyone bid Grand Hand instead? (y/n) " << endl;
                    if (yes()) {
                        game.hand[game.current_hand].contract = Skat_Game::GRAND;
                        game.hand[game.current_hand].multipliers = Skat_Game::HAND;
                        game.hand[game.current_hand].grand_during_ramsch = true;
                        cout << "Who is the declarer? " << endl;
                        for (i = 0; i < game.number_of_players; i++) {
                            if ( (game.current_hand % game.number_of_players != i) || (game.number_of_players == 3) )
                                cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                        }
                        game.hand[game.current_hand].declarer = input_and_validate(1, game.number_of_players) - 1;
                        game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                        state = HAND_SUMMARY;
                    } else {
                        game.hand[game.current_hand].bid = 0;
                        game.hand[game.current_hand].contract = Skat_Game::RAMSCH;
                        game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                        state = HAND_SUMMARY;
                    }
                } else {
                    if (game.bock_count > 0) {
                        cout << "This hand is Bock. Scores double." << endl;
                        game.hand[game.current_hand].bock = Skat_Game::BOCK;
                    } else game.hand[game.current_hand].bock = Skat_Game::NOBOCK;
                    cout << game.player_name[game.current_hand % game.number_of_players] << " is the dealer" << endl;
                    cout << game.player_name[(game.current_hand + 1) % game.number_of_players] << " listens" << endl;
                    cout << game.player_name[(game.current_hand + 2) % game.number_of_players] << " speaks"  << endl;
                    if (game.number_of_players == 4) {
                        cout << game.player_name[(game.current_hand + 3) % game.number_of_players] << " continues"  << endl;
                    }
                    cout << "Enter winning bid (0 for Ramsch): " << endl;
                    game.hand[game.current_hand].bid = input_and_validate(0, 264);
                    if (game.hand[game.current_hand].bid == 0) {
                        game.hand[game.current_hand].contract = Skat_Game::RAMSCH;
                        state = HAND_SUMMARY;
                    } else {
                        state = HAND_CONTRACT;
                    }
                }
                break;
            case HAND_CONTRACT:
                game.set_contract(game.current_hand);
                cout << "Who is the declarer? " << endl;
                for (i = 0; i < game.number_of_players; i++) {
                    if ( (game.current_hand % game.number_of_players != i) || (game.number_of_players == 3) )
                        cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                }
                game.hand[game.current_hand].declarer = input_and_validate(1, game.number_of_players) - 1;
                if (game.hand[game.current_hand].contract == Skat_Game::Skat_Game::NULLL) {
                    cout << "Will it be played Hand? (y/n) " << endl;
                    if (yes()) {
                        game.hand[game.current_hand].multipliers |= Skat_Game::HAND;
                    }
                    cout << "Will it be played Open? (y/n) " << endl;
                    if (yes()) {
                        game.hand[game.current_hand].multipliers |= Skat_Game::OPEN;
                    }
                } else {
                    cout << "Will it be played Hand? (y/n) " << endl;
                    if (yes()) game.hand[game.current_hand].multipliers = Skat_Game::HAND;
                    if (game.hand[game.current_hand].multipliers & Skat_Game::HAND) {
                        cout << "Will it be played Open -- must take all tricks? (y/n) " << endl;
                        if (yes()) {
                            game.hand[game.current_hand].multipliers = (Skat_Game::HAND | Skat_Game::OPEN | Skat_Game::SCHNEIDER | Skat_Game::SCHN_ANNC | Skat_Game::SCHW_ANNC);
                            cout << "Playing Open implies announcing Schneider and Schwarz." << endl;
                        } else {
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
                if (game.hand[game.current_hand].grand_during_ramsch) {
                    cout << game.player_name[game.hand[game.current_hand].declarer]
                        << " plays Grand Hand during Ramsch." << endl;
                } else if (game.hand[game.current_hand].contract == Skat_Game::RAMSCH) {
                        cout << "This hand will be played Ramsch." << endl;
                } else {
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
            case SCORE_HAND:
                cout << "Scoring the hand after play..." << endl;
                if (game.hand[game.current_hand].contract == game.RAMSCH) {
                    score_ramsch(game.current_hand);
                } else {
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
                    cout << "Was there a Kontra? (y/n) " << endl;
                    if (yes()) {
                        game.hand[game.current_hand].kontrare = Skat_Game::KONTRA;
                        cout << "Did declarer Rekontra? (y/n) " << endl;
                        if (yes()) game.hand[game.current_hand].kontrare = Skat_Game::RE;
                    } else game.hand[game.current_hand].kontrare = game.SINGLE;
                }
                game.calculate_hand_score(game.current_hand);
                game.calculate_game_score();
                game.print_game_status();
                if (game.bock_count > 0) {
                    cout << "This hand was Bock. " << endl;
                }
                cout << "Enter 'c' to continue game, 'r' to re-score the hand, or 'e' to end game (c/r/e): " << endl;
                switch (rce()) {
                    case CONTINUEGAME:
                        if (game.current_hand < game.number_of_hands - 1) {
                                state = NEW_HAND_BID;
                                if (game.ramsch_count > 0) {
                                    if (--game.ramsch_count == 0) {
                                        cout << "End of Ramschround" << endl;
                                    } else {
                                            cout << "There ";
                                            if (game.ramsch_count == 1) {
                                                cout << "is 1 Ramsch hand  ";
                                            } else {
                                                cout << "are " << game.ramsch_count << " Ramsch hands ";
                                            }
                                            cout << "remaining." << endl;
                                    }
                                }
                                h = game.calculate_new_bocks(game.current_hand);
                                if (game.bock_count > 0) game.bock_count--;
                                if ( (game.hand[game.current_hand].bock == Skat_Game::BOCK) &&
                                    (game.bock_count % game.number_of_players == 0) ) {
                                        cout << "End of a Bockround. Next hand starts a Ramschround" << endl;
                                        game.ramsch_count = game.number_of_players;
                                }
                                if (h > 0) {
                                    cout << "This hand created new Bocks." << endl;
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
                                game.current_hand++;
                        } else {
                            state = END_GAME;
                        }
                        break;
                    case ENDTHEGAME:
                        state = END_GAME;
                        break;
                    case RESCOREGAME:
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
            default:
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
    bool valid = false;
    while (valid == false) {
        cin >> c;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
    }
    return result;
}
RCE rce() {
    RCE result;
    char c;
    bool valid = false;
    while (valid == false) {
        cin >> c;
        switch (c) {
            case 'R':
            case 'r':
                result = RESCOREGAME;
                valid = true;
                break;
            case 'e':
            case 'E':
                result = ENDTHEGAME;
                valid = true;
                break;
            case 'c':
            case 'C':
                result = CONTINUEGAME;
                valid = true;
                break;
            default:
                cout << "Invalid response. Enter r, c, or e: " << endl;
                break;
        }
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return result;
}
int input_and_validate(int min, int max) {
    int i;
    bool valid = false;
    while (valid == false) {
        cin >> i;
        if ( cin.fail() || (i < min) || (i > max) ) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Enter value from " << min << " to " << max << ": " << endl;
        }
        else valid = true;
    }
    return i;
}
void score_ramsch(int h) {
    int i;
    game.hand[h].ramsch = Skat_Game::PLAIN;
    cout << "Did a player win Durchmarsch? (y/n) " << endl;
    if (yes()) {
        game.hand[h].ramsch = Skat_Game::DURCHMARSCH;
        game.hand[h].winlose = Skat_Game::WIN;
        game.hand[h].number_of_losers = 0;
        cout << "Who won the Durchmarsch? " << endl;
        for (i = 0; i < game.number_of_players; i++) {
            if ( (h % game.number_of_players != i) || (game.number_of_players == 3) )
                cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
        }
        game.hand[h].declarer = input_and_validate(1, game.number_of_players) - 1;
    } else {
        cout << "How many players lost the Ramsch? (1, 2, 3)  " << endl;
        switch (input_and_validate(1, 3)) {
            case 1:
                game.hand[h].winlose = Skat_Game::LOSE;
                game.hand[h].number_of_losers = 1;
                cout << "Who lost the Ramsch? " << endl;
                for (i = 0; i < game.number_of_players; i++) {
                    if ( (h % game.number_of_players != i) || (game.number_of_players == 3) )
                        cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                }
                game.hand[h].loser[0] = input_and_validate(1, game.number_of_players) - 1;
                cout << "How many points taken? " << endl;
                game.hand[h].cardpoints = input_and_validate(41, 120);
                if (game.hand[h].cardpoints > 60) {
                    cout << "Was there a Jungf? (y/n) " << endl;
                    if (yes()) game.hand[h].ramsch = Skat_Game::JUNGF;
                }
                break;
            case 2:
                game.hand[h].winlose = Skat_Game::LOSE;
                game.hand[h].number_of_losers = 2;
                cout << "First player to lose the Ramsch? " << endl;
                for (i = 0; i < game.number_of_players; i++) {
                    if ( (h % game.number_of_players != i) || (game.number_of_players == 3) )
                        cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                }
                game.hand[h].loser[0] = input_and_validate(1, game.number_of_players) - 1;
                cout << "Other player to lose the Ramsch? " << endl;
                for (i = 0; i < game.number_of_players; i++) {
                    if ( ((h % game.number_of_players != i) || (game.number_of_players == 3)) &&
                          (game.hand[h].loser[0] != i ) )
                        cout << "  " << i + 1 << ": " << game.player_name[i] << endl;
                }
                game.hand[h].loser[1] = input_and_validate(1, game.number_of_players) - 1;
                cout << "How many points taken? " << endl;
                game.hand[h].cardpoints = input_and_validate(41, 60);
                if (game.hand[h].cardpoints == 60) {
                    game.hand[h].ramsch = Skat_Game::JUNGF;
                }
                break;
            case 3:
                cout << "40/40/40 Ramch." << endl;
                game.hand[h].winlose = Skat_Game::LOSE;
                game.hand[h].number_of_losers = 3;
                game.hand[h].cardpoints = 40;
                i = 0;
                for (int j = 0; j < 3; j++) {
                    if (game.number_of_players == 4) {
                        if (h % game.number_of_players == i) i++;
                    }
                    game.hand[h].loser[j] = i++;
                }
                game.hand[h].ramsch = Skat_Game::PLAIN;
                break;
            default:
                cout << "Invalid number of Ramsch losers." << endl;
                break;
        }
    }
}