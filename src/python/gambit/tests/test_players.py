import gambit
import warnings
from nose.tools import assert_raises

class TestGambitPlayers(object):
    def setUp(self):
        self.strategic_game = gambit.new_table([2,2])
        self.strategic_game.players[0].label = "Alphonse"
        self.strategic_game.players[1].label = "Gaston"
        self.extensive_game = gambit.new_tree()
    
    def tearDown(self):
        del self.strategic_game
        
    def test_initial_player_count(self):
        "Test to ensure 0 initial players"
        assert len(self.strategic_game.players) == 2
        
    def test_game_add_players(self):
        "Test to add player"
        p1 = self.strategic_game.players[0]
        assert p1.label == "Alphonse"
        p2 = self.strategic_game.players[1]
        assert p2.label == "Gaston"

    def test_game_add_duplicate_player_names(self):
        "Test to add player with preexisting name"
        with warnings.catch_warnings(True) as w:
                self.strategic_game.players[1].label = "Alphonse"
                assert str(w[0].message) == "Another player with an identical label exists"
        p1 = self.strategic_game.players[0]
        assert p1.label == "Alphonse"
        p2 = self.strategic_game.players[1]
        assert p2.label == "Alphonse"

    def test_game_players_index_by_string(self):
        "Test find a player by providing a label"
        p1 = self.strategic_game.players["Alphonse"]
        assert p1.label == "Alphonse"
        p2 = self.strategic_game.players["Gaston"]
        assert p2.label == "Gaston"

    def test_game_players_index_exception_int(self):
        "Test to verify when an index is out of range"
        assert_raises(IndexError, self.strategic_game.players.__getitem__, 3)

    def test_game_players_index_exception_string(self):
        "Test to verify when a player label is not in the list of players"
        assert_raises(IndexError, self.strategic_game.players.__getitem__, "None")

    def test_game_players_index_exception_player(self):
        "Test to verify when a player object is not in the players"
        self.game_2 = gambit.new_table([2,2])
        assert_raises(IndexError, self.strategic_game.players.__getitem__, self.game_2.players[0])

    def test_game_players_index_exception_player(self):
        "Test to verify when attempting to retrieve with invalid input"
        assert_raises(TypeError, self.strategic_game.players.__getitem__, 1.3)

    def test_game_add_strategies_to_player_strategic_game(self):
        "Test to add a strategy to a player in a strategic game"
        assert len(self.strategic_game.players[0].strategies) == 2
        self.strategic_game.players[0].strategies.add("1st new strategy")
        assert len(self.strategic_game.players[0].strategies) == 3

        with warnings.catch_warnings(True) as w:
            self.strategic_game.players[0].strategies.add("1st new strategy")
            assert str(w[0].message) == "This player has another strategy with an identical label"
        assert len(self.strategic_game.players[0].strategies) == 4

    def test_game_add_strategies_to_player_extensive_game(self):
        "Test to ensure that an exception is raised when attempting to add a strategy to a player in an extensive game"
        self.extensive_game.players.add("Alice")
        assert_raises(TypeError, self.extensive_game.players[0].strategies.add, "Test")