/* ARGP++ is a C++ wrapper library around the GNU argp library.
   Copyright (C) 2014 by Robert Hairgrove <code@roberthairgrove.com>.

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

#ifndef ARGPP_H
#define ARGPP_H

/**
 * @file argpp.hpp
 * @brief Include just this file in your own application code.
 * @details This file defines the base class argpp::ArgppBase, which clients should use
 * to define their own derived classes, as well as some typedefs used mostly by the
 * implementation class argpp::impl::ArgppBaseImpl. Perhaps the most interesting
 * of these is the argpp::MapOfChildren which is used in the
 * argpp::impl::ArgppBaseImpl::parserRouter() function to populate the \b child_inputs[]
 * array member of the \b argp_state argument.
 *
 * The application uses the so-called
 * <a href="http://c2.com/cgi/wiki?PimplIdiom" target="_blank">Pimpl idiom</a>
 * in order to facilitate binary compatibility with future releases.
 * The only data member of the abstract base class argpp::ArgppBase is
 * an opaque pointer to the implementation class
 * argpp::impl::ArgppBaseImpl.
 */

//---------------------------------------------------------
// These are only necessary for Windows implementations:
//---------------------------------------------------------
#if defined _WIN32 || defined __CYGWIN__
#  define ARGPP_EXPORT __declspec(dllexport)
#  define ARGPP_IMPORT __declspec(dllimport)
#else
#  define ARGPP_EXPORT
#  define ARGPP_IMPORT
#endif // defined _WIN32 || defined __CYGWIN__

#if defined(ARGPP_DLL_BUILD) && !defined(ARGPP_DLL)
#  define ARGPP_DLL
#endif

#ifdef ARGPP_DLL         // Must be defined if ARGPP is compiled or used as a DLL on Windows!
#  ifdef ARGPP_DLL_BUILD // This should be defined if we are building the shared library.
#    define ARGPP_API     ARGPP_EXPORT
#  else
#    define ARGPP_API     ARGPP_IMPORT
#  endif // ARGPP_DLL_BUILD
#else // ARGPP is a static library, or we are building or using it on a non-Windows platform.
#  define ARGPP_API
#endif // ARGPP_DLL
//---------------------------------------------------------

#define  ARGPP_MAJOR  0
#define  ARGPP_MINOR  9
#define  ARGPP_BUILD  0

#define  ARGPP_VERSION        ((ARGPP_MAJOR << 16) | (ARGPP_MINOR << 8) | (ARGPP_BUILD))

#define  STR_HELPER(x)        #x
#define  STR(x)               STR_HELPER(x)
#define  ARGPP_STR_VERSION    STR(ARGPP_MAJOR) "." STR(ARGPP_MINOR) "." STR(ARGPP_BUILD)

//---------------------------------------------------------------
// Is std::unique_ptr available?
//---------------------------------------------------------------
#if defined(__GXX_EXPERIMENTAL_CXX0X) \
  || (__cplusplus >= 201103L)
#  define ARGPP_HAVE_UNIQUE_PTR
#endif // check for std::unique_ptr

#include <map>
#include <memory>
#include <string>
#include <vector>

//---------------------------------------------------------
// The STL headers should be included BEFORE including
// argp.h, otherwise there is a bug in some implementations
// of the GNU compiler concerning __attribute__ which could 
// cause problems:
//---------------------------------------------------------
#ifdef HAVE_SYSTEM_ARGP_H
#include <argp.h>
#else
#include "argp.h"
#endif

#define ARGPP_SUCCESS    0
#define ARGPP_UNKNOWN    ARGP_ERR_UNKNOWN
#define ARGPP_DONT_CARE  ARGP_ERR_UNKNOWN

#ifndef ENODATA
#  define ARGPP_ENODATA 61
#else
#  define ARGPP_ENODATA ENODATA
#endif

//---------------------------------------
// These are return values from the
// ArgppBase::supportedOption() function:
//---------------------------------------
#define  ARGPP_OPTION_OK         0
#define  ARGPP_OPTION_UNKNOWN    1
#define  ARGPP_OPTION_NEEDS_ARG  2

/**
 * @namespace argpp
 * @brief Encapsulates the public interface
 *  of the \b argp++ library.
 */
namespace argpp {

  /**
 * @fn library_build_version
 * @brief Returns the version with which the library was built as a numeric value.
 */
  size_t ARGPP_API library_build_version();

  /**
 * @fn str_library_build_version
 * @brief Returns the version with which the library was built as a string.
 */
  const char * ARGPP_API str_library_build_version();

  /**
 * @enum argpp_parse_flag
 * @brief These can be used in the "flags" argument to the
 * argp_parse() function. Values given as \c ARGP_... symbols
 * are defined in the header file \b argp.h.
 */
  enum argpp_parse_flag {
    fl_parse_argv_0 = ARGP_PARSE_ARGV0, //!< = ARGP_PARSE_ARGV0,
    fl_no_errs      = ARGP_NO_ERRS,     //!< = ARGP_NO_ERRS,
    fl_no_args      = ARGP_NO_ARGS,     //!< = ARGP_NO_ARGS,
    fl_no_reorder   = ARGP_IN_ORDER,    //!< = ARGP_IN_ORDER,
    fl_no_help      = ARGP_NO_HELP,     //!< = ARGP_NO_HELP,
    fl_no_exit      = ARGP_NO_EXIT,     //!< = ARGP_NO_EXIT,
    fl_long_only    = ARGP_LONG_ONLY,   //!< = ARGP_LONG_ONLY,
    fl_silent       = ARGP_SILENT,      //!< = ARGP_SILENT,
    fl_all_check = ( ARGP_PARSE_ARGV0
    | ARGP_NO_ERRS
    | ARGP_NO_ARGS
    | ARGP_IN_ORDER
    | ARGP_NO_HELP
    | ARGP_NO_EXIT
    | ARGP_LONG_ONLY
    | ARGP_SILENT ) //!< = all of the above OR'ed together
  };

  /**
 * @enum argpp_option_flag
 * @brief Gnulib also has OPTION_NO_TRANS ... glibc doesn't.
 */
  enum argpp_option_flag {
    opt_arg_is_mandatory = 0,
    opt_arg_is_optional = OPTION_ARG_OPTIONAL, //!< = OPTION_ARG_OPTIONAL,
    opt_is_hidden       = OPTION_HIDDEN,       //!< = OPTION_HIDDEN,
    opt_is_alias        = OPTION_ALIAS,        //!< = OPTION_ALIAS,
    opt_is_doc          = OPTION_DOC,          //!< = OPTION_DOC,
    opt_not_in_usage    = OPTION_NO_USAGE,     //!< = OPTION_NO_USAGE,
#ifdef OPTION_NO_TRANS
    opt_no_translate    = OPTION_NO_TRANS,
#endif
    opt_all_check = ( OPTION_ARG_OPTIONAL
    | OPTION_HIDDEN
    | OPTION_ALIAS
    | OPTION_DOC
    | OPTION_NO_USAGE
#ifdef OPTION_NO_TRANS
    | OPTION_NO_TRANS
#endif
    ) //!< all of the above OR'ed together
  };

