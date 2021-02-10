//---------------------------------------------------------------------------//

#ifndef IMCPARAMETER
#define IMCPARAMETER

#include <iomanip>
#include <map>
#include <string>
#include <sstream>
#include <vector>

//---------------------------------------------------------------------------//

namespace imc
{
  // single parameter (in a block) is determined by offset of its first/last byte
  class parameter
  {
    // offset of first/last byte of parameter
    unsigned long int begin_, end_;

  public:

    parameter(unsigned long int begin, unsigned long int end):
      begin_(begin), end_(end)
    {
      if ( end_ < begin_ )
      {
        throw std::logic_error("parameter: offset of first byte larger than last byte's offset");
      }
    }

    // set members
    void begin(unsigned long int begin)
    {
      if ( end_ < begin )
      {
        throw std::logic_error("parameter: offset of first byte larger than last byte's offset");
      }
      begin_ = begin;
    }
    void end(unsigned long int end)
    {
      if ( end < begin_ )
      {
        throw std::logic_error("parameter: offset of first byte larger than last byte's offset");
      }
      end_ = end;
    }

    // access members
    const unsigned long int& begin() const { return begin_; }
    const unsigned long int& end() const { return end_; }

    // comparison operator
    bool operator==(const parameter& param)
    {
      return ( this->begin_ == param.begin_ && this->end_ == param.end_ );
    }

    // get info
    std::string get_info() const
    {
      return ( std::string("[") + std::to_string(begin_) + std::string(",")
                                + std::to_string(end_) + std::string("]") );
    }
  };

}

#endif

//---------------------------------------------------------------------------//
