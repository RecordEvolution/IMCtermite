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
#include <limits>

//---------------------------------------------------------------------------//

class raw_merger : public raw_eater
{
private:

  // number of channels
  int num_channels_;

  // (merged) channel data related to SINGLE time series
  std::vector<std::vector<double>> channels_;
  std::vector<double> timeseries_;

  // timestep of original timeseries
  double dt_;

  // temporal unit and channel meta data
  std::string temp_unit_;
  std::vector<std::string> channel_names_;

public:

  // constructor
  raw_merger(std::string rawfile): raw_eater(rawfile)
  {
    // add first initial channel
    //this->add_channel(rawfile);
  }

  raw_merger(): raw_eater()
  {

  }

  // destructor
  ~raw_merger()
  {

  }

  // get list of channel names
  std::vector<std::string> get_channel_names()
  {
    return this->channel_names_;
  }

  // get unified timeseries
  std::vector<double> get_time_series()
  {
    return this->timeseries_;
  }

  // get number of channels
  unsigned long int get_num_channels()
  {
    return this->channels_.size();
  }

  // get array representation i-th channel
  std::vector<double> get_channel(long unsigned int chidx)
  {
    return chidx >= 0 && chidx < this->channels_.size() ?
      this->channels_[chidx] : std::vector<double>();
  }

  // add a single channel and its associated time series
  bool add_channel(std::string rawfile, bool showlog = false)
  {
    // set raw file and perform conversion
    this->set_file(rawfile,showlog);
    // try {
    this->setup_and_conversion();
    // } catch (const std::exception& e) {
    //   // throw;
    //   std::cout<<e.what();
    // }

    // show channel name, unit, timestep, time unit, etc.
    if ( showlog && this->get_valid() )
    {
      std::cout<<"Time ["<<this->get_temp_unit()<<"]"<<"\n";
      for ( unsigned long int i = 0; i < 5; i++ ) std::cout<<this->get_time()[i]<<"\n";
      std::cout<<this->get_name()<<" ["<<this->get_unit()<<"]"<<"\n";
      for ( unsigned long int i = 0; i < 5; i++ ) std::cout<<this->get_data()[i]<<"\n";
      std::cout<<"lenght of channel "<<this->get_time().size()<<"\n";
      std::cout<<"\n";
    }

    // add first/initial time series (and channel data)
    if ( this->get_valid() && timeseries_.size() == 0 && channels_.size() == 0 )
    {
      if ( showlog ) std::cout<<"adding initial channel "<<rawfile<<"\n\n";

      // insert timeseries and its unit
      this->timeseries_ = this->get_time();
      this->temp_unit_ = this->get_temp_unit();

      // get timestep size
      this->dt_ = this->get_dt();

      // insert channel data and its meta data
      this->channels_.push_back(this->get_data());
      this->channel_names_.push_back(this->get_name() + std::string(" [")
                                   + this->get_unit() + std::string("]"));

      return true;
    }
    else if ( this->get_valid() )
    {
      if ( showlog ) std::cout<<"adding next channel "<<rawfile<<"\n\n";

      // check consistency of temporal unit
      if ( this->get_temp_unit() == this->temp_unit_ )
      {
        // get new time series and data
        std::vector<double> ts = this->get_time();
        std::vector<double> td = this->get_data();

        // compare start/end of timeseries (define tolerance)
        double deltat = 9*fmax(this->get_dt(),this->dt_);
        if ( ( fabs( this->timeseries_[0]     - ts[0] )     < deltat  )
          && ( fabs( this->timeseries_.back() - ts.back() ) < deltat ) )
        {
          // resulting new time series
          std::vector<double> newts;
          std::vector<std::vector<double>> newchannels;

          // submerge channel with currently defined time series
          bool succ = this->merge_channels(this->timeseries_,this->channels_, // current timeseries and exisiting channels
                                           ts,td,                             // new timeseries and associated channel to add
                                           newts,newchannels);                // resulting new timeseries and new group of channels

          // save result in members
          this->timeseries_ = newts;
          this->channels_ = newchannels;

          // insert channel's meta data
          this->channel_names_.push_back(this->get_name() + std::string(" [")
                                       + this->get_unit() + std::string("]"));

          return succ;
        }
        else
        {
          // refuse to merge due to inconsistent start of timeseries
          std::string errmess = std::string("rawmerge: add_channel '") + rawfile
                              + std::string("' : inconsistent start of time series, i.e. ")
                              + std::to_string(timeseries_[0]) + std::string(" vs. ")
                              + std::to_string(ts[0])
                              + std::string("( timesteps: ") + std::to_string(this->get_dt())
                              + std::string(" and ") + std::to_string(this->dt_) + std::string(")");
          // std::cerr<<errmess<<"\n";
          throw std::runtime_error(errmess);

          return false;
        }
      }
      else
      {
        // refuse to merge due to different temporal units
        std::string errmess = std::string("rawmerge: add_channel '") + rawfile
                            + std::string("' : inconsistent time units: ', i.e. '")
                            + this->get_temp_unit() + std::string("' vs. '")
                            + this->temp_unit_ + std::string("'");
        // std::cerr<<errmess<<"\n";
        throw std::runtime_error(errmess);

        return false;
      }
    }
    else
    {
      // provided file does not feature a valid .raw format
      std::string errmess = std::string("rawmerge: add_channel '") + rawfile
                          + std::string("' : invalid .raw file");
      // std::cerr<<errmess<<"\n";
      throw std::runtime_error(errmess);

      return false;
    }
  }

