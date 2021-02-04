
<p align="center">
  <a href="https://record-evolution.de/reswarm">
    <img
      alt="raweater.svg"
      src="assets/raweater.svg"
      width="400"
    />
  </a>
</p>

The _imcRawBuster_ provides access to the proprietary data format
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

### Markers

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

| marker | structure (example)                                                                           | description                                             |
|--------|-----------------------------------------------------------------------------------------------|---------------------------------------------------------|
| CF     | CF,2,1,1;                                                                                     | format version and processor                            |
| CK     | CK,1,3,1,<closed>;                                                                            | start of group of keys                                  |
|        |                                                                                               | length is always 3                                      |
|        |                                                                                               | <closed> must be 0 or 1 depending correct closure of the measurment series |
| CB     | CB,1,<length>,<groupindex>,<namelength>,<name>,<commentlength>,<comment>;                     | defines a group of cannels                              |
| CT     | CT,1,<length>,<groupindex>,<namelength>,<name>,<textlength>,<text>,<commentlength>,<comment>; | text definition                                         |
| CG     | CG,1,<length>,<numberofcompontent>,<fieldtype>,<dimension>;                                   | introduces group of components corresponding to CC keys |
|        |                                                                                               | field type is one of:  1 = real number                  |
|        |                                                                                               |                        2 = XY                           |
|        |                                                                                               |                        3 = 3XY                          |
|        |                                                                                               |                        4 = complex with Re + Im         |
|        |                                                                                               |                        5 = complex with Abs + Phase     |
|        |                                                                                               |                        6 = complex: Abs in dB + Phase   |
|        |                                                                                               | dimension: 1 for fieldtype 1, 2 for fieldtype 2-6       |
| CD     | CD,1,<length>,<dx>,<calibration>,<unitlength>,<unit>,0,0,0;                                   | dx = distance between two samples                       |
|        |                                                                                               | calibration = 1 : calibrated time-basis                 |
|        |                                                                                               |             = 0 : noncalibrated time-basis              |
|        |                                                                                               | unit: unit of this axis                                 |
| CZ     | CZ,1,<length>,<dz>,<dzcalibration>,<z0>,<z0calibration>,<unitlength>,<unit>,<segmentlength>;  | scaling of z-axis for segments                          |
| CC     | CC,1,<length>,<componentindex>,<analogdigital>;                                               | start of single component:                              |
|        |                                                                                               |  - componentindex = 1 real, real part, abs              |
|        |                                                                                               |                   = 2 complex, imaginar part, phase     |
|        |                                                                                               |  - analogdigital = 1 analog = 2 digital                 |
| CP     | CP,1,<lenght>,<buffref>,<bytes>,<datatype>,<signbits>,<mask>,<offset>,<numsamples>,<bytedist>;| buffref = int > 0 corresp.to buffer descript. in Cb key |
|        |                                                                                               | bytes: number of bytes for singel value                 |
|        |                                                                                               | datatype: 1 = unsigned byte, 2 = signed byte,           |
|        |                                                                                               |           3 = unsigned short, 4 = signed short          |
|        |                                                                                               |           5 = unsigned long, 6 = signed long            |
|        |                                                                                               |           7 = float, 8 = double                         |
|        |                                                                                               |           9 = imc Devices transitional recording        |
|        |                                                                                               |           10 = timestamp ascii, 11 = 2byte word digital |
|        |                                                                                               |           13 = 6byte unsigned long                      |
|        |                                                                                               | signbits: number of significant bits                    |
|        |                                                                                               | mask: default = 0                                       |
|        |                                                                                               | offset: offset of first sample in data block            |
|        |                                                                                               | numsamples: number of subsequent samples in channel     |
|        |                                                                                               | bytedist: number of bytes between last sample of channel|
|        |                                                                                               | to start of next channel                                |
| Cb     | Cb,1,<lenght>,<numbuff>,<bytesinuserinfo>,<buffref>,<indexsamplekey>,<offsetbuffer>,          |                                                         |
|        |      <bufferlength>,<offset>,<fillbytes>,0,X0,<addtime>,<userinfo>,<newevent>,[buffref,...];  | buffer description                                      |
| CR     |                      |                                                                                                                        |
| CN     |                      |                                                                                                                        |
| CS     |                      |
| CI     |                      |                                                                                                                        |
| Ca     |                      |                                                                                                                        |

## Installation

## Usage

## References

# Deprecated!!

The following markers are defined:

1. CF (0x 43 46)
1. CK (0x 43 4b)
1. NO (0x 4e 4f)
1. CG (0x 43 47)
1. CD (0x 43 44)
1. NT (0x 4e 54)
1. CC (0x 43 43)
1. CP (0x 43 50)
1. CR (0x 43 52)
1. CN (0x 43 4e)
1. Cb (0x 43 62)
1. CS (0x 43 53)

