//#
//# FILE: efcsum.cc -- Constant Sum Extensive Form Game Solution Module
//#
//# $Id$
//#

#include "rational.h"
#include "gwatch.h"
#include "gpvector.h"

#include "efg.h"
#include "efgiter.h"

#include "efcsum.h"

//---------------------------------------------------------------------------
//                        EFCSumParams: member functions
//---------------------------------------------------------------------------

CSSeqFormParams::CSSeqFormParams(gStatus &status_) 
  :  trace(0), stopAfter(0), tracefile(&gnull), status(status_)
{ }


//-------------------------------------------------------------------------
//                    CSSeqFormModule<T>: Member functions
//-------------------------------------------------------------------------

template <class T>
CSSeqFormModule<T>::CSSeqFormModule(const Efg<T> &E, const CSSeqFormParams &p,
			  const EFSupport &S)
  : EF(E), params(p), support(S), A(0), b(0), c(0), npivots(0), 
    ns1(S.NumSequences(1)), ns2(S.NumSequences(2)),
    ni1(E.PlayerList()[1]->NumInfosets()+1),  
    ni2(E.PlayerList()[2]->NumInfosets()+1)
{ }

template <class T> CSSeqFormModule<T>::~CSSeqFormModule()
{ if(A) delete A; if(b) delete b; if(c) delete c;}

template <class T> bool CSSeqFormModule<T>::IsConstSum()
{ 
  return EF.IsConstSum();
}

template <class T> void CSSeqFormModule<T>::Make_Abc()
{
  A = new gMatrix<T>(1,ns1+ni2,ns2+ni1);
  b = new gVector<T>(1,ns1+ns2);
  c = new gVector<T>(1,ns2+ni1);

  maxpay = EF.MaxPayoff()+(T)1; 

  (*A) = (T)0;
  T prob = (T)1;
  FillTableau(EF.RootNode(),prob,1,1,0,0);

  (*b) = (T)0;
  (*b)[ns1+1] = -(T)1;

  (*c) = (T)0;
  (*c)[ns2+1] = (T)1;

//  gout.SetWidth(1).SetPrec(1);
  gout << "\nA";
  A->Dump(gout);
  gout << "\nb";
  b->Dump(gout);
  gout << "\nc";
  c->Dump(gout);
}

template <class T> int CSSeqFormModule<T>::CSSeqForm(int /*dup*/)
{
  BFS<T> cbfs((T) 0);
  
  if (EF.NumPlayers() != 2 || !params.tracefile)   return 0;
  if(!IsConstSum()) return 0;;
  gWatch watch;
  
  List = BFS_List();  // Ted -- is this necessary?
  
  Make_Abc();
  LPSolve<T> LP(*A,*b,*c,ni2);
//  Add_BFS(LP); 
  
  npivots = LP.NumPivots();
  time = watch.Elapsed();
  return List.Length();
}

/*
template <class T> int CSSeqFormModule<T>::Add_BFS(const LPSolve<T> &lp)
{
  BFS<T> cbfs((T) 0);
  T value;

  lp.OptBFS(cbfs);
  value=cbfs(EF.NumStrats(1)+1)+maxpay;
  cbfs.Remove(EF.NumStrats(1)+1);
  cbfs.Remove(-EF.NumStrats(2)-1);
  cbfs.Dump(gout);
  gout << "\nvalue =" << value << "\n\n";
  if (List.Contains(cbfs))  return 0;
//  if(params.trace >=2) (*params.tracefile) << "\nFound CBFS";
//  (*params.tracefile)  << "\nB = ";
//  B.Dump(*params.tracefile);
//  (*params.tracefile)  << "\ncbfs = ";
//  cbfs.Dump(*params.tracefile );
  List.Append(cbfs);
  
  return 1;
}
*/

/*
template <class T> void CSSeqFormModule<T>
::GetProfile(gDPVector<T> &v, const gVector<T> &sol,
	       const Node *n, int s1,int s2)
{

  int i,pl,inf,snew,ind,ind2;
//  T eps;
//  eps = tab->Epsilon();
//  gout << "\nv = " << v;
  if(n->GetInfoset()) {
    if(n->GetPlayer()->IsChance()) {
      for(i=1;i<=n->NumChildren();i++)
	GetProfile(v,sol,n->GetChild(i),s1,s2);
    }
    pl = n->GetPlayer()->GetNumber();
    inf= n->GetInfoset()->GetNumber();
//    gout << "\niset: (" << pl << "," << inf << ")"; 
    if(pl==1) {
      snew=1;
      for(i=1;i<inf;i++)
	snew+=n->GetPlayer()->InfosetList()[i]->NumActions(); 
      for(i=1;i<=n->NumChildren();i++) {
	v(pl,inf,i) = (T)0;
//	gout << "\npl1 ";
//	gout << "\n  v = " << v;
	if(tab->Member(s1)) {
	  ind = tab->Find(s1);
//	  if(sol[ind]!=(T)0) {
	  if(sol[ind]<-eps) {
	    if(tab->Member(snew+i)) {
	      ind2 = tab->Find(snew+i);
	      if(sol[ind2]<-eps)
		v(pl,inf,i) = sol[ind2]/sol[ind];
//	      gout << "\nind: " << ind << " " << sol[ind] << " ";
//	      gout << "\nind2: " << ind2 << " " << sol[ind2] << " ";
//	      gout << "\n  v = " << v;
	    }
	  } 
	} 
	GetProfile(v,sol,n->GetChild(i),snew+i,s2);
      }
    }
    if(pl==2) {
      snew=1;
      for(i=1;i<inf;i++)
	snew+=n->GetPlayer()->InfosetList()[i]->NumActions(); 
      for(i=1;i<=n->NumChildren();i++) {
	v(pl,inf,i) = (T)0;
//	gout << "\npl2 ";
//	gout << "\n  v = " << v;
	if(tab->Member(ns1+s2)) {
	  ind = tab->Find(ns1+s2);
//	  if(sol[ind]!=(T)0) {
	  if(sol[ind]<-eps) {
	    if(tab->Member(ns1+snew+i)) {
	      ind2 = tab->Find(ns1+snew+i);
	      if(sol[ind2]<-eps)
		v(pl,inf,i) = sol[ind2]/sol[ind];
//	      gout << "\nind: " << ind << " " << sol[ind] << " ";
//	      gout << "\nind2: " << ind2 << " " << sol[ind2] << " ";
//	      gout << "\n  v = " << v;
	    }
	  } 
	} 
	GetProfile(v,sol,n->GetChild(i),s1,snew+i);
      }
    }
  }
}
*/

