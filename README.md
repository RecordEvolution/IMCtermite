
<p align="center">
  <a href="https://record-evolution.de/reswarm">
    <img
      alt="imctermite.svg"
      src="assets/imctermite.svg"
      width="400"
    />
  </a>
</p>

_IMCtermite_ provides access to the proprietary data format
_IMC Bus Format_ with file extension _.raw_ introduced and developed by
[imc Test & Measurement GmbH](https://www.imc-tm.de/). This data format is
employed i.a. by the measurement hardware
[imc CRONOSflex](https://www.imc-tm.de/produkte/messtechnik-hardware/imc-cronosflex/ueberblick/)
to dump and store data and the software packages
[imc Studio](https://www.imc-tm.de/produkte/messtechnik-software/imc-studio/ueberblick/)
and [imc FAMOS](https://www.imc-tm.de/produkte/messtechnik-software/imc-famos/)
for measurement data control and analysis. The extracted measurement data can
be stored in various open source file formats like _csv_, _json_, _parquet_ or
_hdf5_.

## Overview

* [File format](#Fileformat)
* [Build and Installation](#Installation)
* [Usage and Examples](#Usage)
* [References](#References)

## Fileformat

A data file of _IMC Bus Format_ type with extension _.raw_ is a _mixed text/binary
file_ featuring a set of markers (keys) that indicate the start of various blocks
of data providing meta information and the actual measurement data. Every single
marker is introduced by character `"|" = 0x 7c` followed by two uppercase letters,
which characterize the type of marker. Each block is further divided into several
parameters separated by commata `"," = 0x 2c` and terminated by a semicolon
`";" = 0x 3b`. For instance, the header - first 600 bytes - of a raw file may
look like this (in UTF-8 encoding)

```
|CF,2,1,1;|CK,1,3,1,1;
|NO,1,86,0,78,imc STUDIO 5.0 R10 (04.08.2017)@imc DEVICES 2.9R7 (25.7.2017)@imcDev__15190567,0,;
|CG,1,5,1,1,1; |CD,2,  63,  5.0000000000000001E-03,1,1,s,0,0,0,  0.0000000000000000E+00,1;
|NT,1,16,1,1,1980,0,0,0.0;       |CC,1,3,1,1;|CP,1,16,1,4,7,32,0,0,1,0;
|CR,1,60,0,  1.0000000000000000E+00,  0.0000000000000000E+00,1,4,mbar;|CN,1,27,0,0,0,15,pressure_Vacuum,0,;
|Cb,1, 117,1,0,    1,         1,         0,      9608,         0,      9608,1,  2.0440300000000000E+03,  1.2416717060000000E+09,;
|CS,1,      9619,         1,�oD	�nD6�nD)�nD�
```

where line breaks where introduced for readability. Most of the markers introduce
blocks of text, while only the last one identified by `|CS` contains binary data.
The format supports the storage of _multiple data sets (channels)_ in a single
file. The channels may be ordered in _multiplex_ mode (ordering w.r.t. time) or
_block_ mode (ordering w.r.t. to channels).

The markers (keys) are introduced by `"|" = 0x 7c` followed by two uppercase
letters. There are _two types_ of markers distinguished by the first letter:

1. _critical_ markers: introduced by `|C` featuring uppercase `C`
1. _noncritical_ markers: introduced by `|N` featuring uppercase `N`  

The second letter represents further details of the specific key. Note, that
while the _noncritical_ keys are optional, any _.raw_ file _cannot be_ correctly
decoded if any of the _critical_ markers are misinterpreted, invalid or damaged.
The second uppercase letter is followed by the first comma and the _version_
of the key starting from 1. After the next comma, an _(long) integer_ (in text
representation) specifies the length of the entire block, i.e. the number of
bytes between the following comma and the block-terminating semicolon. The further
structure of a block is not defined and may feature different numbers of additional
parameters. The format allows for any number of carriage returns (`CR = 0x0d`)
and line feeds (`LF = 0x 0a`) between keys, i.e. the block terminating semicolon
and the vertical bar (pipe) of the next key. The following _critical markers_
are defined


| marker | description                                                                                         |
|--------|-----------------------------------------------------------------------------------------------------|
| CF     | format version and processor                                                                        |
| CK     | start of group of keys, no. parameters = 3, indicates (in)correct closure of the measurement series |
| CB     | defines a group of cannels                                                                          |
| CT     | text definition including group association index                                                   |
| CG     | introduces group of components corresponding to CC keys                                             |
| CD1,2  | old/new version of abscissa description                                                             |
| CZ     | scaling of z-axis for segments                                                                      |
| CC     | start of a component                                                                                |
| CP     | information about buffer, datatype and samples of component                                         |
| Cb     | buffer description                                                                                  |
| CR     | permissible range of values in component                                                            |
| CN     | name and comment of channel                                                                         |
| CS     | raw binary data                                                                                     |
| CI     | single numerical value (including unit)                                                             |
| Ca     | add reference key                                                                                   |

Among the _noncritical_ markers, there are

| marker | description                                |
|--------|--------------------------------------------|
| NO     | origin of data                             |
| NT     | timestamp of trigger                       |
| ND     | (color) display properties                 |
| NU     | user defined key                           |
| Np     | property of a channel                      |
| NE     | extraction rule for channels from BUS data |

The format loosely defines some rules for the ordering of the markers in the
file stream. The rules for critical keys include: _CK_ has to follow up on _CF_,
_CK_ may be followed by any number of _CG_ blocks, each _CG_ has to be followed
by (any number of) component sequences comprised of the series _CC_ , _CP_,
(_CR_), (_ND_) and terminated by either _CS_ or the start of a new group,
component, text field or buffer.

## Installation

## Usage

## References
