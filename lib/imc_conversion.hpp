//---------------------------------------------------------------------------//

#ifndef IMCCONVERSION
#define IMCCONVERSION

#include <vector>

//---------------------------------------------------------------------------//

namespace imc
{
  // convert raw data in buffer into specific datatype
  template<typename datatype>
  void convert_data_to_type(std::vector<unsigned char>& subbuffer,
                            std::vector<imc::datatype>& channel)
  {
    // check number of elements of type "datatype" in buffer
    if ( subbuffer.size() != channel.size()*sizeof(datatype) )
    {
      throw std::runtime_error( std::string("size mismatch between subbuffer (")
                              + std::to_string(subbuffer.size())
                              + std::string(") and datatype (")
                              + std::to_string(channel.size()) + std::string("*")
                              + std::to_string(sizeof(datatype)) + std::string(")") );
    }

    // extract every single number of type "datatype" from buffer
    for ( unsigned long int i = 0; i < channel.size(); i++ )
    {
      // declare number of required type and point it to first byte in buffer
      // representing the number
      datatype df;
      uint8_t* dfcast = reinterpret_cast<uint8_t*>(&df);

      for ( unsigned long int j = 0; j < sizeof(datatype); j++ )
      {
        dfcast[j] = (int)subbuffer[i*sizeof(datatype)+j];
      }

      // save number in channel
      channel[i] = df;
    }

    // for ( auto el: channel ) std::cout<<el<<"\n";
  }

}

#endif

//---------------------------------------------------------------------------//
