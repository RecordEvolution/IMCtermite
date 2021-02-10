//---------------------------------------------------------------------------//

#ifndef IMCRESULT
#define IMCRESULT

#include "imc_datatype.hpp"

//---------------------------------------------------------------------------//

namespace imc
{
  struct channel_tab
  {
    std::string name_;

    // abscissa
    std::vector<double> xaxis_;
    std::string xunit_;

    // ordinate
    // std::vector<imc::datatype> yaxis_;
    std::vector<double> yaxis_;
    std::string yunit_;
  };

}

#endif

//---------------------------------------------------------------------------//
