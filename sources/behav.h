//
// FILE: behav.h -- Behavioral strategy profile classes
//
// $Id$
//

#ifndef BEHAV_H
#define BEHAV_H

#include "gmisc.h"
#include "gstring.h"
#include "efstrat.h"

class BaseEfg;

class BaseBehavProfile   {
  protected:
    const BaseEfg *E;
    bool truncated;
    EFSupport support;
    BaseBehavProfile(const BaseEfg &, bool trunc);
    BaseBehavProfile(const BaseBehavProfile &);
    BaseBehavProfile(const BaseEfg &, bool trunc, const EFSupport &);
    BaseBehavProfile &operator=(const BaseBehavProfile &);

  public:
    virtual ~BaseBehavProfile();

    DataType Type(void) const;
    virtual bool IsPure(void) const = 0;
    virtual bool IsPure(int pl) const = 0;
    bool IsTruncated(void) const    { return truncated; }
    const EFSupport &GetEFSupport(void) const;
    const gString &GetPlayerName(int p) const;
    const gString &GetInfosetName(int p, int iset) const;
    const gString &GetActionName(int p, int iset, int act) const;
};


#include "gdpvect.h"

template <class T> class Efg;
class Infoset;

template <class T> class BehavProfile
  : public BaseBehavProfile, public gDPVector<T>  {
  private:

    void Payoff(Node *n, T prob, int pl, T &value) const;
    void NodeValues(Node *n, int pl, gArray<T> &valarray,
		    int &index) const;
    void CondPayoff(Node *n, T prob, gPVector<T> &, gDPVector<T> &) const;
    void NodeRealizProbs(Node *n, T prob, int &index, gArray<T> &NRProbs) const;
    void Beliefs(Node *n, T prob, gDPVector<T> &BProbs, 
		 gPVector<T> &gpv) const;

  public:
    BehavProfile(const Efg<T> &, bool truncated = false);
    BehavProfile(const Efg<T> &, const gDPVector<T> &);
    BehavProfile(const EFSupport &);
    BehavProfile(const BehavProfile<T> &);
    virtual ~BehavProfile();

    BehavProfile<T> &operator=(const BehavProfile<T> &);

    bool IsPure(void) const;
    bool IsPure(int pl) const;

    Efg<T> *BelongsTo(void) const   { return (Efg<T> *) E; }

    const T &GetValue(Infoset *s, int act) const;

    T Payoff(int pl) const;
    gArray<T> NodeValues(int pl) const;
    void CondPayoff(gDPVector<T> &value, gPVector<T> &probs) const;
    gArray<T> NodeRealizProbs(void) const;
    gDPVector<T> Beliefs(void) const
;

    T LiapValue(void) const;
    bool IsNash(void) const;
    void Gripe(gDPVector<T> &value) const;
    T MaxGripe() const;

    void Centroid(void) const;

    bool operator==(const BehavProfile<T> &) const;
    

};
#ifndef __BORLANDC__
template <class T> gOutput &operator<<(gOutput &f, const BehavProfile<T> &p);
#endif
#endif   // BEHAV_H
