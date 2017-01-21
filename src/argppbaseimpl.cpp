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
#include <algorithm>
#include "argppbaseimpl.hpp"

#ifdef _WIN32
#  define ARGPP_DIR_SEPARATOR '\\'
#else
#  define ARGPP_DIR_SEPARATOR '/'
#endif

namespace argpp {
  namespace impl {

#ifdef ARGPP_HAVE_UNIQUE_PTR
    typedef std::unique_ptr<argpp::ArgppBase> DeleteHelper;
#else
///
/// @class DeleteHelper
/// @brief The DeleteHelper class facilitates destruction of the singleton
/// root instance (an ArgppBase pointer).
/// @details Since it is defined as a static variable in the argppbaseimpl.cpp file,
/// its destructor calls the destructor of the root instance which destroys
/// all of its children recursively (via the destructor of the pimpl_ member
/// it contains).
///
class DeleteHelper {
  public:
    ///
    /// @brief The defaul constructor sets the pointer member initially to NULL.
    ///
    DeleteHelper() : p_(NULL) {}
    explicit DeleteHelper(ArgppBase *p) : p_(p) {}

    ~DeleteHelper() { if (p_) delete p_; }

    ArgppBase *get() { return p_; }

    const ArgppBase *get() const { return p_; }

    ArgppBase * operator->() { return p_; }

    const ArgppBase * operator->() const { return p_; }

    void reset(ArgppBase *p = NULL) {
      if (p_) {
        delete p_;
      }
      p_ = p;
    }