  /**
 * @enum argpp_argp_key
 * @brief These can be passed to the parser function
 * as the "key" argument.
 */
  enum argpp_argp_key {
    key_is_arg       = ARGP_KEY_ARG,     //!< = ARGP_KEY_ARG,
    key_more_args    = ARGP_KEY_ARGS,    //!< = ARGP_KEY_ARGS,
    key_is_end       = ARGP_KEY_END,     //!< = ARGP_KEY_END,
    key_no_more_args = ARGP_KEY_NO_ARGS, //!< = ARGP_KEY_NO_ARGS,
    key_init         = ARGP_KEY_INIT,    //!< = ARGP_KEY_INIT,
    key_success      = ARGP_KEY_SUCCESS, //!< = ARGP_KEY_SUCCESS,
    key_has_error    = ARGP_KEY_ERROR,   //!< = ARGP_KEY_ERROR,
    key_finito       = ARGP_KEY_FINI     //!< = ARGP_KEY_FINI
  };

  /**
 * @enum argpp_argp_key_help
 * @brief Used in the help filter function.
 */
  enum argpp_argp_key_help {
    khp_pre_doc  = ARGP_KEY_HELP_PRE_DOC,       //!< = ARGP_KEY_HELP_PRE_DOC,
    khp_post_doc = ARGP_KEY_HELP_POST_DOC,      //!< = ARGP_KEY_HELP_POST_DOC,
    khp_header   = ARGP_KEY_HELP_HEADER,        //!< = ARGP_KEY_HELP_HEADER,
    khp_extra    = ARGP_KEY_HELP_EXTRA,         //!< = ARGP_KEY_HELP_EXTRA,
    khp_dup_note = ARGP_KEY_HELP_DUP_ARGS_NOTE, //!< = ARGP_KEY_HELP_DUP_ARGS_NOTE,
    khp_args_doc = ARGP_KEY_HELP_ARGS_DOC //!< same as parser's args_doc field,
    //!< i.e. a string for the "Usage:" message
  };

  /**
 * @enum argpp_help_flag
 * @brief These are used when calling argp_help() or argp_state_help().
 */
  enum argpp_help_flag {
    hlp_usage       = ARGP_HELP_USAGE,       //!< = ARGP_HELP_USAGE,
    hlp_short_usage = ARGP_HELP_SHORT_USAGE, //!< = ARGP_HELP_SHORT_USAGE,
    hlp_see_help    = ARGP_HELP_SEE,         //!< = ARGP_HELP_SEE,
    hlp_long_help   = ARGP_HELP_LONG,        //!< = ARGP_HELP_LONG,
    hlp_pre_doc     = ARGP_HELP_PRE_DOC,     //!< = ARGP_HELP_PRE_DOC,
    hlp_post_doc    = ARGP_HELP_POST_DOC,    //!< = ARGP_HELP_POST_DOC,
    hlp_help_doc    = ARGP_HELP_DOC,         //!< = ARGP_HELP_DOC,
    hlp_bug_addr    = ARGP_HELP_BUG_ADDR,    //!< = ARGP_HELP_BUG_ADDR,
    hlp_long_only   = ARGP_HELP_LONG_ONLY,   //!< = ARGP_HELP_LONG_ONLY,
    hlp_exit_err    = ARGP_HELP_EXIT_ERR,    //!< = ARGP_HELP_EXIT_ERR,
    hlp_exit_ok     = ARGP_HELP_EXIT_OK,     //!< = ARGP_HELP_EXIT_OK,
    hlp_std_error   = ARGP_HELP_STD_ERR,     //!< = ARGP_HELP_STD_ERR,
    hlp_std_usage   = ARGP_HELP_STD_USAGE,   //!< = ARGP_HELP_STD_USAGE,
    hlp_std_help    = ARGP_HELP_STD_HELP     //!< = ARGP_HELP_STD_HELP
  };

  /**
 * @struct ArgppOption
 * @brief A wrapper around \b argp_option.
 * @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Option-Vectors.html#Argp-Option-Vectors
 * for documentation of the \b argp_option struct.
 */
  struct ARGPP_API ArgppOption {
      int         key_;
      unsigned    flags_;
      int         group_;
      std::string long_name_;
      std::string arg_;
      std::string doc_or_header_;

      ArgppOption()
        : key_          (0)
        , flags_        (0)
        , group_        (0)
        , long_name_    ()
        , arg_          ()
        , doc_or_header_()
      {}

      ArgppOption(const ArgppOption &other)
        : key_          ( other.key_          )
        , flags_        ( other.flags_        )
        , group_        ( other.group_        )
        , long_name_    ( other.long_name_    )
        , arg_          ( other.arg_          )
        , doc_or_header_( other.doc_or_header_)
      {}

      ArgppOption& operator=(const ArgppOption &other)
      {
        key_          = other.key_;
        flags_        = other.flags_;
        group_        = other.group_;
        long_name_    = other.long_name_;
        arg_          = other.arg_;
        doc_or_header_= other.doc_or_header_;
        return *this;
      }

      bool operator==(const ArgppOption &other) const {
        if (key_ && key_ == other.key_) {
          return true;
        } else if (!long_name_.empty() && !other.long_name_.empty()) {
          return (long_name_ == other.long_name_);
        }
        return false;
      }

      bool operator<(const ArgppOption &other) const {
        if (key_ && other.key_) {
          return key_ < other.key_;
        } else if (key_ && !other.long_name_.empty()) {
          return key_ < other.long_name_[0];
        } else if (!long_name_.empty() && other.key_) {
          return (long_name_[0] < other.key_);
        } else if (!long_name_.empty() && !other.long_name_.empty()) {
          return (long_name_.compare(other.long_name_) < 0);
        }
        return false;
      }
  };

  /**
 * @struct ParsedOption
 * @brief Used to store the successfully parsed options
 * and any option arguments the user entered on the command line.
 */
  struct ARGPP_API ParsedOption {
      int         key_;
      std::string long_name_;
      std::string arg_;

      ParsedOption()
        : key_      (0)
        , long_name_()
        , arg_      () {}

