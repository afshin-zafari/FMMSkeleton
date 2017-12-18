#ifndef SW_DIST_HPP
#define SW_DIST_HPP
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "sg_taskbase.hpp"
#include "dt_timestepstask.hpp"
#include "ductteip.hpp"
#include "sg/superglue.hpp"
#ifndef LOG_DTSW
#define LOG_DTSW 0x0FFFFFFFF
#endif

namespace dtsw{
  
  /*---------------------------------------------------------*/
  class SWAlgorithm: public IContext{
  private:
    std::vector<SWTask*> tasks;
    SuperGlue<Options> *sg_engine;
    bool pure_mpi;
  public:
  /*---------------------------------------------------------*/
    SWAlgorithm(int num_thrd,bool mpi):IContext("ShallowWater"){
      pure_mpi = mpi;
      if ( !pure_mpi)
	sg_engine = new SuperGlue<Options>(num_thrd);

      
    }
  /*---------------------------------------------------------*/
    void flush(){
#if ATOMIC_COUNTER
      for(auto t:tasks)
	dtEngine.register_task(t);
#endif
      tasks.clear();
    }
  /*---------------------------------------------------------*/
    void submit(SWTask *t){
#if ATOMIC_COUNTER
      tasks.push_back(t);
#else
      dtEngine.register_task(t);
#endif    
    }
  /*---------------------------------------------------------*/
    void submit( SGTask *child){      
      if ( pure_mpi){
	child->set_parent(nullptr);
	child->run();
      }
      else
	sg_engine->submit(child);
    }
  /*---------------------------------------------------------*/    
    void subtask( SWTask *parent, SGTask *child){      
      child->set_step_no ( parent->step_no);
      if ( pure_mpi){
	child->set_parent(nullptr);
	LOG_INFO(LOG_DTSW,"Sub task of %s run immediately.\n",parent->getName().c_str());
	child->run();
      }
      else{
	child->set_parent(parent);
	sg_engine->submit(child);
      }
    }
  /*---------------------------------------------------------*/
    void dump_tasks(){
      for(auto t: tasks){
	t->dump();
      }
    }
  /*---------------------------------------------------------*/
    virtual void runKernels(IDuctteipTask *task )override ;
  /*---------------------------------------------------------*/
    void finalize(){
      dtEngine.finalize();
    }
  /*---------------------------------------------------------*/
    string getTaskName(unsigned long key){return string("DLB_SIM");}
  /*---------------------------------------------------------*/
    void  taskFinished(DuctTeip_Task *task, TimeUnit dur){}
  /*---------------------------------------------------------*/
    int get_tasks_count(){return dtEngine.getTaskCount();}
    void local_finished();
    
  };
  extern SWAlgorithm *sw_engine;
  /*----------------------------------------------*/
}
#endif // SW_DIST_HPP
