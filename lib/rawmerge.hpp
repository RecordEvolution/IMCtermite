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

  // (merged) channel data related to SINGLE time series
  std::vector<std::vector<double>> channels_;
  std::vector<double> timeseries_;

  // temporal unit and channel meta data
  std::string temp_unit_;
  std::vector<std::string> channel_names_;

public:

  // constructor
  raw_merger(std::string rawfile): raw_eater(rawfile)
  {
    // add first initial channel
    this->add_channel(rawfile);
  }

  raw_merger(): raw_eater()
  {

  }

  // destructor
  ~raw_merger()
  {

  }

  // add a single channel and its associated time series
  bool add_channel(std::string rawfile)
  {
    // set raw file and perform conversion
    this->set_file(rawfile);

    // add first/initial time series (and channel data)
    if ( timeseries_.size() == 0 && channels_.size() == 0 )
    {
      std::cout<<"adding first channel\n";
      // insert timeseries and its unit
      timeseries_ = this->get_time();
      temp_unit_ = this->get_temp_unit();

      // insert channel data and its meta data
      channels_.push_back(this->get_data());
      channel_names_.push_back(this->get_name() + std::string(" [")
                             + this->get_unit() + std::string("]"));

      return true;
    }
    else
    {
      // TODO check consistency of time series, time unit, channels....
      return false;
    }
  }

  // print all data to file
  void write_table(std::string filename, char delimiter = ',', int precision = 6, int width = 25)
  {
    // if at least one channel including its time series is present
    if ( timeseries_.size() > 0 && channels_.size() > 0 )
    {
      // open file
      std::ofstream fout(filename.c_str());

      if ( fout.good() )
      {
        // define temporal column header
        std::string colTime = std::string("Time [") + temp_unit_ + std::string("]");

        // write header
        if ( delimiter != ' ' )
        {
          fout<<colTime;
          for ( std::string chnam: this->channel_names_ ) fout<<delimiter<<chnam;
        }
        else
        {
          fout<<std::setw(width)<<std::right<<colTime;
          for ( std::string chnam: this->channel_names_ ) fout<<std::setw(width)<<std::right<<chnam;
        }
        fout<<"\n";

        // write actual time and channel data
        unsigned long int tidx = 0;
        for ( double ti: this->timeseries_ )
        {
          if ( delimiter != ' ' )
          {
            fout<<std::fixed<<std::dec<<std::setprecision(precision)<<ti;
            for ( std::vector<double> chn: this->channels_ )
            {
              fout<<delimiter<<std::dec<<std::setprecision(precision)<<chn[tidx];
            }
          }
          else
          {
            fout<<std::fixed<<std::dec<<std::setprecision(precision)
                <<std::setw(width)<<std::right<<ti;
            for ( std::vector<double> chn: this->channels_ )
            {
              fout<<std::fixed<<std::dec<<std::setprecision(precision)
                  <<std::setw(width)<<std::right<<chn[tidx];
            }
          }
          fout<<"\n";

          // count number of samples
          tidx++;
        }

        // close file
        fout.close();
      }
    }
  }

};

#endif

//---------------------------------------------------------------------------//
