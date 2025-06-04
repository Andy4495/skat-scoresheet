/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code
   1.0.0 21-Feb-2025 Andy4495 Version 1.0
   1.1.0 02-Jun-2025 Andy4495 Version 1.1

*/

#ifndef SKAT_GAME_H
#define SKAT_GAME_H

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
    int     no_bock_count;  // Used for the "new bock if no bocks for 2 rounds case"
    static const char* const contract_name[7];
    enum    Contract_Type {CLUBS = 12, SPADES = 11, HEARTS = 10, DIAMONDS = 9, NULLL = 23, GRAND= 24, RAMSCH = 1};
    enum    Other_Multipliers {NO_MULTIPLIERS = 0, HAND = 1, OPEN = 2, SCHNEIDER = 4, 
                               SCHN_ANNC = 8, SCHWARZ = 16, SCHW_ANNC = 32};
    enum    Kontra_Re {SINGLE = 1, KONTRA = 2, RE = 4};
    enum    Ramsch_Special {PLAIN = -1, JUNGF = -2, DURCHMARSCH = 99};
    enum    Win_Lose {LOSE = -2, WIN = 1};
    enum    Bock_Type {NOBOCK = 1, BOCK = 2};
    struct {
        bool    ramschround_after_bockround;
        bool    grand_hand_during_ramschround;
        bool    bockround_for_60_60_tie;
        bool    bockround_for_120_hand_score;
        bool    bockround_for_lost_contra_or_rekontra;
        bool    bockround_for_schneider;
        bool    bockround_for_schneider_only_if_not_bockround;
        bool    bockround_max_one_per_hand;
        bool    bockround_can_be_created_in_grand_during_ramsch;
        bool    bockround_if_two_rounds_without_bock;

    } rules;
    struct  {
        int         declarer;       // Index to player_names[]
        int         bid;
        Contract_Type contract;     // Clubs, Spades, Hearts, Diamonds, Null, Grand, Ramsch
        Win_Lose    winlose;
        int         multipliers;    // Logical OR of Hand, Open, Schneider, Announce, Schwarz, Announce
        int         matadors;       // Top trumps, use 0 for Ramsch or Null
        int         overbid;        // Added multipliers when there is an overbid
        int         loser[3];       // Index to player_names[]; May have one, two, or three losers in Ramsch
        int         number_of_losers; // How many loser[] entries are valid
        int         cardpoints;     // Declarer points taken, or Points taken by Ramsch loser, 
        Kontra_Re   kontrare; 
        Ramsch_Special ramsch;
        Bock_Type   bock;
        int         score[4];       // Index to player_names[]; Initialize to zero, updated per hand
        int         edited;         // Indicates if a score was edited for this hand at end of game
        bool        grand_during_ramsch;    // Player bid Grand Hand during a Ramsch round or Schieberamsch game
        bool        takealltricks;  // Flag for Schwarz to differentiate it from a 120 point hand w/o all tricks
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

#endif
