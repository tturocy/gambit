/*
 * strategy_support_const_extension.h
 *
 *  Created on: 27.05.2011
 *      Author: jbecker
 */

#ifndef STRATEGY_SUPPORT_CONST_EXTENSION_H_
#define STRATEGY_SUPPORT_CONST_EXTENSION_H_

class StrategySupportConstExtension {
private:
	const std::tr1::shared_ptr<const Gambit::StrategySupport> extendedOne;

public:
	StrategySupportConstExtension (std::tr1::shared_ptr<const Gambit::StrategySupport> objToBeExtended)
	: extendedOne (objToBeExtended) {
	}

	virtual ~StrategySupportConstExtension () {   }
/*
	operator const Gambit::StrategySupport & () const {
		return *extendedOne;
	}
	*/
	/*
	// forwards to methods from StrategySupport
	Gambit::Game GetGame() const {
		return extendedOne->GetGame ();
	}

	/// Returns an iterator over the players in the game
	Gambit::GamePlayerIterator Players() const {
		return extendedOne->Players();
	}

	/// Returns an iterator over the strategies for the player
	Gambit::SupportStrategyIterator Strategies(const Gambit::GamePlayer &p_player) const {
		return extendedOne->Strategies (p_player);
	}
*/
	std::tr1::shared_ptr<const Gambit::StrategySupport> getSupport () const {
		return extendedOne;
	}
};


#endif /* STRATEGY_SUPPORT_CONST_EXTENSION_H_ */
