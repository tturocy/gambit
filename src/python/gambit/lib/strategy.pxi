cdef class Strategy:
    cdef c_GameStrategy strategy

    def __repr__(self):
        return "<Strategy [%d] '%s' for player '%s' in game '%s'>" % \
                (self.strategy.deref().GetNumber()-1, self.label,
                 self.strategy.deref().GetPlayer().deref().GetLabel().c_str(),
                 self.strategy.deref().GetPlayer().deref().GetGame().deref().GetTitle().c_str())
    
    def __richcmp__(Strategy self, other, whichop):
        if isinstance(other, Strategy):
            if whichop == 2:
                return self.strategy.deref() == ((<Strategy>other).strategy).deref()
            elif whichop == 3:
                return self.strategy.deref() != ((<Strategy>other).strategy).deref()
            else:
                raise NotImplementedError
        else:
            if whichop == 2:
                return False
            elif whichop == 3:
                return True
            else:
                raise NotImplementedError

    def __hash__(self):
        return long(<long>self.strategy.deref())

    property label:
        def __get__(self):
            return self.strategy.deref().GetLabel().c_str()
        def __set__(self, char *value):
            # check to see if the player's name has been used elsewhere
            c = Strategies()
            c.player = self.strategy.deref().GetPlayer()
            
            # variable to controlling raising duplicate label name after assignment
            raise_exception = 0
            
            for i in range(0, len(c)):
                d = c[i]
                if d.label == value and d != self:
                    raise_exception = 1
            
            cdef cxx_string s
            s.assign(value)
            self.strategy.deref().SetLabel(s)

            if raise_exception:
                raise Warning("This player has another strategy with an identical label")

    property strategy_number:
        def __get__(self):
            return self.strategy.deref().GetNumber()-1