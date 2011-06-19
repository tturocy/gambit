/*
 * object_needing_helper.h
 *
 *  Created on: May 28, 2011
 *      Author: jbecker
 */

#ifndef OBJ_NEEDING_HELPER_H_
#define OBJ_NEEDING_HELPER_H_

#include <tr1/memory>
#include "helper_in_use_exception.h"



/* ***********************************************************************************************
 * ObjNeedingHelper_Base und ObjNeedingHelper
 * ***********************************************************************************************/

// auxiliary base-class

class ObjNeedingHelper_Base {
protected:
	ObjNeedingHelper_Base () { }

public:
	virtual ~ObjNeedingHelper_Base () { }
};


// class ObjNeedingHelper

template<typename T>
class ObjNeedingHelper
: private ObjNeedingHelper_Base {
public:
	class Helper;

private:
	std::tr1::shared_ptr<Helper> helper;

protected:
	ObjNeedingHelper (std::tr1::shared_ptr<Helper> theHelper)
	throw (HelperInUseException);

public:
	virtual ~ObjNeedingHelper ();

protected:
	static std::tr1::shared_ptr<Helper> createHelper (std::tr1::shared_ptr<T> tool);

	std::tr1::shared_ptr<T> getHelperTool () const;
};



/* ***********************************************************************************************
 * HelperBox
 * ***********************************************************************************************/

template<typename T>
class HelperBox {
private:
	ObjNeedingHelper_Base * objNeedingHelperBase;
	std::tr1::shared_ptr<T> helperTool;

public:
	HelperBox (std::tr1::shared_ptr<T> tool);
	virtual ~HelperBox () { };

	std::tr1::shared_ptr<T> unbox () const { return helperTool; }

	void connect (ObjNeedingHelper_Base * objBase) throw (HelperInUseException);
	void release (ObjNeedingHelper_Base * objBase);
};



template<typename T>
HelperBox<T>::HelperBox (std::tr1::shared_ptr<T> tool)
: objNeedingHelperBase (NULL), helperTool (tool) {
};

template<typename T>
void HelperBox<T>::connect (ObjNeedingHelper_Base * objBase)
throw (HelperInUseException)
{
	if (!objNeedingHelperBase) {
		objNeedingHelperBase = objBase;
	} else {
		throw (HelperInUseException ());
	}
}

template<typename T>
void HelperBox<T>::release (ObjNeedingHelper_Base * objBase)
{
	objNeedingHelperBase = NULL;
}


/* ***********************************************************************************************
 * ObjNeedingHelper
 * ***********************************************************************************************/


// inner class: Helper

template<typename T>
class ObjNeedingHelper<T>::Helper : private HelperBox<T> {
	friend class ObjNeedingHelper;
private:
	Helper (std::tr1::shared_ptr<T> tool) : HelperBox<T> (tool) {
	}
public:
	virtual ~Helper () {   }
};


// implementations

template<typename T>
ObjNeedingHelper<T>::ObjNeedingHelper (std::tr1::shared_ptr<Helper> theHelper)
throw (HelperInUseException)
: ObjNeedingHelper_Base (), helper (theHelper) {
	helper->connect (this);
}

template<typename T>
ObjNeedingHelper<T>::~ObjNeedingHelper () {
	helper->release (this);
}

template<typename T>
std::tr1::shared_ptr<typename ObjNeedingHelper<T>::Helper>
ObjNeedingHelper<T>::createHelper (std::tr1::shared_ptr<T> tool) {
	std::tr1::shared_ptr<typename ObjNeedingHelper<T>::Helper> p (new Helper (tool));
	return p;
}

template<typename T>
std::tr1::shared_ptr<T>
ObjNeedingHelper<T>::getHelperTool () const {
	return helper->unbox ();
}


#endif /* OBJ_NEEDING_HELPER_H_ */
