//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/test/tstPingPong.cc
 * \author Thomas M. Evans
 * \date   Tue Apr  2 15:57:11 2002
 * \brief  Ping Pong communication test.
 * \note   Copyright (C) 2006-2010 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "c4_test.hh"
#include "ds++/Release.hh"
#include "../global.hh"
#include "../SpinLock.hh"
#include "ds++/Assert.hh"
#include "ds++/Soft_Equivalence.hh"

#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>

using namespace std;

using rtt_c4::C4_Req;
using rtt_c4::C4_Traits;
using rtt_c4::send;
using rtt_c4::receive;
using rtt_c4::send_async;
using rtt_c4::receive_async;
using rtt_dsxx::soft_equiv;
using rtt_c4::probe;
using rtt_c4::blocking_probe;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void blocking_ping_pong()
{
    if (rtt_c4::nodes() != 2) return;
    
    char   c = 0;
    int    i = 0;
    long   l = 0;
    float  f = 0;
    double d = 0;

    // assign on node 0
    if (rtt_c4::node() == 0)
    {
        c = 'A';
        i = 1;
        l = 1000;
        f = 1.5;
        d = 2.5;

        // send out data
        // Test both active and depricated forms of the send command. 
        send(&c, 1, 1);
        C4::Send(&i, 1, 1);  // This form is deprecated.
        send(&l, 1, 1);
        send(&f, 1, 1);
        send(&d, 1, 1);

        // receive back
        // Test both active and depricated forms of the receive command.
        receive(&c, 1, 1);
        C4::Recv(&i, 1, 1); // This form is deprecated.
        receive(&l, 1, 1);
        receive(&f, 1, 1);
        receive(&d, 1, 1);

        // check values
        if (c != 'B')             ITFAILS;
        if (i != 2)               ITFAILS;
        if (l != 2000)            ITFAILS;
        if (!soft_equiv(f, 2.5f)) ITFAILS;
        if (!soft_equiv(d, 3.5))  ITFAILS;
    }

    // receive and send on node 1
    if (rtt_c4::node() == 1)
    {
        // receive from node 0
        receive(&c, 1, 0);
        receive(&i, 1, 0);
        receive(&l, 1, 0);
        receive(&f, 1, 0);
        receive(&d, 1, 0);

        // check values
        if (c != 'A')             ITFAILS;
        if (i != 1)               ITFAILS;
        if (l != 1000)            ITFAILS;
        if (!soft_equiv(f, 1.5f)) ITFAILS;
        if (!soft_equiv(d, 2.5))  ITFAILS;

        // assign new values
        c = 'B';
        i = 2;
        l = 2000;
        f = 2.5;
        d = 3.5;

        // send them back
        send(&c, 1, 0);
        send(&i, 1, 0);
        send(&l, 1, 0);
        send(&f, 1, 0);
        send(&d, 1, 0);
    }

    rtt_c4::global_barrier();

    if (rtt_c4_test::passed)
    {
        ostringstream m;
        m << "Blocking Send/Recv communication ok on " << rtt_c4::node();
        PASSMSG(m.str());

    }
    else
    {
        ostringstream m;
        m << "Blocking Send/Recv communication failed on " << rtt_c4::node();
        FAILMSG(m.str());
    }
    return;
}
//---------------------------------------------------------------------------//

