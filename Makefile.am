ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src
EXTRA_DIST = AUTHORS \
             ChangeLog \
             COPYING \
             COPYING.LESSER \
             INSTALL \
             NEWS \
             README.md \
             TODO \
             doc 

# =============================================================================
# This target is only built when "make examples" was entered:
# =============================================================================
.PHONY: examples

examples: all
	cd $(top_builddir)/examples/build && $(MAKE) $(AM_MAKEFLAGS) examples
	cd $(top_builddir)/examples && rm -rf .libs
	cd $(top_builddir)/examples && rm -rf .dirstamp
	cd $(top_builddir)/examples && rm -rf boilerplate/.deps
	cd $(top_builddir)/examples && rm -rf boilerplate/.libs
	cd $(top_builddir)/examples && rm -rf boilerplate/.dirstamp
	cd $(top_builddir)/examples && rm -rf boilerplate/*.lo
	cd $(top_builddir)/examples && rm -rf boilerplate/*.o
	cd $(top_builddir)/examples && rm -rf build/*.la
	cd $(top_builddir)/examples && rm -rf build/.libs
	cd $(top_builddir)/examples && rm -rf ex_argp_test/.deps
	cd $(top_builddir)/examples && rm -rf ex_argp_test/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_argp_test/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_0/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_0/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_0/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_1/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_1/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_1/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_2/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_2/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_2/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_3/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_3/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_3/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_4/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_4/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_4/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_5/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_5/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_5/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_6/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_6/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_6/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_7/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_7/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_7/*.o
	cd $(top_builddir)/examples && rm -rf ex_step_8/.deps
	cd $(top_builddir)/examples && rm -rf ex_step_8/.dirstamp
	cd $(top_builddir)/examples && rm -rf ex_step_8/*.o

# =============================================================================
# This target is built when "make install" is called if the user enabled writing 
# a pkg-conf (.pc) file by running './configure' with the option
# "--enable-pkgconfig":
# =============================================================================
install-data-hook:
if ARGPP_PKGCONFIG_COND
	@echo "======================================================================"
	@mkdir -p $(top_srcdir)/.tmp
	@rm -f $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@touch $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "# Argp++: Package information for pkg-config" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "prefix=$(prefix)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "exec_prefix=$(exec_prefix)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "bindir=$(bindir)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "libdir=$(libdir)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "datarootdir=$(datarootdir)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "datadir=$(datadir)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "sysconfdir=$(sysconfdir)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "includedir=$(includedir)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "package=$(PACKAGE)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "suffix=$(suffix)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "Name: $(ARGPP_PKGCONFIG_NAME)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "URL: $(PACKAGE_URL)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "Description: $(ARGPP_PKGCONFIG_DESCR)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "Version: $(PACKAGE_VERSION)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "Requires: $(PACKAGE_REQUIRES)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "Libs: -L$(libdir) $(LDFLAGS) $(ARGPP_PKGCONFIG_LIB)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@echo "Cflags: -I$(includedir) $(CPPFLAGS) $(CXXFLAGS)" >> $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE)
	@mv -f $(top_srcdir)/.tmp/$(ARGPP_PKGCONFIG_TMPFILE) -T $(libdir)/pkgconfig/$(ARGPP_PKGCONFIG_NAME).pc
	@rmdir $(top_srcdir)/.tmp
	@echo "Created file: '$(ARGPP_PKGCONFIG_NAME).pc' in directory '$(libdir)/pkgconfig/'"
	@echo "======================================================================"
else
	@echo "======================================================================"
	@echo "pkg-config: No .pc file was created."
	@echo "======================================================================"
endif
	@echo "IMPORTANT NOTE for Ubuntu/Debian users:"
	@echo "---------------------------------------"
	@echo "You will need to run the following command if you installed Argp++ to"
	@echo "its default location (/usr/local/...) before you can link it with"
	@echo "other programs:"
	@echo "    sudo ldconfig"
	@echo "This is because make runs ldconfig with the \"-n\" option on these"
	@echo "platforms (see \"man ldconfig\" for details.)"
	@echo "----------------------------------------------------------------------"

# =============================================================================
# It would be nice if I could just add these to EXTRA_DIST, but I haven't
# yet figured out how to do it without pulling in all the object files
# which `make distcheck` insists on leaving behind in its wake:
# =============================================================================
dist-hook:
	@mkdir -p -m 755 $(distdir)/examples
	@mkdir -p -m 755 $(distdir)/examples/build
	@mkdir -p -m 755 $(distdir)/examples/boilerplate
	@mkdir -p -m 755 $(distdir)/examples/ex_argp_test
	@mkdir -p -m 755 $(distdir)/examples/ex_step_0
	@mkdir -p -m 755 $(distdir)/examples/ex_step_1
	@mkdir -p -m 755 $(distdir)/examples/ex_step_2
	@mkdir -p -m 755 $(distdir)/examples/ex_step_3
	@mkdir -p -m 755 $(distdir)/examples/ex_step_4
	@mkdir -p -m 755 $(distdir)/examples/ex_step_5
	@mkdir -p -m 755 $(distdir)/examples/ex_step_6
	@mkdir -p -m 755 $(distdir)/examples/ex_step_7
	@mkdir -p -m 755 $(distdir)/examples/ex_step_8
	@cp $(top_srcdir)/examples/build/Makefile.am $(distdir)/examples/build
	@cp $(top_srcdir)/examples/build/Makefile.in $(distdir)/examples/build
	@cp $(top_srcdir)/examples/boilerplate/boilerplate.cpp $(distdir)/examples/boilerplate
	@cp $(top_srcdir)/examples/ex_argp_test/ex_argp_test.cpp $(distdir)/examples/ex_argp_test
	@cp $(top_srcdir)/examples/ex_step_0/ex_step_0.cpp $(distdir)/examples/ex_step_0
	@cp $(top_srcdir)/examples/ex_step_1/ex_step_1.cpp $(distdir)/examples/ex_step_1
	@cp $(top_srcdir)/examples/ex_step_2/ex_step_2.cpp $(distdir)/examples/ex_step_2
	@cp $(top_srcdir)/examples/ex_step_3/ex_step_3.cpp $(distdir)/examples/ex_step_3
	@cp $(top_srcdir)/examples/ex_step_4/ex_step_4.cpp $(distdir)/examples/ex_step_4
	@cp $(top_srcdir)/examples/ex_step_5/ex_step_5.cpp $(distdir)/examples/ex_step_5
	@cp $(top_srcdir)/examples/ex_step_6/ex_step_6.cpp $(distdir)/examples/ex_step_6
	@cp $(top_srcdir)/examples/ex_step_7/ex_step_7.cpp $(distdir)/examples/ex_step_7
	@cp $(top_srcdir)/examples/ex_step_8/ex_step_8.cpp $(distdir)/examples/ex_step_8

