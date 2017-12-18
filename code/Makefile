
outdir          = ./bin
outdir_rel      = ./bin/release
SUPERGLUE_DIR   = /pica/h1/afshin/sg/include
SUPERGLUE_FLAGS = -pthread -I$(SUPERGLUE_DIR)  #-pedantic -Wno-long-long -Wno-format
DUCTTEIP_DIR    = /pica/h1/afshin/Damavand/D4/DuctTeip/ResStudio/Prototype
DUCTTEIP_INC         = $(DUCTTEIP_DIR)/include
DUCTTEIP_LIBPATH     = $(DUCTTEIP_DIR)/lib/
DUCTTEIP_LIBPATH_REL = $(DUCTTEIP_DIR)/lib/release
DUCTTEIP_LIB    = ductteip
SOURCE_DIR      = ./src
HEADER_DIR      = ./include
DUCTTEIP_FLAGS  = -I$(HEADER_DIR) -DWITH_MPI=1 -I$(SUPERGLUE_DIR) -I$(DUCTTEIP_INC)  



ifeq ($(CXX),icpc)
#-----------------------Intel Compiler set---------------------
	CPP  = mpiicpc
	LINK_FLAGS    =-lm -lrt -pthread -L$(DUCTTEIP_LIBPATH)     -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition 
	LINK_FLAGS_REL=-lm -lrt -pthread -L$(DUCTTEIP_LIBPATH_REL) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition 
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 -qopt-prefetch -O3  -xCORE-AVX2 
       appfile         = ./bin/intel_dlbsim_debug
       appfile_rel     = ./bin/intel_dlbsim_release
else
#-----------------------GCC Compiler set---------------------
	CPP  = mpicxx
	GCOV_FLAGS=-fprofile-arcs -ftest-coverage
	OPTIM_FLAGS=-mavx2 -mfma -march=broadwell -O3 -Wwrite-strings
	SPECIAL_FLAGS=$(OPTIM_FLAGS)     
	LINK_FLAGS=-lm -lrt -lpthread -L$(DUCTTEIP_LIBPATH) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition -Wl,--whole-archive
	LINK_FLAGS_REL=-lm -lrt -lpthread -L$(DUCTTEIP_LIBPATH_REL) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition -Wl,--whole-archive
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 #$(SPECIAL_FLAGS) 
       appfile         = ./bin/gcc_dlbsim_debug
       appfile_rel     = ./bin/gcc_dlbsim_release
endif
#########################################################
source:=$(notdir $(shell ls -Sr $(SOURCE_DIR)/*.cpp))
objnames:=$(source:%.cpp=%.o)
objects:=$(addprefix $(outdir)/,$(objnames))
objects_rel:=$(addprefix $(outdir_rel)/,$(objnames))
RED=\033[0;31m
GREEN=\033[1;32m
NC=\033[0m

all: $(appfile) $(appfile_rel)

$(appfile) : $(appfile)($(outdir)/*.o)
	@echo -e "$(GREEN) generate output exe $(NC) $(RED)$(appfile)$(NC)."

$(appfile)($(outdir)/*.o) : $(objects)
	@echo -e "$(GREEN) *** link objects *** $(NC)"
	@$(CPP) -o $(appfile) $? $(LINK_FLAGS)

$(objects):  $(outdir)/%.o:  $(SOURCE_DIR)/%.cpp 
	@echo -e "$(GREEN) compile $(notdir $<) $(NC)"
	@$(CPP) -c -o $@ $< $(COMP_FLAGS) -g -DLOG_DLBSIM=0xFFFFFFFF
#------------------------------------------------------
$(appfile_rel) : $(appfile_rel)($(outdir_rel)/*.o)
	@echo -e "$(GREEN) generate output exe $(NC) $(RED)$(appfile_rel)$(NC)."

$(appfile_rel)($(outdir_rel)/*.o) : $(objects_rel)
	@echo -e "$(GREEN) *** link release objects *** $(NC)"
	@$(CPP) -o $(appfile_rel) $? $(LINK_FLAGS_REL)

$(objects_rel):  $(outdir_rel)/%.o:  $(SOURCE_DIR)/%.cpp 
	@echo -e "$(GREEN) compile $(notdir $<) $(NC)"
	@$(CPP) -c -o $@ $< $(COMP_FLAGS) -DLOG_DLBSIM=0

clean: 
	rm -f $(outdir)/*.o $(appfile) $(outdir_rel)/*.o $(appfile_rel)
copy:
	cp $(appfile) /crex1/proj/snic2017-7-18/nobackup/DLBSim/bin
	cp $(appfile_rel) /crex1/proj/snic2017-7-18/nobackup/DLBSim/bin
