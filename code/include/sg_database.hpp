#ifndef SG_DATABASE_HPP
#define SG_DATABASE_HPP
//#include "util.hpp"
#ifndef LOG_DTSW
#define LOG_DTSW 0
#endif
namespace dtsw{
  struct Dimension{
    int num_blocks,num_elems;
  };
  struct Partitions{
    Dimension x,y;
  };
  Partitions Level[3];
  //  Level[0].y.num_elems;
  /*----------------------------------------------------------------*/
  struct Options: public DefaultOptions<Options> {
    typedef Enable TaskName;
    typedef sg::Trace<Options> Instrumentation;
  };
  typedef Handle <Options> SGHandle;
  /*----------------------------------------------------------------*/
  struct DataPack{
    double data[4];
  };
  /*----------------------------------------------------------------*/
  typedef DataPack *DataPackList;
  /*===================================================================*/
  class DTSWData;
  class SGSWData{
  private:
    typedef SGSWData         SGData;
    typedef vector<SGData *> SGDataList;
    DTSWData         *dt_data;
    int          rows,cols,elem_rows_deprec,elem_cols_dep,my_row,my_col;
    SGDataList   parts;
    SGHandle     sg_handle;
    double       val;
    byte         *memory_p;
    int           mem_size_in_bytes,mem_size_in_elements;
    string       name;
  public:
    DataPack     *pack_data;
    double      *data;
    /*----------------------------------------------------------------*/
    SGSWData(){memory_p = nullptr;}
    void partition_data(DTSWData &d,int R,int C);

    /*----------------------------------------------------------------*/
    ~SGSWData(){
    }
    /*----------------------------------------------------------------*/
    SGSWData(int i, int j){my_row=i;my_col=j;}
    /*----------------------------------------------------------------*/
    SGHandle&get_sg_handle(){return sg_handle;}
    /*----------------------------------------------------------------*/
    int get_blocks(){return rows*cols;}
    int get_row_blocks(){
      return rows;
    }
    int get_col_blocks(){
      return cols;
    }
    /*----------------------------------------------------------------*/
    SGSWData &  operator()(int i, int j){
      return *parts[j*rows+i];
    }
    /*----------------------------------------------------------------*/
    SGSWData &  operator()(int i){
      return operator()(i,0);
    }
    /*----------------------------------------------------------------*/
    double v(int i, int j){
      return 0;
    }
    /*----------------------------------------------------------------*/
    double  x(int i, int j){      
      return 0;
    }
    /*----------------------------------------------------------------*/
    double  y(int i, int j){
      return 0;
    }
    /*----------------------------------------------------------------*/
    double  z(int i, int j){
      return 0;
    }
    /*----------------------------------------------------------------*/
    double  l(int i, int j){
      return 0;
    }
    /*----------------------------------------------------------------*/
    double  operator[](int i){
      return 0;
    }
    /*----------------------------------------------------------------*/
    double *get_data(){return data;}
    /*----------------------------------------------------------------*/
    int     get_row_index(){return my_row;}
    void    report_data();
    string  get_name(){return name;}
    void    set_rows_old(int n){}
    int get_mem_size_in_bytes(){return mem_size_in_bytes;}
    int get_mem_size_in_elems(){return mem_size_in_elements;}
    int get_rows(){return get_mem_size_in_elems();}
    /*----------------------------------------------------------------*/
  };
  /*----------------------------------------------------------------*/
  typedef SGSWData SGData;
  typedef vector<SGData *> SGDataList;
  class TData : public SGSWData {
  public:
  };
  /*----------------------------------------------------------------*/
}
#endif //SG_DATABASE_HPP

