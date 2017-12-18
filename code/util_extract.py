import sys 
_S,_D=2,3 #start and duration positions
base=0
T=1
P,N,p,c=0,0,0,0
def extract(s,d,out):  
  e=s+d
  ts=s//T
  te=e//T
  while te>ts:
    out.append([ts,s,(ts+1)*T-s])    
    ts+=1
    d-=ts*T-s
    s=ts*T
    e=s+d
    te=e//T
  out.append([ts,s,d])
  return out
def read_file(f,T):
  out=list()
  line = f.readline()
  while len(line)!=0:
    if line[0]=='#':break
    s=int(line.split()[_S])
    d=int(line.split()[_D])  
    extract(s,d,out)
    line = f.readline()
  return out
def process(recs):
  out=list()  
  if len(recs)==0:return out
  t1=recs[0][0]
  busy=recs[0][2]
  for r in recs[1:]:
    t,s,d=r    
    if t != t1:                  
      out.append([P,N,p,c,t1,float(T-busy)/T,float(busy)/T])
      t1=t
      busy=d
    else:
      busy+=d
  if busy!=0:
    out.append([P,N,p,c,t,float(T-busy)/T,float(busy)/T])
  return out
if __name__=='__main__':
  if len(sys.argv) <2:
    sys.exit(-1)
  fn=sys.argv[1]
  pars=fn.split('_')
  P = int(pars[1])
  N = int(pars[2])
  p = int(pars[3].split('.')[0])
  c = int(pars[5])
  
  f = open(fn,'rb')
  line=f.readline()
  start=int(line.split()[_S])
  line=f.readline()
  end=int(line.split()[_S])
  T = (end-start)/1000
  recs=read_file(f,T)
  f.close()
  util=process(recs)
  l=len(util)
  for ut in util:
    for u in ut:
      print u,
    print
