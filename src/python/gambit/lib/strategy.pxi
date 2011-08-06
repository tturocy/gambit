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
            
            if value in [i.label for i in c]:
                warnings.warn("This player has another strategy with an identical label")

            cdef cxx_string s
            s.assign(value)
            self.strategy.deref().SetLabel(s)


    property strategy_number:
        def __get__(self):
            return self.strategy.deref().GetNumber()-1

    property player:
        def __get__(self):
            p = Player()
            p.player = self.strategy.deref().GetPlayer()
