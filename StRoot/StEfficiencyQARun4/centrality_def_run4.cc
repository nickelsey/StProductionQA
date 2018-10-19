
#include "centrality_def_run4.hh"

#include <iostream>
#include <math.h>

#include "TRandom.h"

CentralityDefRun4::CentralityDefRun4() {   
    cent_bin_9_ = {520, 441, 319, 222, 150, 96, 57, 31, 14}; 
}

CentralityDefRun4::~CentralityDefRun4() {
  
}

int CentralityDefRun4::centrality9(int refmult) {
    for (int i = 0; i < cent_bin_9_.size(); ++i) {
        if (refmult >= cent_bin_9_[i])
            return i;
    }
    return -1;
}
