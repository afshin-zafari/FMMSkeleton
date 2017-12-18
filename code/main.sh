set -x
COMP=intel
#----------------------------------------------
LAMBDA_STAR=$[ 2 * ${LAMBDA_BAR} ]
DLB_PARS="--dlb --silent-dur 10 --dlb-threshold ${LAMBDA_BAR} --failure-max 5 --silent-mode 1 "
#DLB_PARS=""

M=$[ $x * $x * 8 ]; 

B=$[ ${LAMBDA_STAR} * $K ]
N=$[ $x * $B ]
b=5
nt=20;#$[ 20 / $P ];

ipn=1;
iter=1

#assert ( B >= p ) 
#assert ( B >= q )




#====================================
set +x
module load gcc openmpi
#module load intel intelmpi

JobID=${SLURM_JOB_ID}
set -x

app=../bin/${COMP}_dlbsim_debug
params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn -lambda ${LAMBDA_BAR} --lambda_star ${LAMBDA_STAR} -K $K -W $W --iter-no $iter --timeout 100 ${DLB_PARS}" 
echo "Params: $params"

tempdir=./temp
mkdir -p $tempdir

orgdir=$(pwd)

outfile=$tempdir/tests_${KK}_${K}_${P}_${COMP}_${JobID}.out
cd $tempdir
pwd

echo "========================================================================================="
set -x 

date
if [ "z${CXX}z" == "zg++z" ] ; then 
	mpirun -n $P --map-by ppr:$ipn:node --output-filename $outfile   $app ${params}
else
	srun  --ntasks-per-node=$ipn -n $P -c $nt -m cyclic:cyclic:* -l --output $outfile $app ${params}
fi
rm *file*.txt *.log pend*.dat comm*.txt
cd $orgdir
date
