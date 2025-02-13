/* Skat Card Game Scoring Sheet
   Copyright 2025 Andreas Taylor
   https://github.com/Andy4495/skat-scoresheet
   MIT License

   0.1  10-Feb-2025 Andy4495 Initial code

*/

#ifndef SKAT_GAME_H
#define SKAT_GAME_H

#define MAX_NAME_SIZE 8

class Skat_Game {
public:
    int     number_of_players;
    char    player_name[4][MAX_NAME_SIZE + 1];
    int     number_of_hands;
    int     current_hand;
    int     total_score[4];
    static const char* const contract_name[7];
    enum    Contract_Type {CLUBS = 12, SPADES = 11, HEARTS = 10, DIAMONDS = 9, NULLL = 23, GRAND= 24, RAMSCH = 1};
    enum    Other_Multipliers {NO_MULTIPLIERS = 0, HAND = 1, OPEN = 2, SCHNEIDER = 4, 
                               SCHN_ANNC = 8, SCHWARZ = 16, SCHW_ANNC = 32};
    enum    Kontra_Re {SINGLE = 1, KONTRA = 2, RE = 4};
    enum    Ramsch_Special {PLAIN, JUNGFRAU, DURCHMARSCH};
    enum    Win_Lose {LOSE = -1, WIN = 1};
    struct  {
        int declarer;     // Index to player_names[]; -1 for Ramsch
        int bid;
        Contract_Type contract;   // Clubs, Spades, Hearts, Diamonds, Null, Grand, Ramsch
        Win_Lose winlose;
        Other_Multipliers multipliers;  // Logical OR of Hand, Open, Schneider, Announce, Schwarz, Announce
        int matadors;   // Top trumps, use 0 for Ramsch or Null
        int losers[3];  // Index to player_names[]; May have two or three losers in Ramsch
        Kontra_Re kontrare; 
        Ramsch_Special ramsch;
        int bock;
        int score[4];   // Index to player_names[]; Initialize to zero, updated per hand
    } hand[36];

    Skat_Game( const char *  p1,  const char *  p2,  const char *  p3);
    void calculate_hand_score(int hand); 
    void calculate_game_score();
    void print_game_status();
    const char* const get_contract_name(int hand);
    
};

#endif
