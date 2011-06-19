/*
 * homhp2001_nfg_module_impl.h
 *
 *  Created on: May 18, 2011
 *      Author: jbecker
 */

#ifndef HOMHP2001_NFG_MODULE_IMPL_H_
#define HOMHP2001_NFG_MODULE_IMPL_H_

#include "libgambit/libgambit.h"
#include <tr1/memory>

class Homhp2001NFGModuleImpl
: public Homhp2001NFGModule {
public:
	Homhp2001NFGModuleImpl (std::tr1::shared_ptr<const Gambit::StrategySupport> theSupport);
	virtual ~Homhp2001NFGModuleImpl () { };

private:
	const std::tr1::shared_ptr<const Gambit::StrategySupport> support;
	virtual void doCalculation ();
};

#endif /* HOMHP2001_NFG_MODULE_IMPL_H_ */
