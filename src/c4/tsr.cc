//----------------------------------*-C++-*----------------------------------//
// tsr.cc
// Geoffrey Furnish
// 24 May 1996.
//---------------------------------------------------------------------------//
// @> Test program for C4 send and receive.
//---------------------------------------------------------------------------//

#include "c4/global.hh"

#include <iostream.h>
#include <stdio.h>

int node, nodes;

void stall_node()
{
    for( int i=1000000; i; i-- ) ;
}

//---------------------------------------------------------------------------//
// First, simplest test of point to point messaging.
//---------------------------------------------------------------------------//

void t1()
{
    C4_gsync();

    if (node == 0) {
	int x[4];
	for( int i=0; i < 4; i++ )
	    x[i] = i;

	C4_Send( &x, 4*sizeof(int), 1, 1 );
    }		

    C4_gsync();

// By this point we should have the message sitting in the inbox on node 1.

    if (node == 0)
	cout << "0 sent the message, time for 1 to receive it.\n" << flush;

    C4_gsync();

    if (node == 1) {
	int x[4];
	C4_Recv( &x, 4*sizeof(int), 0, 1 );

	cout << "1 got the message.\n" << flush;

	int fail = 0;
	for( int i=0; i < 4; i++ )
	    if ( x[i] != i ) fail = 1;

	if (fail)
	    cout << "*** t1 failed! ***\n" << flush;
	else
	    cout << "\t\t >> t1 succeeded <<\n" << flush;
    }

// By this point the inbox should be empty, and the msg_queues empty.

    C4_gsync();
}

//---------------------------------------------------------------------------//
// Try to force a message into the message queue.
//---------------------------------------------------------------------------//

void t2()
{
    C4_gsync();

// At this point, pe_ready[1] on node 0 should be 1.

    if (node == 0) {
	cout << "t2 beginning.\n" << flush;

	int x1[4] = {0,1,2,3};
	int x2[4] = {4,5,6,7};

	C4_Send( &x1, sizeof(x1), 1, 1 );
	cout << "msg1 sent.\n" << flush;

	C4_Send( &x2, sizeof(x2), 1, 2 );
	cout << "msg2 sent.\n" << flush;
    }
    if (node == 1) {
	stall_node();

	int x1[4], x2[4];

	C4_Recv( &x2, sizeof(x2), 0, 2 );
	cout << "msg2 recv'd.\n" << flush;

	C4_Recv( &x1, sizeof(x1), 0, 1 );
	cout << "msg1 recv'd.\n" << flush;

    // Now validate data.

	int fail, fail1 = 0, fail2 = 0;
	for( int i=0; i < 4; i++ ) {
// 	    cout << i << ": x1=" << x1[i] << " x2=" << x2[i] << endl;

	    if (x1[i] != i) fail1 = 1;
	    if (x2[i] != i+4) fail2 = 1;
	}
	fail = fail1 || fail2;
	if (fail) {
	    if (fail1)
		cout << "msg1 hosed.\n";
	    if (fail2)
		cout << "msg2 hosed.\n";
	    cout << "*** t2 failed! ***\n" << flush;
	} else
	    cout << "\t\t >> t2 succeeded <<\n" << flush;
    }

    C4_gsync();
}

//---------------------------------------------------------------------------//
// Test async send.
//---------------------------------------------------------------------------//

void t3()
{
    C4_gsync();

    int x[4];
    {
	C4_Req r;

	if (node == 0) {
	    for( int i=0; i < 4; i++ )
		x[i] = i;

	    r = C4_SendAsync( (void *) &x[0], 4*sizeof(int), 1, 47 );

	    cout << "0: msg sent to node 1.\n" << flush;
	}

	C4_gsync();

	if (node == 1) {
	    C4_Recv( (void *) x, 4*sizeof(int), 0, 47 );

	    cout << "1: msg recv'd.\n" << flush;
	}

	C4_gsync();
    }
    printf( "%d: C4_Req destroyed.\n", node );
    C4_gsync();

    if (node == 1) {
	int fail = 0;
	for( int i=0; i < 4; i++ ) {
	    printf( "x[%d]=%d\n", i, x[i] );
	    if (x[i] != i)
		fail++;
	}
	if (fail)
	    cout << "*** t3 failed! ***\n" << flush;
	else
	    cout << "\t\t >> t3 succeeded <<\n" << flush;
    }
}

//---------------------------------------------------------------------------//
// Test async recv.
//---------------------------------------------------------------------------//

