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
                for i in xrange(len(game.players[0].strategies)) ]
        problem += sum(var)
        for s in game.players[0].strategies:
            problem += (pulp.lpDot([ game[s,t][0] for t in game.players[1].strategies ], var) <= 1)
        return (problem, var)

    def solve_one(self, game):
        "Compute one Nash equilibrium of the game."
        problem, var = self.construct_lp(game)
        try:
            status = problem.solve(pulp.PYGLPK(msg=1, options=["--cgr"]))
        except PulpSolverError:
            status = problem.solve(pulp.GLPK(msg=1, options=["--cgr"]))
        if pulp.LpStatus[status] != "Optimal":
            raise ValueError("LP solver did not return optimal solution")
        soln = game.mixed_profile(rational=False)
        s1 = soln[game.players[0]]
        s2 = soln[game.players[1]]
        for (i, v) in enumerate(var):
            s1[i] = pulp.value(v) / pulp.value(problem.objective)
            s2[i] = pulp.value(v) / pulp.value(problem.objective)
        return soln
        
