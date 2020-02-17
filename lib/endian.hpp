//---------------------------------------------------------------------------//

#ifndef MY_LITTLE_ENDIAN
#define MY_LITTLE_ENDIAN

#include <iostream>

#define IS_BIG_ENDIAN (*(uint16_t*)"\0\xff" < 0x100)

//---------------------------------------------------------------------------//

class endian
{

private:

  // endianness
  bool little_endian_;

public:

  // constructor
  endian()
  {
    // check endianness on machine
    little_endian_ = get_endian();
  }
  
  // provide information about little versus big endian architecture
  bool get_endian()
  {
    // declare short (16 bit) integer representing "1"
    short int wd = 0x001;

    // get byte pattern of integer
    uint8_t *pwd = (uint8_t*)(&wd);

    //for ( int i = 0; i < 2; i ++ ) std::cout<<std::hex<<(int)pwd[i]<<" ";
    //std::cout<<"\n";

    // check if byte pattern is 0x 0001 or 0x 0100
    return ((int)pwd[0] == 1 ? true : false);
  }

  // retrieve information
  bool little_endian()
  {
    return little_endian_?true:false;
  }

  bool big_endian()
  {
    return little_endian_?false:true;
  }

};

#endif

//---------------------------------------------------------------------------//
