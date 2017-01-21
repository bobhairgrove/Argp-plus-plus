/**
 * @file ex_argp_test.cpp
 * @example ex_argp_test.cpp
 * @brief Based on the file "argp-test.c"
 * located here:
 * https://github.com/lattera/glibc/blob/master/argp/argp-test.c
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
#include <cstdio>
#include "../../include/argpp.hpp"

using namespace std;
using namespace argpp;

string prg_name;
extern void do_test();

static const string prg_version  = "version 1.0 (originally: \"argp-test 1.0\")";
static const string bug_addr     = "<code@roberthairgrove.com>";

static const string sub_args_doc = "STRING...\n-";
static const string sub_doc      = "\vThis is the doc string from the sub-arg-parser.";

static const string args_doc = "STRING";
static const string doc      = "Test program for argp."
                               "\vThis doc string comes after the options."
                               "\nHey!  Some manual formatting!"
                               "\nThe current time is: %s";

/* Structure used to communicate with the parsing functions.  */
struct params
{
  unsigned int foonly;		/* Value parsed for foonly.  */
  unsigned int foonly_default;	/* Default value for it.  */
};

#define OPT_PGRP 1
#define OPT_SESS 2

//-------------------------------------------------------------------------
// In C++, all of the members of the struct must be initialized explicitly:
//-------------------------------------------------------------------------
argp_option sub_options[] =
{
  {"subopt1",       's',     0,  0, "Nested option 1", 0},
  {"subopt2",       'S',     0,  0, "Nested option 2", 0},

  { 0, 0, 0, 0, "Some more nested options:", 10},
  {"subopt3",       'p',     0,  0, "Nested option 3", 0},

  {"subopt4",       'q',     0,  0, "Nested option 4", 1},

  {0,0,0,0,0,0}
};

argp_option options[] =
{
  {"pid",       'p',     "PID", 0, "List the process PID", 0},
  {"pgrp",      OPT_PGRP,"PGRP",0, "List processes in the process group PGRP", 0},
  {"no-parent", 'P',	 0,     0, "Include processes without parents", 0},
  {0,           'x',     0,     OPTION_ALIAS, 0, 0},
  {"all-fields",'Q',     0,     0, "Don't elide unusable fields (normally"
           " if there's some reason ps can't"
           " print a field for any process, it's"
           " removed from the output entirely)", 0 },
  {"reverse",   'r',    0,      0, "Reverse the order of any sort", 0},
  {"gratuitously-long-reverse-option", 0, 0, OPTION_ALIAS, 0, 0},
  {"session",  OPT_SESS,"SID",  OPTION_ARG_OPTIONAL,
           "Add the processes from the session"
           " SID (which defaults to the sid of"
           " the current process)", 0 },

  {0,0,0,0, "Here are some more options:", 0},
  {"foonly", 'f', "ZOT", OPTION_ARG_OPTIONAL, "Glork a foonly", 0},
  {"zaza", 'z', 0, 0, "Snit a zar", 0},
  // another way of delimiting an options array:
  argp_option()
};

//---------------------------------------------
static void
popt (int key, const char *arg)
{
  char buf[10];
  if (isprint (key))
    sprintf (buf, "%c", key);
  else
    sprintf (buf, "%d", key);

  if (arg)
    printf ("KEY %s: %s\n", buf, arg);
  else
    printf ("KEY %s\n", buf);
}

//---------------------------------------------
/**
 * @brief The Argex_test class
 */
class Argex_test : public ArgppBase {
  public:
    /**
     * @brief Argex_test constructor
     * @param cargs
     * @param vargs
     * @param parent
     */
    Argex_test(int cargs, char** vargs, ArgppBase* parent)
      : ArgppBase(cargs, vargs, parent)
    {
      params_.foonly = 0;
      params_.foonly_default = random();
      installHelpFilter();
    }
    /**
     * @brief params_
     */
    params params_;
  private:
    //--------------------------------------
    /**
     * @brief parserImpl
     * @param key
     * @param arg
     * @return
     */
    error_t parserImpl(int key, const char *arg, argp_state *state) {
      // record it:
      genericParserImpl(key, arg, state);
      error_t retval = ARGPP_SUCCESS;
      switch(key) {
        case 'f':
          if (arg)
            params_.foonly = strtoul(arg, NULL, 10);
          else
            params_.foonly = params_.foonly_default;
          popt(key, arg);
          break;
        case 'p': case 'P': case OPT_PGRP: case 'x': case 'Q':
        case 'r': case OPT_SESS: case 'z':
          popt(key, arg);
          break;
        default:
          retval = ARGPP_UNKNOWN;
          break;
      }
      return retval;
    }
    //-------------------------------------
    /**
     * @brief keyNoArgsImpl
     * @return
     */
    error_t keyNoArgsImpl(argp_state *) {
      printf ("NO ARGS\n");
      return ARGPP_SUCCESS;
    }
    //-------------------------------------
    /**
     * @brief keyArgImpl
     * @param arg
     * @param state
     * @return
     */
    error_t keyArgImpl(const char *arg, argp_state *state) {
      if (state->arg_num > 0)
        return ARGPP_UNKNOWN; /* Leave it for the sub-arg parser.  */
      printf ("ARG: %s\n", arg);
      // record it:
      ArgppBase::keyArgImpl(arg, state);
      return ARGPP_SUCCESS;
    }
    //-------------------------------------
    /**
     * @brief helpFilterImpl
     * @param key
     * @param text
     * @return
     */
    string helpFilterImpl(int key, const char * text) {
      string retval;
      int res = -1;
      char new_text[1024];
      if (text && (key == khp_post_doc)) {
        time_t now = time (0);
        res = snprintf(new_text, 1024, text, ctime (&now));
      } else if (text && key == 'f') {
        /* Show the default for the --foonly option.  */
        res = snprintf(new_text, 1024, "%s (ZOT defaults to %x)",
            text, params_.foonly_default);
      } else if (text) {
        retval = string(text);
      }

      if ((res > 0) && (res < 1024))
        retval = string(new_text);
      return retval;
    }
};

