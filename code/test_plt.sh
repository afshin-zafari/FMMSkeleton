#!/bin/bash
#SBATCH -A SNIC2017-1-448
#SBATCH -o tests-%j.out
#SBATCH -p devel
#SBATCH -t 00:05:00
#SBATCH -N 1
#SBATCH -n 20
#SBATCH -J N01-DLBSIM

ml add python/3.6.0
export MPLBACKEND="agg"
python3.6 -c "import pylab as plt; plt.plot(range(10),range(10)); plt.savefig('test.pdf')"

