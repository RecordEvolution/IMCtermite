//---------------------------------------------------------------------------//

#ifndef RAW_EATER
#define RAW_EATER

#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iterator>
#include <map>
#include <cmath>

// support for 16bit floats
#include <emmintrin.h>
#include <immintrin.h>
//#include <f16cintrin.h>
#include "half.hpp"
//#include "half_precision_floating_point.hpp"
#include "endian.hpp"

//---------------------------------------------------------------------------//

class raw_eater
{

private:

  // filename and path
  std::string rawfile_;

  // raw buffer
  std::vector<unsigned char> rawdata_;

  // file format markers
  std::map<std::string,std::vector<unsigned char>> markers_ = { 
      {"intro marker",{0x7c,0x43,0x46}},
      {"fileo marker",{0x7c,0x43,0x4b}},
      {"vendo marker",{0x7c,0x4e,0x4f}},
      {"param marker",{0x7c,0x43,0x47}},
      {"sampl marker",{0x7c,0x43,0x44}},
      {"typei marker",{0x7c,0x4e,0x54}},
      {"dimen marker",{0x7c,0x43,0x43}},
      {"datyp marker",{0x7c,0x43,0x50}},
      {"punit marker",{0x7c,0x43,0x52}},
      {"ename marker",{0x7c,0x43,0x4e}},
      {"minma marker",{0x7c,0x43,0x62}},
      {"datas marker",{0x7c,0x43,0x53}}
   };

  // data sections corresponding to markers
  std::map<std::string,std::vector<unsigned char>> datasec_;

  // length of data array
  unsigned long int datsize_;

  // TODO preliminary: for now, we assume 32/64 bit ? floats in all data
  // index in buffer of datasec_["datas marker"] where actual data starts
  unsigned long int datstartidx_;
  std::vector<double> datmes_;

public:

  // constructor
  raw_eater(std::string rawfile) : rawfile_(rawfile)
  {
    // open file and put data in buffer
    std::ifstream fin(rawfile.c_str(),std::ifstream::binary);
    assert( fin.good() && "failed to open file" );
    try {
      std::ifstream fin(rawfile.c_str(),std::ifstream::binary);
    }
    catch (std::ifstream::failure e) {
      std::cerr<<"opening file " + rawfile + " failed";
    }
    std::vector<unsigned char> rawdata((std::istreambuf_iterator<char>(fin)),
                                       (std::istreambuf_iterator<char>()));
    rawdata_ = rawdata;

  }

  // destructor
  ~raw_eater()
  {
  
  }

  // display buffer/data properties
  void show_buffer(int numel = 128)
  {
    // show size of buffer
    std::cout<<"size of buffer "<<rawdata_.size()<<"\n\n";

    // show excerpt from buffer
    int ista = 0, iend = numel;
    for ( int i= ista; i < iend; i++ )
    {
      std::cout<<std::hex<<(int)rawdata_[i]<<" ";
      if ( (i+1)%16 == 0 ) std::cout<<"\n";
    }
    std::cout<<"\n";
  }

  // show predefined markers
  void show_markers()
  {
    std::cout<<"\n";
    for ( auto el: markers_ )
    {
      std::cout<<el.first<<"  ";
      for ( unsigned char c: el.second) std::cout<<std::hex<<int(c);
      std::cout<<"\n\n";
    }
    std::cout<<std::dec;
  }

  // find predefined markers in data buffer
  void find_markers()
  {
    for (std::pair<std::string,std::vector<unsigned char>> mrk : markers_ )
    {
      assert( mrk.second.size() > 0 && "please don't defined any empty marker" );

      // find marker's byte sequence in buffer
      for ( unsigned long int idx = 0; idx < rawdata_.size(); idx++ )
      {
        bool gotit = true;
        for ( unsigned long int mrkidx = 0; mrkidx < mrk.second.size() && gotit; mrkidx ++ )
        {
          if ( ! (mrk.second[mrkidx] == rawdata_[idx+mrkidx]) ) gotit = false;
        }
        
        // if we got the marker, collect following bytes until end of marker byte 0x 3b
        if ( gotit ) 
        {
          // array of data associated to marker
          std::vector<unsigned char> markseq;

          if ( mrk.first != "datas marker" )
          {
            // collect bytes until we find semicolon ";", i.e. 0x3b
            int seqidx = 0;
            while ( rawdata_[idx+seqidx] != 0x3b )
            {
              markseq.push_back(rawdata_[idx+seqidx]);
              seqidx++;
            }
          }
          else 
          {
            // make sure the data marker is actually the last and extends until end of file
            //assert( TODO && "data marker doesn't appear to be the very last");

            // that's the data itself
            for ( unsigned long int didx = idx; didx < rawdata_.size()-1; didx++ )
            {
              markseq.push_back(rawdata_[didx]);
            }

            // obtain length of data segment
            datsize_ = markseq.size();

            // find starting index (supposed to be after fourth comma = 0x2c)
            int countcomma = 0;
            for ( unsigned long int buidx = 0; buidx < datsize_; buidx++ )
            {
              // count number of comma chars in head of data segment
              if ( markseq[buidx] == 0x2c ) countcomma++;

              // save position following fourth comma
              if ( countcomma == 4 )
              {
                datstartidx_ = buidx + 1;
                break;
              }
            }
          }

          // save segment corresponding to marker
          datasec_.insert(std::pair<std::string,std::vector<unsigned char>>(mrk.first,markseq));
        }
      } 
    }

  }

