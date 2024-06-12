
[![LICENSE](https://img.shields.io/github/license/RecordEvolution/IMCtermite)](https://img.shields.io/github/license/RecordEvolution/IMCtermite)
[![STARS](https://img.shields.io/github/stars/RecordEvolution/IMCtermite)](https://img.shields.io/github/stars/RecordEvolution/IMCtermite)
![CI/CD](https://github.com/RecordEvolution/IMCtermite/actions/workflows/pypi-deploy.yml/badge.svg?branch=master&event=push)
[![PYPI](https://img.shields.io/pypi/v/IMCtermite.svg)](https://pypi.org/project/IMCtermite/)

# IMCtermite

_IMCtermite_ provides access to the proprietary data format
_IMC Bus Format_ with the file extension _.raw_ introduced and developed by
[imc Test & Measurement GmbH](https://www.imc-tm.de/). This data format is
employed i.a. by the measurement hardware
[imc CRONOSflex](https://www.imc-tm.de/produkte/messtechnik-hardware/imc-cronosflex/ueberblick/)
to dump and store data and the software packages
[imc Studio](https://www.imc-tm.de/produkte/messtechnik-software/imc-studio/ueberblick/)
& [imc FAMOS](https://www.imc-tm.de/produkte/messtechnik-software/imc-famos/)
for measurement data control and analysis. Thanks to the integrated Python module,
the extracted measurement data can be stored in any open-source file format
accessible by Python like i.a. _csv_, _json_ or _parquet_.

On the [Record Evolution Platform](https://www.record-evolution.de/en/home-en/), the library can be used both as a command line tool for interactive usage and as a Python module to integrate the _.raw_  format into any ETL workflow.   

## Overview

* [File format](#Fileformat)
* [Build and Installation](#Installation)
* [Usage and Examples](#Usage)
* [References](#References)

## File format

[Warning: Take a look at [this issue](https://github.com/RecordEvolution/IMCtermite/issues/14) when reading this section regarding the file format.]

A data file of the _IMC Bus Format_ type with the extension _.raw_ is a _mixed text/binary
file_ featuring a set of markers (keys) that indicate the start of various blocks
of data that provide meta information and the actual measurement data. Every single
marker is introduced by the character `"|" = 0x 7c` followed by two uppercase letters that characterize the type of marker. Each block is further divided into several
parameters separated by commata `"," = 0x 2c` and terminated by a semicolon
`";" = 0x 3b`. For instance, the header - first 600 bytes - of a raw file may
look like this (in UTF-8 encoding):

```
|CF,2,1,1;|CK,1,3,1,1;
|NO,1,86,0,78,imc STUDIO 5.0 R10 (04.08.2017)@imc DEVICES 2.9R7 (25.7.2017)@imcDev__15190567,0,;
|CG,1,5,1,1,1; |CD,2,  63,  5.0000000000000001E-03,1,1,s,0,0,0,  0.0000000000000000E+00,1;
|NT,1,16,1,1,1980,0,0,0.0;       |CC,1,3,1,1;|CP,1,16,1,4,7,32,0,0,1,0;
|CR,1,60,0,  1.0000000000000000E+00,  0.0000000000000000E+00,1,4,mbar;|CN,1,27,0,0,0,15,pressure_Vacuum,0,;
|Cb,1, 117,1,0,    1,         1,         0,      9608,         0,      9608,1,  2.0440300000000000E+03,  1.2416717060000000E+09,;
|CS,1,      9619,         1,�oD	�nD6�nD)�nD�
```

Line breaks are introduced for readability. Most of the markers introduce
blocks of text, while only the last block identified by `|CS` contains binary data.
The format supports the storage of _multiple data sets (channels)_ in a single
file. The channels may be ordered in _multiplex_ mode (ordering w.r.t. time) or
_block_ mode (ordering w.r.t. to channels).

The markers (keys) are introduced by `"|" = 0x 7c` followed by two uppercase
letters. There are _two types_ of markers distinguished by the first letter:

1. _critical_ markers: introduced by `|C` featuring uppercase `C`
1. _noncritical_ markers: introduced by `|N` featuring uppercase `N`  

The second letter represents further details of the specific key. Note that
while the _noncritical_ keys are optional, any _.raw_ file _cannot be_ correctly
decoded if any of the _critical_ markers are misinterpreted, invalid or damaged.
The second uppercase letter is followed by the first comma and the _version_
of the key starting from 1. After the next comma, an _(long) integer_ (in text
representation) specifies the length of the entire block, i.e. the number of
bytes between the following comma and the block-terminating semicolon. The further
structure of a block is not defined and may feature different numbers of additional
parameters. The format allows for any number of carriage returns (`CR = 0x0d`)
and line feeds (`LF = 0x 0a`) between keys, i.e. the block-terminating semicolon
and the vertical bar (pipe) of the next key. The following _critical markers_
are defined:


| marker | description                                                                                         |
|--------|-----------------------------------------------------------------------------------------------------|
| CF     | format version and processor                                                                        |
| CK     | start of group of keys, no. parameters = 3, indicates (in)correct closure of the measurement series |
| CB     | defines a group of channels                                                                         |
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

The _IMCtermite_ library may be employed both as a _CLI_ tool and a _python_
module.

### CLI tool

To build the CLI tool locally, use the default target `make` resulting
in the binary `imctermite`. To ensure system-wide availability, the installation
of the tool (in the default location `/usr/local/bin`) is done via

```
make install
````

which may require root permissions.

### Python

To integrate the library into a customized ETL toolchain, several cython targets
are available. For a local build that enables you to run the examples, use:

```
make cython-build
```

However, in a production environment, a proper installation of the module with
`make cython-install` is recommended for system-wide availability of the module.

#### Installation with pip

The package is also available in the [Python Package Index](https://pypi.org)
at [IMCtermite](https://pypi.org/project/IMCtermite/).
To install the latest version simply do

```Shell
python3 -m pip install IMCtermite
```

which provides binary wheels for multiple architectures on _Windows_ and _Linux_
and most _Python 3.x_ distributions. However, if your platform/architecture is
not supported you can still compile the source distribution yourself, which
requires _python3_setuptools_ and an up-to-date compiler supporting C++11
standard (e.g. _gcc version >= 10.2.0_).

## Usage

### CLI

The usage of the `imctermite` binary looks like this:

```
imctermite <raw-file> [options]
```

You have to provide a single _raw_ file and any option to specify what
to do with the data. All available options can be listed with `imctermite --help`:

```
Options:

 -c, --listchannels      list channels
 -b, --listblocks        list IMC key-blocks
 -d, --output            output directory to print channels
 -s, --delimiter         csv delimiter/separator char for output
 -h, --help              show this help message
 -v, --version           display version
```

For instance, to show a list of all channels included in `sample-data.raw`, you
do `imctermite sample-data.raw --listchannels`. No output files are
written by default. Output files are written only when an existing (!) directory
is provided as argument to the `--output` option. By default, every output file
is written using a `,` delimiter. You may provide any custom separator with the
option `--delimiter`. For example, in order to use `|`, the binary is called with
options `imctermite sample-data.raw -b -c -s '|'`.

### Python

Given the `IMCtermite` module is available, we can import it and declare an instance
of it by passing a _raw_ file to the constructor:

```Python
import IMCtermite

imcraw = IMCtermite.imctermite(b"sample/sampleA.raw")
```

An example of how to create an instance and obtain the list of channels is:

```Python
import IMCtermite

# declare and initialize instance of "imctermite" by passing a raw-file
try :
    imcraw = IMCtermite.imctermite(b"samples/sampleA.raw")
except RuntimeError as e :
    print("failed to load/parse raw-file: " + str(e))

# obtain list of channels as list of dictionaries (without data)
channels = imcraw.get_channels(False)
print(channels)
```

A more complete [example](python/examples/usage.py), including the methods for
obtaining the channels, i.a. their data and/or directly printing them to files,
can be found in the `python/examples` folder.

## References

- https://www.imc-tm.de/produkte/messtechnik-software/imc-famos/funktionen/im-und-export/
- https://www.imc-tm.de/produkte/messtechnik-hardware/imc-cronosflex/ueberblick/
- https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html
- https://github.com/Apollo3zehn/ImcFamosFile
- https://apollo3zehn.github.io/ImcFamosFile/api/ImcFamosFile.FamosFileKeyType.html
- https://pypi.org/help/#apitoken
- https://sgoel.dev/posts/uploading-binary-wheels-to-pypi-from-github-actions/
- https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#jobsjob_idstepsrun
- https://github.com/pypa/cibuildwheel/blob/main/examples/github-deploy.yml
- https://cibuildwheel.readthedocs.io/en/stable/deliver-to-pypi/
- https://www.gnu.org/software/libiconv/
- https://vcpkg.io/en/packages.html
- https://vcpkg.io/en/getting-started