void non_blocking_ping_pong()
{

   if (rtt_c4::nodes() != 2) return;

   char   c = 0;
   int    i = 0;
   long   l = 0;
   float  f = 0;
   double d = 0;

   char   cr = 0;
   int    ir = 0;
   long   lr = 0;
   float  fr = 0;
   double dr = 0;

   // send requests
   C4_Req crs, irs, lrs, frs, drs;

   // receive requests
   C4_Req crr, irr, lrr, frr, drr;

   // assign on node 0
   if (rtt_c4::node() == 0)
   {
       // post receives
       // Test two forms of the receive_async command plus one deprecated
       // form (namespace C4::)

       receive_async(crr, &cr, 1, 1);
       irr = receive_async(&ir, 1, 1);
       C4::RecvAsync(lrr, &lr, 1, 1);
       frr = C4::RecvAsync(&fr, 1, 1);
       receive_async(drr, &dr, 1, 1);

       // give values to the send data
       c = 'A';
       i = 1;
       l = 1000;
       f = 1.5;
       d = 2.5;
      
       // send out data
       // Test two forms of the send_async command plus one deprecated
       // form (namespace C4::)
       send_async(crs, &c, 1, 1);
       irs = send_async( &i, 1, 1);
       C4::SendAsync(lrs, &l, 1, 1);
       frs = C4::SendAsync(&f,1,1);
       send_async(drs, &d, 1, 1);

       // wait for sends to be finished
       crs.wait();
       irs.wait();
       lrs.wait();
       frs.wait();
       drs.wait();

       // wait on receives and check
       crr.wait();
       irr.wait();
       lrr.wait();
       frr.wait();
       drr.wait();


       // check values       
       if (cr != 'B')             ITFAILS;
       if (ir != 2)               ITFAILS;
       if (lr != 2000)            ITFAILS;
       if (!soft_equiv(fr, 2.5f)) ITFAILS;
       if (!soft_equiv(dr, 3.5))  ITFAILS;

       if (crr.count() != 1) ITFAILS;
       if (irr.count() != sizeof(int))    ITFAILS;
       if (lrr.count() != sizeof(long))   ITFAILS;
       if (frr.count() != sizeof(float))  ITFAILS;
       if (drr.count() != sizeof(double)) ITFAILS;
      
   }

   // receive and send on node 1
   if (rtt_c4::node() == 1)
   {
       // post receives
       // Test both function that provide equivalent functionality.
       receive_async(crr, &cr, 1, 0);
       irr = receive_async( &ir, 1, 0);
       receive_async(lrr, &lr, 1, 0);
       receive_async(frr, &fr, 1, 0);
       receive_async(drr, &dr, 1, 0);

       // check that all are inuse
       if (!crr.inuse()) ITFAILS;
       if (!irr.inuse()) ITFAILS;
       if (!lrr.inuse()) ITFAILS;
       if (!frr.inuse()) ITFAILS;
       if (!drr.inuse()) ITFAILS;

       // check on receives
       int done = 0;
       while (done < 5)
       {
           if (crr.complete()) done++;
           if (irr.complete()) done++;
           if (lrr.complete()) done++;
           if (frr.complete()) done++;
           if (drr.complete()) done++;
       }

       if (cr != 'A')             ITFAILS;
       if (ir != 1)               ITFAILS;
       if (lr != 1000)            ITFAILS;
       if (!soft_equiv(fr, 1.5f)) ITFAILS;
       if (!soft_equiv(dr, 2.5))  ITFAILS;

       // assign new values
       c = 'B';
       i = 2;
       l = 2000;
       f = 2.5;
       d = 3.5;

       // send them back
       // Test both function that provide equivalent functionality.

       send_async(crs, &c, 1, 0);
       irs = send_async( &i, 1, 0);
       send_async(lrs, &l, 1, 0);
       send_async(frs, &f, 1, 0);
       send_async(drs, &d, 1, 0);

       // wait for sends to be finished
       crs.wait();
       irs.wait();
       lrs.wait();
       frs.wait();
       drs.wait();
   }
   rtt_c4::global_barrier();

   // check that all requests are done
   if (crs.inuse()) ITFAILS;
   if (irs.inuse()) ITFAILS;
   if (lrs.inuse()) ITFAILS;
   if (frs.inuse()) ITFAILS;
   if (drs.inuse()) ITFAILS;

   if (crr.inuse()) ITFAILS;
   if (irr.inuse()) ITFAILS;
   if (lrr.inuse()) ITFAILS;
   if (frr.inuse()) ITFAILS;
   if (drr.inuse()) ITFAILS;

   if (rtt_c4_test::passed)
   {
       ostringstream m;
       m << "Non-blocking Send/Recv communication ok on " << rtt_c4::node();
       PASSMSG(m.str());
   }

   return;
}


