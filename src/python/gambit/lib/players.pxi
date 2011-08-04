cdef class Players:
    cdef c_Game game

    def __repr__(self):
        return str(list(self))

    def __len__(self):
        return self.game.deref().NumPlayers()

    def __getitem__(self, pl):
        cdef Player p

        if type(pl) is int:
            if pl < 0 or pl >= len(self):
                raise IndexError("no player with index '%s'" % pl)
            p = Player()
            p.player = self.game.deref().GetPlayer(pl+1)
            return p
    
        elif type(pl) is str:
            for i in range(1, len(self) + 1):
                c = Player()
                c.player = self.game.deref().GetPlayer(i)
                if c.label == pl:
                    return c

            raise IndexError("no Player with label '%s'" % pl)
        
        elif type(pl) is Player:
            for i in range(1, len(self) + 1):
                c = Player()
                c.player = self.game.deref().GetPlayer(i)
                if c == pl:
                    return c

            raise IndexError("Player does not exist in this game '%s'" % pl)

        else:
            raise IndexError("Invalid input to find Player. Please use ints, strings, or Player objects")

    def add(self, label=""):
        cdef Player p
        p = Player()
        p.player = self.game.deref().NewPlayer()
        p.label = str(label)
        return p

    property chance:
        def __get__(self):
            cdef Player p
            p = Player()
            p.player = self.game.deref().GetChance()
            return p
