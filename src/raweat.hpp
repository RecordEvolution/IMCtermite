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

#include "endian.hpp"

// support for 16bit floats
#include <emmintrin.h>
#include <immintrin.h>
//#include <f16cintrin.h>
#include "half.hpp"
//#include "half_precision_floating_point.hpp"

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

  // split segments into arrays of simple number/string element
  std::map<std::string,std::vector<std::string>> segments_;

  // length of data array
  unsigned long int datsize_;

  // save all data, i.e. physical values of measured entities as 64bit double
  std::vector<double> datmes_;

public:

  // constructor
  raw_eater(std::string rawfile) : rawfile_(rawfile)
  {
    // open file and put data in buffer
    std::ifstream fin(rawfile.c_str(),std::ifstream::binary);
    assert( fin.good() && "failed to open file" );
//    try {
//      std::ifstream fin(rawfile.c_str(),std::ifstream::binary);
//    }
//    catch (std::ifstream::failure e) {
//      std::cerr<<"opening file " + rawfile + " failed";
//    }
    std::vector<unsigned char> rawdata((std::istreambuf_iterator<char>(fin)),
                                       (std::istreambuf_iterator<char>()));
    rawdata_ = rawdata;

    // prepare and convert data
    find_markers();
    split_segments();
    convert_data();
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

//---------------------------------------------------------------------------//

  // find predefined markers in data buffer
  void find_markers()
  {
    for (std::pair<std::string,std::vector<unsigned char>> mrk : markers_ )
    {
      assert( mrk.second.size() > 0 && "please don't define any empty marker" );

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
            // data marker is actually assumed to be the last and should extend until end of file
            for ( unsigned long int didx = idx; didx < rawdata_.size()-1; didx++ )
            {
              markseq.push_back(rawdata_[didx]);
            }

            // obtain length of data segment
            datsize_ = markseq.size();
          }

          // save segment corresponding to marker
          datasec_.insert(std::pair<std::string,std::vector<unsigned char>>(mrk.first,markseq));
        }
      } 
    }

    // check length of all markers, i.e. check if we actually have a valid .raw file
    unsigned long int totalmarksize = 0;
    for (std::pair<std::string,std::vector<unsigned char>> mrk : markers_ )
    {
      //assert ( datasec_[mrk.first].size() > 0 && "marker segment of length zero" );
      totalmarksize += datasec_[mrk.first].size();
    }
    assert ( totalmarksize > 0 && "didn't find any predefined marker => probably not a valid .raw-file" );

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

  // split data segments into arrays
  void split_segments()
  {
    // split segments of all markers 
    for (std::pair<std::string,std::vector<unsigned char>> mrk : markers_ )
    {
      // declare empty array for this segment and auxiliary string
      std::vector<std::string> segvec;
      std::string elstr("");

      // only start collecting after first comma in segment
      bool parse = false;

      // count number of commata
      long int commcount = 0;

      // parse data segment
      for ( unsigned char el: datasec_[mrk.first] )
      {
        // note that data segment of "datas marker" may contain any number of 0x2c's
        if ( ( el != 0x2c && parse ) || ( mrk.first == "datas marker" && commcount > 2 ) )
        {
          elstr.push_back(el);
        }
        else if ( el == 0x2c && parse )
        {
          // comma marks end of element of segment: save string and reset it
          segvec.push_back(elstr);
          elstr = std::string("");
          commcount++;
        }
        else 
        {
          // enable parsing after first comma
          if ( el == 0x2c ) parse = true;
        }
      }
      // include last element
      segvec.push_back(elstr);

      // save array of elements 
      segments_.insert(std::pair<std::string,std::vector<std::string>>(mrk.first,segvec));;
    }
  }

//---------------------------------------------------------------------------//

  // convert actual measurement data
  void convert_data()
  {
    assert( segments_.size() > 0 && "extract markers and separate into segments before conversion!" );

    // by convention, the actual data is the 4th element in respective segment
    std::string datstr = segments_["datas marker"][3];
    std::vector<unsigned char> datbuf(datstr.begin(),datstr.end());

    // retrieve datatype from datatype segment
    int dattype = std::stoi(segments_["datyp marker"][4]);
    int typesize = std::stoi(segments_["datyp marker"][5]);

    // retrieve transformation index, factor and offset
    int trafo = std::stoi(segments_["punit marker"][2]);
    double factor = std::stod(segments_["punit marker"][3]);
    double offset = std::stod(segments_["punit marker"][4]);

    // if traf = 0, make sure that factor and offset don't affect result
    assert ( ( trafo == 0 && factor == 1.0 && offset == 0.0 )
          || ( trafo == 1 ) ); 

    // just don't support weird datatypes
    assert ( dattype > 2 && dattype < 9 );

    // switch for datatypes
    switch ( dattype )
    {
      case 3 :
        assert ( sizeof(unsigned short int)*8 == typesize ); 
        convert_data_as_type<unsigned short int>(datbuf,factor,offset); 
        break;
      case 4 :
        assert ( sizeof(signed short int)*8 == typesize ); 
        convert_data_as_type<signed short int>(datbuf,factor,offset); 
        break;
      case 5 :
        assert ( sizeof(unsigned long int)*8 == typesize ); 
        convert_data_as_type<unsigned long int>(datbuf,factor,offset); 
        break;
      case 6 :
        assert ( sizeof(signed long int)*8 == typesize ); 
        convert_data_as_type<signed short int>(datbuf,factor,offset); 
        break;
      case 7 :
        assert ( sizeof(float)*8 == typesize ); 
        convert_data_as_type<float>(datbuf,factor,offset); 
        break;
      case 8 :
        assert ( sizeof(double)*8 == typesize ); 
        convert_data_as_type<double>(datbuf,factor,offset); 
        break;
    }

  }

  // convert bytes to specific datatype
  template<typename dattype> void convert_data_as_type(std::vector<unsigned char> &datbuf, double factor, double offset)
  {
    // check consistency of bufffer size with size of datatype
    assert ( datbuf.size()%sizeof(dattype) == 0 && "length of buffer is not a multiple of size of datatype" );
   
    // get number of numbers in buffer
    unsigned long int totnum = datbuf.size()/sizeof(dattype); 

    for ( unsigned long int numfl = 0; numfl < totnum; numfl++ )
    {
      // declare instance of required datatype and perform recast in terms of uint8_t
      dattype num;
      uint8_t* pnum = reinterpret_cast<uint8_t*>(&num);

      // parse all  bytes of the number
      for ( int byi = 0; byi < (int)sizeof(dattype); byi++ )
      {
        pnum[byi] = (int)datbuf[(unsigned long int)(numfl*sizeof(dattype)+byi)];
      }

      // add number of array
      datmes_.push_back((double)num * factor + offset);
    }
  }

