/* mpGWH.do command file for Asymmetric matching pennies */

log using mp.log, replace
set more off
about

capture drop Gpay*
capture drop pay_*

capture program drop gambit
program gambit, plugin
set obs 1

generate pay_r1c1_1=200 
generate pay_r1c1_2=160
generate pay_r1c2_1=160
generate pay_r1c2_2=10
generate pay_r2c1_1=370 
generate pay_r2c1_2=200
generate pay_r2c2_1=10 
generate pay_r2c2_2=370

egen pay_min=rowmin(pay_*) 
egen pay_max=rowmax(pay_*)

save tmp, replace

.g = .game.new
.g.create 2 2

file open results using results.txt, write text replace

forvalues r=-0.1(0.01)0.95 {

  * transform the payoffs
  foreach R in r1 r2 { 
  foreach C in c1 c2 { 
  foreach P in 1 2 {
    qui generate Gpay_`R'`C'_`P' = ((pay_`R'`C'_`P')^(1-`r') - (pay_min)^(1-`r')) / ((pay_max)^(1-`r') - (pay_min)^(1-`r'))
    qui summ Gpay_`R'`C'_`P'    
    local payoff_`R'`C'_`P'=r(mean)
  } 
  } 
  }  
  drop Gpay*
  .g.setpayoff 1 `payoff_r1c1_1' 1 1
  .g.setpayoff 2 `payoff_r1c1_2' 1 1
  .g.setpayoff 1 `payoff_r1c2_1' 1 2
  .g.setpayoff 2 `payoff_r1c2_2' 1 2
  .g.setpayoff 1 `payoff_r2c1_1' 2 1
  .g.setpayoff 2 `payoff_r2c1_2' 2 1
  .g.setpayoff 1 `payoff_r2c2_1' 2 2
  .g.setpayoff 2 `payoff_r2c2_2' 2 2
 
  .mp = .g.qre .47 .53 .65 .35
  di "r:`r' lambda:`.mp.lambda' logL:`.mp.logL'"

  * save the results
  file write results (`r') " " (`.mp.lambda') " " (`.mp.logL') _n
  classutil drop .mp

}

file close results

* now collect the results
infile r lambda LLmle using results.txt, clear
list, noobs
summ LLmle
summ r lambda LLmle if LLmle==r(max)

* adjust to match N=340 of Goeree et al.
di "LL value with N of Georee et al.: " -1.338799*340
di "Mu of Goeree et al.:	" 1/6.723996

* some pictures
twoway (connected LLmle r), ytitle(Log Likelihood) ylabel(, angle(horizontal)) xlabel(-0.1(.1)1) xline(0.45, lpattern(dash)) title(Maximum Likelihood QRE for Varying Risk Attitudes) subtitle("r = 0 is RN, r>0 is RA and r<0 is RL") saving(MLll, replace)
twoway (connected lambda r), ytitle(Lambda) ylabel(, angle(horizontal)) xlabel(-0.1(.1)1) xline(0.45, lpattern(dash)) title(Maximum Likelihood QRE for Varying Risk Attitudes) subtitle("r = 0 is RN, r>0 is RA and r<0 is RL") saving(MLlambda, replace)
