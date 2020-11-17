//---------------------------------------------------------------------------//
//
//  @file raweat.hpp
//  @author Mario Fink <mario.fink@record-evolution.de>
//  @date Feb 2020, 2020-07-03
//  @brief header only class for decoding the imc ".raw" format
//
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

// #include "endian.hpp"
#include "hexshow.hpp"

// support for 16bit floats
// #include <emmintrin.h>
// #include <immintrin.h>
// #include <f16cintrin.h>
// #include "half.hpp"
// #include "half_precision_floating_point.hpp"

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

  // error message queue
  std::string error_queue_;

  // check format validity
  bool valid_ = true;

public:

  // constructor
  raw_eater(std::string rawfile, bool showlog = false) : rawfile_(rawfile)
  {
    // trigger setup and conversion
    //if ( ! rawfile.empty() ) this->setup_and_conversion(showlog);
  }

  raw_eater()
  {

  }

  // destructor
  ~raw_eater()
  {

  }

  // provide/set new raw file
  void set_file(std::string rawfile, bool showlog = false)
  {
    // set file
    rawfile_ = rawfile;

    // reset arrays
    datasec_.clear();
    segments_.clear();
    datmes_.clear();

    // reset error queue and validity flag
    error_queue_ = std::string("");
    valid_ = true;

    // do setup and conversion
    //setup_and_conversion(showlog);
  }

  // set up and conversion
  void setup_and_conversion(bool showlog = false)
  {
    // make sure 'raw_file' is already set
    assert ( !rawfile_.empty() );

    // open file
    std::ifstream fin(rawfile_.c_str(),std::ifstream::binary);
    assert ( fin.good() &&  "failed to open file" );

    // put data in buffer
    std::vector<unsigned char> rawdata((std::istreambuf_iterator<char>(fin)),
                                       (std::istreambuf_iterator<char>()));
    rawdata_ = rawdata;

    // close file
    fin.close();

    if ( showlog )
    {
      // show raw data and display predefined markers
      this->show_buffer();
      this->show_markers();
    }

    // extract data corresponding to predefined markers from buffer
    find_markers();

    // proceed only, if markers suggest data is valid *.raw format
    if ( valid_ )
    {
      // show detected markers
      if ( showlog ) display_markers();

      // split data corresponding to markers into segments
      split_segments();

      // convert binary data to arrays of intrinsic data types
      convert_data(showlog);

      // check result
      if ( segments_.size() == 0 || datmes_.size() == 0 ) valid_ = false;
    }
    else
    {
      // throw error with collected error messages
      throw std::runtime_error(error_queue_);
    }
  }

  // display buffer/data properties
  void show_buffer(int numel = 32)
  {
    hex::show(rawdata_,numel,0,0);
    // this->show_hex(rawdata_,32,0);
  }

  // show predefined markers
  void show_markers()
  {
    std::cout<<"\n";
    for ( auto el: markers_ )
    {
      assert( el.second.size() > 0 && "please don't define any empty markers" );

      std::cout<<el.first<<"  ";
      for ( unsigned char c: el.second) std::cout<<std::hex<<int(c);
      std::cout<<"\n";
    }
    std::cout<<std::dec;
    std::cout<<"\n";
  }

  //-------------------------------------------------------------------------//

  // find predefined markers in data buffer
  void find_markers()
  {
    assert( datasec_.size() == 0 );
    assert( segments_.size() == 0 );

    for (std::pair<std::string,std::vector<unsigned char>> mrk : markers_ )
    {
      // find marker's byte sequence in buffer
      for ( unsigned long int idx = 0; idx < (rawdata_.size() - mrk.second.size()); idx++ )
      {
        // for every byte in buffer, check, if the three subsequent bytes
        // correspond to required predefined marker
        bool gotit = true;
        for ( unsigned long int mrkidx = 0; mrkidx < mrk.second.size() && gotit; mrkidx ++ )
        {
          if ( ! (mrk.second[mrkidx] == rawdata_[idx+mrkidx]) ) gotit = false;
        }

        // if we got the marker, collect following bytes until end of marker byte 0x3b
        if ( gotit )
        {
          // array of data associated to marker
          std::vector<unsigned char> markseq;
          int seqidx = 0;

          // read any marker but the data marker
          if ( mrk.first != "datas marker" )
          {
            // collect bytes until we find a semicolon ";", i.e. 0x3b (or until buffer is depleted)
            while ( rawdata_[idx+seqidx] != 0x3b )
            {
              markseq.push_back(rawdata_[idx+seqidx]);
              seqidx++;

              // if buffer is depleted before we find the proper termination of
              // the markers, the data seems to be corrupted!!
              if ( idx+seqidx == rawdata_.size()-1 )
              {
                std::string errmess = mrk.first + std::string(" is corrupted");
                // throw std::runtime_error(errmess);
                error_queue_ += errmess + std::string(" - ");
                break;
              }
            }
          }
          // data marker is supposed to be located at the very end of the buffer
          // but still be terminated by a semicolon (but may contain any number
          // of semicolons in between)
          else
          {
            // collect data sequence (ignoring final semicolon)
            while ( idx+seqidx < rawdata_.size()-1 )
            {
              markseq.push_back(rawdata_[idx+seqidx]);
              seqidx++;
            }

            // check for terminating semicolon
            if ( rawdata_.back() != 0x3b )
            {
              std::string errmess = mrk.first + std::string(" is corrupted");
              error_queue_ += errmess + std::string(" - ");
            }
          }

          // find length of data sequence
          if ( mrk.first == "datas marker" )
          {
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
      if ( datasec_[mrk.first].size() == 0 )
      {
        std::string errmess = mrk.first + std::string(" not found in buffer");
        // // std::cerr<<errmess;
        // try {
        //   throw std::runtime_error(errmess);
        // } catch( const std::exception& e ) {
        //   throw;
	      //   //std::cout<<e.what()<<"\n";
        // }
        error_queue_ += errmess + std::string(" - ");

        // if any of the required (essential) markers are missing => invalid!
        valid_ = false;
      }
      totalmarksize += datasec_[mrk.first].size();
    }
    // std::cout<<"totalmarksize "<<totalmarksize<<"\n";
    // std::cout<<"\n";

    // check validity of format
    // valid_ = ( totalmarksize < 100 ) ? false : true;
  }

  // display content of found markers
  void display_markers()
  {
    for (std::pair<std::string,std::vector<unsigned char>> mrk : markers_ )
    {
      // extract data of marker
      std::vector<unsigned char> mrkdat = this->datasec_[mrk.first];

      // show name of marker and its data
      std::cout<<mrk.first<<" : "<<mrkdat.size()
               <<"\n------------------------------------\n";
      hex::show(mrkdat,32,0,0);
    }
    std::cout<<"\n";
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
    assert( datasec_.size() > 0 );
    assert( segments_.size() == 0 );

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
        // note that data segment of "datas marker" may contain any number
        // of 0x2c's (0x2c = comma ',')
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

  //-------------------------------------------------------------------------//

  // convert actual measurement data
  void convert_data(bool showlog = false)
  {
    assert( segments_.size() > 0 && "extract markers and separate into segments before conversion!" );
    assert( datmes_.size() == 0 );

    // by convention, the actual data is the 4th element in respective segment
    std::string datstr = segments_["datas marker"][3];
    std::vector<unsigned char> datbuf(datstr.begin(),datstr.end());

    // retrieve datatype from datatype segment
    int dattype = std::stoi(segments_["datyp marker"][4]);
    int typesize = std::stoi(segments_["datyp marker"][5]);

    // retrieve transformation index, factor and offset
    int trafo = 0;
    double factor = 1., offset = 0.;
    if ( segments_["punit marker"].size() > 4 )
    {
      trafo = std::stoi(segments_["punit marker"][2]);
      factor = std::stod(segments_["punit marker"][3]);
      offset = std::stod(segments_["punit marker"][4]);
    }

    if ( showlog )
    {
      std::cout<<"dattype:    "<<dattype<<"\n"
               <<"typesize:   "<<typesize<<"\n"
               <<"trafo:      "<<trafo<<"\n"
               <<"factor:     "<<factor<<"\n"
               <<"offset:     "<<offset<<"\n\n";
      std::cout<<"sizeof(unsigned char):      "<<sizeof(unsigned char)<<"\n"
               <<"sizeof(signed char):        "<<sizeof(signed char)<<"\n"
               <<"sizeof(unsigned short int): "<<sizeof(unsigned short int)<<"\n"
               <<"sizeof(signed short int):   "<<sizeof(signed short int)<<"\n"
               <<"sizeof(int):                "<<sizeof(int)<<"\n"
               <<"sizeof(unsigned long int):  "<<sizeof(unsigned long int)<<"\n"
               <<"sizeof(signed long int):    "<<sizeof(signed long int)<<"\n"
               <<"sizeof(float):              "<<sizeof(float)<<"\n"
               <<"sizeof(double):             "<<sizeof(double)<<"\n"
               <<"\n\n";
    }

    // if trafo = 0, make sure that factor and offset don't affect result
    assert ( ( ( trafo == 0 && factor == 1.0 && offset == 0.0 )
            || ( trafo == 1 ) )
            && "internally inconsistent 'punit' marker" );

    // just don't support weird datatypes
    // assert ( dattype > 2 && dattype < 12 );
    if ( dattype == 9 || dattype == 10 || dattype > 11 )
    {
      valid_ = false;
    }
    else
    {
      // switch for datatypes
      switch ( dattype )
      {
        case 1 :
          assert ( sizeof(unsigned char)*8 == typesize );
          convert_data_as_type<unsigned char>(datbuf,factor,offset);
          break;
        case 2 :
          assert ( sizeof(signed char)*8 == typesize );
          convert_data_as_type<signed char>(datbuf,factor,offset);
          break;
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
          // assert ( sizeof(signed long int)*8 == typesize );
          // convert_data_as_type<signed long int>(datbuf,factor,offset);
          // throw std::runtime_error("warning: 'signed long int' datatype with experimental support");
          assert ( sizeof(int)*8 == typesize );
          convert_data_as_type<int>(datbuf,factor,offset);
          break;
        case 7 :
          assert ( sizeof(float)*8 == typesize );
          convert_data_as_type<float>(datbuf,factor,offset);
          break;
        case 8 :
          assert ( sizeof(double)*8 == typesize );
          convert_data_as_type<double>(datbuf,factor,offset);
          break;
        case 9 :
          throw std::runtime_error("'imc Devices Transitional Recording' datatype not supported");
          break;
        case 10 :
          throw std::runtime_error("'Timestamp Ascii' datatype not supported");
          break;
        case 11 :
          // throw std::runtime_error("warning: '2-Byte-Word digital' datatype with experimental support");
          assert ( sizeof(short int)*8 == typesize );
          convert_data_as_type<short int>(datbuf,factor,offset);
          break;
      }

      // show excerpt of result
      if ( showlog )
      {
        std::cout<<"\nlength of data: "<<datmes_.size()<<"\n";
        std::cout<<"\nheader excerpt of data:\n";
        for ( unsigned long int i = 0; i < datmes_.size() && i < 10; i++ )
        {
          std::cout<<datmes_[i]<<" ";
        }
        std::cout<<"\n\n";
      }

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

      // parse all bytes of the number
      for ( int byi = 0; byi < (int)sizeof(dattype); byi++ )
      {
        pnum[byi] = (int)datbuf[(unsigned long int)(numfl*sizeof(dattype)+byi)];
      }

      // add number to array
      datmes_.push_back((double)num * factor + offset);
    }
  }

  //-------------------------------------------------------------------------//

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

  // get validity of format
  bool get_valid()
  {
    return valid_;
  }

  // get timestep
  double get_dt()
  {
    return valid_ && segments_["sampl marker"].size() > 1 ?
      std::stod(segments_["sampl marker"][2]) : 0.0;
  }

  // get time unit
  std::string get_temp_unit()
  {
    return valid_ && segments_["sampl marker"].size() > 4 ?
      segments_["sampl marker"][5] : std::string("None");
  }

  // get name of measured entity
  std::string get_name()
  {
    return valid_ && segments_["ename marker"].size() > 5
      ? segments_["ename marker"][6] : std::string("None");
  }

  // get unit of measured entity
  std::string get_unit()
  {
    if ( datasec_["punit marker"].size() > 0
     && segments_["punit marker"].size() > 6 )
    {
      return valid_ ? segments_["punit marker"][7] : std::string("None");
    }
    else
    {
      return std::string("None");
    }
  }

  // get time offset
  double get_time_offset()
  {
    return valid_ && segments_["minma marker"].size() > 10 ?
      std::stod(segments_["minma marker"][11]) : -1.0;
  }

  // get time array
  std::vector<double> get_time()
  {
    // assert ( datmes_.size() > 0 );

    if ( valid_ )
    {
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
    else
    {
      return std::vector<double>();
    }
  }

  // get size/length of data
  unsigned long int get_n()
  {
    return datmes_.size();
  }

  // get data array encoded as floats/doubles
  std::vector<double>& get_data()
  {
    return datmes_;
  }

  // get segment's array of elements
  std::vector<std::string> get_segment(std::string marker)
  {
    assert ( segments_.size() > 0 );

    return segments_[marker];
  }

  // write csv file
  void write_table(std::string filename, char delimiter = ',', int precision = 6, int width = 25)
  {
    if ( valid_ )
    {
      // open file
      std::ofstream fout(filename.c_str());

      // define column names (including units)
      std::string colA = std::string("Time [") + get_temp_unit() + std::string("]");
      std::string colB = get_name() + std::string(" [") + get_unit() + std::string("]");

      // write header
      if ( delimiter != ' ' )
      {
        fout<<colA<<delimiter<<colB<<"\n";
      }
      else
      {
        fout<<std::setw(width)<<std::right<<colA;
        fout<<std::setw(width)<<std::right<<colB;
        fout<<"\n";
      }

      // get time step and offset
      double dt = get_dt();
      double timoff = get_time_offset();

      // count sample index
      unsigned long int tidx = 0;
      for ( auto el : datmes_ )
      {
        // get time
        double tim = tidx*dt + timoff;

        if ( delimiter != ' ' )
        {
          fout<<std::fixed<<std::dec<<std::setprecision(precision)
              <<tim<<delimiter<<el<<"\n";
        }
        else
        {
          fout<<std::fixed<<std::dec<<std::setprecision(precision)
              <<std::setw(width)<<std::right<<tim;
          fout<<std::fixed<<std::dec<<std::setprecision(precision)
              <<std::setw(width)<<std::right<<el;
          fout<<"\n";
        }

        // keep track of timestep
        tidx++;
      }

      // close file
      fout.close();
    }
  }

};

#endif

//---------------------------------------------------------------------------//