//---------------------------------------------
/**
 * @brief The Argex_child class
 */
class Argex_child : public ArgppBase {
  public:
    /**
     * @brief Argex_child constructor
     * @param cargs
     * @param vargs
     * @param parent
     */
    Argex_child(int cargs, char** vargs, ArgppBase* parent)
      : ArgppBase(cargs, vargs, parent) {}
  private:
    //------------------------------------------------
    /**
     * @brief parserImpl
     * @param key
     * @return
     */
    error_t parserImpl(int key, const char *arg, argp_state *state) {
      error_t retval = ARGPP_SUCCESS;
      switch(key) {
        case 's' : case 'S': case 'p': case 'q':
          genericParserImpl(key, arg, state);
          printf ("SUB KEY %c\n", key);
          break;
        default:
          retval = ARGPP_UNKNOWN;
      }
      return retval;
    }
    //------------------------------------------------
    /**
     * @brief keyNoArgsImpl
     * @param argp_state (unused)
     * @return
     */
    error_t keyNoArgsImpl(argp_state *) {
      printf ("NO SUB ARGS\n");
      return ARGPP_SUCCESS;
    }
    //------------------------------------------------
    /**
     * @brief keyArgImpl
     * @param arg
     * @param argp_state (unused)
     * @return
     */
    error_t keyArgImpl(const char *arg, argp_state *) {
      printf ("SUB ARG: %s\n", arg);
      return ARGPP_SUCCESS;
    }
};

//----------------------------------------------
/**
 * @brief setup_parsers
 * @param cargc
 * @param vargv
 * @return
 */
bool setup_parsers(int cargc, char** vargv) {

  bool retval = false;

  // create the root instance:
  Argex_test* root = Factory<Argex_test>::createParser(cargc, vargv);

  //------------------------------------------------------
  // The child is automatically attached to root as its parent.
  // Any additional children should specify their parent:
  //------------------------------------------------------
  Argex_child* child = Factory<Argex_child>::createParser();

  if (root && child) {
    root->installProgVersion(prg_version);
    root->installBugAddress(bug_addr);

    // add options:
    retval = root->addOptions(options);
    if (retval) {
      retval = child->addOptions(sub_options);
    }
    if (retval) {
      child->setDocMessage(sub_doc);
      child->setUsageMessage(sub_args_doc);
      root->setDocMessage(doc);
      root->setUsageMessage(args_doc);
    }

  }
  return retval;
}

//----------------------------------------------
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv) {
  prg_name = argv[0];
  if (setup_parsers(argc, argv)) {
    //----------------------------------------------
    // This function might not return ... see comments
    // in the file "boilerplate.cpp" and in the
    // documentation for argp++ for an explanation:
    //----------------------------------------------
    do_test();
    //-------------------------------------------------------------------
    // We could have made params_ a global variable, but instead it is a
    // public member of Argex_test. In order to access it now, we have
    // to do a dynamic_cast because getRootInstance() returns a pointer
    // to the base class. "Real" code wouldn't need to do this:
    //-------------------------------------------------------------------
    Argex_test *root = dynamic_cast<Argex_test*>(ArgppBase::getRootInstance());
    if (root) {
      //-------------------------------------------------
      // It would be nice to have this print out before
      // the boilerplate code, but it is too much trouble
      // to change boilerplate.cpp just for this:
      //-------------------------------------------------
      printf ("==============================\nAfter parsing: foonly = %x\n", root->params_.foonly);
    }
  }
  return 0;
}
