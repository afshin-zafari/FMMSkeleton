#ifndef DTSW_HPP
#define DTSW_HPP
#include "sw_dist.hpp"

namespace dtsw{
  typedef byte* Buffer;
  /*------------------------------------------*/
  void init(int argc, char *argv[]);
  void finalize();
  void run(int , char *[]);
  /*----------------------------------------*/
  typedef struct Partition{    
    union {
      int rows_per_block;
      int M;
      int chunk_size;
    };
    union {
      int cols_per_block;
      int N;
    };
    union{
      int blocks_per_row;
      int Mb;
    };
    union{
      int blocks_per_col;
      int Nb;
    };
  }Partition_t;
  /*----------------------------------------*/
  typedef struct parameters{
    int         p,q,P,lambda_bar,lambda_star,K,M,N,W;
    double      dt;
    Partition_t partition_level[3];
    int         IterNo;
    bool pure_mpi;
  }Parameters_t;

  extern Parameters_t Parameters;
  /*----------------------------------------*/
}
#endif // DTSW_HPP
