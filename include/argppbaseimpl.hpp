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

#ifndef ARGPPBASEIMPL_H
#define ARGPPBASEIMPL_H

/**
 * @file argppbaseimpl.hpp
 * @brief Contains the declarations and the private implementation details
 * used by the ArgppBase class.
 * @details It is only necessary to include this file if you are building argp++
 * as a library. If you have already built argp++ as a library, you can use it by
 * linking against the shared or static library (however you built it) and including
 * only the header file argpp.h in your own code.
 */

#include "argpp.hpp"

namespace argpp {
  namespace impl {
    ///
    /// @class ArgppBaseImpl
    /// @brief Encapsulates the implementation details and data members of ArgppBase.
    /// @details Uses the <a href="http://c2.com/cgi/wiki?PimplIdiom" target="_blank">Pimpl idiom</a>
    /// in order to facilitate binary compatibility with future releases.
    ///
    class ArgppBaseImpl
    {
        friend class argpp::ArgppBase;
        //------------------------------------------------------------------
        // The default constructor, copy constructor, and copy assignment
        // operator of ArgppBaseImpl are not implemented:
        //------------------------------------------------------------------
        ///
        /// @brief The default constructor of ArgppBaseImpl is not implemented.
        ///
        ArgppBaseImpl();
        ///
        /// @brief The copy constructor of ArgppBaseImpl is not implemented.
        ///
        ArgppBaseImpl(const ArgppBaseImpl &);
        ///
        /// @brief The copy assignment operator of ArgppBaseImpl is not implemented.
        ///
        ArgppBaseImpl& operator=(const ArgppBaseImpl &);
        //------------------------------------------------------------------
        // ArgppBase uses this constructor to initialize its pimpl_ member:
        //------------------------------------------------------------------
        ///
        /// @brief ArgppBase uses this constructor to initialize its pimpl_ member.
        /// @param argc : Count of command line arguments passed to main().
        /// @param argv : Vector of command line arguments passed to main().
        /// @param parent : Pointer to the parent object (can be NULL).
        /// @param holder : Pointer to the ArgppBase object doing the initialization.
        ///
        ArgppBaseImpl(  int        argc
                      , char **    argv
                      , ArgppBase *parent
                      , ArgppBase *holder);
        //---------------------------------------------------------------------
        // The destructor of ArgppBaseImpl deletes all of the children parsers.
        //---------------------------------------------------------------------
        ///
        /// @brief The destructor of ArgppBaseImpl deletes all of the children parsers.
        ///
        ~ArgppBaseImpl();

