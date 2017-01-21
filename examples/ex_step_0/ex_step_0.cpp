/**
 * @file ex_step_0.cpp
 * @example ex_step_0.cpp
 * @brief A minimal argp++ application with no options
 * except for the built-ins: `help`, `usage` and `version`.
 * @details Based on the book "Step-by-Step into Argp"
 * by Ben Asselstine (http://www.nongnu.org/argpbook/).
 * Takes each of the "step" examples and transforms them
 * into a C++ version using libargp++.
 */

/* ARGP++ is a C++ wrapper library around the GNU argp library.
   Copyright (C) 2017 by Robert Hairgrove <code@roberthairgrove.com>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

   The argp library is part of the GNU C Library.
   Written by Miles Bader <miles@gnu.ai.mit.edu>.
   Copyright (C) 1995-1999, 2003, 2004, 2005, 2006, 2007, 2009
   Free Software Foundation, Inc.
*/

#include <iostream>
#include "../../include/argpp.hpp"

using namespace std;
using namespace argpp;

string prg_name;
static const string prg_version = "version 1.0";
static const string bug_addr    = "<code@roberthairgrove.com>";

// Implementation of this function is in boilerplate.cpp:
extern void do_test();

//----------------------------------------------
/**
 * @brief The Argex_0 class
 */
class Argex_0 : public ArgppBase {
  public:
      Argex_0(int cargs, char** vargs, ArgppBase* parent)
          : ArgppBase(cargs, vargs, parent)
      {}
  private:
      //---------------------------------------------------------
      // Since parserImpl() is a pure virtual function in the
      // base class, we must provide an implementation of it.
      // This one simply returns the standard ARGPP_SUCCESS
      // return code (which is == 0).
      //
      // Admittedly, this class is pretty useless!
      // But it shows how little work is necessary to provide
      // the basic functionality of the standard options:
      // --help, --usage, and --version.
      //---------------------------------------------------------
      error_t parserImpl(int, const char *, argp_state *)
      { return ARGPP_SUCCESS; }
};

//----------------------------------------------
bool setup_parsers(int cargc, char** vargv) {
  //--------------------------
  // create the root instance:
  //--------------------------
  Argex_0* root = Factory<Argex_0>::createParser(cargc, vargv);
  if (root) {
    root->installProgVersion(prg_version);
    root->installBugAddress(bug_addr);
  }
  //------------------------------------------------------
  // We have no parsers to set up, so we can just return:
  //------------------------------------------------------
  return (bool)root;
}

//----------------------------------------------
int main(int argc, char** argv) {
  prg_name = argv[0];
  if (setup_parsers(argc, argv)) {
    //------------------------------------------------
    // This function might not return ... see comments
    // in the file "boilerplate.cpp" and in the other
    // documentation for argp++ for an explanation:
    //------------------------------------------------
    do_test();
  }
  return 0;
}
