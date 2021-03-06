## Argp++:

A C++ class library wrapper around the GNU C-language **argp** options 
parsing utility for command-line programs. The GNU documentation for 
the native _argp_ library is available here:

https://www.gnu.org/software/libc/manual/html_node/Argp.html

### Introduction:

For an excellent introduction to using GNU _argp_ in programs written
in C, please read:
    
**"Step-by-Step into Argp"**
(Copyright © 2010, 2015 by Ben Asselstine) 

which you can find here: http://www.nongnu.org/argpbook/

The **source code** in this package consists of four C++ files:
* include/argpp.hpp
* include/argppbaseimpl.hpp
* src/argpp.cpp
* src/argppbaseimpl.cpp

There is a dependency on the header `argp.h`, otherwise no additional
libraries are necessary to use the code.
    
### Installation:

For installation instructions for this package, please refer to the 
**INSTALL** file.
    
It is probably easiest just to add all the sources to a new or
existing project; then there is nothing to install. ;)
    
There are several demo programs using **argp++** located in the
**examples** subfolder. These are not built by default.
`make examples` will build them and leave the executables directly
in the `examples` folder if you have run `configure` and `make`.
You do NOT have to install `libargp++` as a shared library first in order
to build the example programs.

### Using Argp++ in your own programs:

Either way, using the sources directly or as a library, just add 
`#include "argpp.hpp"` (or `#include <argpp.hpp>` if you installed it 
that way) to your code **AFTER** any other `#include` lines (see below 
about bug...), define a class of your own derived from `argpp::ArgppBase`, 
implement at least the pure virtual function `ArgppBase::parserImpl` 
and any others you might need, and off you go! The demo programs in the
`examples` subfolder will get you started.
    
There are some traps to avoid when mixing C++ object variables and C code 
such as native _argp_ functions, which can call `exit` at times with total 
unawareness of any automatic C++ variable objects whose destructors might 
need to be called beforehand. Special care was taken not to fall into that
trap in designing the base class.
    
There is also a very annoying bug which was discovered when the `argp.h`
header file is included **before** some of the STL headers and compiling
with C++11 functionality. You can read about this here:
    
https://sourceware.org/bugzilla/show_bug.cgi?id=16907
    
Apparently, this has recently been fixed, but there are plenty of
platforms which might not be completely up-to-date here. Writing
`#include <argp.h>` as the last header included by `argpp.hpp`
seems to mitigate the effects of this bug.

NOTE: There is no **man** page and no **info** for this package. All 
documentation is contained either in the above-mentioned files, in the
`doc` folder, or in the source code itself. For more details, please 
consult the INSTALL file as well as the HTML documentation which was 
generated by **Doxygen**, available here:

http://www.stack.nl/~dimitri/doxygen/

### C++ language standard:

The header `argpp.hpp` has macros in place which will discover how 
the code is being compiled. If **C++11** support is available, it will 
be used by default. In particular, `std::unique_ptr<T>` is used to hold
a pointer to the root parser instance in a global variable. If C++11 is
**not** available, a substitute `DeleteHelper` class is used instead. 
Otherwise, none of the more advanced C++11 syntax is used in order to
make it possible to compile the code on older platforms (but hopefully
not TOO old...)
    
Please read the other information in the HTML docs (located in the 
`doc/html` subfolder) for further details about the design
and use of the library. The file "ChangeLog" will contain more details 
about the latest developments.

### Credits and license:

Project URL: https://github.com/bobhairgrove/Argp-plus-plus
    
Version 0.9.0 is the initial public release. Please see the files
COPYING and COPYING.LESSER for complete license details.

Written by Robert Hairgrove. Copyright (c) 2017 by Robert Hairgrove and
the Free Software Foundation. All feature requests and bug reports are
welcome and should be sent to Robert Hairgrove, <code@roberthairgrove.com>,
or you can create an issue at Github under the project URL mentioned above.

--

    This library is free software; you can redistribute it and/or modify it 
    under the terms of the GNU Lesser General Public License as published by 
    the Free Software Foundation; either version 2.1 of the License, or 
    (at your option) any later version. This library is distributed in the 
    hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details. You should 
    have received a copy of the GNU Lesser General Public License along with 
    this library; if not, write to the Free Software Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