//---------------------------------------------------------------------------//
// Test the C4_Req.free() function.
// After a asynchronous receive command is given, it can be terminated by
// using the free() command.
//---------------------------------------------------------------------------//

void tstC4_Req_free()
{

   if (rtt_c4::nodes() != 2) return;

   // receive requests
   C4_Req crr;
   char cr = 0;
   
   // assign on node 0
   if (rtt_c4::node() == 0)
   {
       // post receives
       receive_async(crr, &cr, 1, 1);
       // void the receive request.
       crr.free();
   }

   return;
}

//---------------------------------------------------------------------------//

void probe_ping_pong()
{

   if (rtt_c4::nodes() != 2) return;

   int   i = 0;
   int   ir = 0;

   // send requests
   C4_Req irs;

   // receive requests
   C4_Req irr;

   // Only 2 procs so, dest is either 0 or 1.
   int dest( std::abs( rtt_c4::node() - 1 ) );

   // assign on node 1
   if (rtt_c4::node() == 1)
   {

       // give values to the send data
       i = 2;

       // send out data
       send_async(irs, &i, 1, dest);
   }

   // receive and send on node 0
   if (rtt_c4::node() == 0)
   {
       // test the probe function
       int message_size;
       for (;;)
       {
           if (probe(dest, C4_Traits<int*>::tag, message_size))
           {
               if (message_size==sizeof(int))
               {
                   PASSMSG("Probe returned correct size");
               }
               else
               {
                   FAILMSG("Probe returned WRONG size");
               }
               break;
           }
       }

       // test the blocking probe function
       blocking_probe(dest, C4_Traits<int*>::tag, message_size);
       if (message_size==sizeof(int))
       {
           PASSMSG("Blocking probe returned correct size");
       }
       else
       {
           FAILMSG("Blocking probe returned WRONG size");
       }

       // post receives
       receive_async(irr, &ir, 1, dest);
   }

   rtt_c4::global_barrier();

   if (rtt_c4_test::passed)
   {
       ostringstream m;
       m << "Probe communication ok on " << rtt_c4::node();
       PASSMSG(m.str());
   }

}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    std::cout << "starting main" << std::endl;
    
    rtt_c4::initialize(argc, argv);

    // version tag
    for (int arg = 1; arg < argc; arg++)
    {
        if (string(argv[arg]) == "--version")
        {
            if (rtt_c4::node() == 0)
                cout << argv[0] << ": version " << rtt_dsxx::release() 
                << endl;
            rtt_c4::finalize();
            return 0;
        }
    }

    try
    {
        // >>> UNIT TESTS
        blocking_ping_pong();
        non_blocking_ping_pong();
        probe_ping_pong();
        tstC4_Req_free();        
    }
    catch (rtt_dsxx::assertion &error)
    {
        cout << "While testing tstPingPong, " << error.what()
            << endl;
        rtt_c4::abort();
        return 1;
    }

    {
        rtt_c4::HTSyncSpinLock slock;
        // status of test
        cout <<     "\n*********************************************";
        if (rtt_c4_test::passed) 
            cout << "\n**** tstPingPong Test: PASSED on " << rtt_c4::node(); 
        cout <<     "\n*********************************************\n" << endl;
    }

    rtt_c4::global_barrier();

    cout << "Done testing tstPingPong on " << rtt_c4::node() << endl;
    rtt_c4::finalize();
    return 0;
}   

//---------------------------------------------------------------------------//
// end of tstPingPong.cc
//---------------------------------------------------------------------------//
