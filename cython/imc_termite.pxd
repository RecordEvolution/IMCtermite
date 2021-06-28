# cython: language_level = 3

# use some C++ STL libraries
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "imc_raw.hpp" namespace "imc":
  cdef cppclass imc_termite "imc::raw":
    # constructor(s)
    imc_termite() except +
    imc_termite(string rawfile) except +
    # provide raw file
    void set_file(string rawfile) except +
    # get JSON list of channels
    vector[string] get_channels(bool json, bool data) except +
    # print single channel/all channels
    void print_channel(string channeluuid, string outputdir, char delimiter) except +
    void print_channels(string outputdir, char delimiter) except +
    void print_table(string outputfile) except +
