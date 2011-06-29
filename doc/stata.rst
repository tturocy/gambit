An experimental STATA plugin for econometric analysis in games
==============================================================

This section documents an *experimental* `STATA <href://www.stata.com>`
plugin which exposes features of the Gambit library for representing
games.  The immediate objective of this project is to support
maximum likelihood estimation in laboratory games in which the logit
QRE parameter (lambda, in the specification used by Gambit) is
estimated simultaneously with one or more other parameters, such as
risk preferences, social preferences, and so forth.

The QRE tracing call is a wrapper around the code also exposed in the
:program:`gambit-logit` command-line tool.  This operates in a mode which
identifies local maxima of the likelihood function, using a Newton
search method to locate the maximum precisely along the branch of the
correspondence.


Intermediate-level interface: game.class
----------------------------------------

The intermediate-level interface to the library is via the ADO class
``game``, in the file ``game.class``.

Creating or loading a game
~~~~~~~~~~~~~~~~~~~~~~~~~~

A new strategic form game is created by a call to the member program
``.create``. Parameters are a list of the number of strategies for each
player.  For a 2-player game with 2 strategies each::

   .g = .game.new
   .g.create 2 2

For a 3-player game, where the first player has 4 strategies, the second
player has 2 strategies, and the third player 3 strategies::

   .g = .game.new
   .g.create 4 2 3

Games can also be loaded from existing Gambit savefiles.  To load a game from
the file ``mygame.nfg``::

   .g = .game.new
   .g.load mygame.nfg


Manipulating payoffs
~~~~~~~~~~~~~~~~~~~~

Payoffs are manipulated using the ``.getpayoff`` and ``.setpayoff`` member
programs of the game class.

The member program ``.getpayoff`` returns the payoff to a player from a
given combination of pure strategies.  The program takes N+1 arguments for
an N-player game.  The first argument is the index number of the player; the
remaining arguments list the indices of the pure strategies played by each
player, in order of player index.

For example, in a 3-player game, to obtain the payoff to player 2, when
player 1 chooses his fifth strategy, player 2 his first, and player 3 his
fourth, use::

   .g.getpayoff 2 5 1 4

The mbmer program ``.setpayoff`` sets the payoff to a player from a given
combination of pure strategies.  The program takes N+2 arguments for an
N-player game.  The first argument is the index number of the player; the
second argument is the value of the payoff; the remaining arguments list the
indices of the pure strategies played by each player, in order.

For example, in a 3-player game, to set the payoff for player 2 to 0.5,
when player 1 chooses his fifth strategy, player 2 his first, and player 3
his fourth, use::

   .g.setpayoff 2 0.5 5 1 4


Low-level interface: gambit.plugin
----------------------------------


 

