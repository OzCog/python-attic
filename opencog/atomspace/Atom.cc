/*
 * opencog/atomspace/Atom.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Thiago Maia <thiago@vettatech.com>
 *            Andre Senna <senna@vettalabs.com>
 *            Welter Silva <welter@vettalabs.com>
 *
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

#include "Atom.h"

#include <set>

#ifndef WIN32
#include <unistd.h>
#endif

#include <opencog/atomspace/AtomSpaceDefinitions.h>
#include <opencog/atomspace/AtomTable.h>
#include <opencog/atomspace/ClassServer.h>
#include <opencog/atomspace/Link.h>
#include <opencog/atomspace/StatisticsMonitor.h>
#include <opencog/atomspace/TLB.h>
#include <opencog/util/Logger.h>
#include <opencog/util/exceptions.h>
#include <opencog/util/misc.h>
#include <opencog/util/platform.h>

//#define DPRINTF printf
#define DPRINTF(...)

#undef Type

using namespace opencog;

void Atom::init(Type t, const TruthValue& tv)
{
    handle = Handle::UNDEFINED;
    flags = 0;
    atomTable = NULL;
    incoming = NULL;
    type = t;

    truthValue = NULL;
    setTruthValue(tv);
}

Atom::Atom(Type type, const TruthValue& tv )
{
    init(type, tv);
}

Atom::~Atom() throw (RuntimeException)
{
    // checks if there is still an atom pointing to the one being removed
    // WARNING: I (Joel) disabled this check as it makes it impossible to clone
    // Atoms via the AtomSpace for code that wants an Atom object. *Most* code
    // should only use the AtomSpace to view/alter the hypergraph though...so
    // it should be the AtomTable/AtomSpace that does this check.
    /*if (incoming != NULL) {
        throw RuntimeException(TRACE_INFO, "Attempting to remove atom with non-empty incoming set.");
    }*/
    if (incoming != NULL) {
        delete incoming;
    }

    if (truthValue != &(TruthValue::DEFAULT_TV())) {
        delete truthValue;
    }
}

const AttentionValue& Atom::getAttentionValue() const
{
    return attentionValue;
}

const TruthValue& Atom::getTruthValue() const
{
    return *truthValue;
}

void Atom::setTruthValue(const TruthValue& tv)
{
    if (truthValue != NULL && &tv != truthValue && truthValue != &(TruthValue::DEFAULT_TV())) {
        delete truthValue;
    }
    truthValue = (TruthValue*) & (TruthValue::DEFAULT_TV());
    if (!tv.isNullTv() && (&tv != &(TruthValue::DEFAULT_TV()))) {
        truthValue = tv.clone();
    }
}

void Atom::setAttentionValue(const AttentionValue& new_av) throw (RuntimeException)
{
    if (new_av == attentionValue) return;

    int oldBin = -1;
    if (atomTable != NULL) {
        // gets current bin
        oldBin = ImportanceIndex::importanceBin(attentionValue.getSTI());
    }

    attentionValue = new_av;

    if (atomTable != NULL) {
        // gets new bin
        int newBin = ImportanceIndex::importanceBin(attentionValue.getSTI());

        // if the atom importance has changed its bin,
        // updates the importance index
        if (oldBin != newBin) {
            atomTable->updateImportanceIndex(this, oldBin);
            StatisticsMonitor::getInstance()->atomChangeImportanceBin(type, oldBin, newBin);
        }
    }
}

void Atom::addIncomingHandle(Handle handle)
{
    // creates a new entry with handle
    HandleEntry* entry = new HandleEntry(handle);
    // entry is placed in the first position of the incoming set
    entry->next = incoming;
    incoming = entry;
}

void Atom::removeIncomingHandle(Handle handle) throw (RuntimeException)
{
    DPRINTF("Entering Atom::removeIncomingHandle(): handle:\n%s\nincoming:\n%s\n", TLB::getAtom(handle)->toShortString().c_str(), incoming->toString().c_str());
    HandleEntry* current = incoming;
    // checks if incoming set is empty
    if (incoming == NULL) {
        throw RuntimeException(TRACE_INFO, "unable to extract incoming element from empty set");
    }

    // checks if the handle to be removed is the first one
    if (incoming->handle == handle) {
        incoming = incoming->next;
        current->next = NULL;
        delete current;
    } else {
        if (current->next == NULL) {
            throw RuntimeException(TRACE_INFO, "unable to extract incoming element");
        }
        // scans the list looking for the desired handle
        while (current->next->handle != handle) {
            current = current->next;
            if (current->next == NULL) {
                throw RuntimeException(TRACE_INFO, "unable to extract incoming element");
            }
        }
        // deletes entry when the handle is found
        HandleEntry* foundit = current->next;
        current->next = foundit->next;
        foundit->next = NULL;
        delete foundit;
    }
    DPRINTF("Exiting Atom::removeIncomingHandle(): incoming:\n%s\n", incoming->toString().c_str());
}

bool Atom::getFlag(int flag) const
{
    return (flags & flag) != 0;
}

void Atom::setFlag(int flag, bool value)
{
    if (value) {
        flags |= flag;
    } else {
        flags &= ~(flag);
    }
}

void Atom::unsetRemovalFlag(void)
{
    flags &= ~MARKED_FOR_REMOVAL;
}

void Atom::markForRemoval(void)
{
    flags |= MARKED_FOR_REMOVAL;
}

void Atom::setAtomTable(AtomTable *tb)
{
    atomTable = tb;
}

