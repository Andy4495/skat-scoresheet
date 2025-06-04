# Skat Scoresheet

[![Check Markdown Links](https://github.com/Andy4495/skat-scoresheet/actions/workflows/check-links.yml/badge.svg)](https://github.com/Andy4495/skat-scoresheet/actions/workflows/check-links.yml)

Scoresheet for the card game [Skat][1]. Either three or four players are supported.

The initial version is text-based and run from the command line.

A [web-based version][8] with similar functionality is in progress.

## House Rules

### The following house rules are not configurable

- Player #1 is the first dealer
- In four player games, the dealer sits out play for that hand
- Declaring
  - Must play Hand in order to declare Schneider or Schwarz
  - Suit or Grand Open bid only allowed with Hand bid and implies that Schneider and Schwarz announced
- Scoring
  - Losses count double for non-Ramsch contracts
  - Grand base value is 24
  - Opponents must take 31 to avoid Schneider
  - Declarer must collect at least 31 points to avoid Schneider
  - Null scores:
    - Standard bid: 23
    - Hand not Open: 35
    - Open not Hand: 46
    - Hand and Open: 59
- Ramsch
  - There is no Bock during a Ramsch round
    - Except a "nobody bids 18" Ramsch hand has Bock scoring during Bock round
  - A player taking all tricks (Durchmarsch) scores +120 points
  - If no single player takes all tricks, then the player who collects the most card points has those card points deducted
    - More than one player can lose points in the case of a tie
    - If a player takes zero tricks, the losing hand(s) score(s) double
- Bock round
  - Scores double during a Bock round

### The following house rules are configurable

- Ramsch
  - Play a Ramsch round (3 or 4 hands, depending on number of players) after playing a Bock round
  - During a Ramsch round, each player has a chance to bid a Grand Hand, in which case the declarer leads and the hand is scored as if it were a regular Grand Hand contract
    - No bock can be created in this case
  - Entire game can be played Ramsch ("Schieberamsch")
- Bock round
  - A new Bock round (3 or 4 hands, depending on number of players) is created in the following cases:
    - For suit and Grand contracts:
      - Declarer and opponents each take 60 points
      - Raw hand score >= 120 (before Bock/Lose/Kontra/Re doubling)
    - Successful Kontra (declarer loses)
    - Any Rekontra (i.e. declarer says Re to opponent's Kontra)
    - The contract results in a Schneider (but only if current hand is not a Bock hand)
  - A maximum of one Bock round can be created per hand
    - For example, a 120 raw hand score with a Schneider only creates 1 Bock round

### Player-enforced rules

These rules are enforced by the players, not the scoresheet, so there is no code or configuration associated with them.

- Kontra and Rekontra allowed
  - An opponent must not have passed on an 18 bid in order to say Kontra
- Ramsch
  - Play a Ramsch hand if nobody bids 18

## To-Do

Create a web-based HTML/JavaScript version

## Future Features

Features that may be added in a future release:

- Saving the game to a file
- Configuration file with setup parameters (# of players, player names)
  - Default name; if it exists, use it. Add default name to .gitignore
- Check for declarer taking zero tricks on a non-null contract (opponents Schwarz the declarer)
  - This would be an extremely rare occurrence
- Add hands to an existing game (play longer than originally planned)
  - This may not be necessary, since the program currently allocates 99 hands which can be ended early
  - I don't think that I've ever played a game even close to 99 hands

## Building

The code is fully text-based and should work on any modern Unix/Linux-based system. I have compiled and tested it on MacOS (clang version 12.0.0) and Ubuntu 24 (GNU version 13.3.0).

To build:

``` shell
cd src
make
```

The executable is named `skat-scoresheet` and takes no arguments.

## Testing

The code is tested with the GitHub action workflow [`Test.yml`][7].

This workflow builds the code and then runs several test input files (`test-*-input.txt`) through the program and compares the output to files containing the expected output (`test-*-output.txt`).

## Web-based Version

A web-based version is in progress (see [docs/index.html](docs/index.html)). This is a manual port of the C++ code over to JavaScript and uses HTML to provide a GUI. It is entirely run in the browser; there is no back-end processing involved.

The web page is published using [GitHub Pages][9] and can be accessed [on github.io][8].

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
[7]: ./.github/workflows/Test.yml
[8]: https://andy4495.github.io/skat-scoresheet/
[9]: https://pages.github.com
[100]: https://choosealicense.com/licenses/mit/
[101]: ./LICENSE.txt
[//]: # ([200]: https://github.com/Andy4495/skat-scoresheet)

[//]: # (This is a way to hack a comment in Markdown. This will not be displayed when rendered.)
