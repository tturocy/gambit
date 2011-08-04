import gambit
from nose.tools import assert_raises

class TestGambitGameOutcomes(object):
	def setUp(self):
		self.game = gambit.new_table([2,2])
	
	def tearDown(self):
		del self.game

	def test_outcomes_number_index(self):
		"Test to verify outcome labels and indexing"
		self.game.outcomes[0].label = "trial"
		self.game.outcomes[1].label = "trial 2"
		
		assert self.game.outcomes[0].label == "trial"
		assert self.game.outcomes[1].label == "trial 2"
		
		print "kkkkkkk"
		print self.game.outcomes[1.2]
		#assert_raises(IndexError, self.game.outcomes[1.2]) 
		#assert self.game.outcomes["trial"].label == "trial"
		
