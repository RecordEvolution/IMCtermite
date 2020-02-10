//---------------------------------------------------------------------------//

#ifndef HALF_PRECISION_FLOATING_POINT
#define HALF_PRECISION_FLOATING_POINT

#include <vector>
#include <bitset>
#include "endian.hpp"

//---------------------------------------------------------------------------//

class half_precision_floating_point
{

private:

  // size of half precision floating point number in bits
  int size_ = 16;

  // construction of the number (binary16) according to IEEE 754-2008 standard
  std::bitset<1>  sign_;
  std::bitset<5>  expo_;
  std::bitset<10> frac_;

  // endianness of machine/architecture
  bool little_endian_;

  // number as single-precision floating point number
  float num_;

  // array of bytes representing the number
  std::vector<uint8_t> numbytes_;

public:

  // constructors
  half_precision_floating_point(std::vector<uint8_t> numbytes) : 
    numbytes_(numbytes)
  {
    assert( numbytes_.size() == 2 );
    endian endi;
    little_endian_ = endi.little_endian();
    assert( little_endian_ && "just don't work on big endian machines!" );
  }

  half_precision_floating_point(float num) :
    num_(num)
  {

  }

  // destructor
  ~half_precision_floating_point()
  {

  }

  // assignment operator
  half_precision_floating_point& operator=(const half_precision_floating_point& other)
  {
    if ( this != &other )
    {
      size_ = other.size_;
      sign_ = other.sign_;
      expo_ = other.expo_;
      frac_ = other.frac_;
      little_endian_ = other.little_endian_;
      num_ = other.num_;
      numbytes_ = other.numbytes_;
    }

    return *this;
  }


  // convert byte pattern to number
  void convert_to_float()
  {
    // declare a single precision floating point number
  //  float myfloat = 0.0;

    if ( little_endian_ )
    {
    
    }

  } 

  // convert floating point number to bytes
  void convert_to_byte()
  {

  }

};

#endif

//---------------------------------------------------------------------------//

