//---------------------------------------------------------------------------//
//
//  @file rawmerge.hpp
//  @author Mario Fink <mario.fink@record-evolution.de>
//  @date Aug 2020
//  @brief unify multiple channels and time series from .raw in single table
//
//---------------------------------------------------------------------------//

#ifndef RAW_MERGER
#define RAW_MERGER

#include "raweat.hpp"

//---------------------------------------------------------------------------//

class raw_merger : public raw_eater
{
private:

  // number of channels
  int num_channels_;

  // (merged) channel data
  std::vector<std::vector<double>> channels_;

  // channel meta data
  std::vector<std::string> channel_names_;

public:

  // constructor
  raw_merger(std::string rawfile): raw_eater(rawfile)
  {

  }

  // destructor
  ~raw_merger()
  {

  }

  // add a single channel and its associated time series
  bool add_channel(std::string rawfile)
  {
    return true;
  }

  // print all data to file
  void write_table(std::string filename, char delimiter = ',', int precision = 6, int width = 25)
  {

  }



};

#endif

//---------------------------------------------------------------------------//
