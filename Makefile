#-------------------------------------------------------------------------------
#   Copyright (c) 2004 Media Development Loan Fund
#
#   This file is part of the LiveSupport project.
#   http://livesupport.campware.org/
#   To report bugs, send an e-mail to bugs@campware.org
#
#   LiveSupport is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   LiveSupport is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with LiveSupport; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
#   Author   : $Author: maroy $
#   Version  : $Revision: 1.6 $
#   Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/Attic/Makefile,v $
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   General command definitions
#-------------------------------------------------------------------------------
MKDIR      = mkdir -p
RM         = rm -f
RMDIR      = rm -rf
DOXYGEN    = doxygen
XSLTPROC   = xsltproc
ECHO       = @echo
FLAWFINDER = flawfinder


#-------------------------------------------------------------------------------
#   Basic directory and file definitions
#-------------------------------------------------------------------------------
BASE_DIR     = .
DOC_DIR      = ${BASE_DIR}/doc
DOXYGEN_DIR  = ${DOC_DIR}/doxygen
COVERAGE_DIR = ${DOC_DIR}/coverage
ETC_DIR      = ${BASE_DIR}/etc

DOXYGEN_CONFIG = ${ETC_DIR}/doxygen.config

TESTRESULTS_XSLT = ${ETC_DIR}/testResultsToHtml.xsl
TESTRESULTS_IN   = ${ETC_DIR}/testResults.xml
TESTRESULTS_FILE = ${DOC_DIR}/testResults.html

FLAWFINDER_FILE  = ${DOC_DIR}/flawfinderReport.html

TOOLS_DIR  = ${BASE_DIR}/tools

BOOST_DIR         = ${TOOLS_DIR}/boost
BOOST_VERSION     = boost-1.31
LIBXMLXX_DIR      = ${TOOLS_DIR}/libxml++
LIBXMLXX_VERSION  = libxml++-1.0.4
CXXUNIT_DIR       = ${TOOLS_DIR}/cppunit
CXXUNIT_VERSION   = cppunit-1.10.2
LIBODBCXX_DIR     = ${TOOLS_DIR}/libodbc++
LIBODBCXX_VERSION = libodbc++-0.2.3
XMLRPCXX_DIR      = ${TOOLS_DIR}/xmlrpc++
XMLRPCXX_VERSION  = xmlrpc++-20040713
LCOV_DIR          = ${TOOLS_DIR}/lcov
LCOV_VERSION      = lcov-1.3

MODULES_DIR = ${BASE_DIR}/modules
CORE_DIR    = ${MODULES_DIR}/core
DB_DIR      = ${MODULES_DIR}/db
STORAGE_DIR = ${MODULES_DIR}/storage

PRODUCTS_DIR  = ${BASE_DIR}/products
SCHEDULER_DIR = ${PRODUCTS_DIR}/scheduler


#-------------------------------------------------------------------------------
#   Targets
#-------------------------------------------------------------------------------
.PHONY: all doc clean docclean depclean distclean doxygen testresults
.PHONY: setup tools_setup modules_setup products_setup

all: printusage

printusage:
	${ECHO} "LiveSupport project main Makefile"
	${ECHO} "http://livesupport.campware.org/"
	${ECHO} "Copyright (c) 2004 Media Development Loan Fund under the GNU GPL"
	${ECHO} ""
	${ECHO} "Useful targets for this makefile:"
	${ECHO} "   setup        - set up the development environment"
	${ECHO} "   doc          - build autogenerated documentation"
	${ECHO} "   doxygen      - build autogenerated doxygen documentation only"

doc: doxygen testresults

doxygen:
	${DOXYGEN} ${DOXYGEN_CONFIG}

testresults:
	${XSLTPROC} ${TESTRESULT_XSLT} ${TESTRESULTS_IN} > ${TESTRESULTS_FILE}

flawfinder:
	${FLAWFINDER} -c --immediate --html \
                  ${CORE_DIR}/include ${CORE_DIR}/src \
                  ${DB_DIR}/include ${DB_DIR}/src \
                  ${STORAGE_DIR}/include ${STORAGE_DIR}/src \
                  ${SCHEDULER_DIR}/src \
                  > ${FLAWFINDER_FILE}

clean:
	${RMDIR} ${DOXYGEN_DIR}/html
	${RMDIR} ${COVERAGE_DIR}/*

setup: tools_setup modules_setup products_setup

tools_setup:
	${BOOST_DIR}/${BOOST_VERSION}/bin/install.sh
	${LIBXMLXX_DIR}/${LIBXMLXX_VERSION}/bin/install.sh
	${CXXUNIT_DIR}/${CXXUNIT_VERSION}/bin/install.sh
	${LIBODBCXX_DIR}/${LIBODBCXX_VERSION}/bin/install.sh
	${XMLRPCXX_DIR}/${XMLRPCXX_VERSION}/bin/install.sh
	${LCOV_DIR}/${LCOV_VERSION}/bin/install.sh

modules_setup:
	${CORE_DIR}/bin/autogen.sh
	${DB_DIR}/bin/autogen.sh
	${STORAGE_DIR}/bin/autogen.sh

products_setup:
	${SCHEDULER_DIR}/bin/autogen.sh

