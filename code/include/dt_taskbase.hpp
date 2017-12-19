#ifndef DT_TASKBASE_HPP
#define DT_TASKBASE_HPP
#ifndef LOG_DLBSIM
#define LOG_DLBSIM 0xFFFFFFFF
#endif

#include <list>
#include <atomic>
#include "dt_database.hpp"
#include "ductteip.hpp"
#include "sg/platform/atomic.hpp"

namespace dtsw{
  class Node;
  class SWTask: public IDuctteipTask {
  private:
  public:
    bool  is_submitting, sub_tasks_submitted;
    //SWTask *parent;
    SGHandle sg_handle_child,sg_handle_comm;
    //std::atomic<size_t> child_count;
    //int child_count;
    int step_no;
    virtual void dump()=0;
    virtual void runKernel()=0;
    virtual ~SWTask(){}
    /*------------------------------------------------------------*/
    SWTask &operator <<(Data &d1){ // Read data
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getWriteVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::READ;
      data_list->push_back(daxs);
      d->incrementVersion(IData::READ);
      LOG_INFO(LOG_DTSW,"(****)Daxs Read %s for  %s is %p\n",
	       getName().c_str(), d1.getName().c_str(),daxs);
      return *this;
	
    }
    /*------------------------------------------------------------*/
    SWTask &operator >>(Data &d1){// Write Data 
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getReadVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::WRITE;
      data_list->push_back(daxs);
      d->incrementVersion(IData::WRITE);
      LOG_INFO(LOG_DTSW,"(****)Daxs Write %s for  %s is %p\n",
	       getName().c_str(),d1.getName().c_str(),daxs);
      return *this;
    }
    /*------------------------------------------------------------*/
    SWTask(){
      data_list = new list<DataAccess*>;
      LOG_INFO(LOG_DTSW,"(****)Daxs dlist new %p\n",data_list);
      child_count = 0;
      task_parent = nullptr;
      is_submitting = false;
      sub_tasks_submitted = false;
    }
    /*------------------------------------------------------------*/
    void after_check_dependencies(){
      //      submit_next_level_tasks();
    }
    /*------------------------------------------------------------*/
    virtual void finished(){
      LOG_INFO(LOG_DLBSIM,"Task's virtual finished is called for %s, paernt:%p,state:%d, Fin:%d.\n",
	       getName().c_str(),task_parent,state, Finished);
      if ( state < Finished  ) 
	setFinished(true);
      if ( task_parent){
	LOG_INFO(LOG_DLBSIM,"Parent child-count: %d .\n",task_parent->child_count);
	if (  task_parent->child_count ==0 )
	  return;
	if ( Atomic::decrease_nv(&task_parent->child_count) ==0)
	  {
	    LOG_INFO(LOG_DTSW,"child task :%s is waiting for parent: %s finishes submission.\n",getName().c_str(),task_parent->getName().c_str());
	    LOG_INFO(LOG_DTSW, "%s finished from parent's task :%s child_count :%d\n " ,getName().c_str(),task_parent->getName().c_str(),(int)task_parent->child_count );
	    task_parent->finished();
	}
      }
      LOG_INFO(LOG_DTSW,"\n");    
    }
    /*------------------------------------------------------------*/
    virtual void submit_next_level_tasks(){}
    /*------------------------------------------------------------*/
    bool is_still_submitting(){return is_submitting;}
    /*------------------------------------------------------------*/
    void set_submitting(bool f);
    /*------------------------------------------------------------*/
    void setNameWithParent(const char*n){
      if ( !task_parent ){
	setName(n);
	return;
      }
      std::stringstream ss;
      ss << task_parent->getName() << "_" << task_parent->child_count << n ;
      setName(ss.str());
    }

  };
  /*=========================================*/
  class DLBTask : public SWTask{
  private:
    dtsw::Data *A,*B,*C;
  public:
    DLBTask(Node &,SWTask *);
    DLBTask(Node &,Node&,SWTask *);
    DLBTask(dtsw::Data &a, dtsw::Data &b, dtsw::Data &c,SWTask *p);
    void runKernel();
    void submit_next_level_tasks(){}
    static void static_run();
    void dump(){}
  };
}
#endif //DT_TASKBASE_HPP