      explicit ParsedOption(
          int k
          , const std::string &s = std::string()
          , const std::string &a = std::string())
        : key_      (k)
        , long_name_(s)
        , arg_      (a) {}

      bool empty() const { return (key_ == 0) && long_name_.empty(); }

      bool operator<(const ParsedOption &other) const {
        //---------------------------------------------
        // Compares the keys first, then the key with
        // the first character of the long name if
        // not empty, otherwise compare the long names:
        //---------------------------------------------
        if (key_ && other.key_ ) {
          return key_ < other.key_;
        } else if (key_ && !other.long_name_.empty()) {
          return key_ < other.long_name_[0];
        } else if (!long_name_.empty() && other.key_) {
          return (long_name_[0] < other.key_);
        } else if (!long_name_.empty() && !other.long_name_.empty()) {
          return (long_name_.compare(other.long_name_) < 0);
        }
        return false;
      }

      bool operator==(const ParsedOption &other) const {
        return (key_ == other.key_)
            || (long_name_ == other.long_name_);
      }
  };

  /**
 * @typedef VersionFunc
 * @brief See documentation for \b argp_program_version_hook
 * at http://www.gnu.org/software/libc/manual/html_node/Argp-Global-Variables.html#Argp-Global-Variables
 */
  typedef void (*VersionFunc)(FILE* f, argp_state* s);

  /**
 * @typedef HelpFilter
 * @brief See documentation for struct \b argp
 * at http://www.gnu.org/software/libc/manual/html_node/Argp-Parsers.html#Argp-Parsers
 */
  typedef char* (*HelpFilter)(int key, const char *text, void *input);

  // forward declare:
  class ArgppBase;

  typedef std::vector<argp_child>                   Argp_Child_Vec;
  typedef std::vector<argp_option>                  Argp_Option_Vec;
  typedef std::vector<ArgppBase*>                   ArgppChildren;
  typedef ArgppChildren::iterator                   Child_It;
  typedef ArgppChildren::const_iterator             Child_It_const;
  typedef std::vector<ArgppOption>                  ArgppOptions;
  typedef std::map<argp_child*, ArgppBase*>         MapOfChildren;
  typedef ArgppOptions::iterator                    Opt_It;
  typedef ArgppOptions::const_iterator              Opt_It_const;
  typedef std::vector<ParsedOption>                 ParsedOptions;
  typedef std::vector<ParsedOption>::iterator       P_Opts_it;
  typedef std::vector<ParsedOption>::const_iterator P_Opts_cit;
  typedef std::vector<std::string>                  OtherArgs;

  /**
 * @namespace argpp::impl
 * @brief Encapsulates the private implementation interface used by the base class
 * argpp::ArgppBase<br>(a design pattern known as the
 * <a href="http://c2.com/cgi/wiki?PimplIdiom" target="_blank">Pimpl idiom</a>)
 *
 * If C++11 or C++0x language support is available, our code uses a \c \b std::unique_pointer<T>
 * in the argppbaseimpl.cpp file to hold the pointer to the singleton root parser
 * instance. Otherwise, we use a very simple auto pointer for that which is default constructible
 * with a NULL pointer and never uses copying or assignment.
 */
  namespace impl {
    // forward declarations:
    class ArgppBaseImpl;
#ifndef ARGPP_HAVE_UNIQUE_PTR
    class DeleteHelper;
#endif
  }

  /**
 * @brief The ArgppBase class
 * @details Derive your own class from this base class and create instances
 * by calling the argpp::Factory<T>::createParser() function (replacing the
 * "T" with your own class name).
 */
  class ARGPP_API ArgppBase {
      friend class impl::ArgppBaseImpl;
#ifndef ARGPP_HAVE_UNIQUE_PTR
      friend class impl::DeleteHelper;
#else
      friend class std::default_delete<ArgppBase>;
#endif

    private:
      ///
      /// @brief The opaque pointer to our implementation class.
      ///
      impl::ArgppBaseImpl* pimpl_;
      //-----------------------------------------------------------------------
      // The default constructor, copy constructor, and copy assignment
      // operator of argpp::ArgppBase are not implemented.
      //-----------------------------------------------------------------------
      /**
     * @brief The default constructor of argpp::ArgppBase is not implemented.
     */
      ArgppBase();
      /**
     * @brief The copy constructor of argpp::ArgppBase is not implemented.
     */
      ArgppBase(const ArgppBase &);
      /**
     * @brief The copy assignment operator of argpp::ArgppBase is not implemented.
     */
      ArgppBase& operator=(const ArgppBase &);

    protected:
      /**
     * @brief The destructor of ArgppBase is protected.
     * @details Derived classes should NOT be created automatically, i.e.
     * on the stack. ONLY use the argpp::Factory<T>::createParser() function
     * to create parser instances. This way ensures that the hierarchy
     * from the singleton root instance with any children parsers is
     * implemented correctly.
     *
     * Declaring the base class destructor protected does not prohibit
     * derived classes from creating objects on the stack. The base class
     * is abstract anyway, so instances cannot be created directly.
     * Unfortunately, it is not feasible to try to prevent derived
     * classes from creating their objects on the stack
     * (cf. Scott Meyers, Item 27 of &ldquo;More Effective C++&rdquo;
     * Addison-Wesley, ISBN 0-201-63371-X), and it probably wouldn&rsquo;t be worth
     * the effort for this application, anyway (although it might be a good idea).
     */
      virtual ~ArgppBase();
      ///
      /// @brief This is the only base class constructor available to derived classes.
      /// @param argc : The number of elements in the command line passed to main()
      /// @param argv : The vector of command line arguments passed to main()
      /// @param parent : A pointer to the parent parser, or NULL if it is the
      /// root instance.
      ///
      ArgppBase(int        argc
                , char     **argv
                , ArgppBase *parent
                );
      ///
      /// @brief Wraps the argp_error() function.
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Helper-Functions.html#Argp-Helper-Functions
      /// for further details.
      ///
      static void argppError    (const argp_state *state, const std::string &errmsg);
      ///
      /// @brief Wraps the argp_failure() function.
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Helper-Functions.html#Argp-Helper-Functions
      /// for further details.
      ///
      static void argppFailure  (const argp_state *state, int status, int errnum, const std::string &errmsg);
      ///
      /// @brief Wraps the argp_usage() function.
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Helper-Functions.html#Argp-Helper-Functions
      /// for further details.
      ///
      static void argppUsage    (const argp_state *state);
      ///
      /// @brief Wraps the argp_state_help() function
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Helper-Functions.html#Argp-Helper-Functions
      /// for further details.
      ///
      static void argppStateHelp(const argp_state *state, FILE *stream, unsigned flags);
      ///
      /// @brief Does sanity checking on flags passed to the argp_parse() function.
      /// @param val : Passed as a non-const reference. Flags are sometimes interdependent.
      /// If there is an inconsistent flag set which can be corrected by adding or subtracting
      /// one of the other flags, then the flagOk() function can change the value of \b val if necessary.
      /// The current implementation does not alter the value of \b val for these flags except
      /// for the overloaded version which checks option flags ... this function checks parser
      /// flags passed to the \b argp_parse() function.
      /// @param chk : Contains fl_all_check as default which has all of the flags in argpp_parse_flag OR'ed together
      /// @returns Returns false if val contains any flags not contained in the enumeration argpp_parse_flag.
      ///
      static bool flagOk(unsigned & val, argpp_parse_flag chk);
      ///
      /// @brief Does sanity checking on flags of option parameters.
      /// @param val : Passed as a non-const reference. Flags are sometimes interdependent.
      /// If there is an inconsistent flag set which can be corrected by adding or subtracting
      /// one of the other flags, then the flagOk() function can change the value of val if necessary.
      /// At the moment, this is only done for OPTION_NO_TRANS if using gnulib (glibc doesn't have it)
      /// and OPTION_DOC is not set. OPTION_NO_TRANS can only be used together with OPTION_DOC.
      /// @param chk : Contains opt_all_check as default which has all of the flags in argpp_option_flag OR'ed together
      /// @returns Returns false if val contains any flags not contained in the enumeration argpp_option_flag.
      ///
      static bool flagOk(unsigned & val, argpp_option_flag chk);

