#include <libplugin/plugin.h>
#include"psi4-dec.h"
#include<libdpd/dpd.h>

#include<boost/shared_ptr.hpp>
#include<liboptions/liboptions.h>
#include<libtrans/integraltransform.h>
#include<libtrans/mospace.h>
#include<libmints/matrix.h>
#include<libmints/vector.h>
#include<libchkpt/chkpt.h>
#include<libiwl/iwl.h>
#include <libpsio/psio.hpp>
#include <libciomr/libciomr.h>
#include <ccfiles.h>
#include <cstdio>
#include <cstdlib>


#include"globals.h"
#include"gpuhelper.h"
#include"blas.h"
#include"cepa.h"

using namespace psi;
using namespace boost;

namespace psi{
  void OPDM(boost::shared_ptr<psi::CoupledPair>cepa,Options&options);
}

namespace psi{

void RunCoupledPair(Options &options){

  boost::shared_ptr<CoupledPair> cepa(new CoupledPair);
  PsiReturnType status;

  tstart();
  cepa->WriteBanner(options);
  cepa->Initialize(options);
  cepa->AllocateMemory(options);
  status = cepa->CEPAIterations(options);
  tstop();

  // mp2 energy
  Process::environment.globals["MP2 CORRELATION ENERGY"] = cepa->emp2;
  Process::environment.globals["MP2 TOTAL ENERGY"] = cepa->emp2 + cepa->escf;

  // cepa energy
  char*cepatype = (char*)malloc(100*sizeof(char));
  if (cepa->cepa_level == 0){
     Process::environment.globals["CEPA(0) CORRELATION ENERGY"] = cepa->ecepa;
     Process::environment.globals["CEPA(0) TOTAL ENERGY"] = cepa->ecepa + cepa->escf;
  }
  if (cepa->cepa_level == 1){
     Process::environment.globals["CEPA(1) CORRELATION ENERGY"] = cepa->ecepa;
     Process::environment.globals["CEPA(1) TOTAL ENERGY"] = cepa->ecepa + cepa->escf;
  }
  if (cepa->cepa_level == 2){
     Process::environment.globals["CEPA(2) CORRELATION ENERGY"] = cepa->ecepa;
     Process::environment.globals["CEPA(2) TOTAL ENERGY"] = cepa->ecepa + cepa->escf;
  }
  if (cepa->cepa_level == 3){
     Process::environment.globals["CEPA(3) CORRELATION ENERGY"] = cepa->ecepa;
     Process::environment.globals["CEPA(3) TOTAL ENERGY"] = cepa->ecepa + cepa->escf;
  }
  if (cepa->cepa_level == -1){
     Process::environment.globals["CISD CORRELATION ENERGY"] = cepa->ecepa;
     Process::environment.globals["CISD TOTAL ENERGY"] = cepa->ecepa + cepa->escf;
  }
  if (cepa->cepa_level == -2){
     Process::environment.globals["ACPF CORRELATION ENERGY"] = cepa->ecepa;
     Process::environment.globals["ACPF TOTAL ENERGY"] = cepa->ecepa + cepa->escf;
  }
  if (cepa->cepa_level == -3){
     Process::environment.globals["AQCC CORRELATION ENERGY"] = cepa->ecepa;
     Process::environment.globals["AQCC TOTAL ENERGY"] = cepa->ecepa + cepa->escf;
  }
  Process::environment.globals["CURRENT ENERGY"] = cepa->ecepa + cepa->escf;


  // dipole moments for CISD, ACPF, or AQCC
  if (options.get_bool("DIPMOM")){
     if (cepa->cepa_level>0) 
        throw PsiException("dipole moments available only for CEPA(0), CISD, ACFP, and AQCC",__FILE__,__LINE__);
     OPDM(cepa,options);
  }

  free(cepatype);
  cepa.reset();
}

}

