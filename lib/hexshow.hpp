//---------------------------------------------------------------------------//

#ifndef HEXSHOW
#define HEXSHOW

#include <sstream>
#include <assert.h>

//---------------------------------------------------------------------------//

namespace hex
{
  void show(std::vector<unsigned char> &mydata, unsigned long int width = 32,
                                                unsigned long int startidx = 0,
                                                unsigned long int breakidx = 512)
  {
    // check input
    assert ( startidx <= breakidx );
    breakidx = ( breakidx > mydata.size() || breakidx == 0 ) ? mydata.size() : breakidx;

    // introduce additional line break
    std::cout<<"\n";

    // declare two stringstreams for hexadecimal and ASCII output
    std::stringstream sshex, sschr;

    // count bytes
    unsigned long int idx = 0;

    // process data
    for ( std::size_t i = startidx; i < breakidx; i++ )
    {
      unsigned char el = mydata[i];

      // hex representation
      if ( (idx+0)%width == 0 )
      {
        sshex<<std::hex<<"0x"<<std::setw(6)<<std::setfill('0')<<idx<<": ";
      }
      sshex<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)el;
      sshex<<std::dec<<std::setfill(' ')<<" ";

      // ascii representation
      unsigned char elcl = ( (int)el > 0x1f && (int)el < 0x7f) ? el : '.';
      sschr<<std::dec<<std::setfill(' ')<<elcl;

      // do actual printing (including very last potentially incomplete line)
      if ( (idx+1)%width == 0 || i == breakidx-1 )
      {
        // fill up any hexadecimal string that is shorter than default width
        while ( sshex.str().size() < 3*width+10 ) sshex<<" ";

        // print hexadecimal and ascii representation
        std::cout<<sshex.str()<<"    "<<sschr.str();
        std::cout<<"\n";

        // clear stringstreams
        sshex.str(std::string());
        sschr.str(std::string());
      }

      // count number of bytes
      idx++;
    }
    std::cout<<"\n";
  }
}

#endif

//---------------------------------------------------------------------------//
