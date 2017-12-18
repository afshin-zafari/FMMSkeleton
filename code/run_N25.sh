#!/bin/bash
#SBATCH -A SNIC2017-1-448
#SBATCH -o tests-%j.out
#SBATCH -p node
#SBATCH -t 00:15:00
#SBATCH -N 25
#SBATCH -n 500
#SBATCH -J N25-DLBSIM

P=25;p=$P;q=1

nt=20;#$[ 20 / $P ];

# 1MB   2M    5M   10M   20M  50M   100M
x=353; #500; 790; 1118; 1581; 2500;  3535
for KK in 1 3 5 7
do
  K=$[ $KK * $P / 10 ] 
  LAMBDA_BAR=5
  W=1000
  source ./main.sh
done
