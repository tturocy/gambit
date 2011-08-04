cdef class Outcomes:
    cdef c_Game game

    def __repr__(self):
        return "<Outcomes of game '%s'>" % self.game.deref().GetTitle().c_str()

    def __len__(self):
        return self.game.deref().NumOutcomes()

    def __getitem__(self, outc):
        cdef Outcome c
        if type(outc) is int:
            if outc < 0 or outc >= len(self):
                raise IndexError("no outcome with index '%s'" % outc)
            c = Outcome()
            c.outcome = self.game.deref().GetOutcome(outc+1)
            
            return c

        elif type(outc) is str:
            for i in range(1, len(self) + 1):
                c = Outcome()
                c.outcome = self.game.deref().GetOutcome(i)
                if c.label == outc:
                    return c

            raise IndexError("no outcome with label '%s'" % outc)

        elif type(outc) is Outcome:
            for i in range(1, len(self) + 1):
                c = Outcome()
                c.outcome = self.game.deref().GetOutcome(i)
                if c == outc:
                    return c

            raise IndexError("Outcome does not exist in this game '%s'" % outc)

        else:
            raise IndexError("Invalid input to find Outcome. Please use ints, strings, or Outcome objects")
  