        //---------------------------------------------------------
        // Static data members...
        //---------------------------------------------------------
        ///
        /// @brief When parse() returns, this is set to true.
        ///
        static bool          parsed_;
        ///
        /// @brief Where to collect the parsed options and their arguments.
        /// @details When options are parsed, e.g. in the parserImpl() member function,
        /// this flag determines where they will be added. The default behavior is
        /// for the root instance to collect all of them.
        /// To change this behavior, you can call setRootHasParsedOptions() before calling
        /// the static parse() function.
        ///
        /// Also, see the documentation for rootHasParsedOptions().
        ///
        static bool          root_has_parsed_options_;
        ///
        /// @brief Where to put the non-option arguments.
        /// @details When non-option arguments are parsed, e.g. in the
        /// keyArgImpl() member function, this flag determines where they will be added.
        /// The default value is for the root instance to collect all of them. To
        /// change this behavior, you can call setRootHasNonOptionArgs() before calling
        /// the static parse() function.
        ///
        /// Also, see the documentation for rootHasNonOptionArgs().
        ///
        static bool          root_has_nonopt_args_;
        ///
        /// @brief These flags are passed to the \b argp_parse() function.
        /// @details Flags are OR'ed together as a bitmask. For details on the values allowed,
        /// please refer to the http://www.gnu.org/software/libc/manual/html_node/Argp-Flags.html#Argp-Flags
        /// document.
        ///
        /// Also, see the documentation for the argpp::argpp_parse_flag enumeration.
        ///
        static unsigned int  argp_flags_;
        ///
        /// @brief Determines whether or not argp++ should call exit() after
        /// one of the
        /// <a href="http://www.gnu.org/software/libc/manual/html_node/Argp-Helper-Functions.html#Argp-Helper-Functions" target="_blank">helper functions</a>
        /// was called.
        /// @details Unfortunately, argp does not clean up all of its memory when exit() is called.
        /// The most typical use case here is when the user enters "--help", "--version" or "--usage"
        /// on the command line.
        /// In such cases, the \b valgrind \b memcheck tool reports one block not freed but "still reachable" which
        /// means that after exit() is called, the OS will free any such memory left dangling by the
        /// application. This is not a problem in C code; however, in C++ applications, care must be taken to call
        /// destructors of objects which allocate memory in their constructors.
        /// This can be difficult to manage, especially where temporary objects are involved.
        ///
        /// Therefore, all of the wrappers for the argp helper functions which take references to
        /// std::string as arguments are implemented by copying the argp_state data, passing the flags together with
        /// ARGP_NO_EXIT, then calling the native helper function, and funally by setting the flag \b argpp_should_exit. Since the helper function wrappers are
        /// invariably called during one of the parser routines, the code in the static parserRouter()
        /// function checks this flag after the routine returns and calls exit() if it is set. That way,
        /// we can be assured that all of the temporary string objects have been cleaned up before exit()
        /// is called.
        ///
        /// Clients can pass ARGP_NO_EXIT as well, in which case this flag is always false.
        ///
        static bool          argpp_should_exit_;
        ///
        /// @brief This is typically returned by one of the
        /// <a href="http://www.gnu.org/software/libc/manual/html_node/Argp-Helper-Functions.html#Argp-Helper-Functions" target="_blank">helper functions</a>.
        /// We store it here in case clients wish to examine it when argp++ doesn't call exit():
        ///
        static error_t       error_code_from_argp_;
        ///
        /// @brief Stores the error code returned from argp_parse().
        /// @details See documentation for ArgppBase::getRetvalErrorFromArgpParse().
        ///
        static error_t       retval_error_;
        ///
        /// @brief See documentation for ArgppBase::setDomain() for further details.
        ///
        static std::string   argp_domain_;
        ///
        /// @brief This stores the \b argc parameter passed to main().
        ///
        static int    argc_;
        ///
        /// @brief This stores the \b argv parameter passed to main().
        ///
        static char **argv_;
        ///
        /// @brief The constructor of the implementation class examines the command line
        /// in an attempt to find out whether the user entered one of the special options
        /// "--help", "-V" or "--version", or "--usage" before \b argp_parse() is called.
        /// Afterwards, if clients have decided that they should pass the flag ARGP_NO_EXIT,
        /// they can check this flag by calling wasHelpEntered() in their implementation
        /// of the various "key...Impl" routines and avoid running code which isn't
        /// appropriate in such cases.
        ///
        static bool   help_called_;
        ///
        /// @brief This is the maximum number of characters allowed in the
        /// help filter string allocation. Currently it is set to MAX_BLK_SIZE = 16384.
        /// This value can be overridden by setting an environment variable
        /// "ARGPP_MAX_HELP_TEXT". The constructor of ArgppBaseImpl queries this
        /// environment variable and sets it to that size, if set.
        ///
        static size_t help_max_blk_size_;
        ///
        /// @brief The name of the environment variable to check, currently
        /// set to "ARGPP_MAX_HELP_TEXT".
        ///
        static const char *help_max_env_name_;

