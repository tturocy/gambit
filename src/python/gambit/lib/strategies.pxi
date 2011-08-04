cdef class Strategies:
    cdef c_GamePlayer player

    def __repr__(self):
        return str(list(self))

    def __len__(self):
        return self.player.deref().NumStrategies()

    def __getitem__(self, st):
        cdef Strategy s
        
        if type(st) is int:
            if st < 0 or st >= len(self):
                raise IndexError("no strategy with index '%s'" % st)
            s = Strategy()
            s.strategy = self.player.deref().GetStrategy(st+1)
            return s
        elif type(st) is str:
            for i in range(1, len(self) + 1):
                c = Strategy()
                c.strategy = self.player.deref().GetStrategy(i)
                if c.label == st:
                    return c

            raise IndexError("no Strategy with label '%s'" % st)

        elif type(st) is Strategy:
            for i in range(1, len(self) + 1):
                c = Strategy()
                c.strategy = self.player.deref().GetStrategy(i)
                if c == st:
                    return c

            raise IndexError("Strategy does not exist in this game '%s'" % st)

        else:
            raise IndexError("Invalid input to find Strategy. Please use ints, strings, or Strategy objects")
        
