/*------------------------------------------------------------------------------

    Copyright (c) 2004 Media Development Loan Fund
 
    This file is part of the LiveSupport project.
    http://livesupport.campware.org/
    To report bugs, send an e-mail to bugs@campware.org
 
    LiveSupport is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    LiveSupport is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with LiveSupport; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
    Author   : $Author: maroy $
    Version  : $Revision: 1.3 $
    Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/products/scheduler/src/PostgresqlScheduleTest.cxx,v $

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#else
#error "Need unistd.h"
#endif


#include <string>
#include <iostream>

#include "LiveSupport/Db/ConnectionManagerFactory.h"
#include "PostgresqlSchedule.h"
#include "PostgresqlScheduleTest.h"


using namespace boost::posix_time;

using namespace LiveSupport::Scheduler;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

CPPUNIT_TEST_SUITE_REGISTRATION(PostgresqlScheduleTest);

/**
 *  The name of the configuration file for the connection manager factory.
 */
static const std::string configFileName = "etc/connectionManagerFactory.xml";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Set up the test environment
 *----------------------------------------------------------------------------*/
void
PostgresqlScheduleTest :: setUp(void)                         throw ()
{
    try {
        Ptr<xmlpp::DomParser>::Ref  parser(
                                    new xmlpp::DomParser(configFileName, true));
        const xmlpp::Document * document = parser->get_document();
        const xmlpp::Element  * root     = document->get_root_node();

        Ptr<ConnectionManagerFactory>::Ref   cmf =
                                        ConnectionManagerFactory::getInstance();
        cmf->configure(*root);
        cm = cmf->getConnectionManager();
        
        schedule.reset(new PostgresqlSchedule(cm));
        schedule->install();

    } catch (std::invalid_argument &e) {
        CPPUNIT_FAIL("semantic error in configuration file");
    } catch (xmlpp::exception &e) {
        CPPUNIT_FAIL("error parsing configuration file");
    }
}


/*------------------------------------------------------------------------------
 *  Clean up the test environment
 *----------------------------------------------------------------------------*/
void
PostgresqlScheduleTest :: tearDown(void)                      throw ()
{
    schedule->uninstall();
    schedule.reset();
    cm.reset();
}


/*------------------------------------------------------------------------------
 *  Test for an available timeframe in an empty schedule database.
 *----------------------------------------------------------------------------*/
void
PostgresqlScheduleTest :: firstTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    // check with any two arbitary dates, the timeframe should be available
    Ptr<ptime>::Ref from(new ptime(time_from_string("2004-07-23 10:00:00")));
    Ptr<ptime>::Ref to(new ptime(time_from_string("2004-07-23 11:00:00")));

    CPPUNIT_ASSERT(schedule->isTimeframeAvailable(from, to));
}


/*------------------------------------------------------------------------------
 *  Schedule a single playlist.
 *----------------------------------------------------------------------------*/
void
PostgresqlScheduleTest :: simpleScheduleTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    // create a 1 hour long playlist, from 10 o'clock 2004-07-23
    Ptr<UniqueId>::Ref      id = UniqueId::generateId();
    Ptr<time_duration>::Ref playlength(new time_duration(1, 0, 0));
    Ptr<Playlist>::Ref      playlist(new Playlist(id, playlength));
    Ptr<ptime>::Ref         from(new ptime(time_from_string(
                                                    "2004-07-23 10:00:00")));

    try {
        schedule->schedulePlaylist(playlist, from);
    } catch (std::invalid_argument &e) {
        CPPUNIT_FAIL(e.what());
    }
}


/*------------------------------------------------------------------------------
 *  Schedule a single playlist, and then query for available timeframes
 *  around it.
 *----------------------------------------------------------------------------*/
void
PostgresqlScheduleTest :: scheduleAndQueryTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    // create a 1 hour long playlist, from 10 o'clock 2004-07-23
    Ptr<UniqueId>::Ref      id = UniqueId::generateId();
    Ptr<time_duration>::Ref playlength(new time_duration(1, 0, 0));
    Ptr<Playlist>::Ref      playlist(new Playlist(id, playlength));
    Ptr<ptime>::Ref         from(new ptime(time_from_string(
                                                    "2004-07-23 10:00:00")));

    try {
        schedule->schedulePlaylist(playlist, from);
    } catch (std::invalid_argument &e) {
        CPPUNIT_FAIL(e.what());
    }

    // check for available timeframes around the inserted one
    Ptr<ptime>::Ref     to;

    // this is the exact same timeframe as the scheduled playlist
    from.reset(new ptime(time_from_string("2004-07-23 10:00:00")));
    to.reset(new ptime(time_from_string("2004-07-23 11:00:00")));

    CPPUNIT_ASSERT(!schedule->isTimeframeAvailable(from, to));

    // a timeframe before our playlist
    from.reset(new ptime(time_from_string("2004-07-23 09:00:00")));
    to.reset(new ptime(time_from_string("2004-07-23 09:50:00")));

    CPPUNIT_ASSERT(schedule->isTimeframeAvailable(from, to));

    // a timeframe after our playlist
    from.reset(new ptime(time_from_string("2004-07-23 11:10:00")));
    to.reset(new ptime(time_from_string("2004-07-23 12:00:00")));

    CPPUNIT_ASSERT(schedule->isTimeframeAvailable(from, to));

    // a timeframe inside ours
    from.reset(new ptime(time_from_string("2004-07-23 10:10:00")));
    to.reset(new ptime(time_from_string("2004-07-23 10:50:00")));

    CPPUNIT_ASSERT(!schedule->isTimeframeAvailable(from, to));

    // a timeframe encapsulating ours
    from.reset(new ptime(time_from_string("2004-07-23 09:50:00")));
    to.reset(new ptime(time_from_string("2004-07-23 11:10:00")));

    CPPUNIT_ASSERT(!schedule->isTimeframeAvailable(from, to));

    // a timeframe starting earlier, but flowing into ours
    from.reset(new ptime(time_from_string("2004-07-23 09:00:00")));
    to.reset(new ptime(time_from_string("2004-07-23 10:10:00")));

    CPPUNIT_ASSERT(!schedule->isTimeframeAvailable(from, to));

    // a timeframe starting inside ours, and continuing afterwards
    from.reset(new ptime(time_from_string("2004-07-23 10:50:00")));
    to.reset(new ptime(time_from_string("2004-07-23 11:50:00")));

    CPPUNIT_ASSERT(!schedule->isTimeframeAvailable(from, to));

    // a timeframe ending exaclty when ours starts, which is OK
    from.reset(new ptime(time_from_string("2004-07-23 09:00:00")));
    to.reset(new ptime(time_from_string("2004-07-23 10:00:00")));

    CPPUNIT_ASSERT(schedule->isTimeframeAvailable(from, to));

    // a timeframe starting exactly when ours ends, which is OK
    from.reset(new ptime(time_from_string("2004-07-23 11:00:00")));
    to.reset(new ptime(time_from_string("2004-07-23 12:00:00")));

    CPPUNIT_ASSERT(schedule->isTimeframeAvailable(from, to));
}