        //---------------------------------------------------------
        // Static member functions:
        //---------------------------------------------------------
        ///
        /// @brief See documentation for argpp::ArgppBase::wasHelpEntered()
        /// and for the static member variable help_called_ for more details.
        ///
        static bool wasHelpEntered() { return help_called_; }
        ///
        /// @brief See documentation for argpp::ArgppBase::getRetvalErrorFromArgpParse()
        /// for more details.
        ///
        static error_t getRetvalErrorFromArgpParse();
        ///
        /// @brief See documentation for argpp::ArgppBase::getErrorCodeFromArgp()
        /// for more details.
        ///
        static error_t getErrorCodeFromArgp();
        ///
        /// @brief See documentation for argpp::ArgppBase::setRootHasParsedOptions()
        /// for more details.
        ///
        static void setRootHasParsedOptions(bool set_root) { root_has_parsed_options_ = set_root; }
        ///
        /// @brief See documentation for argpp::ArgppBase::setRootHasNonOptionArgs()
        /// for more details.
        ///
        static void setRootHasNonOptionArgs(bool set_root) { root_has_nonopt_args_ = set_root; }
        ///
        /// @brief See documentation for argpp::ArgppBase::rootHasParsedOptions()
        /// for more details.
        ///
        static bool rootHasParsedOptions() { return root_has_parsed_options_; }
        ///
        /// @brief See documentation for argpp::ArgppBase::rootHasNonOptionArgs()
        /// for more details.
        ///
        static bool rootHasNonOptionArgs() { return root_has_nonopt_args_; }
        ///
        /// @brief Called by addOptions() to detect the null delimiting option
        /// (an entry with zero in all fields).
        /// @details See
        /// http://www.gnu.org/software/libc/manual/html_node/Argp-Option-Vectors.html#Argp-Option-Vectors
        /// for more details.
        /// Note that the header \b argp.h declares a similar function \b __option_is_end().
        /// We cannot use it, however, because most of the time a linker error occurs
        /// if argp is a system library.
        /// @param opt : See the definition of struct \c \b argp_option in the
        /// header \c \b argp.h for more details.
        /// @return Returns true if all fields of \c \b opt are NULL or 0.
        ///
        static bool isOptionEnd (const argp_option *opt);
        ///
        /// @brief See documentation for
        /// ArgppBase::argppError()
        ///
        static void argppError (const argp_state *state
                             , std::string const &errmsg);
        ///
        /// @brief See documentation for
        /// ArgppBase::argppFailure()
        ///
        static void argppFailure(const argp_state *state
                               , int status
                               , int errnum
                               , std::string const &errmsg);
        ///
        /// @brief See documentation for
        /// ArgppBase::argppUsage()
        ///
        static void argppUsage (const argp_state *state);
        ///
        /// @brief See documentation for
        /// ArgppBase::argppStateHelp()
        ///
        static void argppStateHelp(const argp_state *state
                                 , FILE *stream
                                 , unsigned flags);
        ///
        /// @brief See documentation for
        /// ArgppBase::clearAllParsers()
        ///
        static void clearAllParsers();
        ///
        /// @brief See documentation for ArgppBase::flagOk(unsigned &val, argpp_parse_flag  chk).
        ///
        static bool flagOk(unsigned &val, argpp_parse_flag  chk);
        ///
        /// @brief See documentation for ArgppBase::flagOk(unsigned &val, argpp_option_flag  chk).
        ///
        static bool flagOk(unsigned &val, argpp_option_flag chk);
        ///
        /// @brief See documentation for ArgppBase::getRootInstance().
        ///
        static ArgppBase* getRootInstance();
        ///
        /// @brief See documentation for ArgppBase::getArgvIndexAfterArgpParse().
        ///
        static int getArgvIndexAfterArgpParse();
        ///
        /// @brief This is the function passed to \b argp in the root instance's \b argp_ struct.
        /// @details See documentation for ArgppBase::installHelpFilter().
        ///
        static char *  helpFilter(int key, const char *text, void *input);
        ///
        /// @brief See documentation for ArgppBase::flagOk().
        ///
        static bool internalFlagCheck(unsigned &val, unsigned chk);
        ///
        /// @brief See documentation for ArgppBase::installArgppRootInstance().
        ///
        static void installArgppRootInstance (ArgppBase  *pInstance);
        ///
        /// @brief See documentation for ArgppBase::installBugAddress().
        ///
        static void installBugAddress(std::string const &ba);
        ///
        /// @brief See documentation for ArgppBase::installProgVersion().
        ///
        static void installProgVersion(std::string const &pvr);
        ///
        /// @brief See documentation for ArgppBase::installVersionHook().
        ///
        static void installVersionHook(VersionFunc f);
        ///
        /// @brief See documentation for ArgppBase::isParentValid().
        ///
        static bool isParentValid(ArgppBase  *parent);
        ///
        /// @brief This is the only parser function used in argp++ which communicates with \b argp
        /// as a callback function.
        /// @details Normally, each parser and child parser supply a pointer to their own parser functions
        /// which are called by \c \b argp parsing code in turn. However, we use the child_inputs[] vector
        /// to store the "this" pointers to the ArgppBase-derived children class instances. This eliminates
        /// the necessity of defining multiple parser functions. Each child class merely reimplements the
        /// virtual functions of the base class which they need.<br><br>
        /// Please refer to the code and comments in the argppbaseimpl.cpp file for more details.
        /// @param key   : Please refer to http://www.gnu.org/software/libc/manual/html_node/Argp-Parser-Functions.html#Argp-Parser-Functions for further details.
        /// @param arg   : Please refer to http://www.gnu.org/software/libc/manual/html_node/Argp-Parser-Functions.html#Argp-Parser-Functions for further details.
        /// @param state : Please refer to http://www.gnu.org/software/libc/manual/html_node/Argp-Parser-Functions.html#Argp-Parser-Functions for further details.
        /// @return Please refer to http://www.gnu.org/software/libc/manual/html_node/Argp-Parser-Functions.html#Argp-Parser-Functions for further details.
        ///
        static error_t parserRouter(int key, char *arg, argp_state *state);
        ///
        /// @brief See documentation for ArgppBase::parse()
        ///
        static bool parse();
        ///
        /// @brief See documentation for ArgppBase::setArgpParseFlags()
        ///
        static bool setArgpParseFlags(unsigned int);
        ///
        /// @brief See documentation for ArgppBase::getArgpParserFlags()
        ///
        static unsigned int getArgpParserFlags();
        ///
        /// @brief See documentation for ArgppBase::setDomain()
        ///
        static void setDomain(const std::string &dom);
        ///
        /// @brief See documentation for ArgppBase::getDomain()
        ///
        static const std::string &getDomain  ();
        ///
        /// @brief See documentation for ArgppBase::getArgc()
        ///
        static int getArgc() { return argc_; }
        ///
        /// @brief See documentation for ArgppBase::getArgv()
        ///
        static char** getArgv() { return argv_; }