void t4()
{
    C4_gsync();

    int x[4];
    {
	C4_Req r;

	if (node == 0) {
	    for( int i=0; i < 4; i++ )
		x[i] = i;

	    C4_Send( (void *) &x[0], 4*sizeof(int), 1, 47 );

	    cout << "0: msg sent to node 1.\n" << flush;
	}

	C4_gsync();

	if (node == 1) {
	// In this case, the send will have gone straight to the inbox, so
	// this should complete immediately.

	    r = C4_RecvAsync( (void *) x, 4*sizeof(int), 0, 47 );

	    cout << "1: msg recv posted.\n" << flush;
	}

	C4_gsync();
    }
    printf( "%d: C4_Req destroyed.\n", node );
    C4_gsync();

    if (node == 1) {
	int fail = 0;
	for( int i=0; i < 4; i++ ) {
	    printf( "x[%d]=%d\n", i, x[i] );
	    if (x[i] != i)
		fail++;
	}
	if (fail)
	    cout << "*** t4 failed! ***\n" << flush;
	else
	    cout << "\t\t >> t4 succeeded <<\n" << flush;
    }
}

//---------------------------------------------------------------------------//
// Test async recv.  This will force the async message onto the msg queue.
//---------------------------------------------------------------------------//

void t5()
{
    C4_gsync();

    int x[4];
    {
	C4_Req r;

	if (node == 0) {
	    for( int i=0; i < 4; i++ )
		x[i] = i;

	    C4_Send( (void *) &x[0], 4*sizeof(int), 1, 47 );
	    C4_Send( x[0], 1 );

	    cout << "0: msg sent to node 1.\n" << flush;
	}

	if (node == 1) {
	    C4_Recv( x[0], 0 );
	// This should've forced the desired message onto the msg_queue.
	    r = C4_RecvAsync( (void *) x, 4*sizeof(int), 0, 47 );

	    cout << "1: msg recv posted.\n" << flush;
	}

	C4_gsync();
    }
    printf( "%d: C4_Req destroyed.\n", node );
//     C4_shm_dbg_1();
    C4_gsync();

    if (node == 1) {
	int fail = 0;
	for( int i=0; i < 4; i++ ) {
	    printf( "x[%d]=%d\n", i, x[i] );
	    if (x[i] != i)
		fail++;
	}
	if (fail)
	    cout << "*** t5 failed! ***\n" << flush;
	else
	    cout << "\t\t >> t5 succeeded <<\n" << flush;
    }
}

//---------------------------------------------------------------------------//
// Test async recv.  Here we will arrange to get the receive posted, and
// check that send (blocking) can short circuit to it.
//---------------------------------------------------------------------------//

void t6()
{
    C4_gsync();

    int x[4];
    {
	C4_Req r;

	if (node == 1) {
	    r = C4_RecvAsync( (void *) x, 4*sizeof(int), 0, 47 );
	    cout << "1: posted async recv\n";
	}

	C4_gsync();

	if (node == 0) {
	    stall_node();

	    for( int i=0; i < 4; i++ )
		x[i] = i;

	    C4_Send( (void *) &x[0], 4*sizeof(int), 1, 47 );

	    cout << "0: msg sent to node 1.\n" << flush;
	}

// 	if (node == 1) {
// 	    C4_Recv( x[0], 0 );
// 	// This should've forced the desired message onto the msg_queue.
// 	    r = C4_RecvAsync( (void *) x, 4*sizeof(int), 0, 47 );

// 	    cout << "1: msg recv posted.\n" << flush;
// 	}

// 	C4_gsync();
    }
    printf( "%d: C4_Req destroyed.\n", node );
    C4_gsync();

    if (node == 1) {
	int fail = 0;
	for( int i=0; i < 4; i++ ) {
	    printf( "x[%d]=%d\n", i, x[i] );
	    if (x[i] != i)
		fail++;
	}
	if (fail)
	    cout << "*** t6 failed! ***\n" << flush;
	else
	    cout << "\t\t >> t6 succeeded <<\n" << flush;
    }
}

//---------------------------------------------------------------------------//

int main( int argc, char *argv[] )
{
//     ios::sync_with_stdio();
    C4_Init( argc, argv );

    node = C4_node();
    nodes = C4_nodes();

    t1();
    t2();
    t3();
    t4();
    t5();
//     C4_shm_dbg_1();
    t6();

    C4_gsync();
    C4_Finalize();
    return 0;
}

//---------------------------------------------------------------------------//
//                              end of tsr.cc
//---------------------------------------------------------------------------//
