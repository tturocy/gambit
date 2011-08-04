cdef class Game:
    cdef c_Game game

    def __str__(self):
        return "<Game '%s'>" % self.title

    def __repr__(self):
        return self.write()

    def __richcmp__(Game self, other, whichop):
        if isinstance(other, Game):
            if whichop == 2:
                return self.game.deref() == ((<Game>other).game).deref()
            elif whichop == 3:
                return self.game.deref() != ((<Game>other).game).deref()
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
        return long(<long>self.game.deref())

    property is_tree:
        def __get__(self):
            return True if self.game.deref().IsTree() != 0 else False

    property title:
        def __get__(self):
            return self.game.deref().GetTitle().c_str()
        def __set__(self, char *value):
            cdef cxx_string s
            s.assign(value)
            self.game.deref().SetTitle(s)

    property players:
        def __get__(self):
            cdef Players p
            p = Players()
            p.game = self.game
            return p

    property outcomes:
        def __get__(self):
            cdef Outcomes c
            c = Outcomes()
            c.game = self.game
            return c

    property contingencies:
        def __get__(self):
            return gambit.gameiter.Contingencies(self)

    property root:
        def __get__(self):
            cdef Node n
            n = Node()
            n.node = self.game.deref().GetRoot()
            return n

    def _get_contingency(self, *args):
        cdef c_PureStrategyProfile *psp
        cdef Outcome outcome
        psp = new_PureStrategyProfile(self.game)
        
        
        for (pl, st) in enumerate(args):
            psp.SetStrategy(self.game.deref().GetPlayer(pl+1).deref().GetStrategy(st+1))

        outcome = Outcome()
        outcome.outcome = psp.GetOutcome()
        del_PureStrategyProfile(psp)
        return outcome



    # As of Cython 0.11.2, cython does not support the * notation for the argument
    # to __getitem__, which is required for multidimensional slicing to work. 
    # We work around this by providing a shim.
    def __getitem__(self, i):
        if type(i) is tuple:
            acceptable_types = [str,int,Strategy]
            element_types = type(i[0])
            for item in i:
                if type(item) != element_types:
                    raise TypeError("All elements of the selection must be of the same type")
                elif type(item) not in acceptable_types:
                    raise TypeError("Elements of the selection must be ints, strategy labels, or strategies")
            if element_types == int:        
                return self._get_contingency(*i)
            elif element_types == str:
                strategy_num_list = []
                step_counter = 0
                for item in i:
                    strategy_num_list.append(self.players[step_counter].strategies[item].strategy_number)
                    step_counter += 1
                return self._get_contingency(*tuple(strategy_num_list))
            elif element_types == Strategy:
                strategy_num_list = []
                step_counter = 0
                for item in i:
                    strategy_num_list.append(self.players[step_counter].strategies[item].strategy_number)
                    step_counter += 1
                return self._get_contingency(*tuple(strategy_num_list))
        else:
            raise IndexError("Must use a tuple of ints, strategy labels, or strategies")
            


    def mixed_profile(self):
        cdef MixedStrategyProfileDouble msp
        msp = MixedStrategyProfileDouble()
        msp.profile = new_MixedStrategyProfileDouble(self.game)
        return msp
 
    def num_nodes(self):
        return self.game.deref().NumNodes()

    def write(self, strategic=False):
        if strategic or not self.is_tree:
            return WriteGame(self.game, 1).c_str()
        else:
            return WriteGame(self.game, 0).c_str()