        //----------------------------------------------------------
        // Non-static member functions:
        //----------------------------------------------------------
        ///
        /// @brief See documentation for ArgppBase::addChild()
        ///
        void addChild(ArgppBase* pChild);
        ///
        /// @brief See documentation for ArgppBase::addOption(const ArgppOption &opt)
        ///
        bool addOption(const ArgppOption &opt);
        ///
        /// @brief Overloaded version. See documentation for ArgppBase::addOption(const argp_option &ao)
        ///
        bool addOption(const argp_option &ao);
        ///
        /// @brief See documentation for ArgppBase::addOptions()
        ///
        bool addOptions(const argp_option *ao, size_t len);
        ///
        /// @brief See documentation for ArgppBase::addParsedOption()
        ///
        void addParsedOption(ParsedOption const &opt);
        ///
        /// @brief Overload taking just the key and an optional argument.
        ///
        void addParsedOption(int key, const std::string & ln = std::string(), const std::string & arg = std::string());
        ///
        /// @brief See documentation for ArgppBase::addNonOptionArg()
        ///
        void addNonOptionArg(const std::string &arg);
        ///
        /// @brief Returns the ArgppOption corresponding to `key` in `opt`.
        /// @details This function is useful within code which has no direct access to anything
        /// but the `key`, but `addParsedOption` needs to be called.
        /// @param key : integer or printable character corresponding to the short option key
        /// @param opt : struct ArgppOption which contains the long name and any other parameters
        /// @return    : returns `true` if the option was found, otherwise `false`
        ///
        bool findOption(int key, ArgppOption &opt) const;
        ///
        /// @brief See documentation of argpp::ArgppBase::setChildFlags() for details.
        ///
        unsigned int childFlags() const { return static_cast<unsigned int>(child_argp_.flags); }
        ///
        /// @brief See documentation for ArgppBase::keyArgImpl()
        ///
        error_t implKeyArg(const char* arg, argp_state *state);
        ///
        /// @brief See documentation for ArgppBase::genericParserImpl()
        ///
        error_t genericParserImpl(int key, const char *arg, argp_state *state);
        ///
        /// @brief See documentation for ArgppBase::getChildren()
        ///
        ArgppChildren & getChildren();
        ///
        /// @brief See documentation for ArgppBase::getChildren() const
        ///
        const ArgppChildren & getChildren() const;
        ///
        /// @brief See documentation for ArgppBase::getParent()
        ///
        ArgppBase * getParent();
        ///
        /// @brief See documentation for ArgppBase::getParent() const
        ///
        const ArgppBase * getParent() const;
        ///
        /// @brief See documentation for ArgppBase::getParsedOptions()
        ///
        const ParsedOptions &getParsedOptions() const;
        ///
        /// @brief See documentation for ArgppBase::getNonOptionArgs()
        ///
        OtherArgs const & getNonOptionArgs() const;
        ///
        /// @brief See documentation for ArgppBase::installHelpFilter()
        ///
        void installHelpFilter();
        ///
        /// @brief See documentation for ArgppBase::isChildOfThisParent()
        ///
        bool isChildOfThisParent(ArgppBase* pChild) const;
        ///
        /// @brief See documentation for ArgppBase::isGrandChild()
        ///
        bool isGrandChild(ArgppBase* pChild) const;
        ///
        /// @brief This is called by parse() before argp_parse() is called.
        /// @details It calls setupOptions() and setupChildren(), and calls
        /// prepareParserVecs() on all of its children.
        ///
        void prepareParserVecs();
        ///
        /// @brief Removes the child from the parent's vector of children if it is found.
        /// @param pChild : See documentation for ArgppBase::addChild().
        ///
        void removeChild(ArgppBase *pChild);
        ///
        /// @brief See documentation for ArgppBase::setChildFlags().
        ///
        bool setChildFlags(unsigned int flags);
        ///
        /// @brief See documentation for ArgppBase::setChildHeader().
        ///
        void setChildHeader(const std::string &header, bool use_empty_cstring);
        ///
        /// @brief See documentation for ArgppBase::setDocMessage().
        ///
        void setDocMessage(const std::string &msg);
        ///
        /// @brief See documentation for ArgppBase::setGroup().
        ///
        void setGroup(int group);
        ///
        /// @brief See documentation for ArgppBase::setPreDocMessage().
        ///
        void setPreDocMessage(const std::string &msg);
        ///
        /// @brief See documentation for ArgppBase::setPostDocMessage().
        ///
        void setPostDocMessage(const std::string &msg);
        ///
        /// @brief Helper function to keep pre- and post-doc messages in synch.
        ///
        void syncDocMsg();
        ///
        /// @brief See documentation for ArgppBase::setUsageMessage().
        ///
        void setUsageMessage(const std::string &msg);
        ///
        /// @brief Called by prepareParserVecs().
        /// @details Each child sets up the vectors of \c \b argp_option and \c \b argp_child
        /// and fills in the appropriate members of the main_argp_ struct so that the parent
        /// can fill in its main_argp_ before parse() is called.
        ///
        void setupChildren();
        ///
        /// @brief Called by prepareParserVecs().
        /// @details Each child sets up the vectors of \c \b argp_option and \c \b argp_child
        /// and fills in the appropriate members of the main_argp_ struct so that the parent
        /// can fill in its main_argp_ before parse() is called.
        ///
        void setupOptions();
        ///
        /// @brief See documentation of ArgppBase::supportedOption() for details.
        ///
        int  supportedOption(int key, const char* arg, std::string* long_name = NULL);

