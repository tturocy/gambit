/*
 * homhp2001_nfg_module.h
 *
 *  Created on: May 18, 2011
 *      Author: jbecker
 */

#ifndef HOMHP2001_NFG_MODULE_H_
#define HOMHP2001_NFG_MODULE_H_

class Homhp2001NFGModule {
public:
	virtual ~Homhp2001NFGModule () { };
	void performCalculation () { doCalculation (); }

private:
	virtual void doCalculation () = 0;
};

#endif /* HOMHP2001_NFG_MODULE_H_ */
