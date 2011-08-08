import gambit

class TestGambitMixedStrategyGame(object):
	def setUp(self):
		self.game = gambit.new_table([2,2])
		self.game.players[0].label = "joe"
		self.game.players["joe"].strategies[0].label = "cooperate"
		self.game.players[1].label = "dan"
		self.game.players["dan"].strategies[1].label = "defect"
		
		self.p = self.game.mixed_profile()
		
	def tearDown(self):
		del self.game
		
			

	def test_getting_profile(self):
		"Test to ensure that payoffs are returned correctly"
		assert self.p.payoff(self.game.players[0]) == 0.0

	def test_payoffs(self):
		"Test to ensure that payoffs are returned correctly"
		assert self.p.payoff(self.game.players[0]) == 0.0
		
	def test_finding_payoffs_by_string(self):
		"Test to find payoffs by string values"
		assert self.p.payoff("joe") == 0.0	

	def test_strategy_value(self):
		"Test to retrieve strategy value based on given strategy"
		assert self.p.strategy_value(self.game.players[0].strategies[1]) == 0.0	

	def test_strategy_value_by_string(self):
		"Test expected payoff based on given strategy"
		assert self.p.strategy_value("defect") == 0.0
				
	def test_strategy_values(self):
		"Test retrieving strategy values based on player"
		assert self.p.strategy_values(self.game.players[0]) == [0.0, 0.0]

	def test_strategy_values_by_string(self):
		"Test retrieving strategy values based on player"
		assert self.p.strategy_values("joe") == [0.0, 0.0]