  // get all predefined markers
  std::map<std::string,std::vector<unsigned char>> get_markers()
  {
    return markers_;
  }

  // get data associated to specific marker
  std::vector<unsigned char> get_marker_data(std::string marker)
  {
    return datasec_[marker];
  }

  // show hex dump
  void show_hex(std::vector<unsigned char> &datavec, int width = 32, unsigned long int maxchars = 512)
  {
    // compose hex string and encoded string
    std::stringstream hex, enc;

    for ( unsigned long int i = 0; i < datavec.size() && i < maxchars; i++ )
    {
      if ( (int)(i+1)%width == 0 )
      {
        // print both strings
        std::cout<<std::setw(3*width)<<std::left<<hex.str()<<"    "<<enc.str()<<"\n";
        std::cout<<std::right;
        
        // clear stringstreams
        hex.str(std::string());
        enc.str(std::string());
      }
      else
      {
        // accumulate in stringstreams
        hex<<std::nouppercase<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)datavec[i]<<" ";
        // check if byte corresponds to some control character and if it's printable
        int ic = (int)datavec[i];
        if ( ic > 0x20 && ic < 0x7f )
        {
          enc<<(char)(datavec[i]);
        }
        else 
        {
          enc<<".";
        }
      }
    }
    // print final remaining part
    std::cout<<std::setw(3*width)<<std::left<<hex.str()<<"    "<<enc.str()<<"\n";
    std::cout<<std::right;
    std::cout<<std::dec;
  }

  // convert actual measurement data
  void convert_data_32_bit_float()
  {
    assert ( (datsize_-28)%4 == 0 && "length of buffer is not a multiple of 4" );

    unsigned long int totnumfl = (datsize_-28)/(int)sizeof(float);
    for ( unsigned long int numfl = 0; numfl < totnumfl; numfl++ )
    {
      // assuming 4 byte (32bit) float
      float num = 0.0;
      uint8_t* pnum = reinterpret_cast<uint8_t*>(&num);
      for ( int byi = 0; byi < (int)sizeof(float); byi++ )
      {
        // TODO what's the byte order in the file??
        // for now, we just don't care...
        pnum[byi] = (int)datasec_["datas marker"][(unsigned long int)(28+numfl*sizeof(float)+byi)];
      }

      // add number of array
      datmes_.push_back((double)num);
    }      
  }

  // convert half-precision (16bit) floating point numbers
  void convert_data_16_bit_float()
  {
    assert ( (datsize_-28)%2 == 0 && "length of buffer is not a multiple of 2" );

    unsigned long int totnumby = (datsize_-28)/2;
    for ( unsigned long int by = 0; by < totnumby; by++ )
    {
      // declare single (16bit) floating point number
      half_float::half hfl;

      // reinterpret bytes in buffer as memory of floating point number
      uint8_t* pnum = reinterpret_cast<uint8_t*>(&hfl);
      for ( int i = 0; i < (int)sizeof(half_float::half); i++ )
      {
        pnum[i] = (int)datasec_["datas marker"][(unsigned long int)(28+by*sizeof(half_float::half)+i)];
      }

      // add number to array
      datmes_.push_back((double)hfl);
    }
  }

  // convert 16bit "decimal-encoding" floating point numbers
  void convert_data_16_bit_decimal()
  {
    assert ( (datsize_-datstartidx_)%2 == 0 && "length of data is not a multiple of 2" );

    double flstp = 0.04395;

    // parse bytes in data buffer
    unsigned long int totnumby = (datsize_-datstartidx_)/2;
    for ( unsigned long int by = 0; by < totnumby; by++ )
    {
      // retrieve set of two subsequent bytes
      std::vector<uint8_t> pnum; 
      for ( int i = 0; i < 2; i++ ) pnum.push_back(datasec_["datas marker"][(unsigned long int)(datstartidx_+by*2+i)]);

      // convert to double
      //datmes_.push_back((double)(     (((int)pnum[0]-128)*256 + (int)pnum[1])/100.0       )); 
      datmes_.push_back((double)(     (((int)pnum[1]-128)*256. + (int)pnum[0])/100.0       )); 
      //datmes_.push_back( (double) ( ( (int)pnum[0] + (int)pnum[1]*256 )*flstp ) );
    }
  }

  // get data array encoded as floats/doubles
  std::vector<double>& get_data()
  {
    return datmes_;
  }

  // write data to csv-like file
  void write_data(std::string filename, int precision = 9)
  {
    // open file
    std::ofstream fout(filename.c_str());

    for ( auto el : datmes_ )
    {
      fout<<std::dec<<std::setprecision(precision)<<el<<"\n";
    }

    // close file
    fout.close();
  }

};

#endif

//---------------------------------------------------------------------------//
