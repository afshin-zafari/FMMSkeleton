#!/bin/bash
#SBATCH -A SNIC2017-1-448
#SBATCH -o tests-%j.out
#SBATCH -p node
#SBATCH -t 00:05:00
#SBATCH -N 5
#SBATCH -n 100
#SBATCH -J N05-DLBSIM

P=5;p=$P;q=1

# 1MB   2M    5M   10M   20M  50M   100M
x=353; #500; 790; 1118; 1581; 2500;  3535

for KK in 1 3 5 7
do
  K=$[ $KK * $P / 10 ] 
  if [ "$K" == "0" ] ; then 
    K=1
  fi
  LAMBDA_BAR=5
  W=1000
  source ./main.sh
done
