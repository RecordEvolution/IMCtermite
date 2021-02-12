# distutils: language = c++

from imc_termite cimport imc_termite

import json as jn
# import numpy as np

cdef class imctermite:

  # C++ instance of class => stack allocated (requires nullary constructor!)
  cdef imc_termite cpp_imc

  # constructor
  def __cinit__(self, string rawfile):
    self.cpp_imc = imc_termite(rawfile)

  # provide raw file
  def submit_file(self,string rawfile):
    self.cpp_tdm.set_file(rawfile)

  # get JSON list of channels
  def get_channels(self, bool data):
    return self.cpp_imc.get_channels()

  # print a channels
  def print_channel(self, string outputdir):
    self.cpp_tdm.print_channels(outputdir)