      //-----------------------------------------------------------------------
      // Non-static protected member functions:
      //-----------------------------------------------------------------------
      ///
      /// @brief Derived classes can call this function in the implementation of parserImpl() when
      /// an option and any argument have been recognized.
      /// @details Either each derived class can collect their own parsed options, or they can be
      /// added to the root instance which can be more convenient when they are stored all in one place.
      /// The default behavior is to add everything to the root instance's data. To change this
      /// behavior, call the static member function setRootHasParsedOptions() with its
      /// argument \b set_root set to \b false.
      /// @param opt : Contains the short option as an integer, the long option name, if any, and any
      /// argument associated with this option as a std::string.
      ///
      void addParsedOption(const ParsedOption &opt);
      ///
      /// \brief Overload taking just the key and an optional argument.
      ///
      void addParsedOption(int key, const std::string & ln = std::string(), const std::string & arg = std::string());
      ///
      /// @brief Derived classes can call this function in the implementation of keyArgImpl() when
      /// an option and any argument have been recognized.
      /// @details The base class default implementation does this (see documentation for
      /// ArgppBase::keyArgImpl() for more details). Either each derived class can collect
      /// their own non-option arguments, or they can be added to the
      /// root instance. It might be more convenient when they are stored all in one vector.
      /// The default behavior is to add everything to the root instance's data. To change this
      /// behavior, call the static member function setRootHasParsedOptions() with its
      /// argument \b set_root set to \b false.
      /// @param arg : Contains the non-option argument as a std::string.
      ///
      void addNonOptionArg(const std::string &arg);
      ///
      /// @brief Derived classes must implement the pure virtual
      /// parserImpl() member function.
      /// @details This function is called whenever argp calls argpp::impl::ArgppBaseImpl::parserRouter()
      /// with any key value which is not one of the special ARGP_KEY_...
      /// enumeration values (see documentation for argpp::argpp_argp_key).
      /// @param key : one of the user-defined short options or the numeric
      /// value (if it is not a printable character) associated with some
      /// long option.
      /// @param arg : If there is an argument specified for the key, this will
      /// contain the value entered, otherwise NULL.
      /// @param state : See documentation contained in the comments to the
      /// struct definition contained in the \b argp.h header file.
      /// @returns The return type \c error_t is an alias for int. If a key passed to
      /// parserImpl() was not recognized by the derived class implementation,
      /// the value \b ARGPP_UNKNOWN (or its alias \b ARGPP_DONT_CARE) should be returned.
      /// Otherwise, \b ARGPP_SUCCESS should be returned, or else \b argp will output
      /// an error message similar to this:<br>
      /// \code PROGRAM ERROR: Option should have been recognized!? \endcode
      ///
      virtual error_t parserImpl(int key, const char *arg, argp_state *state) = 0;
      ///
      /// @brief A generic implementation of parserImpl() which clients can
      /// call.
      /// @details If the key corresponds to one of the options which were passed to
      /// \b argp_parse(), this implementation stores it immediately as a parsed option
      /// by calling argpp::ArgppBase::addParsedOption().
      /// Clients can delay evaluating the options and option arguments entered by the
      /// user until after argp_parse() returns. Otherwise, clients would either have
      /// to implement something similar to this themselves, or else put key-specific
      /// code into their implementation of parserImpl().
      ///
      /// See documentation of parserImpl() for more details as well as descriptions of
      /// the parameters and the return value type.
      ///
      error_t genericParserImpl(int key, const char *arg, argp_state *state);
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// key \b ARGP_KEY_INIT.
      /// @details The special key \b ARGP_KEY_INIT is always the first key passed
      /// to the parser function argpp::impl::ArgppBaseImpl::parserRouter().
      /// Please refer to the comments in the
      /// source code in the file argppbaseimpl.cpp for details on how the child_inputs[]
      /// array is set up. After initializing child_inputs[], argpp::impl::ArgppBaseImpl::parserRouter() will call
      /// initImpl(). If the derived class has reimplemented this function, its version
      /// will be called; otherwise, the dummy implementation in the base class is called.
      /// @returns The return type error_t is an alias for int.
      /// Derived classes can return either \b ARGPP_SUCCESS or \b ARGPP_DONT_CARE (the same as \b ARGPP_UNKNOWN)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values can be returned (for example, \b ENOMEM or \b EINVAL).
      ///
      virtual error_t initImpl(argp_state *) { return ARGPP_DONT_CARE; }
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// \b ARGP_KEY_ERROR.
      /// @details Behaves similarly to initImpl().
      /// See documentation for parserImpl() and initImpl() for more details.
      /// Passed in if an error has occurred and parsing is terminated.
      /// In this case a call with a key of \b ARGP_KEY_SUCCESS is never made.
      /// @returns The return type error_t is an alias for int.
      /// Derived classes can return either \b ARGPP_SUCCESS or \b ARGPP_UNKNOWN (or \b ARGPP_DONT_CARE)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values (e.g. \b ENOMEM or \b EINVAL) can be returned.
      ///
      virtual error_t errorImpl     (argp_state *) { return ARGPP_DONT_CARE; }
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// ARGP_KEY_FINI.
      /// @details The special argp key ARGP_KEY_FINI is always the last key passed to
      /// the parserRouter() function, even after ARGP_KEY_SUCCESS and ARGP_KEY_ERROR.
      /// Any resources allocated by ARGP_KEY_INIT may be freed here. At times,
      /// certain resources allocated are to be returned to the caller after a
      /// successful parse. In that case, those particular resources can be
      /// freed in the ARGP_KEY_ERROR case.
      /// @returns The return type error_t is an alias for int.
      /// Derived classes can return either \b ARGPP_SUCCESS or \b ARGPP_UNKNOWN (or \b ARGPP_DONT_CARE)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values (e.g. \b ENOMEM or \b EINVAL) can be returned.
      ///
      virtual error_t finiImpl      (argp_state *) { return ARGPP_DONT_CARE; }
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// \b ARGP_KEY_ARG.
      /// @details After parsing all of the options and their arguments, any remaining
      /// non-option arguments entered on the command line are parsed. The first argument is
      /// passed on to the parser with the special key \b ARGP_KEY_ARG. If the parser function returns
      /// \b ARGPP_SUCCESS, the next one or more calls will contain the key \b ARGP_KEY_ARGS.
      /// Parsers can fetch each subsequent non-option argument by using the corresponding
      /// index to the argv command line, stored in the \c state->next member, or it
      /// can do this immediately when receiving the \b ARGP_KEY_ARG key value in the preceding
      /// call. The base class implementation of keyArgImpl() is therefore not a dummy
      /// implementation, but does exactly this.
      /// @returns The return type error_t is an alias for int.
      /// Derived classes, if you choose to override this, can return either \b ARGPP_SUCCESS
      /// or \b ARGPP_UNKNOWN (or \b ARGPP_DONT_CARE)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values (e.g. \b ENOMEM or \b EINVAL) can be returned.
      ///
      virtual error_t keyArgImpl    (const char *arg, argp_state *state);
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// \b ARGP_KEY_ARGS.
      /// @details See documentation for keyArgImpl() for more details. If you choose not
      /// to override keyArgImpl(), you won't need to override this function, either,
      /// because \b argp would never call it after keyArgImpl() fetches all of the
      /// remaining non-option arguments.
      /// @returns The return type \c error_t is an alias for int.
      /// Derived classes can return either \b ARGPP_SUCCESS or \b ARGPP_UNKNOWN (or \b ARGPP_DONT_CARE)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values (e.g. \b ENOMEM or \b EINVAL) can be returned.
      ///
      virtual error_t keyArgsImpl   (argp_state* /*state*/) { return ARGPP_DONT_CARE; }
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// \b ARGP_KEY_NO_ARGS.
      /// @details If there were no non-option arguments entered on the command line, the
      /// argpp::impl::ArgppBaseImpl::parserRouter() function receives this key. Derived classes can override this
      /// and return an error, or call argppUsage(), if additional arguments were expected
      /// and none were given.
      /// @returns The return type \c error_t is an alias for int.
      /// Derived classes can return either \b ARGPP_SUCCESS or \b ARGPP_UNKNOWN (or \b ARGPP_DONT_CARE)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values (e.g. \b ENOMEM or \b EINVAL) can be returned.
      ///
      virtual error_t keyNoArgsImpl (argp_state* /*state*/) { return ARGPP_DONT_CARE; }
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// \b ARGP_KEY_SUCCESS.
      /// @details Passed in when parsing has successfully been completed, even if arguments remain.
      /// @returns The return type \c error_t is an alias for int.
      /// Derived classes can return either \b ARGPP_SUCCESS or \b ARGPP_UNKNOWN (or \b ARGPP_DONT_CARE)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values (e.g. \b ENOMEM or \b EINVAL) can be returned.
      ///
      virtual error_t keySuccessImpl(argp_state* /*state*/) { return ARGPP_DONT_CARE; }
      ///
      /// @brief Called by argpp::impl::ArgppBaseImpl::parserRouter() when it is called by \b argp with the key
      /// \b ARGP_KEY_END.
      /// @details This indicates that there are no more command line arguments.
      /// Parser functions are called in a different order, children first.
      /// This allows each parser to clean up its state for the parent.
      /// @returns The return type \c error_t is an alias for int.
      /// Derived classes can return either \b ARGPP_SUCCESS or \b ARGPP_UNKNOWN (or \b ARGPP_DONT_CARE)
      /// unless some more serious error condition has occurred, in which case one of the
      /// standard C error values (e.g. \b ENOMEM or \b EINVAL) can be returned.
      ///
      virtual error_t keyEndImpl    (argp_state* /*state*/) { return ARGPP_DONT_CARE; }
      ///
      /// @brief Derived classes must call installHelpFilter() in order to activate calling
      /// the virtual helpFilterImpl() function. Otherwise, it is never called by \b argp.
      /// @details The most typical use case for helpFilterImpl() is to provide
      /// run-time translations of help strings.
      /// See http://www.gnu.org/software/libc/manual/html_node/Argp-Help-Filtering.html#Argp-Help-Filtering
      /// for further details.
      /// @return Any string returned by helpFilterImpl() is copied by the static argpp::impl::ArgppBaseImpl::HelpFilter() function
      /// to a C string allocated with malloc() before returning it to the caller. This is according
      /// to the argp documentation at
      /// http://www.gnu.org/software/libc/manual/html_node/Argp-Help-Filtering.html#Argp-Help-Filtering.
      ///
      virtual std::string helpFilterImpl(int /*key*/, const char * /*text*/) { return std::string(); }
      ///
      /// @brief Clients can call this function in their parserImpl() implementation.
      /// @details What it does is to check the \b key and \b arg arguments with the
      /// options passed to the \b argp_parse() function. If the \b key is found, the stored
      /// option's ArgppOption::flags_ member is checked for OPTION_ARG_OPTIONAL and if the ArgppOption::arg_
      /// member is not empty. If an argument is required but none is given, the return value
      /// will be set to \b ARGPP_OPTION_NEEDS_ARG.
      /// @param key : the key or short option value passed to the parser function
      /// @param arg : the string containing any argument given (can be NULL)
      /// @param long_name : a pointer to a std::string in which the long name, if any, is returned
      /// when the key was found and any required argument was supplied. This can be NULL if
      /// returning the long name isn't necessary.
      /// @return Returns one of the following values:
      /// \code
      /// ARGPP_OPTION_OK        == 0 // (key was found, and arg is available if required)
      /// ARGPP_OPTION_UNKNOWN   == 1 // (key was not found)
      /// ARGPP_OPTION_NEEDS_ARG == 2 // (key was found, but arg was NULL and an argument was required)
      /// \endcode
      ///
      int  supportedOption(int key, const char* arg, std::string* long_name = NULL);

