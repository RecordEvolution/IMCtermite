//---------------------------------------------------------------------------//

#ifndef IMCKEY
#define IMCKEY

#include <iomanip>
#include <map>
#include <string>
#include <sstream>
#include <vector>

//---------------------------------------------------------------------------//

namespace imc
{
  // define "magic bytes" announcing critical (=C) non-critical keys (=N)
  const unsigned char key_crit_ = 0x43, key_non_crit_ = 0x4e;

  // define properties of marker/key
  struct key
  {
    // (non)critical key
    bool critical_;
    // name of key (two uppercase letters)
    std::string name_;
    // short description
    std::string description_;
    // version of marker
    int version_;

    // constructor
    key(bool critical, std::string name,
        std::string description = std::string(""), int version = 1)
    {
      critical_ = critical;
      if ( name.size() != 2 ) throw std::logic_error("invalid key name");
      name_ = name;
      description_ = description;
      version_ = version;
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"critical:"<<(critical_?"yes":"no")<<"\n"
        <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
        <<std::setw(width)<<std::left<<"decription:"<<description_<<"\n"
        <<std::setw(width)<<std::left<<"version:"<<version_<<"\n";
      return ss.str();
    }

  };

  // define (non)critial markers/keys
  std::map<std::string,key> keys = {

    // critical keys
    {"CF", key(true,"CF","format version and processor",1)},
    {"CK", key(true,"CK","start of group of keys",1)},
    {"CB", key(true,"CB","group of channels",1)},
    {"CT", key(true,"CT","text definition",1)},
    {"CG", key(true,"CG","group of components",1)},
    {"CD", key(true,"CD","abscissa description",1)},
    {"CD", key(true,"CD","abscissa description",2)},
    {"CZ", key(true,"CZ","scaling of z-axis",1)},
    {"CC", key(true,"CC","start of component",1)},
    {"CP", key(true,"CP","buffer, datatype and samples of component ",1)},
    {"Cb", key(true,"Cb","buffer description",1)},
    {"CR", key(true,"CR","permissible range of values in component",1)},
    {"CN", key(true,"CN","name and comment of channel",1)},
    {"CS", key(true,"CS","raw binary data",1)},
    {"CI", key(true,"CI","single numerical value",1)},
    {"Ca", key(true,"Ca","add reference key",1)},

    // noncritical keys
    {"NO", key(false,"NO","origin of data",1)},
    {"NT", key(false,"NT","timestamp of trigger",1)},
    {"ND", key(false,"ND","(color) display properties",1)},
    {"NU", key(false,"NU","user defined key",1)},
    {"Np", key(false,"Np","property of channel",1)},
    {"NE", key(false,"NE","extraction rule for BUS channels",1)}

  };


}

#endif

//---------------------------------------------------------------------------//
