/**
 * @file boilerplate.cpp
 * @example boilerplate.cpp
 * @brief Contains code commonly used by all of the examples.
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
#include <cstring>
#include "../include/argpp.hpp"

using namespace std;
using namespace argpp;

extern string prg_name;

//-----------------------------------------------------------------------
// code that is always the same for each test:
//-----------------------------------------------------------------------
static void do_boilerplate(ArgppBase* root, bool parse_retval);

void do_test()
{
  ArgppBase* root = ArgppBase::getRootInstance();
  if (root) {
    //----------------------------------------------------------
    // This function will not return if the user entered
    // "--help", "--usage", or "--version" on the command line,
    // or if an invalid option was entered, because argp_parse()
    // calls exit() before returning:
    //----------------------------------------------------------
    bool ret = ArgppBase::parse();
    //----------------------------------------------------------
    // However, since there are (hopefully) only global and static
    // objects created up to now, the C++ standard guarantees that
    // their destructors will be called before terminating the
    // program.
    //----------------------------------------------------------
    do_boilerplate(root, ret);
  } else {
    cout << "ARGPP ERROR: No root instance available!\n";
  }
  cout << "ARGPP TEST: " << prg_name << " finished." << endl;
}

//-----------------------------------------------------------------------
static void do_boilerplate(ArgppBase* root, bool parse_retval)
{
  const string prg_version
    = argp_program_version ? argp_program_version : "<unknown version>";

  if (root && parse_retval) {
    cout << "\n^^^ Output from " << prg_name << " ^^^\n";
    cout << "==========================================================\n";
    const ParsedOptions& po = root->getParsedOptions();
    cout << "ARGPP TEST: " << prg_version << " begin...\n";
    cout << "Options parsed:\n";
    if (!po.empty()) {
      cout << "\tShort:\tLong:\tArgument:\n";
      for (P_Opts_cit cit = po.begin();
           cit != po.end(); ++cit) {
        cout << "\t";
        if (isprint(cit->key_)) {
          cout << (char)(cit->key_);
        } else {
          cout << cit->key_;
        }
        cout << "\t"
             << (cit->long_name_.empty() ? "(no long name)" : cit->long_name_)
             << "\t"
             << cit->arg_ << "\n";
      }
    } else {
      cout << "No parsed options were saved.\n";
    }
    cout << "----------------\n";
    //---------------------------------------------------
    // Get the non-option arguments:
    //---------------------------------------------------
    cout << "Non-option arguments entered:\n";
    const OtherArgs& ar = root->getNonOptionArgs();
    if (!ar.empty()) {
        for (OtherArgs::const_iterator cit = ar.begin();
             cit != ar.end(); ++cit) {
          cout << "\t" << *cit << "\n";
        }
    } else {
      cout << "None.\n";
    }
  } else {
    error_t e = ArgppBase::getRetvalErrorFromArgpParse();
    cout << "parse() returned false...\nError code from argp_parse was: " << e;
    cout << " (" << strerror(e) << ")\n";
  }
}
