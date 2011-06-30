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


Dimensions of a game
~~~~~~~~~~~~~~~~~~~~

The number of players in a game is returned by the member program
``.players``, which takes no parameters::

   .g.players

The number of strategies available to a player is returned by the member
program ``.strategies``, which takes one parameter, the index of the
player. For the number of strategies available to player 2, use::

   .g.strategies 2



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

Example: Creating a prisoner's dilemma game
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As an example, consider creating a standard prisoner's dilemma game. 
There are two players, each with two strategies, ``cooperate`` and
``defect``.  Suppose if both cooperate, both receive a payoff of 8;
if one cooperates and the other defects, the defector receives a payoff
of 10 and the cooperator a payoff of 2; and if both defect, both receive
a payoff of 4.

Let ``cooperate`` be strategy 1 and ``defect`` be strategy 2.
A sequence that creates this game can be written::

   .g = .game.new
   .g.create 2 2
   .g.setpayoff 1 8 1 1
   .g.setpayoff 2 8 1 1
   .g.setpayoff 1 2 1 2
   .g.setpayoff 2 10 1 2
   .g.setpayoff 1 10 2 1
   .g.setpayoff 2 2 2 1
   .g.setpayoff 1 4 2 2
   .g.setpayoff 2 4 2 2


Computation of quantal response equilibria
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Two facilities are provided for evaluating the quantal response equilibria of
the game, ``.qre_mle`` and ``.qre_eval``.

The member program ``.qre_mle`` computes the maximum likelihood estimate of
the QRE parameter lambda, given a vector of observed strategy choice frequencies.
The program takes as parameters a list of observed frequencies, one for
each strategy in the game.  Frequencies for player 1's strategies are given
first in order, followed by player 2's strategies, and so on.  For instance,
in the prisoner's dilemma example, if player 1 played ``cooperate`` 8 times
and ``defect`` 2 times, and player 2 played ``cooperate`` 6 times and
``defect`` 4 times, the appropriate call would be::

   .mp = .g.qre_mle 8 2 6 4

The object returned by the call is a mixed strategy profile.  It contains a
member ``.lambda``, which reports the value of lambda at which the likelihood
is maximized.  The member ``.probs`` reports the probabilities of the
corresponding QRE.  This is a two-dimensional array indexed by player number
and strategy number; e.g., ``.probs[1][2]`` returns the probability player 1
plays his second strategy.  Finally, the member ``.logL`` contains the
log-likelihood of the data at the estimated profile.

The member program ``.qre_eval`` computes a QRE at a specified parameter
lambda, which it takes as its only parameter.  For instance, to compute the
QRE corresponding to a lambda value of 0.9::

   .mp = .g.qre_eval 0.9

This also returns a mixed strategy profile object with members ``.lambda``
and ``.probs``.

Both these routines trace only along the principal branch of the quantal
response equilibrium correspondence, i.e., the one emanating from the centroid
when lambda is zero.  

The program ``.qre_mle`` assumes that all observations are independent in
constructing the likelihood function.  The program uses a Newton search
method to identify the maximizing value of lambda to high precision.

For games in which the principal branch has a
backwards-bending component, ``.qre_eval`` finds only the first QRE with
the specified lambda when traversing starting at the centroid.


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

Each of these calls returns the integer handle assigned to the game
in the local macro ``_handle``.  This
handle is used in all calls to access or manipulate the game.  The list of
games currently defined is displayed by a call to the method ``list``::

   plugin call gambit, list

The handle to a game appears as the first argument to all other calls to the
plugin.

The prisoner's dilemma game from the previous section could be constructed
directly using the low-level interface via the sequence of calls::

   plugin call gambit, create 2 2
   * Game handle is stored in _newhandle; use to reference game
   plugin call gambit, setpayoff `_newhandle' 1 8 1 1
   plugin call gambit, setpayoff `_newhandle' 2 8 1 1
   plugin call gambit, setpayoff `_newhandle' 1 2 1 2
   plugin call gambit, setpayoff `_newhandle' 2 10 1 2
   plugin call gambit, setpayoff `_newhandle' 1 10 2 1
   plugin call gambit, setpayoff `_newhandle' 2 2 2 1
   plugin call gambit, setpayoff `_newhandle' 1 4 2 2
   plugin call gambit, setpayoff `_newhandle' 2 4 2 2

API methods which return values use local macros for the returned
quantities.  These include:

``getpayoff``

   Returns the payoff to a player from a given combination of strategies.
   For the game with handle 1, to get the payoff to player 2 if player 1 chooses
   his fifth strategy, player 2 chooses his first, and player 3 chooses his
   fourth, use::

      plugin call gambit, getpayoff 1 2 5 1 4

   ``getpayoff`` returns the payoff in the local macro ``_payoff``.

``players``

   Returns the number of players in a game.  For the game with handle 1,
   use::

      plugin call gambit, players 1

   ``players`` returns the number of players in the local macro ``_countplayers``.

``strategies``

   Returns the number of strategies available to a player in a game.
   For the game with handle 1, to get the number of strategies available to
   player 3, use::

      plugin call gambit, strategies 1 3

   ``strategies`` returns the number of strategies in the local macro
   ``_countstrategies``.

``qre_mle``

   Computes the maximum likelihood estimate of the QRE parameter lambda
   given a vector of choice frequencies.  The parameters of the function are
   the choice frequences, with player 1's choices specified first in order,
   then player 2's, and so on.

   ``qre_mle`` returns the computed likelihood-maximizing value of lambda
   in the local macro ``_lambda``, and the value of the log-likelihood function
   in the local macro ``_logL``.  The strategy probabilities associated with the
   corresponding QRE are returned in local macros starting with ``_prob`` and
   indexed by the player number and strategy number, e.g., ``_prob_1_2``
   contains the probability that player 1 plays his second strategy in the
   computed QRE.

``qre_eval``

   Computes a QRE of the game for a specified value of the QRE parameter lambda.
   For the game with handle 1, to compute the QRE at a lambda value of 0.9 use::

      plugin call gambit, qre_eval 0.9

   The returned values of ``_lambda`` and ``prob_*_*`` are as described for
   ``qre_mle``.


