Gambit plugin for STATA
=======================

This directory contains the source code for an experimental STATA plugin
which exposes features of the Gambit library for representing games.
The immediate objective of this project is to support maximum
likelihood estimation in laboratory games in which the logit QRE
parameter (lambda) is estimated simultaneously with one or more other
parameters, such as risk preferences, social preferences, and so
forth.



game.class
----------

The intermediate-level interface to the Gambit library is via the ADO
class game, in the file game.class.

Creating or loading a game
~~~~~~~~~~~~~~~~~~~~~~~~~~

A new strategic form game is created by a call to the member program
.create.  Parameters are a list of the number of strategies for each
player.  For a 2-player game with 2 strategies each::

   .g = .game.new
   .g.create 2 2

For a 3-player game, where the first player has 4 strategies, the
second player 2 strategies, and the third player 3 strategies::

   .g = .game.new
   .g.create 4 2 3

Games can also be loaded from existing Gambit savefiles.  To load
a game from the file mygame.nfg::

   .g = .game.new
   .g.load mygame.nfg


Manipulating payoffs
~~~~~~~~~~~~~~~~~~~~

Payoffs are manipulated using the .getpayoff and .setpayoff member
programs of the game class.

The member program .getpayoff returns the payoff to a player from a
given combination of pure strategies.  The program takes N + 1
arguments for a N-player game.  The first argument is the index number
of the player; the remaining arguments list the indices of the
pure strategies played by each player, in order.

For example, to get the payoff in a 3-player game to player 2, when
player 1 chooses his fifth strategy, player 2 his first, and player
3 his fourth strategy::

   .g.getpayoff 2 5 1 4

The member program .setpayoff sets the payoff to a player from a given
combination of pure strategies.  The program takes N + 2 arguments for
a N-player game.  The first argument is the index number of the
player; the second argument is the avlue of the payoff; the remaining
arguments list the indices of the pure strategies played by each
player, in order.

For example, to set the payoff in a 3-player game for player 2 to 0.5,
when player 1 chooses his fifth strategy, player 2 his first, and
player 3 his fourth strategy::

   .g.setpayoff 2 0.5 5 1 4





