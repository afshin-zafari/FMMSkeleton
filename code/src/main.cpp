#include <iostream>
#include "dtsw.hpp"

using namespace std;


int main(int argc , char *argv[])
{
   
    dtsw::init(argc,argv);
    dtsw::run(argc,argv);
    dtsw::finalize();
    

    return 0;
}
