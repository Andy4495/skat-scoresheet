# Skat Scoresheet

[![Check Markdown Links](https://github.com/Andy4495/skat-scoresheet/actions/workflows/check-links.yml/badge.svg)](https://github.com/Andy4495/skat-scoresheet/actions/workflows/check-links.yml)

Text-based scoresheet for the card game [Skat][1]. Either three or four players (dealer sits out) are supported.

This will hopefully evolve into a graphical iOS app.

## House Rules

This scoresheet is hard-coded with our particular house rules:

- Scoring
  - Losses count double for non-Ramsch contracts
  - Grand base value is 24
  - Opponents must take 31 to avoid Schneider
  - Declarer losing hand with less than 31 points is considered Schneider
- Kontra and Re (or Rekontra) allowed
  - An opponent must not have passed on an 18 bid in order to say Kontra
    - This is not currently enforced in the code
- Ramsch
  - Play a Ramsch hand if nobody bids 18
  - Play a Ramschround of 3 hands after playing a Bockround of 3 hands
  - The player who collects the most card points loses (except for Durchmarsch)
    - More than one player can lose in the case of a tie
    - Scores are counted negative (except for Durchmarsch)
    - If a player takes zero tricks, the losing hand(s) scores double
  - A player taking all tricks (Durchmarsch) scores +120 points
  - During a Ramschround, each player has a chance to bid a Grand Hand, in which case the declarer leads and the hand is scored as if it were a regular contract hand
- Bockround
  - Scores double in a Bock hand
  - A new Bockround (3 hands) is created in the following cases:
    - Declarer and opponents each take 60 points (suits and Grand contracts)
    - Raw hand score >= 120 (non-Ramsch; before Bock/Lose/Kontra/Re doubling)
    - Successful Kontra (declarer loses)
    - Any Re (i.e. declarer says Re to opponent's Kontra)
    - The contract results in a Schneider (but only if current hand is not a Bock hand)

## Future Features

- Saving the game to a file
- Configuration file with setup parameters (# of players, player names, # of hands)
- Check for declarer taking zero tricks on a non-null contract
  - This would be an extremely rare occurrence

## References

- [Skat Wikipedia][1]
- Various rules and history references:
  - [Pagat][2]
  - [Skatinsel][3]
  - [Reference Card][4]
  - [Foster's Skat Manual, (c) 1922][5]
  - [Ramsch][6]

## License

The software and other files in this repository are released under what is commonly called the [MIT License][100]. See the file [`LICENSE.txt`][101] in this repository.

[1]: https://en.wikipedia.org/wiki/Skat_(card_game)
[2]: https://www.pagat.com/schafkopf/skat.html
[3]: https://www.skatinsel.academy/en/how-to-skat/rules
[4]: https://skatgame.net/intro/Reference2.pdf
[5]: https://www.fadedpage.com/link.php?file=20090109-a5.pdf
[6]: https://www.pagat.com/schafkopf/sramsch.html
[100]: https://choosealicense.com/licenses/mit/
[101]: ./LICENSE.txt
[//]: # ([200]: https://github.com/Andy4495/skat-scoresheet)

[//]: # (This is a way to hack a comment in Markdown. This will not be displayed when rendered.)
