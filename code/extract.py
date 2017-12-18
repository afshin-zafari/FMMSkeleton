import sys
lines=list()
t1_recs=list()
fb_dec_recs=list()
exp_tasks_recs=list()
recvtask_data_recs=list()
FIND_IDLE,FIND_BUSY=range(2)
fi_t,acc_t,fb_t,impt_t,expt_t=[-1]*5
send_complete_t=-1
expt_l,expd_l=list(),list()
K,p,N,KK,LS=0,0,0,0,0
#------------------------------------------------------------------------------------------------------------------  
#/pica/h1/afshin/DuctTeipAPP/DLB/DLB_Simulation/code/temp/temp/tests_5_2_5_1692518.out/1/rank.0/stderr
def get_pars(fn):
  global K,KK,N,p
  pars=fn.split('tests_')[1]
  ps=pars.split('_')
  KK=int(ps[0])
  K=int(ps[1])
  N=int(ps[2])
  s=fn.split('rank.')[1]
  p=int(s.split('/')[0])                      
#------------------------------------------------------------------------------------------------------------------  
def read_file(fn,filters):
  f=open(fn,'rb')
  for line in f:
    for fil in filters:
      fil(line)
  f.close()
#------------------------------------------------------------------------------------------------------------------  
def fi_filter(line):
  global fi_t,acc_t,t1_recs,K,LS
  if 'lambda_star' in line:
    lines.append(line)
    K=int(line.split(' K=')[1].split(',')[0])
    LS=int(line.split('lambda_star=')[1].split(',')[0])
  #            /dlb.cpp, 199, findIdleNode                    , tid: 77109700,   3015 ::send FIND_IDLE to :2
  if 'send FIND_IDLE' in line:
    lines.append(line)
    fi_t=int(line.split(':')[1].split(',')[1])
    
  #    /eng_process.cpp, 287, processEvent                    , tid: 77109700,   3207 ::ACCEPT  tag recvd from 4
  if '::ACCEPT  tag recvd ' in line:
    lines.append(line)
    acc_t=int(line.split(':')[1].split(',')[1])
    t1_recs.append([FIND_IDLE,acc_t-fi_t])
  return
#------------------------------------------------------------------------------------------------------------------  
def fb_filter(line):
  global fi_t,fb_t,impt_t,t1_recs,fb_dec_recs
  #            /dlb.cpp, 187, findBusyNode                    , tid: F2DFD700,   1534 ::send FIND_BUSY to :3
  if 'send FIND_BUSY' in line:
    lines.append(line)
    fb_t=int(line.split(':')[1].split(',')[1])
    
  #    /eng_process.cpp, 293, processEvent                    , tid: 39203700,   3432 ::TASKS imported from 1
  if '::TASKS imported from' in line:
    lines.append(line)
    impt_t=int(line.split(':')[1].split(',')[1])
    if fb_t>0:
      t1_recs.append([FIND_BUSY,impt_t-fb_t])
      fb_t=-1
  if ' DECLINE' in line:
    lines.append(line)
    dec_t=float(line.split(':')[1].split(',')[1])    
    fb_dec_recs.append(dec_t-max(fb_t,fi_t))
  return
#------------------------------------------------------------------------------------------------------------------  
def export_filter(line):
  global expt_t,send_complete_t,exp_tasks_recs,expt_l,expd_l
  
  if ', exportTask ' in line:
    lines.append(line)
    expt_t=int(line.split(':')[1].split(',')[1])
    expt_l.append(expt_t)
  #        /mailbox.cpp, 259, getEvent                        , tid: BD956700,   3447 ::AnySendCompleted=True, tag:11.
  if '::AnySendCompleted=True, tag:11.' in line:
    lines.append(line)
    send_complete_t = int(line.split(':')[1].split(',')[1])
    expd_l.append(send_complete_t)    
    n=len(expd_l)
    if n % 3 ==0:
      exp_tasks_recs.append(send_complete_t - expt_l[n/3-1])
#------------------------------------------------------------------------------------------------------------------  
def get_task_org_finish(line):
  part_no=int(line.split('C(')[1].split(')')[0])
  if K == 0: 
    return 0
  return ((part_no / K)+1) * 2000

def recv_taskoutdata_filter(line):
  global recvtask_data_recs
  #            /dlb.cpp, 369, receiveTaskOutData              , tid: BD956700,   5748 ::task out data C(7) is returned back.
  if '378, receiveTaskOutData' in line:
    lines.append(line)
    task_finish_t=int(line.split(':')[1].split(',')[1])    
    task_finish_no_dlb = get_task_org_finish(line)
    recvtask_data_recs.append([task_finish_t, task_finish_no_dlb ])
    
wild=sys.argv[1]
import glob as gb
files=gb.glob(wild)
for fn in files:
  LS=0
  read_file(fn,[fi_filter,fb_filter,export_filter,recv_taskoutdata_filter])
  get_pars(fn)
  pars='%d,%d,%d,%d,%d,' %(N,K,KK,p,LS)
  print fn,pars,
  w=open(fn+'_dlb.txt','w')
  for line in lines:
    w.write(line)
  w.write('T1 records\n')
  for t in t1_recs:
    w.write('T1,' + pars+str(t[1])+'\n')
  w.write('T2 records\n')
  for t in exp_tasks_recs:
    w.write('T2,' + pars+str(t)+'\n')
  w.write('T3 records\n')
  if len(recvtask_data_recs)>0:
    print 'full cycle',
  print
  for t in recvtask_data_recs:
    w.write('T3,' + pars+str(t[0])+','+str(t[1])+'\n')
  w.write('Failures records\n')
  if len(fb_dec_recs)>0:
    t=fb_dec_recs
    w.write(str(len(t))+', '+str(sum(t)/len(t))+'\n')
  
  w.close()
  
