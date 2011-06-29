An experimental STATA plugin for econometric analysis in games
==============================================================

This section documents an *experimental* `STATA <href://www.stata.com>`_
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

The member program ``.setpayoff`` sets the payoff to a player from a given
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

The lowest level of the interface to the Gambit engine is ``gambit.plugin``.
The provides the plumbing to connect STATA calls to the Gambit C++ library.

.. note::

   Documentation of the plugin interface is provided primarily for those
   who might wish to extend the features of the interface.  Most users will
   only need to use the more convenient high-level or intermediate-level
   interfaces.

Loading the plugin into STATA is done with the call

::

   program gambit, plugin

This call should be issued once per STATA session, before making any calls to the
plugin.

Calls to the plugin generally have the format

::

   plugin call gambit, method arguments

where ``method`` specifies the operation to be carried out.  Method names
match up closely with the intermediate-level ``game.class`` interface;
``game.class`` is a convenience wrapper around the Gambit API to simplify
manipulation of games in programming applications.  

The plugin maintains a list of games which have been defined.  Each game
is identified by an integer ``handle``.  A new game can be created using the
method ``create``, which takes a list of the dimensions of the game::

   plugin call gambit, create 4 2 3

Alternatively, a game can be loaded from a Gambit savefile using the
method ``load``::

   plugin call gambit, load mygame.nfg

Each of these calls returns the integer handle assigned to the game.  This
handle is used in all calls to access or manipulate the game.  The list of
games currently defined is displayed by a call to the method ``list``::

   plugin call gambit, list

The handle to a game appears as the first argument to all other calls to the
plugin.







 

