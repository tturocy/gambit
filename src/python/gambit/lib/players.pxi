cdef class Players:
    cdef c_Game game

    def __repr__(self):
        return str(list(self))

    def __len__(self):
        return self.game.deref().NumPlayers()

    def __getitem__(self, pl):
        cdef Player p

        if isinstance(pl, int):
            if pl < 0 or pl >= len(self):
                raise IndexError("no player with index '%s'" % pl)
            p = Player()
            p.player = self.game.deref().GetPlayer(pl+1)
            return p
    
        elif isinstance(pl, str):
            for player in self:
                if player.label == pl:
                    return player

            raise IndexError("no Player with label '%s'" % pl)
        
        elif isinstance(pl, Player):
            for player in self:
                if player == pl:
                    return player

            raise IndexError("Player does not exist in this game '%s'" % pl)

        else:
            raise TypeError("Invalid input to find Player. Please use ints, strings, or Player objects")

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
