#ifndef NEVISDECODER_ALGO_BASE_CXX
#define NEVISDECODER_ALGO_BASE_CXX

#include "algo_base.h"

namespace decoder {

  void algo_base::init_checker_info()
  {
    _checksum=0x0;
    _nwords=0;
  }

}


#endif