    public:
      //-------------------------
      // Static member functions:
      //-------------------------
      ///
      /// @brief Flag to let derived classes know that one of the special options
      /// "--help", "--version" or "--usage" was entered by the user on the command line.
      /// @details The constructor of argpp::impl::ArgppBaseImpl does its own parsing
      /// of the command line in the \b argc and \b argv parameters passed in order
      /// to determine whether one of the special options listed above was entered.
      /// If so, a flag is set to \b true which can be queried with this function.
      ///
      static bool wasHelpEntered();
      ///
      /// @brief Returns the \b argc argument passed to the \c main() function, or 0 if the root
      /// parser hasn&rsquo;t yet been created.
      /// @details Once the root instance has been created, the same argc and argv data is used for
      /// all the rest of the parsers. Subsequent (child) parsers can be created without passing
      /// the data for each object.
      /// The argpp::Factory<T>::createParser() function needs access to the data, therefore these
      /// are publicly accessible.
      ///
      static int getArgc();
      ///
      /// @brief Returns the \b argv argument passed to the \c main() function, or NULL if the root
      /// parser hasn&rsquo;t yet been created.
      /// @details Once the root instance has been created, the same argc and argv data is used for
      /// all the rest of the parsers. Subsequent (child) parsers can be created without passing
      /// the data for each object.
      /// The argpp::Factory<T>::createParser() function needs access to the data, therefore these
      /// are publicly accessible.
      ///
      static char** getArgv();
      ///
      /// @brief Determines whether parsed options are stored in the root instance
      /// or in each individual parser.
      /// @details When parserImpl() or some other client code calls addParsedOption(), the target
      /// will be either all in the root instance, or else in each individual parser. The default behavior
      /// is to store all of the results in one place, i.e. in the root instance.
      /// Call this function with an argument of \b false to change this behavior.
      ///
      static void setRootHasParsedOptions(bool set_root = true);
      ///
      /// @brief Determines whether non-option arguments are stored in the root instance
      /// or in each individual parser.
      /// @details When keyArgImpl() or some other client code calls addNonOptionArg(), the target
      /// will be either all in the root instance, or else in each individual parser. The default behavior
      /// is to store all of the results in one place, i.e. in the root instance.
      /// Call this function with an argument of \b false to change this behavior.
      ///
      static void setRootHasNonOptionArgs(bool set_root = true);
      ///
      /// @brief Tells where parsed options are stored: either in the root instance,
      /// or in each individual parser.
      /// @details When parserImpl() or some other client code calls addParsedOption(), the target
      /// will be either all in the root instance, or else in each individual parser. The default behavior
      /// is to store all of the results in one place, i.e. in the root instance.
      /// Call setRootHasParsedOptions() with an argument of \b false to change this behavior.
      /// @return Returns true if all of the parsed options are stored in the root instance.
      ///
      static bool rootHasParsedOptions();
      ///
      /// @brief Tells where non-option arguments entered on the command line are stored:
      /// either in the root instance, or in each individual parser.
      /// @details When keyArgImpl() or some other client code calls addNonOptionArg(), the target
      /// will be either all in the root instance, or else in each individual parser. The default behavior
      /// is to store all of the results in one place, i.e. in the root instance.
      /// Call setRootHasNonOptionArgs() with an argument of \b false to change this behavior.
      /// @return Returns true if all of the non-option arguments are stored in the root instance.
      ///
      static bool rootHasNonOptionArgs();
      ///
      /// @brief This will call \b argp_parse() after setting up the low-level arrays of
      /// options and child parsers (i.e., the C-language structs understood by \b argp ).
      /// @details Call the static parse() function after the
      /// root instance has been created and after adding all children and
      /// options.
      /// @return Returns true if parsing was successful.
      /// If \b argp doesn't exit prematurely, and parse() returns
      /// false, you can examine the return value by calling getRetvalErrorFromArgpParse().
      /// If there is no root instance yet, parse() also returns false.
      /// In that case, getRetvalErrorFromArgpParse() returns
      /// ARGPP_ENODATA (defined as ENODATA, or the decimal
      /// number 61 if ENODATA is not defined).
      ///
      static bool parse();
      ///
      /// @brief This returns the error code that would typically be returned by one of the
      /// <a href="http://www.gnu.org/software/libc/manual/html_node/Argp-Helper-Functions.html#Argp-Helper-Functions" target="_blank">helper functions</a>.
      /// @return \c \b error_t is an alias for \c \b int. It should correspond to one of the standard
      /// C library error codes. See http://www.gnu.org/software/libc/manual/html_node/Error-Codes.html#Error-Codes
      /// for further details.
      ///
      static error_t getErrorCodeFromArgp();
      ///
      /// @brief If parse() returns false (i.e. an error has occurred), you can fetch the
      /// actual return value from argp_parse() by calling this function.
      /// @return \c \b error_t is an alias for \c \b int. It should correspond to one of the standard
      /// C library error codes. See http://www.gnu.org/software/libc/manual/html_node/Error-Codes.html#Error-Codes
      /// for further details. If this is called before parse() is called,
      /// or if there was no root instance yet created, the function returns
      /// ARGPP_ENODATA (defined as ENODATA, or the decimal number 61 if ENODATA is not defined).
      ///
      static error_t getRetvalErrorFromArgpParse();
      ///
      /// @brief These flags are passed to the argp_parse() function.
      /// @details Child parsers can have different flags.
      /// See documentation for argpp::ArgppBase::setChildFlags() for details.
      /// @param flags : a bitmask containing the individual flag values OR'ed together.
      /// The flag values should be one or more of the \b argpp_parse_flag enumeration values.
      /// @return Returns true if the flags pass the check done by the flagsOk() function.
      ///
      static bool setArgpParseFlags  (unsigned int flags);
      ///
      /// @brief Checks the pointer against all of the registered children
      /// in the root instance's tree of child objects.
      /// @param parent: A pointer to a parser instance derived from ArgppBase.
      /// @return Returns true if parent is in the tree of the root instance,
      /// or if it is the root instance itself.
      ///
      static bool isParentValid      (ArgppBase  *parent);
      ///
      /// @brief Called by the argpp::Factory<T>::createParser() function
      /// when the first (root) instance is created. Subsequent calls do not
      /// do anything.
      ///
      static void installArgppRootInstance (ArgppBase  *pInstance);
      ///
      /// @brief Sets the global variable \b argp_program_bug_address.
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Global-Variables.html#Argp-Global-Variables
      /// for further details.
      ///
      static void installBugAddress  (const std::string &ba);
      ///
      /// @brief Sets the global variable \b argp_program_version
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Global-Variables.html#Argp-Global-Variables
      /// for further details.
      ///
      static void installProgVersion (const std::string &pvr);
      ///
      /// @brief Sets the global variable \b argp_program_version_hook
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Global-Variables.html#Argp-Global-Variables
      /// for further details.
      ///
      static void installVersionHook (VersionFunc f);
      ///
      /// @brief Sets the locale for translating \b argp help messages.
      /// @details If this is an empty string, argp uses either the "libc" domain
      /// (i.e. locale) or whatever the sysem locale uses.
      /// @param dom : Values should conform to the BCP 47 standard locale format
      /// or whatever the "locale" command entered at a terminal on POSIX
      /// conforming systems returns (e.g.: "en_US.UTF-8").
      ///
      /// Also, please refer to the GNU \b argp documentation at
      /// http://www.gnu.org/software/libc/manual/html_node/Argp-Help-Filtering.html#Argp-Help-Filtering
      /// for further details.
      ///
      static void setDomain(const std::string &dom);
      ///
      /// @brief Returns a pointer to the initially created parser,
      /// or NULL if there isn't any.
      /// @details Many of the static functions in ArgppBase will expect the existence of the
      /// root parser in order to be effective.
      ///
      static ArgppBase* getRootInstance();
      ///
      /// @brief Destroys all parsers including the root instance.
      /// Subsequent calls to getRootInstance() return NULL.
      ///
      static void clearAllParsers();

