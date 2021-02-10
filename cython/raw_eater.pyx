
from raw_eater cimport raweater
import numpy as np
import re
import os

cdef class raweater:

  # C++ instance of class => stack allocated (requires nullary constructor!)
  cdef raw_eater rawit

  # pointer to C++ instance (if there's no nullary constructor)
  # cdef raw_eater *rawit

  def __cinit__(self, string rawfile = b''):
    if rawfile.decode() == "":
      self.rawit = raw_eater()
      # self.rawit = new raw_eater()
    else:
      if not os.path.isfile(rawfile) :
        raise ValueError("'" + str(rawfile) + "' does not exist")
      self.rawit = raw_eater(rawfile)
      # self.rawit = new raw_eater(rawfile)

  # def __dealloc__(self):
  #   del self.rawit

  def set_file(self, string rawfile):
    if not os.path.isfile(rawfile) :
      raise ValueError("'" + str(rawfile) + "' does not exist")
    self.rawit.set_file(rawfile)

  def do_conversion(self):
      self.rawit.setup_and_conversion()

  def validity(self):
    return self.rawit.get_valid()

  def channel_name(self):
    return self.rawit.get_name()

  def unit(self):
    return self.rawit.get_unit()

  def dt(self):
    return self.rawit.get_dt()

  def time_unit(self):
    return self.rawit.get_temp_unit()

  def get_time(self):
    return self.rawit.get_time()

  def get_channel(self):
    return self.rawit.get_data()

  def write_table(self, const char* csvfile, char delimiter):
    self.rawit.write_table(csvfile,delimiter)
