#ifndef DT_TIMESTEPSTASK_HPP
#define DT_TIMESTEPSTASK_HPP
#include "dt_taskbase.hpp"
#include "ductteip.hpp"
namespace dtsw{
  /*---------------------------------------------------------------*/
  typedef list<DataAccess*> DataAccessList ;
  void data_access(DataAccessList *dlist,Data *d,IData::AccessType rw){
    DataAccess *dx1 = new DataAccess;
    dx1->data = d;
    if ( rw == Data::READ){
      dx1->required_version = d->getWriteVersion();
      dx1->type = IData::READ;
    }
    else{
      dx1->required_version = d->getReadVersion();
      dx1->type = IData::WRITE;
    }
    dx1->required_version.setContext( glbCtx.getLevelString() );
    d->getWriteVersion().setContext( glbCtx.getLevelString() );
    d->getReadVersion().setContext( glbCtx.getLevelString() );
    dlist->push_back(dx1);
    d->incrementVersion(rw);
  }
  /*---------------------------------------------------------------*/
  class TimeStepsTask : public SWTask{
  private:
    int time_step;
  public:
    static IterationData *D;
    static int last_step ;
    int get_time_step(){return time_step;}
  /*---------------------------------------------------------------*/
    TimeStepsTask(){
      time_step = last_step++;
      step_no = time_step;
      register_data();
      child_count =0;
    }
  /*---------------------------------------------------------------*/
    void runKernel();
    void finished();
  /*---------------------------------------------------------------*/
    void dump(){
      std::cout << "TimeStep  "<< time_step << " ";		
      if (D)
	std::cout << D->getName() <<" ";
      
    std:cout << std::endl;
    }
    /*----------------------------------------------------------------*/
    void register_data();
    virtual ~TimeStepsTask();
    /*----------------------------------------------------------------*/
  };
}
#endif