template <class T> void CSSeqFormModule<T>
::FillTableau(const Node *n, T prob,int s1,int s2, int i1,int i2)
{
//  gout << "\ns1,s2,i1,i2: " << s1 << " " << s2  << " " << i1  << " " << i2;
//  gout << " prob = " << prob;
  int i,snew;
  if(n->GetOutcome()) {
    (*A)(s1,s2) = (*A)(s1,s2) +
       prob*(((OutcomeVector<T> &) *n->GetOutcome())[1] -maxpay);
  }
  if(n->GetInfoset()) {
    if(n->GetPlayer()->IsChance()) {
      for(i=1;i<=n->NumChildren();i++)
	FillTableau(n->GetChild(i),
		    prob*((ChanceInfoset<T> *)n->GetInfoset())\
		    ->GetActionProb(i),s1,s2,i1,i2);
    }
    int pl = n->GetPlayer()->GetNumber();
    if(pl==1) {
      i1=n->GetInfoset()->GetNumber();
      snew=1;
      for(i=1;i<i1;i++)
	snew+=n->GetPlayer()->InfosetList()[i]->NumActions();
      (*A)(s1,ns2+i1+1) = -(T)1;
      for(i=1;i<=n->NumChildren();i++) {
	(*A)(snew+i,ns2+i1+1) = (T)1;
	FillTableau(n->GetChild(i),prob,snew+i,s2,i1,i2);
      }
    }
    if(pl==2) {
      i2=n->GetInfoset()->GetNumber();
      snew=1;
      for(i=1;i<i2;i++)
	snew+=n->GetPlayer()->InfosetList()[i]->NumActions();
      (*A)(ns1+i2+1,s2) = (T)1;
      for(i=1;i<=n->NumChildren();i++) {
	(*A)(ns1+i2+1,snew+i) = -(T)1;
	FillTableau(n->GetChild(i),prob,s1,snew+i,i1,i2);
      }
    }
  }
}

template <class T>
void CSSeqFormModule<T>::GetSolutions(gList<BehavSolution<T> > &solutions) const
{
  solutions.Flush();
/*
  int index;
  int n1=support.NumStrats(1);
  int n2=support.NumStrats(2);
  for (int i = 1; i <= List.Length(); i++)    {
    BehavProfile<T> profile(EF, support);
    int j;
    for (j = 1; j <= n1; j++) 
      if (List[i].IsDefined(j))   
	profile(1, j) = List[i](j);
      else  profile(1, j) = (T) 0;

    for (j = 1; j <= n2; j++)
      if (List[i].IsDefined(-j))
	profile(2, j) = List[i](-j);
      else
	profile(2, j) = (T) 0;

    index = solutions.Append(BehavSolution<T>(profile, id_CSSeqForm));
    solutions[index].SetIsNash(T_YES);
  }
*/
}

template <class T> long CSSeqFormModule<T>::NumPivots(void) const
{
  return npivots;
}

template <class T> double CSSeqFormModule<T>::Time(void) const
{
  return time;
}

#ifdef __GNUG__
template class CSSeqFormModule<double>;
template class CSSeqFormModule<gRational>;
#elif defined __BORLANDC__
#pragma option -Jgd
class CSSeqFormModule<double>;
class CSSeqFormModule<gRational>;
#pragma option -Jgx
#endif   // __GNUG__, __BORLANDC__


//-------------------------------------------------------------------------
//                    Convenience functions for CSSeqForm
//-------------------------------------------------------------------------

template <class T>
int CSSeqForm(const Efg<T> &E, const CSSeqFormParams &p,
	  gList<BehavSolution<T> > &solutions,
	  long &npivots, gRational &time)
{
  EFSupport S(E);
  CSSeqFormModule<T> LM(E, p, S);
  int result = LM.CSSeqForm();

  npivots = LM.NumPivots();
  time = LM.Time();
  
  LM.GetSolutions(solutions);

  return result;
}

#ifdef __GNUG__
template int CSSeqForm(const Efg<double> &, const CSSeqFormParams &,
		   gList<BehavSolution<double> > &, long &, gRational &);
template int CSSeqForm(const Efg<gRational> &, const CSSeqFormParams &,
		   gList<BehavSolution<gRational> > &, long &, gRational &);
#elif defined __BORLANDC__
#pragma option -Jgd
int CSSeqForm(const Efg<double> &, const CSSeqFormParams &,
	  gList<BehavProfile<double> > &, long &, gRational &);
int CSSeqForm(const Efg<gRational> &, const CSSeqFormParams &,
	  gList<BehavProfile<gRational> > &, long &, gRational &);
#pragma option -Jgx
#endif   // __GNUG__, __BORLANDC__