        //---------------------------------------------------------
        // non-static data members:
        //---------------------------------------------------------
        ///
        /// @brief Indicates whether or not the HelpFilter callback function has been installed.
        /// @details Queried by the prepareParserVecs() funtion when the main_argp_ struct
        /// is filled in. See documentation for ArgppBase::installHelpFilter() for more details.
        ///
        bool installed_help_filter_;
        ///
        /// @brief Stores a pointer to the ArgppBase instance of which the current ArgppBaseImpl
        /// object is contained in its pimpl_ member.
        ///
        ArgppBase *holder_;
        ///
        /// @brief Stores a pointer to the ArgppBase instance which is the parent of the current
        /// holder_ member.
        ///
        ArgppBase *holders_parent_;
        ///
        /// @brief The top-level argp struct for this instance.
        /// @details The argp structs of the children of holder_ will be formed into an array
        /// before argp_parse() is called. A pointer to the first element of that array
        /// will be stored in the children member of this struct. See
        /// http://www.gnu.org/software/libc/manual/html_node/Argp-Parsers.html#Argp-Parsers
        /// for further details about the \c \b argp struct.
        ///
        argp main_argp_;
        ///
        /// @brief This used only if the ArgppBase object stored in holder_ is not the root
        /// instance.
        /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Children.html#Argp-Children
        /// for further details about the \c \b argp_child struct.
        ///
        argp_child child_argp_;
        ///
        /// @brief A std::vector of \c \b argp_child structs.
        /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Children.html#Argp-Children
        /// for further details about the \c \b argp_child struct.
        ///
        Argp_Child_Vec children_vec_;
        ///
        /// @brief A std::vector of \c \b argp_option structs.
        /// @details See http://www.gnu.org/software/libc/manual/html_node/Argp-Option-Vectors.html#Argp-Option-Vectors
        /// for further details about the \c \b argp_option struct.
        ///
        Argp_Option_Vec option_vec_;
        ///
        /// @brief If argp stops parsing and there are still non-option arguments left in argv,
        /// this member will contain the index to the first such argument in argv.
        ///
        int arg_idx_;
        ///
        /// @brief See documentation of ArgppBase::setUsageMessage() for further details.
        ///
        std::string usage_msg_;
        ///
        /// @brief See documentation of ArgppBase::setDocMessage() for further details.
        ///
        std::string doc_msg_;
        ///
        /// @brief See documentation of ArgppBase::setPreDocMessage() for further details.
        ///
        std::string pre_doc_msg_;
        ///
        /// @brief See documentation of ArgppBase::setPostDocMessage() for further details.
        ///
        std::string post_doc_msg_;
        ///
        /// @brief See documentation of ArgppBase::setChildHeader() for further details.
        ///
        std::string child_header_;
        ///
        /// @brief See documentation of ArgppBase::setChildHeader() for further details.
        ///
        bool child_header_is_empty_cstring_;
        ///
        /// @brief See documentation of ArgppBase::setGroup() for further details.
        ///
        int child_group_;
        ///
        /// @brief A vector of ArgppBase pointers.
        /// @details See documentation of ArgppBase::addChild() for further details.
        ///
        ArgppChildren children_;
        ///
        /// @brief A std::map which correlates the vector of children with the vector
        /// of argp_child structs after prepareParserVecs() has been called.
        /// @details See documentation of parserRouter() as well as comments in the
        /// source code for more details.
        ///
        MapOfChildren map_of_children_;
        ///
        /// @brief A std::vector of ArgppOptions.
        /// @details These are copied into a vector of \c \b argp_option structs in
        /// the prepareParserVecs() function before calling argp_parse().
        ///
        ArgppOptions options_;
        ///
        /// @brief A std::vector of ParsedOption objects.
        /// @details After parsing has finished, derived implementations can fetch the
        /// successfully parsed options by calling getParsedOptions().
        ///
        ParsedOptions parsed_options_;
        ///
        /// @brief A std::vector of strings containing the non-option arguments entered
        /// on the command line.
        /// @details After parsing has finished, derived implementations can fetch the
        /// successfully parsed non-option arguments by calling getNonOptionArgs().
        ///
        OtherArgs other_args_;
    };

  } // namespace impl
} // namespace argpp


#endif // ARGPPBASEIMPL_H