    explicit operator bool() const { return p_ != NULL; }
  private:
    ArgppBase *p_;
    //---------------------------
    // These are not implemented:
    //---------------------------
    ///
    /// @brief The copy constructor is not implemented.
    ///
    DeleteHelper(const DeleteHelper& other);
    ///
    /// @brief The copy assignment operator is not implemented.
    ///
    DeleteHelper& operator=(const DeleteHelper& other);
};
#endif

static DeleteHelper gRootInstance;

//--------------------------------------------------------------------
// For delimiting the argp vectors:
//--------------------------------------------------------------------
static const char *       NULL_CSTRING     = "";
static const argp_child   NULL_ARGP_CHILD  = argp_child();
static const argp_option  NULL_ARGP_OPTION = argp_option();

static std::string  gBugAddress;
static std::string  gProgramVersion;

//--------------------------------------------------------------------
// Static data members:
//--------------------------------------------------------------------
int           ArgppBaseImpl::argc_                    = 0;
char **       ArgppBaseImpl::argv_                    = NULL;
unsigned int  ArgppBaseImpl::argp_flags_              = 0;
error_t       ArgppBaseImpl::error_code_from_argp_    = 0;
error_t       ArgppBaseImpl::retval_error_            = 0;
bool          ArgppBaseImpl::parsed_                  = false;
bool          ArgppBaseImpl::argpp_should_exit_       = false;
bool          ArgppBaseImpl::root_has_parsed_options_ = true;
bool          ArgppBaseImpl::root_has_nonopt_args_    = true;
std::string   ArgppBaseImpl::argp_domain_;
bool          ArgppBaseImpl::help_called_             = false;
size_t        ArgppBaseImpl::help_max_blk_size_       = 16384;
const char   *ArgppBaseImpl::help_max_env_name_       = "ARGPP_MAX_HELP_TEXT";
//--------------------------------------------------------------------
// Static member functions:
//--------------------------------------------------------------------
error_t ArgppBaseImpl::parserRouter(int key, char *arg, argp_state *state) {

  static error_t retval = ARGPP_UNKNOWN;
  //-----------------------------------------------------------------
  // Put everything in a separate scope so that locals are cleaned up
  // if we have to call exit():
  //-----------------------------------------------------------------
  {
    //-------------------------------------------------------------------------
    // The argp_parse() function passes on the "void* input" argument in the
    // state->input member. Our function casts the state->input member to an
    // ArgppBase* since we passed it the "this" pointer in the initial call
    // to argp_parse() as the "void* input" argument.
    //-------------------------------------------------------------------------
    if (state && state->input) {

      ArgppBase     *pThis = static_cast<ArgppBase*>(state->input);
      ArgppBaseImpl *p = pThis->pimpl_;

      switch (key) {
        case ARGP_KEY_INIT:
          //-------------------------------------------------------------------------
          // Argp code always calls the parser function with the key ARGP_KEY_INIT
          // as the very first call to the callback routine.
          //
          // What needs to be done first here is to set up the child_inputs[] vector
          // for each immediate child of the object whose "this" pointer is passed in
          // state->input. Afterwards, the argp code calls the parserRouter for
          // each child in succession with the corresponding child_input value
          // copied into state->input. If the children have any children of their
          // own, they do the same by setting up their child_inputs when parserRouter
          // is called with key == ARGP_KEY_INIT.
          //
          // We depend on the sorting order of std::map, which uses the address of
          // the POD argp_child vector's element as the key, as well as the C++
          // standard's guarantee that elements of a vector are stored in contiguous
          // memory. The map's value element (member "second") contains the "this"
          // pointers to the corresponding children.
          //-------------------------------------------------------------------------
        {
          size_t len = p->map_of_children_.size();
          MapOfChildren::const_iterator cit = p->map_of_children_.begin();
          for(size_t i=0; i<len; ++i, ++cit)
          {
            state->child_inputs[i] = static_cast<void*>(cit->second);
          }
        }
          //-----------------------------------------
          // Let the derived class do some additional
          // initialization work here, if any:
          //-----------------------------------------
          retval = pThis->initImpl(state);
          break;
        case ARGP_KEY_ERROR:
          retval = pThis->errorImpl(state);
          break;
        case ARGP_KEY_FINI:
          retval = pThis->finiImpl(state);
          break;
        case ARGP_KEY_ARG:
          retval = pThis->keyArgImpl(arg, state);
          break;
        case ARGP_KEY_ARGS:
          retval = pThis->keyArgsImpl(state);
          break;
        case ARGP_KEY_NO_ARGS:
          retval = pThis->keyNoArgsImpl(state);
          break;
        case ARGP_KEY_SUCCESS:
          retval = pThis->keySuccessImpl(state);
          break;
        case ARGP_KEY_END:
          retval = pThis->keyEndImpl(state);
          break;
        default:
          retval = pThis->parserImpl(key, arg, state);
          break;
      }
    }
  }
  //------------------------------------------------------
  // This is where we can safely call exit() if necessary:
  //------------------------------------------------------
  if (argpp_should_exit_) {
    clearAllParsers();
    exit(error_code_from_argp_);
  }
  return retval;
}

//--------------------------------------------------------------------
char *ArgppBaseImpl::helpFilter(int key, const char *text, void *input)
{
  //-------------------------------------------------------
  // If the derived class does not return a different string,
  // we have to return the string passed in the "text"
  // argument, but first cast away its const qualifier:
  //-------------------------------------------------------
  char *retval = const_cast<char*>(text);

  //-------------------------------------------------------
  // These are used in the code which calls malloc()
  // later on (that is, if the derived implementation of
  // helpFilterImpl() returns a string we have to copy):
  //-------------------------------------------------------
  static const size_t DEFAULT_MEM_BLK_SIZE = 8;
  static const size_t MAX_BLK_SIZE = ArgppBaseImpl::help_max_blk_size_;

  if (input) {
    ArgppBase * pThis = static_cast<ArgppBase*>(input);
    std::string newtxt = pThis->helpFilterImpl(key, text);

    if (!newtxt.empty() && (!text || newtxt.compare(text) != 0)) {
      //-------------------------------
      // argp wants a malloc'ed string:
      //-------------------------------
      size_t memsize = DEFAULT_MEM_BLK_SIZE;
      size_t len = newtxt.size();

      if (len < MAX_BLK_SIZE) {
        // This allocates slightly more memory than
        // the actual required length plus the
        // terminating '\0' char:
        ++len;
        while (memsize < len) {
          memsize += DEFAULT_MEM_BLK_SIZE;
        }
        --len;
        void *pmem = malloc(memsize);
        if (pmem) {
          char *pTarget = static_cast<char*>(pmem);
          const char *pBegin = newtxt.c_str();
          const char *pEnd   = pBegin + len;
          std::copy(pBegin, pEnd, pTarget);
          pTarget[len] = '\0';
          retval = pTarget;
        }
      }
    }
  }
  return retval;
}

//-----------------------------------------------------------------------------
// Constructor:
//-----------------------------------------------------------------------------
ArgppBaseImpl::ArgppBaseImpl(
    int argc
    , char **argv
    , ArgppBase *parent
    , ArgppBase *holder)
  : installed_help_filter_         (false)
  , holder_                        (holder)
  , holders_parent_                (parent)
  , main_argp_                     ()
  , child_argp_                    ()
  , children_vec_                  ()
  , option_vec_                    ()
  , arg_idx_                       (0)
  , usage_msg_                     ()
  , doc_msg_                       ()
  , pre_doc_msg_                   ()
  , post_doc_msg_                  ()
  , child_header_                  ()
  , child_header_is_empty_cstring_ (false)
  , child_group_                   (0)
  , children_                      ()
  , map_of_children_               ()
  , options_                       ()
  , parsed_options_                ()
  , other_args_                    ()
{
  if (!parent) {
    argc_ = argc;
    argv_ = argv;
  }
  //-----------------------------------------------
  // check the environment variable
  // "ARGPP_MAX_HELP_TEXT":
  //-----------------------------------------------
  char *pMaxHelpText = getenv(ArgppBaseImpl::help_max_env_name_);
  if (pMaxHelpText) {
    size_t maxlen = strtoul(pMaxHelpText, NULL, 10);
    if (maxlen) {
      ArgppBaseImpl::help_max_blk_size_ = maxlen;
    }
  }
  //-----------------------------------------------
  // Set "help_called_" if one of the special options
  // was entered:
  //-----------------------------------------------
  std::string arg;
  for (int i=1; i<argc; ++i) {
    arg = argv[i];
    if ((arg == "-?")
        || (arg == "--h")
        || (arg == "--he")
        || (arg == "--hel")
        || (arg == "--help")

        || ((arg == "-V")         && !gProgramVersion.empty())
        || ((arg == "--ve")       && !gProgramVersion.empty())
        || ((arg == "--ver")      && !gProgramVersion.empty())
        || ((arg == "--vers")     && !gProgramVersion.empty())
        || ((arg == "--versi")    && !gProgramVersion.empty())
        || ((arg == "--versio")   && !gProgramVersion.empty())
        || ((arg == "--version")  && !gProgramVersion.empty())

        || (arg == "--u")
        || (arg == "--us")
        || (arg == "--usa")
        || (arg == "--usag")
        || (arg == "--usage")) {
      help_called_ = true;
    }
  }
}

//-----------------------------------------------------------------------------
ArgppBaseImpl::~ArgppBaseImpl()
{
  if (!children_.empty()) {
    for (ArgppChildren::iterator
         it = children_.begin();
         it != children_.end();
         ++it) {
      // Eat any exceptions thrown in destructors
      // of derived classes...
      try {
        delete *it;
      } catch(...) {}
    }
  }
}
//-----------------------------------------------------------------------------
error_t ArgppBaseImpl::getRetvalErrorFromArgpParse()
{
  if (parsed_) {
    return retval_error_;
  }
  return ARGPP_ENODATA;
}
//-----------------------------------------------------------------------------
error_t ArgppBaseImpl::getErrorCodeFromArgp()
{
  return error_code_from_argp_;
}

//-----------------------------------------------------------------------------
// Static member functions:
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::isOptionEnd(const argp_option *opt)
{
  //----------------------------------------------------
  // There is __option_is_end(), declared in argp.h,
  // but since it is inlined (???) we cannot link to its
  // definition. That is why we have this one.
  //----------------------------------------------------
  return (opt->arg   == NULL)
      && (opt->doc   == NULL)
      && (opt->flags == 0   )
      && (opt->group == 0   )
      && (opt->key   == 0   )
      && (opt->name  == NULL);
}

//-----------------------------------------------------------------------------
void ArgppBaseImpl::argppError(const argp_state *state, const std::string &errmsg)
{
  FILE* stream = NULL;

  if (state) {
    unsigned int f = state->flags;
    std::string prg_name;
    std::string msg;

    if (state->err_stream) {
      stream = state->err_stream;
    } else {
      stream = stderr;
    }

    if (state->name) {
      prg_name = state->name;
    } else if (state->argv && state->argv[0]) {
      std::string n = state->argv[0];
      size_t pos = n.rfind(ARGPP_DIR_SEPARATOR);
      if (pos != std::string::npos) {
        prg_name = n.substr(pos+1);
      } else {
        prg_name = n;
      }
    }
    //--------------------------------------
    // strip off the './' due to program
    // invocation from the shell, if any:
    //--------------------------------------
    if (prg_name.substr(0,2) == "./") {
      prg_name = prg_name.substr(2);
    }
    msg = prg_name + ": " + errmsg + "\n";
    fputs(msg.c_str(), stream);

    //--------------------------------------------------------
    // Prevent argp_state_help() from calling exit() so as not
    // to leak memory in the errmsg argument to this function:
    //--------------------------------------------------------
    struct argp_state s = { state->root_argp
            , state->argc
            , state->argv
            , state->next
            , state->flags | ARGP_NO_EXIT
            , state->arg_num
            , state->quoted
            , state->input
            , state->child_inputs
            , state->hook
            , state->name
            , state->err_stream
            , state->out_stream
            , state->pstate};

    argp_state_help (&s, stream, ARGP_HELP_STD_ERR);

    error_code_from_argp_ = 1;

    if ((f & ARGP_NO_EXIT) == 0) {
      argpp_should_exit_ = true;
    }
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::argppFailure(const argp_state *state, int status, int errnum, const std::string &errmsg)
{
  //-------------------------------------------------
  // If we allow argp_failure() to call exit, we will
  // probably leak memory in the errmsg argument,
  // so we fake it here:
  //-------------------------------------------------
  if (state) {
    unsigned int f = state->flags;
    struct argp_state s = { state->root_argp
            , state->argc
            , state->argv
            , state->next
            , state->flags | ARGP_NO_EXIT
            , state->arg_num
            , state->quoted
            , state->input
            , state->child_inputs
            , state->hook
            , state->name
            , state->err_stream
            , state->out_stream
            , state->pstate};
    argp_failure(&s, status, errnum, "%s", errmsg.c_str());

    if ((f & ARGP_NO_EXIT) == 0) {
      argpp_should_exit_ = true;
    }
    error_code_from_argp_ = status;
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::argppUsage(const argp_state *state)
{
  if (state) {
    unsigned int f = state->flags;
    struct argp_state s = { state->root_argp
            , state->argc
            , state->argv
            , state->next
            , state->flags | ARGP_NO_EXIT
            , state->arg_num
            , state->quoted
            , state->input
            , state->child_inputs
            , state->hook
            , state->name
            , state->err_stream
            , state->out_stream
            , state->pstate };
    argp_state_help(&s, stderr, ARGP_HELP_STD_USAGE);
    if ((f & ARGP_NO_EXIT) == 0) {
      argpp_should_exit_ = true;
    }
    error_code_from_argp_ = argp_err_exit_status;
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::argppStateHelp(const argp_state *state, FILE *stream, unsigned flags)
{
  if (state) {
    unsigned int f = state->flags;
    struct argp_state s = { state->root_argp
            , state->argc
            , state->argv
            , state->next
            , state->flags | ARGP_NO_EXIT
            , state->arg_num
            , state->quoted
            , state->input
            , state->child_inputs
            , state->hook
            , state->name
            , state->err_stream
            , state->out_stream
            , state->pstate };
    argp_state_help(&s, stream, flags);
    if ((f & ARGP_NO_EXIT) == 0) {
      argpp_should_exit_ = true;
    }
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::clearAllParsers()
{
  gRootInstance.reset();
  parsed_ = false;
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::flagOk(unsigned &val, argpp_parse_flag chk)
{
  return internalFlagCheck(val, (unsigned)chk);
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::flagOk(unsigned &val, argpp_option_flag chk)
{
  // gnulib has this flag, but glibc doesn't:
#ifdef OPTION_NO_TRANS
  // unset the OPTION_NO_TRANS if OPTION_DOC is not set:
  if (val & opt_no_translate) {
    if (val & opt_is_doc == 0) {
      val &= (~opt_no_translate);
    }
  }
#endif
  return internalFlagCheck(val, (unsigned)chk);
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::internalFlagCheck(unsigned &val, unsigned chk)
{
  // NOTE: it is OK if val == 0 according to the argp
  // documentation.
  return (chk == (val | chk));
}
//-----------------------------------------------------------------------------
ArgppBase *ArgppBaseImpl::getRootInstance()
{
  return gRootInstance.get();
}
//-----------------------------------------------------------------------------
int ArgppBaseImpl::getArgvIndexAfterArgpParse()
{
  if (parsed_ && gRootInstance) {
    return gRootInstance->pimpl_->argp_flags_;
  }
  return 0;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::installArgppRootInstance(ArgppBase *pInstance)
{
  if (pInstance && !gRootInstance) {
    gRootInstance.reset(pInstance);
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::installBugAddress(const std::string &ba)
{
  gBugAddress = ba;

  if (ba.empty()) {
    argp_program_bug_address = NULL;
  } else {
    argp_program_bug_address = gBugAddress.c_str();
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::installProgVersion(const std::string &pvr)
{
  gProgramVersion = pvr;

  if (pvr.empty()) {
    argp_program_version = NULL;
  } else {
    argp_program_version = gProgramVersion.c_str();
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::installVersionHook(VersionFunc f)
{
  argp_program_version_hook = f;
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::isParentValid(ArgppBase *parent)
{
  bool retval = false;
  // NULL parent is valid, but only for the root instance:
  ArgppBase *root = ArgppBase::getRootInstance();
  if (root == parent) {
    retval = true;
  } else if (root && parent) {
    retval = root->isGrandChild(parent);
  }
  return retval;
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::parse()
{
  bool retval = false;
  ArgppBase* root = ArgppBase::getRootInstance();

  if (root && root->pimpl_ && !parsed_) {
    ArgppBaseImpl *p = root->pimpl_;
    p->prepareParserVecs();
    error_t e = argp_parse( &p->main_argp_
                            ,  argc_
                            ,  argv_
                            ,  p->argp_flags_
                            , &p->arg_idx_
                            ,  root);
    p->retval_error_ = e;
    retval = (e == ARGPP_SUCCESS);
    parsed_ = true;
  }
  return retval;
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::setArgpParseFlags(unsigned int flags)
{
  bool retval = false;
  argpp_parse_flag chk = fl_all_check;
  if (flagOk(flags, chk)) {
    argp_flags_ = flags;
    retval = true;
  }
  return retval;
}
//-----------------------------------------------------------------------------
unsigned int ArgppBaseImpl::getArgpParserFlags()
{
  return argp_flags_;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setDomain(const std::string &dom)
{
  argp_domain_ = dom;
}
//-----------------------------------------------------------------------------
const std::string & ArgppBaseImpl::getDomain()
{
  return argp_domain_;
}

//-----------------------------------------------------------------------------
// Non-static member functions:
//-----------------------------------------------------------------------------
void ArgppBaseImpl::addChild(ArgppBase *pChild)
{
  if (pChild && !isChildOfThisParent(pChild)) {
    ArgppBaseImpl *pc = pChild->pimpl_;
    if ( pc->holders_parent_
         && (pc->holders_parent_ != this->holder_)) {
      pc->holders_parent_->pimpl_->removeChild(pChild);
    }
    pc->holders_parent_ = this->holder_;
    children_.push_back(pChild);
  }
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::addOption(const ArgppOption &opt)
{
  bool retval = false;
  unsigned int flags = opt.flags_;
  argpp_option_flag chk = opt_all_check;
  if (flagOk(flags, chk)) {
    options_.push_back(opt);
    retval = true;
  }
  return retval;
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::addOption(const argp_option &ao)
{
  using std::string;

  ArgppOption opt;

  opt.arg_           = ao.arg ? string(ao.arg) : string();
  opt.doc_or_header_ = ao.doc ? string(ao.doc) : string();
  opt.flags_         = ao.flags;
  opt.group_         = ao.group;
  opt.key_           = ao.key;
  opt.long_name_     = ao.name ? string(ao.name) : string();

  return addOption(opt);
}

//-----------------------------------------------------------------------------
bool ArgppBaseImpl::addOptions(const argp_option *ao, size_t len)
{
  if (!ao) return false;

  bool retval = true;
  if (len) { // maybe not delimited...
    for (size_t i=0; i<len; ++i) {
      retval = addOption(ao[i]);
      if (!retval) break;
    }
  } else {
    size_t i = 0;
    while (!isOptionEnd(&ao[i])) {
      retval = addOption(ao[i]);
      if (!retval) break;
      ++i;
    }
  }
  return retval;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::addParsedOption(const ParsedOption &opt)
{
  if (root_has_parsed_options_) {
    ArgppBase * root = getRootInstance();
    if (root && root->pimpl_) {
      root->pimpl_->parsed_options_.push_back(opt);
    }
  } else {
    parsed_options_.push_back(opt);
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::addParsedOption(int key, const std::string &ln, const std::string &arg)
{
  ParsedOption opt(key,ln,arg);
  addParsedOption(opt);
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::addNonOptionArg(const std::string &arg)
{
  if (root_has_nonopt_args_) {
    ArgppBase *root = getRootInstance();
    if (root && root->pimpl_) {
      root->pimpl_->other_args_.push_back(arg);
    }
  } else {
    this->other_args_.push_back(arg);
  }
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::findOption(int key, ArgppOption &opt) const
{
  bool retval = false;
  ArgppOption o;
  o.key_ = key;

  Opt_It_const cit = std::find(options_.begin(), options_.end(), o);

  if (cit != options_.end()) {
    opt = *cit;
    retval = true;
  }
  return retval;
}
//-----------------------------------------------------------------------------
error_t ArgppBaseImpl::implKeyArg(const char *arg, argp_state *state)
{
  if (arg && state) {
    //---------------------------------------------
    // We copy the first arg to other_args_
    // and "steal" the rest:
    //---------------------------------------------
    addNonOptionArg(arg);
    for (int i=state->next; i<state->argc; ++i) {
      addNonOptionArg(state->argv[(size_t)i]);
    }
  }
  state->next = state->argc;
  return ARGPP_SUCCESS;
}
//-----------------------------------------------------------------------------
error_t ArgppBaseImpl::genericParserImpl(int key, const char *arg, argp_state * /* unused */)
{
  error_t retval = ARGPP_DONT_CARE;
  std::string ln;
  if (supportedOption(key,arg,&ln) == ARGPP_OPTION_OK) {
    addParsedOption(key,ln,arg ? std::string(arg) : std::string());
    retval = ARGPP_SUCCESS;
  }
  return retval;
}
//-----------------------------------------------------------------------------
ArgppChildren &ArgppBaseImpl::getChildren()
{
  return children_;
}
//-----------------------------------------------------------------------------
const ArgppChildren &ArgppBaseImpl::getChildren() const
{
  return children_;
}
//-----------------------------------------------------------------------------
ArgppBase *ArgppBaseImpl::getParent()
{
  return holders_parent_;
}
//-----------------------------------------------------------------------------
const ArgppBase *ArgppBaseImpl::getParent() const
{
  return holders_parent_;
}
//-----------------------------------------------------------------------------
const ParsedOptions &ArgppBaseImpl::getParsedOptions() const
{
  return parsed_options_;
}
//-----------------------------------------------------------------------------
const OtherArgs &ArgppBaseImpl::getNonOptionArgs() const
{
  return other_args_;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::installHelpFilter()
{
  main_argp_.help_filter = ArgppBaseImpl::helpFilter;
  installed_help_filter_ = true;
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::isChildOfThisParent(ArgppBase *pChild) const
{
  bool retval = false;
  if (pChild && !children_.empty()) {
    retval =
        (std::find(children_.begin(), children_.end(), pChild)
         != children_.end());
  }
  return retval;
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::isGrandChild(ArgppBase *pChild) const
{
  bool retval = false;
  if (!children_.empty()) {
    retval = isChildOfThisParent(pChild);
    if (!retval) {
      Child_It_const cit = children_.begin();
      while (cit != children_.end()) {
        retval = (*cit)->isGrandChild(pChild);
        if (retval) break;
        ++cit;
      }
    }
  }
  return retval;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::prepareParserVecs()
{
  setupOptions();
  setupChildren();
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::removeChild(ArgppBase *pChild)
{
  if ( pChild->getParent() == holder_
       && !children_.empty()) {
    ArgppChildren::iterator it
        = std::find(children_.begin()
                    , children_.end()
                    , pChild);
    if (it != children_.end()) {
      children_.erase(it);
    }
  }
}
//-----------------------------------------------------------------------------
bool ArgppBaseImpl::setChildFlags(unsigned int flags)
{
  bool retval = flagOk(flags, fl_all_check);
  // if there is no parent, then this isn't a child:
  if (retval && holders_parent_) {
    child_argp_.flags = flags;
  }
  return retval;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setChildHeader(const std::string &header, bool use_empty_cstring)
{
  child_header_ = header;
  child_header_is_empty_cstring_ = use_empty_cstring;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setDocMessage(const std::string &msg)
{
  doc_msg_ = msg;

  if (doc_msg_.empty()) {
    pre_doc_msg_.clear();
    post_doc_msg_.clear();
  } else {
    size_t pos = doc_msg_.find('\v');
    if ((pos > 0) && (pos != std::string::npos)) {
      pre_doc_msg_ = doc_msg_.substr(0, pos);
      post_doc_msg_ = doc_msg_.substr(pos+1);
    } else if (pos == 0) {
      pre_doc_msg_.clear();
      post_doc_msg_ = doc_msg_.substr(1);
    } else {
      post_doc_msg_.clear();
      pre_doc_msg_ = doc_msg_;
    }
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setGroup(int group)
{
  if (holders_parent_ != NULL) {
    child_argp_.group = group;
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setPreDocMessage(const std::string &msg)
{
  std::string txt = msg;

  size_t pos = msg.find('\v');
  if (pos != std::string::npos) {
    txt = msg.substr(0,pos);
  }

  pre_doc_msg_ = txt;
  syncDocMsg();
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setPostDocMessage(const std::string &msg)
{
  std::string txt = msg;

  size_t pos = msg.find('\v');
  if (pos != std::string::npos) {
    txt = msg.substr(pos+1);
  }

  post_doc_msg_ = txt;
  syncDocMsg();
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::syncDocMsg()
{
  if (!post_doc_msg_.empty()) {
    doc_msg_ = pre_doc_msg_ + "\v" + post_doc_msg_;
  } else {
    doc_msg_ = pre_doc_msg_;
  }
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setupChildren()
{
  //---------------------------------------------------
  // Each child should set up its options,
  // its own children and its main_argp_
  // member in that order.
  //
  // The parent sets up its vectors only after
  // the children are done with their work.
  //---------------------------------------------------
  size_t len_children = children_.size();

  //---------------------------------------------------
  // I know, it's paranoid, but...
  //---------------------------------------------------
  children_vec_.clear();

  for (size_t i=0; i<len_children; ++i) {

    children_[i]->pimpl_->prepareParserVecs();

    //-----------------------------------------------
    // Set up the vector of argp_child structs now:
    //-----------------------------------------------
    children_vec_.push_back(children_[i]->pimpl_->child_argp_);

    //-----------------------------------------------
    // Map the "this" pointers so that parserRouter()
    // can find the right objects later:
    //-----------------------------------------------
    argp_child* ac = &children_vec_[i];
    ArgppBase*  pc = children_[i];

    map_of_children_.insert(std::make_pair(ac, pc));
  }

  //---------------------------------------------------------
  // Make sure that the argp_child vector is NULL-terminated:
  //---------------------------------------------------------
  if (!children_vec_.empty()) {
    children_vec_.push_back(NULL_ARGP_CHILD);
  }

  //------------------------------------
  // Skip this step for the root parser:
  //------------------------------------
  if (holders_parent_) {
    child_argp_.argp = &main_argp_;
    //-----------------------------------------------
    // Use the flags from the parent if ours are 0,
    // unless root is our parent in which case we use
    // the static member argp_flags_:
    //-----------------------------------------------
    if (!child_argp_.flags) {
      if (!holders_parent_->pimpl_->holders_parent_) {
        child_argp_.flags = static_cast<int>(argp_flags_);
      } else {
        child_argp_.flags = holders_parent_->childFlags();
      }
    }
    child_argp_.header = child_header_.empty()
        ? (child_header_is_empty_cstring_ ? NULL_CSTRING : 0)
        : child_header_.c_str();
    // child_argp_.group ... is already set; if not, it is == 0 ...
  }

  // do the main_argp_ now:
  main_argp_.options     = option_vec_.empty()    ? NULL : &option_vec_[0];
  main_argp_.parser      = parserRouter;
  main_argp_.args_doc    = usage_msg_.empty()     ? NULL : usage_msg_.c_str();
  main_argp_.doc         = doc_msg_.empty()       ? NULL : doc_msg_.c_str();
  main_argp_.children    = children_vec_.empty()  ? NULL : &children_vec_[0];
  main_argp_.help_filter = installed_help_filter_ ? helpFilter : NULL;
  main_argp_.argp_domain = argp_domain_.empty()   ? NULL : argp_domain_.c_str();
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setupOptions()
{
  size_t len_opts = options_.size();
  argp_option opt = NULL_ARGP_OPTION;

  //---------------------------------------------------
  // I know, it's paranoid, but...
  //---------------------------------------------------
  option_vec_.clear();

  for (size_t i=0; i<len_opts; ++i) {
    opt.name  = options_[i].long_name_.empty() ? NULL : options_[i].long_name_.c_str();
    opt.key   = options_[i].key_;
    opt.arg   = options_[i].arg_.empty() ? NULL : options_[i].arg_.c_str();
    opt.flags = static_cast<int>(options_[i].flags_);
    opt.doc   = options_[i].doc_or_header_.empty() ? NULL : options_[i].doc_or_header_.c_str();
    opt.group = options_[i].group_;

    option_vec_.push_back(opt);
  }

  if (!option_vec_.empty()) {
    option_vec_.push_back(NULL_ARGP_OPTION);
  }
}
//-----------------------------------------------------------------------------
int ArgppBaseImpl::supportedOption(int key, const char *arg, std::string *long_name)
{
  int retval = ARGPP_OPTION_UNKNOWN;
  ArgppOption opt;
  opt.key_ = key;
  Opt_It_const cit = std::find(options_.begin(), options_.end(), opt);
  if (cit != options_.end()) {
    // check the flags:
    bool arg_required = !cit->arg_.empty()
        && !(cit->flags_ & OPTION_ARG_OPTIONAL);
    if (arg_required && !arg) {
      retval = ARGPP_OPTION_NEEDS_ARG;
    } else {
      retval = ARGPP_OPTION_OK;
      if (long_name) {
        *long_name = cit->long_name_;
      }
    }
  }
  return retval;
}
//-----------------------------------------------------------------------------
void ArgppBaseImpl::setUsageMessage(const std::string &msg)
{
  usage_msg_ = msg;
}
//-----------------------------------------------------------------------------

} // namespace impl
} // namespace argpp