      //------------------------------------
      // Non-static public member functions:
      //------------------------------------
      ///
      /// @brief Call installHelpFilter() to ensure that helpFilterImpl() is called by \b argp.
      /// @details The static argpp::impl::ArgppBaseImpl::helpFilter() function pointer is copied to the corresponding
      /// member of the parser's \b argp struct. See
      /// http://www.gnu.org/software/libc/manual/html_node/Argp-Parsers.html#Argp-Parsers
      /// for further details.
      ///
      void installHelpFilter();
      ///
      /// @brief Appends a single option to the vector of options.
      /// @details Add all of the options by calling addOption() or addOptions() before calling the
      /// static parse() function.
      /// @param opt : takes an argument of type ArgppOption.
      /// @return Returns true if the flags_ member of the argument contains no bits
      /// which are not in the enumeration argpp_option_flag.
      ///
      bool addOption         (const ArgppOption &opt);
      ///
      /// @brief Overloaded version taking an argument of type \c struct \c argp_option.
      /// @param opt : Takes an argument of type \c struct \c argp_option. This type is defined by \b argp
      /// in the \b argp.h header file.
      /// @return Returns true if the flags_ member of the argument contains no bits
      /// which are not in the enumeration argpp_option_flag.
      ///
      bool addOption         (const argp_option &opt);
      ///
      /// @brief Takes an array of \c argp_option structs.
      /// @details This type is defined by \b argp
      /// in the \b argp.h header file.
      /// @param ao : pointer to the initial element of the array of argp_option structs.
      /// @param len : If the array of argp_option structs is delimited by a NULL_ARGP_OPTION element,
      /// the parameter len is ignored by setting it to zero. Otherwise, the size of the
      /// array is used to iterate over the elements.
      /// @returns The function addOptions() is implemented by calling addOption() in a loop.
      /// If any call to addOption() returns false, addOptions stops and returns false.
      /// If all calls to addOption() succeed, true is returned.
      bool addOptions        (const argp_option *ao, size_t len = 0);
      ///
      /// @brief Convenience overload taking a const reference to a std::vector<ArgppOption> object.
      /// @param opts : const reference to a std::vector<ArgppOption> object
      /// @returns The function addOptions() is implemented by calling addOptions(),
      /// passing a pointer to the initial element of the argument `opts` and its length as the second argument.
      /// If any call to addOption() returns false, addOptions stops and returns false.
      /// If all calls to addOption() succeed, true is returned.
      bool addOptions        (const ArgppOptions &opts);
      ///
      /// @brief Returns the option with short key == `key` in `opt`, if found.
      /// @param key : Integer or printable character corresponding to the short option
      /// @param opt : ArgppOption reference to hold the returned option object
      /// @return    : Returns `true` if the option was found, otherwise `false`
      bool findOption        (int key, ArgppOption &opt) const;
      ///
      /// @brief See documentation of setChildFlags() for details.
      /// @returns Returns a bitmask with the currently set flags which are passed to argp_parse()
      /// in the child \b argp struct member.
      /// The flag values are members of the enumeration argpp_parse_flag.
      ///
      unsigned int childFlags() const;
      ///
      /// @brief Sets the \b argp::doc member.
      /// @details The doc member of the argp struct is a string containing extra text to be printed
      /// before and after the options in a long help message. It has two sections which are separated
      /// by a vertical tab ('\\v') character. By convention, the documentation before the
      /// options is just a short string explaining what the program does.
      /// Documentation printed after the options describe behavior in more detail.
      /// In \b argp++, the two sections can also be set individually by calling setPreDocMessage()
      /// and setPostDocMessage(). This is much more convenient for implementing translations
      /// of these strings.
      /// See documentation for GNU argp at
      /// http://www.gnu.org/software/libc/manual/html_node/Argp-Parsers.html#Argp-Parsers
      /// for further details.
      ///
      void setDocMessage     (const std::string &msg);
      ///
      /// @brief Child parsers can have different flags than what was passed to \b argp_parse().
      /// @details The child flags are stored in the \b argp_child struct.
      /// <a href="http://www.gnu.org/software/libc/manual/html_node/Argp-Children.html#Argp-Children" target="_blank" style="font-weight:bold">Gnu documentation</a>
      /// for the child flags leaves a bit to be desired, especially since the argp_parse() flags argument
      /// is declared unsigned, whereas the child \b flags member of \b argp_child is an \b int.
      ///
      bool setChildFlags(unsigned int flags);
      ///
      /// @brief Sets the \b argp::args_doc member.
      /// @details This is a string describing what non-option arguments are called by this parser.
      /// This is only used to print the ‘Usage:’ message. If it contains newlines, the strings
      /// separated by them are considered alternative usage patterns and printed on separate lines.
      /// Lines after the first are prefixed by ‘ or: ’ instead of ‘Usage:’.
      ///
      void setUsageMessage   (const std::string &msg);
      ///
      /// @brief See documentation of setDocMessage() for details.
      ///
      void setPreDocMessage  (const std::string &msg);
      ///
      /// @brief See documentation of setDocMessage() for details.
      ///
      void setPostDocMessage (const std::string &msg);
      ///
      /// @brief Sets the argp_child::header member.
      /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Children.html#Argp-Children
      /// for details. To implement the trick of passing an empty C string instead of a NULL pointer,
      /// the extra parameter "use_empty_cstring" can be specified (default value == false).
      ///
      void setChildHeader(const std::string &header
                          , bool use_empty_cstring=false);
      ///
      /// @brief Sets the argp_child::group member.
      /// @details Options also have a \b group member, and the child options can be grouped
      /// together with parent options by setting the same group. If group == 0, the default
      /// is to increment the value from the previous group.
      /// See http://www.gnu.org/software/libc/manual/html_node/Argp-Children.html#Argp-Children
      /// for details.
      ///
      void setGroup(int group);
      ///
      /// @brief Call getParsedOptions() after parse() has returned.
      /// @details The vector of ParsedOptions contains each valid option entered on the command line
      /// together with the long option name and the associated argument value entered, if any.
      /// These can either be distributed among the child classes doing the parsing or else
      /// collected centrally in the root instance.
      /// If the parsed options are not stored in the root instance,
      /// call getParsedOptions() on each child parser to collect all of the parsed options.
      ///
      /// Also, see documentation for rootHasParsedOptions().
      ///
      ParsedOptions const & getParsedOptions() const;
      ///
      /// @brief Call getNonOptionArgs() after parse() has returned.
      /// @details The vector OtherArgs contains the non-option
      /// arguments entered on the command line.
      /// These can either be distributed among the child classes doing the parsing or else
      /// collected centrally in the root instance.
      /// If the non-option arguments are not stored in the root instance,
      /// call getNonOptionArgs() on each child parser to collect all of the non-option arguments.
      ///
      /// Also, see documentation for rootHasNonOptionArgs().
      ///
      OtherArgs const &     getNonOptionArgs() const;
      ///
      /// @brief Returns the parent of the current object as a
      /// pointer to the base class.
      ///
      ArgppBase *           getParent();
      ///
      /// @brief Returns the parent of the current object as a
      /// pointer to the base class (const overload).
      ///
      ArgppBase const *     getParent() const;
      ///
      /// @brief Returns a non-const reference to the vector of children.
      ///
      ArgppChildren &       getChildren();
      ///
      /// @brief Returns a const reference to the vector of children.
      ///
      ArgppChildren const & getChildren() const;
      ///
      /// @brief Adds a child parser to the current object.
      /// @details If the child object already has a parent,
      /// it will be re-parented here.
      ///
      void addChild(ArgppBase *pChild);
      ///
      /// @brief Returns true if the child is an immediate child
      /// of the current object.
      ///
      bool isChildOfThisParent(ArgppBase* pChild) const;
      ///
      /// @brief Returns true if the child is anywhere in the tree
      /// of children of the current object.
      ///
      bool isGrandChild(ArgppBase* pChild) const;
  };

