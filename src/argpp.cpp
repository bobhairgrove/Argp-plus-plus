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

//-----------------------------------------------------
// This header includes argpp.h, so we don't need to
// include it again:
//-----------------------------------------------------
#include "argppbaseimpl.hpp"

namespace argpp {

  static const size_t LIB_BUILD_VERSION     = ARGPP_VERSION;
  static const char * LIB_STR_BUILD_VERSION = ARGPP_STR_VERSION;

  //--------------------------------------------------------------------
  // Global functions:
  //--------------------------------------------------------------------
  size_t library_build_version()
  {
    return LIB_BUILD_VERSION;
  }
  //--------------------------------------------------------------------
  const char * str_library_build_version()
  {
    return LIB_STR_BUILD_VERSION;
  }

  //--------------------------------------------------------------------
  // Static member functions:
  //--------------------------------------------------------------------
  bool ArgppBase::parse()
  {
    return impl::ArgppBaseImpl::parse();
  }
  //--------------------------------------------------------------------
  error_t ArgppBase::getErrorCodeFromArgp()
  {
    return impl::ArgppBaseImpl::getErrorCodeFromArgp();
  }
  //--------------------------------------------------------------------
  error_t ArgppBase::getRetvalErrorFromArgpParse()
  {
    return impl::ArgppBaseImpl::getRetvalErrorFromArgpParse();
  }
  //--------------------------------------------------------------------
  bool ArgppBase::setArgpParseFlags(unsigned int flags)
  {
    return impl::ArgppBaseImpl::setArgpParseFlags(flags);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::isParentValid(ArgppBase *parent)
  {
    return impl::ArgppBaseImpl::isParentValid(parent);
  }
  //--------------------------------------------------------------------
  void ArgppBase::installArgppRootInstance(ArgppBase *pInstance)
  {
    impl::ArgppBaseImpl::installArgppRootInstance(pInstance);
  }
  //--------------------------------------------------------------------
  void ArgppBase::installBugAddress(const std::string &ba)
  {
    impl::ArgppBaseImpl::installBugAddress(ba);
  }
  //--------------------------------------------------------------------
  void ArgppBase::installProgVersion(const std::string &pvr)
  {
    impl::ArgppBaseImpl::installProgVersion(pvr);
  }
  //--------------------------------------------------------------------
  void ArgppBase::installVersionHook(VersionFunc f)
  {
    impl::ArgppBaseImpl::installVersionHook(f);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setDomain(const std::string &dom)
  {
    impl::ArgppBaseImpl::setDomain(dom);
  }
  //--------------------------------------------------------------------
  void ArgppBase::argppError(const argp_state *state, const std::string &errmsg)
  {
    impl::ArgppBaseImpl::argppError(state, errmsg);
  }
  //--------------------------------------------------------------------
  void ArgppBase::argppFailure(const argp_state *state, int status, int errnum, const std::string &errmsg)
  {
    impl::ArgppBaseImpl::argppFailure(state, status, errnum, errmsg);
  }
  //--------------------------------------------------------------------
  void ArgppBase::argppUsage(const argp_state *state)
  {
    impl::ArgppBaseImpl::argppUsage(state);
  }
  //--------------------------------------------------------------------
  void ArgppBase::argppStateHelp(const argp_state *state, FILE *stream, unsigned flags)
  {
    impl::ArgppBaseImpl::argppStateHelp(state, stream, flags);
  }
  //--------------------------------------------------------------------
  ArgppBase *ArgppBase::getRootInstance()
  {
    return impl::ArgppBaseImpl::getRootInstance();
  }
  //--------------------------------------------------------------------
  void ArgppBase::clearAllParsers()
  {
    impl::ArgppBaseImpl::clearAllParsers();
  }
  //--------------------------------------------------------------------
  // Constructor:
  //--------------------------------------------------------------------
  ArgppBase::ArgppBase(int        argc
                       , char     **argv
                       , ArgppBase *parent)
    : pimpl_(new impl::ArgppBaseImpl(argc, argv, parent, this))
  {
    if (parent != NULL) {
      parent->addChild(this);
    }
  }

  //--------------------------------------------------------------------
  // Destructor:
  //--------------------------------------------------------------------
  ArgppBase::~ArgppBase()
  {
    if (pimpl_) delete pimpl_;
  }

  //--------------------------------------------------------------------
  // Other member functions:
  //--------------------------------------------------------------------
  // virtual: see ArgppBaseImpl::implKeyArg() for the default implementation
  error_t ArgppBase::keyArgImpl(const char *arg, argp_state *state)
  {
    return pimpl_->implKeyArg(arg, state);
  }
  //--------------------------------------------------------------------
  void ArgppBase::installHelpFilter()
  {
    pimpl_->installHelpFilter();
  }
  //--------------------------------------------------------------------
  void ArgppBase::addChild(ArgppBase *pChild)
  {
    pimpl_->addChild(pChild);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::addOption(ArgppOption const &opt)
  {
    return pimpl_->addOption(opt);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::addOption(const argp_option &opt)
  {
    return pimpl_->addOption(opt);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::addOptions(const argp_option *ao, size_t len)
  {
    return pimpl_->addOptions(ao, len);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::addOptions(ArgppOptions const &opts)
  {
    bool retval = true;
    for (Opt_It_const
         cit  = opts.begin();
         cit != opts.end();
         ++ cit) {
      retval = pimpl_->addOption(*cit);
      if (!retval) break;
    }
    return retval;
  }
  //--------------------------------------------------------------------
  bool ArgppBase::findOption(int key, ArgppOption &opt) const
  {
    return pimpl_->findOption(key, opt);
  }
  //--------------------------------------------------------------------
  unsigned int ArgppBase::childFlags() const
  {
    return pimpl_->childFlags();
  }
  //--------------------------------------------------------------------
  void ArgppBase::setUsageMessage(const std::string &msg)
  {
    pimpl_->setUsageMessage(msg);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setDocMessage(const std::string &msg)
  {
    pimpl_->setDocMessage(msg);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::setChildFlags(unsigned int flags)
  {
    return pimpl_->setChildFlags(flags);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setPreDocMessage(const std::string &msg)
  {
    pimpl_->setPreDocMessage(msg);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setPostDocMessage(const std::string &msg)
  {
    pimpl_->setPostDocMessage(msg);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setChildHeader(const std::string &header, bool use_empty_cstring)
  {
    pimpl_->setChildHeader(header, use_empty_cstring);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setGroup(int group)
  {
    pimpl_->setGroup(group);
  }
  //--------------------------------------------------------------------
  const ParsedOptions &ArgppBase::getParsedOptions() const
  {
    return pimpl_->getParsedOptions();
  }
  //--------------------------------------------------------------------
  const OtherArgs &ArgppBase::getNonOptionArgs() const
  {
    return pimpl_->getNonOptionArgs();
  }
  //--------------------------------------------------------------------
  ArgppBase *ArgppBase::getParent()
  {
    return pimpl_->getParent();
  }
  //--------------------------------------------------------------------
  const ArgppBase *ArgppBase::getParent() const
  {
    return pimpl_->getParent();
  }
  //--------------------------------------------------------------------
  ArgppChildren &ArgppBase::getChildren()
  {
    return pimpl_->getChildren();
  }
  //--------------------------------------------------------------------
  const ArgppChildren &ArgppBase::getChildren() const
  {
    return pimpl_->getChildren();
  }
  //--------------------------------------------------------------------
  bool ArgppBase::flagOk(unsigned &val, argpp_parse_flag chk)
  {
    return impl::ArgppBaseImpl::flagOk(val, chk);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::flagOk(unsigned &val, argpp_option_flag chk)
  {
    return impl::ArgppBaseImpl::flagOk(val, chk);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::wasHelpEntered()
  {
    return impl::ArgppBaseImpl::wasHelpEntered();
  }
  //--------------------------------------------------------------------
  void ArgppBase::addParsedOption(const ParsedOption &opt)
  {
    pimpl_->addParsedOption(opt);
  }
  //--------------------------------------------------------------------
  void ArgppBase::addParsedOption(int key, const std::string &ln, const std::string &arg)
  {
    pimpl_->addParsedOption(key, ln, arg);
  }
  //--------------------------------------------------------------------
  void ArgppBase::addNonOptionArg(const std::string &arg)
  {
    pimpl_->addNonOptionArg(arg);
  }
  //--------------------------------------------------------------------
  error_t ArgppBase::genericParserImpl(int key, const char *arg, argp_state *state)
  {
    return pimpl_->genericParserImpl(key, arg, state);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setRootHasParsedOptions(bool set_root)
  {
    impl::ArgppBaseImpl::setRootHasParsedOptions(set_root);
  }
  //--------------------------------------------------------------------
  void ArgppBase::setRootHasNonOptionArgs(bool set_root)
  {
    impl::ArgppBaseImpl::setRootHasNonOptionArgs(set_root);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::rootHasParsedOptions()
  {
    return impl::ArgppBaseImpl::rootHasParsedOptions();
  }
  //--------------------------------------------------------------------
  bool ArgppBase::rootHasNonOptionArgs()
  {
    return impl::ArgppBaseImpl::rootHasNonOptionArgs();
  }
  //--------------------------------------------------------------------
  bool ArgppBase::isChildOfThisParent(ArgppBase *pChild) const
  {
    return pimpl_->isChildOfThisParent(pChild);
  }
  //--------------------------------------------------------------------
  bool ArgppBase::isGrandChild(ArgppBase *pChild) const
  {
    return pimpl_->isGrandChild(pChild);
  }
  //--------------------------------------------------------------------
  int ArgppBase::supportedOption(int key, const char *arg, std::string *long_name)
  {
    return pimpl_->supportedOption(key, arg, long_name);
  }
  //--------------------------------------------------------------------
  int ArgppBase::getArgc()
  {
    return impl::ArgppBaseImpl::getArgc();
  }
  //--------------------------------------------------------------------
  char **ArgppBase::getArgv()
  {
    return impl::ArgppBaseImpl::getArgv();
  }
  //--------------------------------------------------------------------

} // namespace argpp
