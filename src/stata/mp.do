/* mpGWH.do command file for Asymmetric matching pennies */

log using mpGWH.log, replace
set more off
about

* flag whether to do check of code and GAMBIT calls
global DoCheck "y"

* use fake data near NE or near observed GHP data
global DoFakeNE "y"

* name of plugin
global gambit "gambit_64bit_Win7"

* make sure all is clean
capture drop Gpay*
capture drop pay_*
capture program drop $gambit

* get the GAMBIT plugin for this system
program $gambit, plugin


* check the GAMBIT call with simple evaluations
if "$DoCheck" == "y" {

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

}



* do the maximum likelihood estimation

* generate some fake data for the game, close to the NE or close to the observed GHP Game 4 data
set obs 10000
generate tmp=uniform()

if "$DoFakeNE" == "y" {

    generate r1=1 if tmp>0.53
    replace  r1=0 if tmp<=0.53
    generate r2=0
    replace  r2=1 if r1==0

    generate c1=1 if tmp>0.53
    replace  c1=0 if tmp<=0.53
    generate c2=0
    replace  c2=1 if c1==0

}
else {

    generate r1=1 if tmp>0.35
    replace  r1=0 if tmp<=0.35
    generate r2=0
    replace  r2=1 if r1==0

    generate c1=1 if tmp>0.47
    replace  c1=0 if tmp<=0.47
    generate c2=0
    replace  c2=1 if c1==0

}
summ

* generate payoff matrix
generate pay_r1c1_1=200
generate pay_r1c1_2=160

generate pay_r1c2_1=370
generate pay_r1c2_2=200

generate pay_r2c1_1=160
generate pay_r2c1_2=10

generate pay_r2c2_1=10
generate pay_r2c2_2=370

generate qre=0

global strategies "r1 r2 c1 c2"
global payoffs    "pay_r1c1_1  pay_r1c1_2  pay_r1c2_1  pay_r1c2_2  pay_r2c1_1  pay_r2c1_2  pay_r2c2_1  pay_r2c2_2"

* this program calls GAMBIT from within the ML
program define MLqre, rclass

    args lnf r_

    tempvar r c1 c2 r1 r2 lnf_c1 lnf_c2 lnf_r1 lnf_r2 p_c1 p_c2 p_r1 p_r2 tmpc1 tmpc2 tmpr1 tmpr2 pay_r1c1_1 pay_r1c1_2 pay_r1c2_1 pay_r1c2_2 pay_r2c1_1 pay_r2c1_2 pay_r2c2_1 pay_r2c2_2 pay_min pay_max

    qui {

        * constraints on r
        generate double `r' = $rLO + ($rHI-$rLO)*(1/(1+exp(`r_')))

        * get the data in place
        generate double `c1' = $ML_y1
        generate double `c2' = $ML_y2
        generate double `r1' = $ML_y3
        generate double `r2' = $ML_y4

        generate double `pay_r1c1_1' = $ML_y5
        generate double `pay_r1c1_2' = $ML_y6

        generate double `pay_r1c2_1' = $ML_y7
        generate double `pay_r1c2_2' = $ML_y8

        generate double `pay_r2c1_1' = $ML_y9
        generate double `pay_r2c1_2' = $ML_y10

        generate double `pay_r2c2_1' = $ML_y11
        generate double `pay_r2c2_2' = $ML_y12

        generate double `pay_min' = $ML_y13
        generate double `pay_max' = $ML_y14

        * get the frequency counts
        foreach S in r1 r2 c1 c2 {
            summ `S'
            local data_`S'=r(sum)
        }
        di "Evaluating likelihood for data frequencies: " _col(50) `data_r1' _col(70) `data_r2' _col(90) `data_c1' _col(110) `data_c2'

        * generate the data file for GAMBIT
        foreach S in r1 r2 c1 c2 {
            generate double `tmp`S''=`data_`S''
        }
        outfile `tmpr1' `tmpr2' `tmpc1' `tmpc2' using gambit.dat in 1/1, nolabel comma wide replace 

        * transform the payoffs and write them out
        foreach R in r1 r2 {
        foreach C in c1 c2 {
        foreach P in 1 2 {
            replace `pay_`R'`C'_`P'' = ((`pay_`R'`C'_`P'')^(1-`r') - (`pay_min')^(1-`r')) / ((`pay_max')^(1-`r') - (`pay_min')^(1-`r'))
            summ `pay_`R'`C'_`P''
            local payoff_`R'`C'_`P'=r(mean)
        }
        }
        }

        * now define utility payoffs for GAMBIT
        .g.setpayoff 1 `payoff_r1c1_1' 1 1
        .g.setpayoff 2 `payoff_r1c1_2' 1 1
        .g.setpayoff 1 `payoff_r1c2_1' 1 2
        .g.setpayoff 2 `payoff_r1c2_2' 1 2
        .g.setpayoff 1 `payoff_r2c1_1' 2 1
        .g.setpayoff 2 `payoff_r2c1_2' 2 1
        .g.setpayoff 1 `payoff_r2c2_1' 2 2
        .g.setpayoff 2 `payoff_r2c2_2' 2 2
 
        * send GAMBIT the average behavior
        summ `r1'
        local r1plays = r(mean)
        summ `r2'
        local r2plays = r(mean)
        summ `c1'
        local c1plays = r(mean)
        summ `c2'
        local c2plays = r(mean)
        .mp = .g.qre `r1plays' `r2plays' `c1plays' `c2plays'

        * and the GAMBIT output is...
        di "r:`r' lambda:`.mp.lambda' logL:`.mp.logL'"
        local lambda = `.mp.lambda'

        * get the likelihood contributions of each player
        generate double `lnf_r1' = 0
        generate double `lnf_r2' = 0
        replace `lnf_r1' = ln(`r1_qre') if `r1'==1 & `r1_qre'>0
        replace `lnf_r2' = ln(`r2_qre') if `r2'==1 & `r2_qre'>0
        generate double `lnf_c1' = 0
        generate double `lnf_c2' = 0
        replace `lnf_c1' = ln(`c1_qre') if `c1'==1 & `c1_qre'>0
        replace `lnf_c2' = ln(`c2_qre') if `c2'==1 & `c2_qre'>0
        summ r1 r2 c1 c2

        * get the joint likelihood for the game
        replace `lnf' = `lnf_r1' + `lnf_r2' + `lnf_c1' + `lnf_c2' 
        di "Evaluating likelihood for lambda of " `lambda' ": " `r1_qre' "  " `r2_qre' "  " `c1_qre' "  " `c2_qre' " giving LL of " `lnf' " with components " `lnf_r1' " " `lnf_r2' " " `lnf_c1' " " `lnf_c2'

        * save stuff for later display
        global lambda = "`lambda'"
        foreach S in r1 r2 c1 c2 {
            global qre_`S' = "``S'_qre'"
        }

    }

end

summ $strategies

* set bounds on r to skip discontinuity at r=0.09
global rLO=0.15
global rHI=0.99

set rmsg on
ml model lf MLqre (r_: $strategies $payoffs pay_min pay_max = ), maximize technique(dfp)
set rmsg off
ml graph
ml display

di "The MLE lambda implied by the MLE risk aversion coefficient is: " %8.3f $lambda
di "The QRE probabilities at that MLE lambda are: " _col(50) %8.3f $qre_r1 _col(70) %8.3f $qre_r2 _col(90) %8.3f $qre_c1 _col(110) %8.3f $qre_c2

nlcom (r: $rLO + ($rHI-$rLO)*(1/(1+exp([r_]_b[_cons]))0 )
