//---------------------------------------------------------------------------//

#ifndef IMCRAW
#define IMCRAW

#include <fstream>
// #include <filesystem>

#include "hexshow.hpp"
#include "imc_key.hpp"
#include "imc_block.hpp"
#include "imc_datatype.hpp"

//---------------------------------------------------------------------------//

namespace imc
{
  class raw
  {
    // (path of) raw-file and its basename
    std::string raw_file_, file_name_;

    // buffer of raw-file
    std::vector<unsigned char> buffer_;

    // list of imc-blocks
    std::vector<block> rawblocks_;

  public:

    // constructor
    raw() {};
    raw(std::string raw_file): raw_file_(raw_file) { this->parse(); };

    // provide new raw-file
    void set_file(std::string raw_file)
    {
      raw_file_ = raw_file;
      this->parse();
    }

    // list all blocks in buffer
    void parse()
    {
      // open file and put data in buffer
      try {
        std::ifstream fin(raw_file_.c_str(),std::ifstream::binary);
        if ( !fin.good() ) throw std::runtime_error("failed to open file");
        std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(fin)),
                                          (std::istreambuf_iterator<char>()));
        buffer_ = buffer;
        fin.close();
      } catch ( const std::exception& e ) {
        throw std::runtime_error(
          std::string("failed to open raw-file and stream data in buffer: ") + e.what()
        );
      }

      // start parsing raw-blocks in buffer
      for ( std::vector<unsigned char>::iterator it=buffer_.begin();
                                                it!=buffer_.end(); ++it )
      {
        // check for "magic byte"
        if ( *it == ch_bgn_ )
        {
          // check for (non)critical key
          if ( *(it+1) == imc::key_crit_ || *(it+1) == imc::key_non_crit_ )
          {
            // compose entire key
            std::string newkey = { (char)*(it+1), (char)*(it+2) };

            // check for known keys
            if ( keys.count(newkey) == 1 )
            {
              // expecting ch_sep_ after key
              if ( *(it+3) == ch_sep_ )
              {
                // extract key version
                std::string vers("");
                unsigned long int pos = 4;
                while ( *(it+pos) != ch_sep_ )
                {
                  vers.push_back((char)*(it+pos));
                  pos++;
                }
                int version = std::stoi(vers);

                // get block length
                std::string leng("");
                pos++;
                while ( *(it+pos) != ch_sep_ )
                {
                  leng.push_back((char)*(it+pos));
                  pos++;
                }
                unsigned long length = std::stoul(leng);

                // declare corresponding key and block
                imc::key bkey( *(it+1)==imc::key_crit_ , newkey,
                               imc::keys.at(newkey).description_, version );
                imc::block blk(bkey,it-buffer_.begin(),
                                    it-buffer_.begin()+pos+1+length,
                                    raw_file_, &buffer_);

                // add block to list
                rawblocks_.push_back(blk);
              }
              else
              {
                throw std::runtime_error( std::string("invalid block or corrupt buffer at byte: ")
                                        + std::to_string(it+3-buffer_.begin()) );
              }
            }
            else
            {
              throw std::runtime_error(std::string("unknown IMC key: ") + newkey);
            }
          }
        }
      }

    }

    // get blocks
    std::vector<imc::block>& blocks()
    {
      return rawblocks_;
    }

  };

}

#endif

//---------------------------------------------------------------------------//
