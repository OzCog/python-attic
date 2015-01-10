/*
 * FUNCTION:
 * Asynchronous update of indexes.
 *
 * Copyright (c) 2013,2015 Linas Vepstas <linas@linas.org>
 *
 * LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <memory>
#include <thread>

#include <opencog/util/oc_assert.h>

#include "AsyncUpdate.h"

using namespace opencog;


/* ================================================================ */
// Constructors

AsyncUpdate::AsyncUpdate(void)
{
	stopping_writers = false;
	thread_count = 0;
	busy_writers = 0;
	startWriterThread();
	startWriterThread();
	startWriterThread();
	startWriterThread();
}

AsyncUpdate::~AsyncUpdate()
{
	stopWriterThreads();
}

/* ================================================================ */

/// Start a single writer thread.
/// May be called multiple times.
void AsyncUpdate::startWriterThread()
{
	logger().info("AsyncUpdate: starting a writer thread");
	std::unique_lock<std::mutex> lock(write_mutex);
	if (stopping_writers)
		throw RuntimeException(TRACE_INFO,
			"Cannot start; AtomsStorage writer threads are being stopped!");

	write_threads.push_back(std::thread(&AsyncUpdate::writeLoop, this));
	thread_count ++;
}

/// Stop all writer threads, but only after they are done wroting.
void AsyncUpdate::stopWriterThreads()
{
	logger().info("AsyncUpdate: stopping all writer threads");
	std::unique_lock<std::mutex> lock(write_mutex);
	stopping_writers = true;

	// Spin a while, until the writeer threads are (mostly) done.
	while (not store_queue.is_empty())
	{
		// std::this_thread::sleep_for(std::chrono::milliseconds(1));
		usleep(1000);
	}

	// Now tell all the threads that they are done.
	// I.e. cancel all the threads.
	store_queue.cancel();
	while (0 < write_threads.size())
	{
		write_threads.back().join();
		write_threads.pop_back();
		thread_count --;
	}

	// OK, so we've joined all the threads, but the queue
	// might not be totally empty; some dregs might remain.
	// Drain it now, single-threadedly.
	store_queue.cancel_reset();
	while (not store_queue.is_empty())
	{
		AtomPtr atom = store_queue.pop();
		do_store_atom(atom);
	}
	
	// Its now OK to start new threads, if desired ...(!)
	stopping_writers = false;
}

/// A Single write thread. Reds atoms from queue, and stores them.
void AsyncUpdate::writeLoop()
{
	try
	{
		while (true)
		{
			AtomPtr atom = store_queue.pop();
			busy_writers ++; // Bad -- window after pop returns, before increment!
			do_store_atom(atom);
			busy_writers --;
		}
	}
	catch (concurrent_queue<AtomPtr>::Canceled& e)
	{
		// We are so out of here. Nothing to do, just exit this thread.
		return;
	}
}

/// Drain the pending store queue.
/// Caution: this is slightly racy; a writer could still be busy
/// even though this returns. (There's a window in writeLoop, between
/// the dequeue, and the busy_writer increment. I guess we should fix
// this...
void AsyncUpdate::flushStoreQueue()
{
	// std::this_thread::sleep_for(std::chrono::microseconds(10));
	usleep(10);
	while (0 < store_queue.size() or 0 < busy_writers);
	{
		// std::this_thread::sleep_for(std::chrono::milliseconds(1));
		usleep(1000);
	}
}

/* ================================================================ */

/* ============================= END OF FILE ================= */
