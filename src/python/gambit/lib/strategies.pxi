cdef class Strategies:
    cdef c_GamePlayer player

    def __repr__(self):
        return str(list(self))

    def __len__(self):
        return self.player.deref().NumStrategies()

    def __getitem__(self, st):
        cdef Strategy s
        
        if  isinstance(st, int):
            if st < 0 or st >= len(self):
                raise IndexError("no strategy with index '%s'" % st)
            s = Strategy()
            s.strategy = self.player.deref().GetStrategy(st+1)
            return s
        elif  isinstance(st, str):
            for strategy in self:
                if strategy.label == st:
                    return strategy

            raise IndexError("no Strategy with label '%s'" % st)

        elif  isinstance(st, Strategy):
            for strategy in self:
                if strategy == st:
                    return strategy

            raise IndexError("Strategy does not exist in this game '%s'" % st)

        else:
            raise TypeError("Invalid input to find Strategy. Please use ints, strings, or Strategy objects")
        