  ///
  /// @brief Implements an abstract factory for creating
  /// instances of derived parser classes.
  /// @details All instances of derived classes should use the
  /// createParser() function for creation of parsers.
  /// If parent == NULL, the root instance becomes the
  /// parent. If there is no root instance yet,
  /// the newly created object becomes the root instance.
  ///
  template<class T>
  struct Factory {
      ///
      /// @brief Call this function to create instances of classes
      /// derived from ArgppBase.
      /// @details Sanity checking on the parent is done by
      /// calling ArgppBase::isParentValid().
      /// If this fails, createParser() returns NULL.
      /// Any exceptions thrown by the constructor of
      /// the template parameter type T are not caught, but instead
      /// are allowed to propagate back to the caller.
      /// @param cargc : the count of command line arguments passed to main()
      /// @param vargv : the vector of command line arguments passed to main()
      /// @param  opts : a std::vector of ArgppOption objects. It is not
      /// necessary to add an empty element at the end of the vector; the
      /// library manages this when argp_parse() is called.
      /// @param parent : the parent object, or NULL
      /// @return A pointer to the template parameter T
      /// (i.e., the derived class) if successful,
      /// otherwise NULL.
      ///
      static T* createParser( int        cargc
                              , char**     vargv
                              , ArgppOptions const &opts
                                 = ArgppOptions()
                              , ArgppBase* parent = NULL)
      {
        ArgppBase* argpp_root = ArgppBase::getRootInstance();
        T* retval = NULL;
        //----------------------------------------------------
        // If parent == NULL, the root instance becomes the
        // parent.
        //
        // If argpp_root == NULL, the newly created object
        // becomes the root instance.
        //----------------------------------------------------
        bool parent_is_valid = true;

        if (!parent) {
          parent = argpp_root;
        } else {
          parent_is_valid = ArgppBase::isParentValid(parent);
        }

        if (parent_is_valid) {
          bool ok = (argpp_root && (ArgppBase::getArgc() == cargc)
                     && (ArgppBase::getArgv() == vargv))
              || !argpp_root;
          //------------------------------------------------------
          // We don't catch any exceptions generated by user code;
          // instead, we let them propagate up to the caller.
          //------------------------------------------------------
          if (ok) {
            T* pObj = new T( cargc, vargv, parent );
            if (pObj && !argpp_root) {
              ArgppBase::installArgppRootInstance(pObj);
            }
            retval = pObj;
          }
        }
        if (retval && !opts.empty())
          retval->addOptions(opts);
        return retval;
      }
      //-----------------------------------------------------------------------
      // Overloaded version so that clients don't have to pass the command line
      // arguments each time they create an object. Once the root parser has
      // been created, the argc and argv parameters are stored as static data
      // and passed on to all of the child parsers.
      //-----------------------------------------------------------------------
      ///
      /// @brief This is an overloaded version which uses the argc/argv data aleady
      /// stored as static data after the root instance has been created.
      /// @details Intended as a convenience for derived classes since the root
      /// instance already has these.
      /// @param opts   : see documentation for the first createParser() function.
      /// @param parent : see documentation for the first createParser() function.
      /// @return A pointer to the newly created type T, or NULL if there was no root instance.
      ///
      static T* createParser(  ArgppOptions const &opts = ArgppOptions()
                             , ArgppBase * parent = NULL)
      {
        ArgppBase* root = ArgppBase::getRootInstance();
        if (root) {
          return createParser(ArgppBase::getArgc()
                            , ArgppBase::getArgv()
                            , opts
                            , parent);
        }
        return NULL;
      }
  };

} // namespace argpp

#endif // ARGPP_H
