/**
 * @file ex_step_8.cpp
 * @example ex_step_8.cpp
 * @brief An Argp-enabled program that accepts a hidden option. Callback function.
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

static const int MAX_ARG_COUNT = 4;
static const bool TRACK_ARGS = true;

extern void do_test();

/**
 * @brief The Argex_8 class
 */
class Argex_8 : public argpp::ArgppBase {
  public:
    Argex_8(int cargs, char** vargs, ArgppBase* parent)
      : ArgppBase(cargs, vargs, parent)
    {}
  private:
    /**
     * @brief parserImpl
     * @param key
     * @param arg
     * @param state
     * @return
     */
    error_t parserImpl(int key, const char *arg, argp_state *state) {
      // record it:
      genericParserImpl(key, arg, state);
      
      error_t retval = ARGPP_UNKNOWN;
      unsigned int i, dots;

      switch (key) {
        case 'd':
          // Argument to "d" is mandatory, therefore
          // we shouldn't need to check arg here...
          // but it might be wise in production code:
          if (arg) {
            dots = strtoul(arg, NULL, 10);
          } else {
            dots = 1;
          }
          for (i = 0; i < dots; i++) {
            cout << ".";
          }
          retval = ARGPP_SUCCESS;
          break;
        case 777:
          retval = this->parserImpl('d', "3", state);
          break;
        case 888:
          cout << "-";
      }
      return retval;
    }
    /**
     * @brief keyArgImpl
     * @param arg (unused)
     * @param state
     * @return
     */
    error_t keyArgImpl(const char *, argp_state *state) {
      ArgppBase::argppFailure(state, 1, 0, "too many arguments");
      return ARGPP_SUCCESS;
    }
    /**
     * @brief keyEndImpl
     * @param state (unused)
     * @return
     */
    error_t keyEndImpl(argp_state *) {
      cout << endl;
      return ARGPP_SUCCESS;
    }
};

//----------------------------------------------
bool setup_parsers(int cargc, char** vargv) {

  bool retval = false;
  ArgppOption opt;
  //-----------------------------------------------
  // We will add all of the options at once
  // by passing this vector:
  // the base class takes care of delimiting
  // the array of argp_option structs by appending
  // an empty argp_option when the time comes to
  // call the native argp_parse() function:
  //-----------------------------------------------
  ArgppOptions opts;

  opt.key_           = 'd';
  opt.long_name_     = "dot";
  opt.arg_           = "NUM";
  opt.flags_         = opt_arg_is_optional;
  opt.doc_or_header_ = "Show some dots on the screen";

  opts.push_back(opt);

  // 2nd option:
  opt.key_           = 777;
  opt.long_name_     = "ellipsis";
  opt.arg_.clear();
  opt.flags_         = opt_is_hidden;
  opt.doc_or_header_ = "Show an ellipsis on the screen";

  opts.push_back(opt);

  opt.key_           = 888;
  opt.long_name_     = "dash";
  opt.flags_         = opt_arg_is_mandatory;
  opt.doc_or_header_ = "Show a dash on the screen";

  opts.push_back(opt);

  // create the root instance:
  Argex_8* root = Factory<Argex_8>::createParser(cargc, vargv);

  if (root) {
    root->installProgVersion(prg_version);
    root->installBugAddress(bug_addr);

    // add options:
    retval = root->addOptions(opts);
  }
  return retval;
}

//----------------------------------------------
int main(int argc, char** argv) {
  prg_name = argv[0];
  if (setup_parsers(argc, argv)) {
    //----------------------------------------------
    // This function might not return ... see comments
    // in the file "boilerplate.cpp" and in the
    // documentation for argp++ for an explanation:
    //----------------------------------------------
    do_test();
  }
  return 0;
}
