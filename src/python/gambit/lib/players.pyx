cdef class Players:
    cdef c_Game game

    def __repr__(self):
        return str(list(self))

    def __len__(self):
        return self.game.deref().NumPlayers()

    def __getitem__(self, pl):
        cdef Player p
        try:
            if pl < 0 or pl >= len(self):
                raise IndexError("no player with index '%s'" % pl)
            p = Player()
            p.player = self.game.deref().GetPlayer(pl+1)
            return p
        except IndexError:
            if type(pl) is str:
                for i in range(1, len(self) + 1):
                    p = Player()
                    p.player = self.game.deref().GetPlayer(i)
                    if p.label == pl:
                        return p
                    # fall through

                raise IndexError("no player with label '%s'" % pl)
            else:
                raise IndexError("no player matches")


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