  // merge new channel and associated time series with exisiting channels and
  // their already merged timeseries
  bool merge_channels(std::vector<double>& current_timeseries,             // current timeseries and associated...
                      std::vector<std::vector<double>>& current_channels,  // ...exisiting (n) channels
                      std::vector<double>& new_timeseries,                 // new timeseries and associated...
                      std::vector<double>& new_channel,                    // ...new channel
                      std::vector<double>& result_timeseries,              // resulting timeseries ...
                      std::vector<std::vector<double>>& result_channels,   // ...and associated (n+1) channels
                      double placeholder = std::numeric_limits<double>::quiet_NaN(),
                      bool showlog = false)
  {
    if ( showlog )
    {
      std::cout<<"merge_channels:\n"
               <<"current timeseries length: "<<std::setw(5)<<current_timeseries.size()
                                              <<std::setw(10)<<std::setprecision(8)<<current_timeseries[0]
                                              <<std::setw(10)<<std::setprecision(8)<<current_timeseries.back()<<"\n"
               <<"number of exist. channels: "<<std::setw(5)<<current_channels.size()<<"\n"
               <<"new timeseries length    : "<<std::setw(5)<<new_timeseries.size()
                                              <<std::setw(10)<<std::setprecision(8)<<new_timeseries[0]
                                              <<std::setw(10)<<std::setprecision(8)<<new_timeseries.back()<<"\n"
               <<"new channel length       : "<<std::setw(5)<<new_channel.size()<<"\n\n";
    }

    // the prepared resulting timeseries and channels are supposed to be empty
    assert ( result_timeseries.size() == 0 && result_channels.size() == 0 );

    // make sure new channel and its timeseries are consistent
    if ( new_timeseries.size() != new_channel.size() ) return false;

    // check consistency of all exisiting channels and their timeseries as well
    for ( std::vector<double> chan: current_channels )
    {
      if ( chan.size() != current_timeseries.size() ) return false;
    }

    // number of current exisiting channels
    unsigned long int numchannels = current_channels.size();

    // insert empty (n+1) channels
    for ( unsigned long int i = 0; i < numchannels+1; i++ )
    {
      result_channels.push_back(std::vector<double>());
    }

    // current time step index for both (current and new) series
    unsigned long int idxCur = 0, idxNew = 0;

    // process all time steps in both time series
    while ( idxCur < current_timeseries.size() || idxNew < new_timeseries.size() )
    {
      if ( showlog ) std::cout<<"idxCur "<<std::setw(6)<<idxCur
                              <<std::setw(20)<<std::right<<current_timeseries[idxCur]<<"\n"
                              <<"idxNew "<<std::setw(6)<<idxNew
                              <<std::setw(20)<<std::right<<new_timeseries[idxNew]<<"\n";

      // if point in time of "current_timeseries" is BEFORE time of "new_timeseries"
      // or "new_timeseries" is depleted
      if ( idxCur < current_timeseries.size() &&
         ( current_timeseries[idxCur] + 1.0e-10 < new_timeseries[idxNew]
        || idxNew == new_timeseries.size() ) )
      {
        if ( showlog ) std::cout<<"push_back A\n";

        // keep current data as it is ...
        for ( unsigned long int ch = 0; ch < numchannels; ch++ )
        {
          result_channels[ch].push_back(current_channels[ch][idxCur]);
        }
        // and insert placeholder in new channel
        result_channels[numchannels].push_back(placeholder);

        // add timestep to resulting timeseries and increment associated counter
        result_timeseries.push_back(current_timeseries[idxCur]);
        idxCur++;
      }
      // ...just reversed...
      else if ( idxNew < new_timeseries.size() &&
              ( current_timeseries[idxCur] > new_timeseries[idxNew] + 1.0e-10
             || idxCur == current_timeseries.size() ) )
      {
        if ( showlog ) std::cout<<"push_back B\n";

        // insert placeholders for all exisiting channels...
        for ( unsigned long int ch = 0; ch < numchannels; ch++ )
        {
          result_channels[ch].push_back(placeholder);
        }
        //...and add new channel as it is
        result_channels[numchannels].push_back(new_channel[idxNew]);

        // add timestep to resulting timeseries and increment associated counter
        result_timeseries.push_back(new_timeseries[idxNew]);
        idxNew++;
      }
      // ...points in time of both timeseries match...
      else
      {
        if ( showlog ) std::cout<<"push_back C\n";

        // add ALL, i.e. both current and new data to result
        for ( unsigned long int ch = 0; ch < numchannels; ch++ )
        {
          result_channels[ch].push_back(current_channels[ch][idxCur]);
        }
        result_channels[numchannels].push_back(new_channel[idxNew]);

        // add timestep to resulting timeseries and increment both counters
        result_timeseries.push_back(new_timeseries[idxNew]);
        idxNew++;
        idxCur++;
      }
    }

    return true;
  }

  // print all data to file
  void write_table_all(std::string filename, char delimiter = ',', int precision = 6, int width = 25)
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