//---------------------------------------------------------------------------//

  // show hex dump
  void show_hex(std::vector<unsigned char> &datavec, int width = 32, unsigned long int maxchars = 512)
  {
    // compose hex string and encoded string
    std::stringstream hex, enc;

    for ( unsigned long int i = 0; i < datavec.size() && i < maxchars; i++ )
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

      // every 'width' number of chars constitute a row
      if ( (int)(i+1)%width == 0 )
      {
        // print both strings
        std::cout<<std::setw(3*width)<<std::left<<hex.str()<<"    "<<enc.str()<<"\n";
        std::cout<<std::right;
        
        // clear stringstreams
        hex.str(std::string());
        enc.str(std::string());
      }
    }

    // print final remaining part
    std::cout<<std::setw(3*width)<<std::left<<hex.str()<<"    "<<enc.str()<<"\n";
    std::cout<<std::right;
    std::cout<<std::dec;
  }

  // get timestep
  double get_dt()
  {
    assert ( segments_.size() > 0 );

    return std::stod(segments_["sampl marker"][2]);
  }

  // get time unit
  std::string get_temp_unit()
  {
    assert ( segments_.size() > 0 );

    return segments_["sampl marker"][5];
  }

  // get name of measured entity
  std::string get_name()
  {
    assert ( segments_.size() > 0 );

    return segments_["ename marker"][6];
  }

  // get unit of measured entity
  std::string get_unit()
  {
    assert ( segments_.size() > 0 );

    return segments_["punit marker"][7];
  }

  // get time offset 
  double get_time_offset()
  {
    assert ( segments_.size() > 0 );

    return std::stod(segments_["minma marker"][11]);
  }

  // get time array
  std::vector<double> get_time()
  {
    assert ( datmes_.size() > 0 );

    // declare array of time
    std::vector<double> timearr;

    // get time step and offset
    double dt = get_dt();
    double timoff = get_time_offset();

    // fill array
    for ( unsigned long int t = 0; t < datmes_.size(); t++ )
    {
      timearr.push_back(timoff + t*dt);
    }

    return timearr;
  }

  // get data array encoded as floats/doubles
  std::vector<double>& get_data()
  {
    assert ( datmes_.size() > 0 );

    return datmes_;
  }

  // get segment's array of elements
  std::vector<std::string> get_segment(std::string marker)
  {
    assert ( segments_.size() > 0 );

    return segments_[marker];
  }

  // write data to csv-like file
  void write_data(std::string filename, int precision = 9, int width = 25)
  {
    assert ( segments_.size() > 0 );
    assert ( datmes_.size() > 0 );

    // open file
    std::ofstream fout(filename.c_str());

    // write header
//    fout<<"# ";
    std::string colA = std::string("Time [") + get_temp_unit() + std::string("]");
    std::string colB = get_name() + std::string(" [") + get_unit() + std::string("]");
    if ( width > 0 )
    { 
//      fout<<std::setw(width)<<std::left<<colA;
//      fout<<std::setw(width)<<std::left<<colB;
        fout<<std::setw(width)<<std::right<<"Time";
        fout<<std::setw(width)<<std::right<<get_name();
        fout<<"\n";
        fout<<std::setw(width)<<std::right<<get_temp_unit();
        fout<<std::setw(width)<<std::right<<get_unit();
    } 
    else 
    {
//      fout<<colA<<","<<colB;
      fout<<"Time"<<","<<get_name()<<"\n";
      fout<<get_temp_unit()<<";"<<get_unit();
    }
    fout<<"\n";

    // get time step and offset
    double dt = get_dt();
    double timoff = get_time_offset();

    // count sample index
    unsigned long int tidx = 0;
    for ( auto el : datmes_ )
    {
      // get time
      double tim = tidx*dt + timoff;

      if ( width > 0 )
      {
        fout<<std::fixed<<std::dec<<std::setprecision(precision)<<std::setw(width)<<std::right<<tim;
        fout<<std::fixed<<std::dec<<std::setprecision(precision)<<std::setw(width)<<std::right<<el;
      }
      else
      {
        fout<<std::fixed<<std::dec<<std::setprecision(precision)<<tim<<","<<el;
      }
      fout<<"\n";

      // keep track of timestep
      tidx++;
    }

    // close file
    fout.close();
  }

};

#endif

//---------------------------------------------------------------------------//