/*------------------------------------------------------------------------------
 *  See if getScheduleEntries() returns correct lists
 *----------------------------------------------------------------------------*/
void
PostgresqlScheduleTest :: getScheduleEntriesTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    // create a 1 hour long playlist
    Ptr<UniqueId>::Ref      playlistId = UniqueId::generateId();
    Ptr<time_duration>::Ref playlength(new time_duration(1, 0, 0));
    Ptr<Playlist>::Ref      playlist(new Playlist(playlistId, playlength));

    Ptr<ptime>::Ref         from;
    Ptr<ptime>::Ref         to;

    Ptr<std::vector<Ptr<ScheduleEntry>::Ref> >::Ref  entries;
    Ptr<ScheduleEntry>::Ref                          entry;

    try {
        // schedule our playlist for 2004-07-23, 10 o'clock
        from.reset(new ptime(time_from_string("2004-07-23 10:00:00")));
        schedule->schedulePlaylist(playlist, from);

        // schedule our playlist for 2004-07-23, 12 o'clock
        from.reset(new ptime(time_from_string("2004-07-23 12:00:00")));
        schedule->schedulePlaylist(playlist, from);

        // schedule our playlist for 2004-07-23, 14 o'clock
        from.reset(new ptime(time_from_string("2004-07-23 14:00:00")));
        schedule->schedulePlaylist(playlist, from);

        // and now let's see what's scheduled for 2004-07-23 between
        // 9:00 and 11:00
        from.reset(new ptime(time_from_string("2004-07-23 09:00:00")));
        to.reset(new ptime(time_from_string("2004-07-23 11:00:00")));
        entries = schedule->getScheduleEntries(from, to);
        // see that it is a single entry starting from 10 to 11 o'clock
        CPPUNIT_ASSERT(entries->size() == 1);
        entry = (*entries)[0];
        CPPUNIT_ASSERT(*(entry->getPlaylistId()) == *(playlist->getId()));
        from.reset(new ptime(time_from_string("2004-07-23 10:00:00")));
        CPPUNIT_ASSERT(*(entry->getStartTime()) == *from);
        to.reset(new ptime(time_from_string("2004-07-23 11:00:00")));
        CPPUNIT_ASSERT(*(entry->getEndTime()) == *to);

        // let's see what's scheduled for 2004-07-23 between
        // 9:00 and 13:00
        from.reset(new ptime(time_from_string("2004-07-23 09:00:00")));
        to.reset(new ptime(time_from_string("2004-07-23 13:00:00")));
        entries = schedule->getScheduleEntries(from, to);
        // see that it is 2 entries, the one at 10 and the other at 12 o'clock
        CPPUNIT_ASSERT(entries->size() == 2);
        // see the one at 10 o'clock
        entry = (*entries)[0];
        CPPUNIT_ASSERT(*(entry->getPlaylistId()) == *(playlist->getId()));
        from.reset(new ptime(time_from_string("2004-07-23 10:00:00")));
        CPPUNIT_ASSERT(*(entry->getStartTime()) == *from);
        to.reset(new ptime(time_from_string("2004-07-23 11:00:00")));
        CPPUNIT_ASSERT(*(entry->getEndTime()) == *to);
        // see the other at 12 o'clock
        entry = (*entries)[1];
        CPPUNIT_ASSERT(*(entry->getPlaylistId()) == *(playlist->getId()));
        from.reset(new ptime(time_from_string("2004-07-23 12:00:00")));
        CPPUNIT_ASSERT(*(entry->getStartTime()) == *from);
        to.reset(new ptime(time_from_string("2004-07-23 13:00:00")));
        CPPUNIT_ASSERT(*(entry->getEndTime()) == *to);
    } catch (std::invalid_argument &e) {
        CPPUNIT_FAIL(e.what());
    }
}


