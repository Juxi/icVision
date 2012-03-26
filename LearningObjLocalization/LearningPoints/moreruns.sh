#!/bin/bash
# for (( i = 0; i < 10; i++ )); do
#     cat WedThurAll.csv | perl -wnl -e '@f=<>; END{ foreach $i (reverse 0 .. $#f) { $r=int rand ($i+1); @f[$i, $r]=@f[$r,$i] unless ($i==$r); }  chomp @f; foreach $line (@f){ print $line; }}' >WedThurAll.Shuffled.csv 
#     date >>NightRunX.log
#     python learnX.py >>NightRunX.log
# done

for (( i = 0; i < 10; i++ )); do
    cat WedThurAll.csv | perl -wnl -e '@f=<>; END{ foreach $i (reverse 0 .. $#f) { $r=int rand ($i+1); @f[$i, $r]=@f[$r,$i] unless ($i==$r); }  chomp @f; foreach $line (@f){ print $line; }}' >WedThurAll.Shuffled.csv 
    date >>NightRunY.log
    python learnY.py >>NightRunY.log
done
