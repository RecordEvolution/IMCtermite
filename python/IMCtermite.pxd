
# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "lib/imc_raw.hpp" namespace "imc":

  cdef cppclass cppimctermite "imc::raw":

    # constructor(s)
    cppimctermite() except +
    cppimctermite(string rawfile) except +

    # provide raw file
    void set_file(string rawfile) except +

    # get JSON list of channels
    vector[string] get_channels(bool json, bool data) except +

    # print single channel/all channels
    void print_channel(string channeluuid, string outputdir, char delimiter) except +
    void print_channels(string outputdir, char delimiter) except +
    void print_table(string outputfile) except +
