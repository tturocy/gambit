cdef class Strategies:
    cdef c_GamePlayer player

    def __repr__(self):
        return str(list(self))

    def __len__(self):
        return self.player.deref().NumStrategies()

    def __getitem__(self, st):
        cdef Strategy s
        try:
            if st < 0 or st >= len(self):
                raise IndexError("no strategy with index '%s'" % st)
            s = Strategy()
            s.strategy = self.player.deref().GetStrategy(st+1)
            return s
        except IndexError:
            if type(st) is str:
                for i in range(1, len(self) + 1):
                    s = Strategy()
                    s.strategy = self.player.deref().GetStrategy(i)
                    if s.label == st:
                        return s
                    # fall through
        
                raise IndexError("no strategy with label '%s'" % st)

            elif type(st) is Strategy:
                if self.player_number != st.player.player_number:
                    raise IndexError("Strategy player mismatch")
                for i in range(1, len(self) + 1):
                    s = Strategy()
                    s.strategy = self.player.deref().GetStrategy(i)
                    if s == st:
                        return s

                raise IndexError("no strategy '%s'" % st)
    
            else:
                raise IndexError("no strategy matches" % st)

    property player_number:
        def __get__(self):
            return self.player.deref().GetNumber()-1