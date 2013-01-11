"Analyze two-person constant-sum games via linear programming."

try:
    import pulp
except ImportError:
    raise ImportError("Module 'pulp' is required to use 'symlp'.")
from pulp.solvers import PulpSolverError


class SymmetricLPSolver(object):
    """
    Compute Nash equilibria of a two-person symmetric constant
    sum game in strategic form, using linear programming.
    """
    def construct_lp(self, game):
        problem = pulp.LpProblem("problem", pulp.LpMaximize)
        var = [ pulp.LpVariable("p%d" % i, 0)
                for (i, s) in enumerate(game.players[0].strategies) ]
        problem += sum(var)
        minpay = min(game.min_payoff, 0)
        for s in game.players[0].strategies:
            problem += (pulp.lpDot([ game[s,t][0]-minpay for t in game.players[1].strategies ], var) <= 1)
        return (problem, var)

    def solve_one(self, game):
        "Compute one Nash equilibrium of the game."
        problem, var = self.construct_lp(game)
        try:
            status = problem.solve(pulp.PYGLPK(msg=0, options=["--cgr"]))
        except PulpSolverError:
            status = problem.solve(pulp.GLPK(msg=0, options=["--cgr"]))
        if pulp.LpStatus[status] != "Optimal":
            raise ValueError("LP solver did not return optimal solution")
        soln = game.mixed_profile(rational=False)
        for (i, v) in enumerate(var):
            soln[game.players[0].strategies[i]] = pulp.value(v) / pulp.value(problem.objective)
            soln[game.players[1].strategies[i]] = pulp.value(v) / pulp.value(problem.objective)
        return soln
        
import time

def support_induction_solver(game, guess, stepsize=1):
    solver = SymmetricLPSolver()
    while True:
        restr = guess.restrict()
        print "%s: Solving with support size=%d" % \
            (time.strftime("%Y%m%dT%H%M%S"),
             len(restr.players[0].strategies))
        restr_eqm = solver.solve_one(restr)
        game_eqm = restr_eqm.unrestrict()
        payoff = game_eqm.payoff(game.players[0])
        vals = [ (i, game_eqm.strategy_value(s))
                 for (i, s) in enumerate(game.players[0].strategies) ]
        payoff = sum([ p*v
                       for (p, (i, v)) in zip(game_eqm, vals) ])
        vals = filter(lambda x: x[1] >= payoff, vals)
        vals.sort(key=lambda x: -x[1])
        topK = [ x[0] for x in vals[:stepsize]]

        newguess = game.support_profile()
        for player in game.players:
            for (i, s) in enumerate(player.strategies):
                if i not in topK and s not in guess:
                    newguess = newguess.remove(s)

        if guess == newguess:  break
        guess = newguess
    return game_eqm
