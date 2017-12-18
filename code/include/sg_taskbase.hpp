#ifndef SG_TASKBASE_HPP
#define SG_TASKBASE_HPP
#include <sstream>
#include "dt_taskbase.hpp" 
#include "sg/superglue.hpp"
#include "sg/platform/atomic.hpp"
#include "sg_database.hpp"
namespace dtsw{
  /*=========================================================*/
  class SGTask : public Task<Options>{
  private:
    int step_no;
  protected:
    SWTask *parent;
    string name;    
  public:
    SWTask* get_parent(){return parent;}
    string get_name(){return name;}
    void set_step_no(int sno){
      step_no = sno;
      std::ostringstream ss;
      ss << name << '_' << sno;
      name=ss.str();
      LOG_INFO(LOG_DTSW,"SGTask name for step: %d, %s.\n",step_no,name.c_str());
      //      name += string(std::itoa(sno));
    }
    virtual void run()=0;
    /*---------------------------------------------------------*/
    void set_parent(SWTask *p){
      parent = p;
#if ATOMIC_COUNTER
      if ( parent )
	sg::Atomic::increase(&parent->child_count);
#else
      if ( parent )
	register_access(ReadWriteAdd::read ,parent->sg_handle_child);
      LOG_INFO(LOG_DTSW,"Parent of %s is set to %s.\n",name.c_str(),p->getName().c_str());
#endif

    }
    /*---------------------------------------------------------*/
    ~SGTask(){
      if (parent){
#if ATOMIC_COUNTER
	if (parent->child_count ==0 )
	  return;
	if ( sg::Atomic::decrease_nv(&parent->child_count) ==0 )
	  parent->finished();
#endif
      }	
    }

  };
  /*=========================================================*/
  class SGSyncTask: public SGTask{
  private:
    SWTask* dt_task;
  public:
    SGSyncTask(SWTask* dt){
      dt_task = dt;
      register_access(ReadWriteAdd::write ,dt_task->sg_handle_child);
      LOG_INFO(LOG_DTSW,"SyncTask CTOR. for parent %s.\n",dt_task->getName().c_str());
    }
    void run(){}
    ~SGSyncTask(){
      if (!dt_task)
	return;
      LOG_INFO(LOG_DTSW,"SyncTask finished for %s.\n",dt_task->getName().c_str());
      dt_task->finished();
    }
  };
  /*=========================================================*/
  class SGAddTask: public SGTask{
  private:
    SGData *a,*b,*c;
    double dt;
    
  public:

      /*---------------------------------------------------------*/
    SGAddTask(SGData &a_, SGData &b_, double dt_ , SGData &c_){
      a = &a_;
      b = &b_;
      c = &c_;
      dt = dt_;
      register_access(ReadWriteAdd::read ,a->get_sg_handle());
      register_access(ReadWriteAdd::read ,b->get_sg_handle());
      register_access(ReadWriteAdd::write,c->get_sg_handle());
      name.assign("SGAdd");
    }
    void run();
  };
  /*---------------------------------------------------------*/
  /*=========================================================*/
  class SGRHSTask: public SGTask{
  private:
    SGData *t,*h,*dh;
    int atm_offset;
    
  public:

    /*---------------------------------------------------------*/
    SGRHSTask(int atm_,SGData &t_, SGData &h_,  SGData &dh_){
      atm_offset = atm_;
      t = &t_;
      h = &h_;
      dh = &dh_;
      register_access(ReadWriteAdd::read , t->get_sg_handle());
      register_access(ReadWriteAdd::read , h->get_sg_handle());
      register_access(ReadWriteAdd::write,dh->get_sg_handle());
      name.assign("SGRHS");
    }
    void run();
  };
  /*=========================================================*/
  class SGDiffTask: public SGTask{
  private:
    SGData *a,*b,*c;
    
  public:

    /*---------------------------------------------------------*/
    SGDiffTask(SGData &a_, SGData &b_,  SGData &c_){
      a = &a_;
      b = &b_;
      c = &c_;
      register_access(ReadWriteAdd::read ,a->get_sg_handle());
      register_access(ReadWriteAdd::read ,b->get_sg_handle());
      register_access(ReadWriteAdd::write,c->get_sg_handle());
      name.assign("Diff");
    }
    void run();
  };
  /*=========================================================*/
  class SGStepTask: public SGTask{
  private:
    SGData *a,*b,*c,*d,*e;
    
  public:

    /*---------------------------------------------------------*/
    SGStepTask(SGData &a_, SGData &b_,  SGData &c_, SGData &d_, SGData &e_){
      a = &a_;
      b = &b_;
      c = &c_;
      d = &d_;
      e = &e_;
      register_access(ReadWriteAdd::read ,a->get_sg_handle());
      register_access(ReadWriteAdd::read ,b->get_sg_handle());
      register_access(ReadWriteAdd::read ,c->get_sg_handle());
      register_access(ReadWriteAdd::read ,d->get_sg_handle());
      register_access(ReadWriteAdd::write,e->get_sg_handle());
      name.assign("Step");
    }
    void run();
  };
  /*=========================================================*/
  class SGDLBTask : public SGTask {
  private:
    int wait;
    IDuctteipTask *dt_task;
  public:
    SGDLBTask(IDuctteipTask *d,int w){
      dt_task = d;
      wait = w;
    }
    SGDLBTask(int w):wait(w){
      LOG_INFO(1,"SGDLBTask CTOR, w=%d.\n",w);
      dt_task = nullptr;
    }
    void run(){
      LOG_INFO(1,"wait started.\n");
      usleep(wait*1000);
      LOG_INFO(1,"wait finished.\n");
    }
    ~SGDLBTask(){
      if(dt_task){
	LOG_INFO(1,"SGDLBTask DTOR %s.\n",dt_task->getName().c_str());
	dt_task->setFinished(true);
      }
    }
    
  };
}
#endif //SG_TASKBASE_HPP
