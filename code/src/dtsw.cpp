#include "dtsw.hpp"
//#include "util.hpp"
#ifndef LOG_DLBSIM
#define LOG_DLBSIM 0xFFFFFFFF
#endif
#define LOG_DTSW_DATA 0 
namespace dtsw{
  /*----------------------------------------*/
  Parameters_t Parameters;
  Data *A,*B,*C;
  SWAlgorithm *sw_engine;
  /*----------------------------------------------------*/
  void runStep(SWTask*);
  IterationData *TimeStepsTask::D = nullptr;
  int TimeStepsTask::last_step=0;
  /*----------------------------------------*/
  /*----------------------------------------*/
  void parse_args(int argc,char *argv[]){
    Parameters.M          = 1    ; // MB
    Parameters.W          = 1000 ; // miliseconds
    Parameters.lambda_bar = 10   ; // min DT tasks running

    for(int i=0;i<argc;i++){
      if(strcmp(argv[i],"-lambda") ==0){
	Parameters.lambda_bar = atoi(argv[++i]);
      }
      if(strcmp(argv[i],"-K") ==0){
	Parameters.K = atoi(argv[++i]);
      }
      if(strcmp(argv[i],"-M") ==0){
	Parameters.M = atoi(argv[++i]);
      }
      if(strcmp(argv[i],"-P") ==0){
	Parameters.N = atoi(argv[++i]);
      }
      if(strcmp(argv[i],"-W") ==0){
	Parameters.W = atoi(argv[++i]);
      }
      if(strcmp(argv[i],"--iter-no") ==0){
	Parameters.IterNo = atoi(argv[++i]);
      }
      if(strcmp(argv[i],"--lambda_star") ==0){
	Parameters.lambda_star = atoi(argv[++i]);
      }
    }
    LOG_INFO(LOG_DLBSIM,"lambda=%d, lambda_star=%d, M=%d, N=%d, K=%d, W=%d, IterNo=%d.\n",
	     Parameters.lambda_bar,
	     Parameters.lambda_star,
	     Parameters.M,
	     Parameters.N,
	     Parameters.K,
	     Parameters.W,
	     Parameters.IterNo
	     );
  }
  /*----------------------------------------*/
  /*----------------------------------------*/
  void init(int argc, char *argv[]){
    parse_args(argc,argv);
    dtEngine.set_memory_policy(engine::ENGINE_ALLOCATION);		
    dtEngine.start(argc,argv);
    sw_engine = new SWAlgorithm(20,false);
    dtEngine.set_user_context(sw_engine);
    int r = Parameters.lambda_star * Parameters.K;
    A = new DTSWData (r,Parameters.M,"A");
    B = new DTSWData (r,Parameters.M,"B");
    C = new DTSWData (r,Parameters.M,"C");
    LOG_INFO(LOG_DLBSIM,"C(1).host=%d, C.rows=%d.\n",(*C)(1).getHost(),C->get_rows());
  }
  /*----------------------------------------*/
  void finalize(){
    sw_engine->finalize();
    delete A;
    delete B;
    delete C;
    delete sw_engine;
  }
  /*----------------------------------------------------*/
  void run(int argc, char *argv[]){
    
    TimeStepsTask::D = new IterationData();
    int n = (Parameters.IterNo<10)?Parameters.IterNo:10;
    for(int i=0; i < n; i++){
      TimeStepsTask *step = new TimeStepsTask;
      sw_engine->submit(step);
    }
    sw_engine->flush();
  }
  /*----------------------------------------------------*/
  void TimeStepsTask::finished(){
    SWTask::finished();
    
    LOG_INFO(LOG_DTSW,"step :%d, Par.StepNo :%d\n",last_step, Parameters.IterNo);
    if ( last_step  < Parameters.IterNo ) {        
      sw_engine->submit(new TimeStepsTask );
      sw_engine->flush();
    }
  }
  /*----------------------------------------------------*/
  void TimeStepsTask::runKernel(){
#if ATOMIC_COUNTER
    is_submitting = true;
    runStep(this);
    is_submitting = false;
    sw_engine->flush();
#else
    runStep(this);
#endif
    if (sw_engine->get_tasks_count()<=Parameters.IterNo)// Only time step tasks are added.
      finished();
    
  }
  /*----------------------------------------------------*/
  void DLBTask::static_run(){
  }
  /*----------------------------------------------------*/
  void DLBTask::runKernel(){
    for(int i=0;i<  config.getNumThreads()-1;i++){
      SGDLBTask *t=new SGDLBTask(Parameters.W);  
      sw_engine->subtask(this,t);
    }
    set_submitting(false);
  }
  /*----------------------------------------------------*/
  void runStep(SWTask *p){
    Data &a=*A,&b=*B,&c=*C;
    for (int i=0;i<c.get_rows();i++){
      sw_engine->submit( new DLBTask(a(i),b(i),c(i),p)
			 );
    }    
  }
  /*----------------------------------------------------------------*/
  TimeStepsTask::~TimeStepsTask(){
    LOG_INFO(LOG_DTSW,"step :%d, Par.StepNo :%d\n",last_step, Parameters.IterNo);
    if ( last_step  < Parameters.IterNo )         
      sw_engine->submit(new TimeStepsTask );
  }
  /*----------------------------------------------------------------*/
  void TimeStepsTask::register_data(){
    parent_context = sw_engine;
    setName("TStep");
    TimeStepsTask::D->setName("TimeStepsData");
    if ( last_step ==1)
      TimeStepsTask::D->setRunTimeVersion("0.0",0);      
    IDuctteipTask::key = key;
    DataAccessList *dlist = new DataAccessList;    
    //    data_access(dlist,TimeStepsTask::D,(last_step==1)?IData::READ:IData::WRITE);
    setDataAccessList(dlist);
    LOG_INFO(LOG_DTSW,"(****)TimeStePDATA gt-ver: rd %s, wr %s --- rt-ver: rd %s wr %s\n",
	     TimeStepsTask::D->getReadVersion().dumpString().c_str(),
	     TimeStepsTask::D->getWriteVersion().dumpString().c_str() ,
	     TimeStepsTask::D->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     TimeStepsTask::D->getRunTimeVersion(IData::WRITE).dumpString().c_str());
    host = me;
  }
  /*----------------------------------------------------------------*/
  DTSWData::DTSWData(){      
    memory_type = SYSTEM_ALLOCATED;
    host_type=SINGLE_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setHostType(SINGLE_HOST);
    setParent(sw_engine);
    sw_engine->addInputData(this);
    LOG_INFO(LOG_DTSW_DATA,"Data handle for new dtswdata:%d\n",my_data_handle->data_handle);
  }
  /*----------------------------------------------------------------*/
  IterationData::IterationData(){      
    memory_type = USER_ALLOCATED;
    host_type=ALL_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setHostType(ALL_HOST);
    setParent(sw_engine);
    sw_engine->addInputData(this);
    setRunTimeVersion("0.0",0);
  }
  /*----------------------------------------------------------------*/
  void SGSWData::partition_data(DTSWData &d,int R,int C){
  }
  /*----------------------------------------------------------------------------*/
  DTSWData::DTSWData(int r,int M, const char  *n)//:IData(n,r,1,static_cast<IContext*>(sw_engine))
  {
    int c=1;
    setName(n);
    for(int i=0;i<r;i++){
      DTSWData*t=new DTSWData;
      t->row_idx = i;
      t->col_idx = 0;
      std::stringstream ss;
      ss << n << "(" << i <<  ")";
      t->name.assign(ss.str());
      t->memory_p = nullptr;
      /*
      int partition_size = total_size_in_bytes / r/ c ;
      t->mem_size_in_bytes = partition_size;
      t->memory_p = new byte[partition_size];
      t->mem_size_in_elements = partition_size / item_size_;
      LOG_INFO(LOG_DTSW_DATA,"Memory :%p L2 mem  size(in bytes):%d (in items count):%d.\n",t->memory_p,t->mem_size_in_bytes, t->mem_size_in_elements);
      */
      int B = Parameters.K;
      t->setHost(i % B);
      if ( i%B == me)
	t->setRunTimeVersion("0.0",0);
      else
	t->setRunTimeVersion("-1",-1);


      t->setHostType(SINGLE_HOST);
      //      t->allocateMemory();
      int  b=Parameters.lambda_star * Parameters.K;
      t->setContentSize ( M/b*M/b*sizeof(double));
      t->data_memory = dtEngine.newDataMemory();
      LOG_INFO(0*LOG_DLBSIM,"Host for %s is set to %d, its memory:%p .\n",ss.str().c_str(),i%B,t->getContentAddress());
      t->sg_data =nullptr;
      Dlist.push_back(t);
    }
    rows = r;
    cols = c;
    row_idx=col_idx = -1;
    sg_data = nullptr;
    memory_p = nullptr;
    /*
    if ( getParent())
      setDataHandle(getParent()->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(config.nb,config.nb);
   
    setPartition(r,1);    
    sw_engine->addInOutData(this);
    */
  }
  /*----------------------------------------------------------------------------*/
  DTSWData::DTSWData (int M, int N, int r,int c, std::string n,int total_size_in_bytes, int item_size_, bool isSparse)
    {
      nnz=0;
      setName(n);
      item_size = item_size_;
    for(int j=0;j<c;j++){
      for(int i=0;i<r;i++){
	DTSWData*t=new DTSWData;
	t->row_idx = i;
	t->col_idx = j;
	t->sp_row  = i;
	t->sp_col  = j;
	std::stringstream ss;
	if ( c>1)
	  ss << n << "(" << i << "," << j << ")";
	else
	  ss << n << "(" << i <<  ")";
	t->name.assign(ss.str());
	t->item_size = item_size_;
	t->memory_p = nullptr;
	if(!isSparse){
	  int partition_size = total_size_in_bytes / r/ c ;
	  t->mem_size_in_bytes = partition_size;
	  t->memory_p = new byte[partition_size];
	  t->mem_size_in_elements = partition_size / item_size_;
	  LOG_INFO(LOG_DTSW_DATA,"Memory :%p L2 mem  size(in bytes):%d (in items count):%d.\n",t->memory_p,t->mem_size_in_bytes, t->mem_size_in_elements);
	  int B = Parameters.partition_level[1].Nb;
	  t->setHost(i / ( B / Parameters.P ));	    
	  LOG_INFO(LOG_DTSW,"Host for %s is set to %d /  %d / %d   = %d .\n",ss.str().c_str(),i,B,Parameters.P,i / ( B / Parameters.P ));
	  t->setHostType(SINGLE_HOST);
	  t->allocateMemory();
	}
	else{// for sparse data D 
	  t->setHostType(ALL_HOST);
	  t->setHost(-1);	    
	}
	t->sg_data =nullptr;
	Dlist.push_back(t);
      }
    }
    rows = r;
    cols = c;
    row_idx=col_idx = -1;
    sg_data = nullptr;
    memory_p = nullptr;
  }
  /*---------------------------------------------
  RHSTask(Data &a, Data &b, Data &c,SWTask *p){
    A = static_cast<Data *>(&a);
    B = static_cast<Data *>(&b);
    C = static_cast<Data *>(&c);
    child_count = 0;
    parent = p;
    if (p)
      step_no = p->step_no;
    host = C->getHost();
    atm_offset = a.get_block_row() * Parameters.atm_block_size_L1;
    key = RHS;
    setNameWithParent("_RHS");	  
    *this << *A << *B >> *C;
    if(getHost() == me ) 
      if(parent)
	Atomic::increase(&parent->child_count);
  }
  ---------------------------------------------*/
  void DTSWData::report_data(){
    LOG_INFO(LOG_DTSW_DATA,"Data %s at (%d,%d) has memory at address:%p size(in-bytes):%d, size in elems:%d\n",
	     name.c_str(),row_idx,col_idx,memory_p, mem_size_in_bytes, mem_size_in_elements );
    for(auto d:Dlist){
      d->report_data();
    }
    if (sg_data)
      sg_data->report_data();
  }
  /*---------------------------------------------*/
  void SGSWData ::report_data(){
  }
  /*---------------------------------------------*/
  void SWAlgorithm::local_finished(){
    if ( !Parameters.pure_mpi ){
      stringstream fn;
      fn << "execution_" << Parameters.P << "_" << Parameters.partition_level[0].M <<"_B"
	 <<Parameters.partition_level[1].blocks_per_row << "_b"
	 <<Parameters.partition_level[2].blocks_per_row << "_"
	 << me << ".log";
      Trace<Options>::dump(fn.str().c_str());
    }
  }
  /*---------------------------------------------*/
  void SWTask::set_submitting(bool f){
    is_submitting = f;
    if  (!f){
      sw_engine->submit(new SGSyncTask(this) );
      LOG_INFO(LOG_DLBSIM,"SyncTask created for %s.\n",getName().c_str());
    }
  }
  /*---------------------------------------------*/
  DLBTask::DLBTask(dtsw::Data &a, dtsw::Data &b, dtsw::Data &c,SWTask *p){
    A = static_cast<Data *>(&a);
    B = static_cast<Data *>(&b);
    C = static_cast<Data *>(&c);
      
    host = C->getHost();
    task_parent = p;
    if (p)
      step_no = p->step_no;
    child_count = 0 ;
    if ( host == me )
      if ( task_parent )
	Atomic::increase(&task_parent->child_count);
    setNameWithParent("_Add");
    message_buffer = new MessageBuffer(getPackSize(),0);
    parent_context= sw_engine;
    *this << *A << *B	>> *C;      
  }
  /*---------------------------------------------*/
  void SWAlgorithm::runKernels ( IDuctteipTask *t){
    LOG_INFO(LOG_DLBSIM,"Imported task :%s.\n",t->getName().c_str());
    SGDLBTask *sgt=new SGDLBTask(t,Parameters.W);  
    sw_engine->submit(sgt);
  }
}

