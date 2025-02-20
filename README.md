# Skat Scoresheet

[![Check Markdown Links](https://github.com/Andy4495/skat-scoresheet/actions/workflows/check-links.yml/badge.svg)](https://github.com/Andy4495/skat-scoresheet/actions/workflows/check-links.yml)

Text-based scoresheet for the card game [Skat][1]. Either three or four players are supported.

This will hopefully evolve into a graphical iOS app.

## House Rules

This scoresheet is hard-coded with our particular house rules:

- In four player games, the dealer sits out play for that hand
- Scoring
  - Losses count double for non-Ramsch contracts
  - Grand base value is 24
  - Opponents must take 31 to avoid Schneider
  - Declarer losing hand with less than 31 points is considered Schneider
- Kontra and Re (or Rekontra) allowed
  - An opponent must not have passed on an 18 bid in order to say Kontra
    - This is not enforced in the code
- Ramsch
  - Play a Ramsch hand if nobody bids 18
  - Play a Ramschround of 3 hands after playing a Bockround of 3 hands
  - There is no Bock during a Ramsch round
    - Except a "nobody bids 18" Ramsch hand has Bock scoring apply
  - A player taking all tricks (Durchmarsch) scores +120 points
  - If no single player takes all tricks, then he player who collects the most card points has those card points deducted
    - More than one player can lose points in the case of a tie
    - If a player takes zero tricks, the losing hand(s) score(s) double
  - During a Ramschround, each player has a chance to bid a Grand Hand, in which case the declarer leads and the hand is scored as if it were a regular Grand Hand contract
    - No bock can be created in this case
  - Entire game can be played Ramsch ("Schieberamsch")
- Bockround
  - Scores double in a Bock hand
  - A new Bockround (3 hands) is created in the following cases:
    - For suit and Grand contracts:
      - Declarer and opponents each take 60 points
      - Raw hand score >= 120 (before Bock/Lose/Kontra/Re doubling)
    - Successful Kontra (declarer loses)
    - Any Re (i.e. declarer says Re to opponent's Kontra)
    - The contract results in a Schneider (but only if current hand is not a Bock hand)
  - A maximum of one Bockrund (3 hands) can be created per hand
    - For example, a 120 raw hand score with a Schneider only creates 1 Bockrund

## To-Do

Tasks to complete before this is ready to be called "Version 1.0.0":

- More thorough testing, including running a test as part of the Build action

## Future Features

Not planned for Version 1.0.0, but may be added in a future release:

- Allow an option to select whether to play a Ramsch round after a Bock round
- Saving the game to a file
- Configuration file with setup parameters (# of players, player names)
- Improved error handling when input is out of range
  - Currently defaults to "no" on yes/no input and the min value on integer input
- Check for declarer taking zero tricks on a non-null contract (opponents Schwarz the declarer)
  - This would be an extremely rare occurrence
- Add hands to an existing game (play longer than originally planned)
  - This may not be necessary, since the program currently allocates 99 hands which can be ended early
  - I don't think that I've ever played a game even close to 99 hands

## Building the code

The code is fully text-based and should work on any modern Unix/Linux-based system. I have compiled and tested it on MacOS (clang version 12.0.0) and Ubuntu 24 (GNU version 13.3.0).

To build:

``` shell
cd src
make
```

The executable is named `skat-scoresheet` and takes no arguments.

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
