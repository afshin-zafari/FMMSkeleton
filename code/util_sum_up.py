import sys
_P,_N,_p,_c,_T,_I,_B=range(7)
X =1
def sum_group_by(fn):
  f=open(fn,'rb')
  line=f.readline()
  fields=line.split()
  t1=int(fields[_T])
  idle=float(fields[_I])
  busy=float(fields[_B])
  line=f.readline()
  out=list()  
  while len(line)!=0:
    line=f.readline()
    if len(line)<10:continue
    fields=line.split()
    P=int(fields[_P])
    N=int(fields[_N])
    p=int(fields[_p])
    c=int(fields[_c])
    t=int(fields[_T])
    i=float(fields[_I])
    b=float(fields[_B])
    if t != t1:
      out.append([P,N,p,c,t1,idle/X,busy/X])
      t1=t
      idle,busy=b,i
    else:
      busy+=b
      idle+=i
    line=f.readline()
  if  busy!=0:
    out.append([P,N,p,c,t,idle/X,busy/X])
  f.close()
  return out
if __name__ =='__main__':
  if len(sys.argv)<3:
    print "Usage: " + sys.argv[0] +"<file-name> <denominator in average>"
    sys.exit(-1)
  fn=sys.argv[1]
  X=float(sys.argv[2])
  out = sum_group_by(fn)  
  for ou in out:
    for o in ou:
      print o,
    print