Each of these markers are followed by multiple commata (0x 2c) separated parameters
and are terminated by a semicolon `;` = 0x 3b, except for the sequence following
the data marker CS, that may have any number of 0x3b occurencies, while still
terminated by a semicolon at the very end of the file (since CS is the last marker
section in the file). The markers have the following meaning:

- *CF* (3 parameters)
        `|CF,2,1,1;`
       specifies file format, key length and processor
- *CK* (4 parameters)
        `|CK,1,3,1,1;`
        start of group of keys  
- *NO* (6 parameters)
       `|NO,1,85,0,77,imc STUDIO 5.0 R3 (10.09.2015)@imc DEVICES 2.8R7 (26.8.2015)@imcDev__15190567,0,;`
       origin of the file, provides some info about the software package/device
       and its version
- *CB* (6 parameters)
       group definition
- *CT* (8 parameters)
       text definition
- *CG* (5 parameters)
       `|CG,1,5,1,1,1;`
       definition of a data field
       |CG,1,KeyLang,AnzahlKomponenten,Feldtyp,Dimension;
- *CD* (mostly 11 parameters)
       since we're dealing with measured entities from the lab this markers contains
       info about the measurement frequency, i.e. sample rate. For instance
       `|CD,2,  63,  5.0000000000000001E-03,1,1,s,0,0,0,  0.0000000000000000E+00,1;`
       indicates a measured entity every 0.005 seconds, i.e. a sample rate = 200Hz
- *NT* (7 parameters)
        `|NT,1,16,1,1,1980,0,0,0.0;`
        |NT,1,KeyLang,Tag,Monat,Jahr,Stunden,Minuten,Sekunden;
        triggerzeit
- *CC*  (mostly 4 parameters)
        `|CC,1,3,1,1;`
        Start einer Komponente (component)
- *CP*  (9 parameters)
        `|CP,1,16,1,4,7,32,0,0,1,0;`
        Pack-Information zu dieser Komponente
        CP,1,KeyLang,BufferReferenz,Bytes,Zahlenformat,SignBits,Maske,Offset,DirekteFolgeAnzahl,AbstandBytes;
        Bytes = 1...8
        Zahlenformat : 1 = unsigned byte
                       2 = signed byte
                       3 = unsigned short
                       4 = signed short
                       5 = unsigned long
                       6 = signed long
                       7 = float
                       8 = double
                       9 = imc Devices
                       10 = timestamp ascii
                       11 =
                       12 =
                       13 =

- *CR*  (7 parameters)
        Wertebereich der Komponente, nur bei analogen, nicht bei digitalen Daten.
        |CR,1,KeyLang,Transformieren,Faktor,Offset,Kalibriert,EinheitLang, Einheit;
         provides the _physical unit_ of the measured entity, maybe shows the
         minimum and maximum value during the measurment, e.g.
        `|CR,1,60,0,  1.0000000000000000E+00,  0.0000000000000000E+00,1,4,mbar;`
        Transformieren : 0 = nein
                         1 = ja, mit faktor und offset transformieren (für ganzzahlige Rohdaten)
        Faktor,Offset:   physikalischer Wert = Faktor * Rohdatenwerten + Offset
- *CN*  (mostly 9 parameters)
         gives the _name_ of the measured entity
         |CN,1,KeyLang,IndexGruppe,0,IndexBit,NameLang,Name,KommLang,Kommentar;
        `|CN,1,27,0,0,0,15,pressure_Vacuum,0,;`
- *Cb*  (mostly 14 paramters) (optional?)
        this one probably gives the minimum/maximum measured values!!
        `|Cb,1,117,1,0,1,1,0,341288,0,341288,1,0.0000000000000000E+00,1.1781711390000000E+09,;`
- *CS*  (mostly 4 parameters)
        this markers announces the actual measurement data in binary format,
        provide the number of values and the actual data,
        e.g. `|CS,1, 341299, 1, ...data... ;`

### Open Issues and question?

- which parameter indicate(s) little vs. big endian?

## .parquet-file writer

The extracted and converted data originating from the *.raw file format may be efficiently grouped and
written as .parquet files
[parquet file writer example](https://github.com/apache/parquet-cpp/blob/master/examples/low-level-api/reader-writer.cc)


## References

- https://ch.mathworks.com/matlabcentral/fileexchange/30187-sequnce-to-read-famos-data-into-matlab-workspace
- https://community.ptc.com/t5/PTC-Mathcad/FAMOS-IMC-raw-data-in-MathCAD/td-p/130378
- http://marmatek.com/wp-content/uploads/2014/04/imc_STUDIO_Manual.pdf

### Parquet

- https://github.com/apache/parquet-cpp
- https://github.com/apache/parquet-cpp/tree/master/examples
