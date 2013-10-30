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

#include <set>

#ifndef WIN32
#include <unistd.h>
#endif

#include <opencog/util/Logger.h>
#include <opencog/util/exceptions.h>
#include <opencog/util/misc.h>
#include <opencog/util/platform.h>

#include <opencog/atomspace/Atom.h>
#include <opencog/atomspace/AtomSpaceDefinitions.h>
#include <opencog/atomspace/AtomTable.h>
#include <opencog/atomspace/ClassServer.h>
#include <opencog/atomspace/Link.h>

//#define DPRINTF printf
#define DPRINTF(...)

#undef Type

using namespace opencog;

// Single, global mutex for locking the incoming set.
std::mutex Atom::InSet::_mtx;

Atom::Atom(Type t, TruthValuePtr tv, AttentionValuePtr av)
{
    _uuid = Handle::UNDEFINED.value();
    flags = 0;
    atomTable = NULL;
    type = t;
    _attentionValue = av;

    if (not tv->isNullTv()) truthValue = tv;

    // XXX FIXME for right now, all atoms will always keep their
    // incoming sets.  In the future, this should only be set by
    // the user, or by the atomtable, when an atom is added to
    // the atomtable.
    keep_incoming_set();
}

Atom::~Atom()
{
    atomTable = NULL;
    drop_incoming_set();
}

void Atom::setTruthValue(TruthValuePtr tv)
{
    if (not tv->isNullTv()) truthValue = tv;
}

void Atom::setAttentionValue(AttentionValuePtr new_av) throw (RuntimeException)
{
    if (new_av == _attentionValue) return;
    if (*new_av == *_attentionValue) return;

    int oldBin = -1;
    if (atomTable != NULL) {
        // gets current bin
        oldBin = ImportanceIndex::importanceBin(_attentionValue->getSTI());
    }

    AttentionValuePtr old_av = _attentionValue;
    _attentionValue = new_av;

    if (atomTable != NULL) {
        // gets new bin
        int newBin = ImportanceIndex::importanceBin(_attentionValue->getSTI());

        // if the atom importance has changed its bin,
        // updates the importance index
        if (oldBin != newBin) {
            AtomPtr a(std::static_pointer_cast<Atom>(shared_from_this()));
            atomTable->updateImportanceIndex(a, oldBin);
        }

        // Notify any interested parties that the AV changed.
        AVCHSigl& avch = atomTable->AVChangedSignal();
        avch(getHandle(), old_av, new_av);
    }
}

bool Atom::isMarkedForRemoval() const
{
    return (flags & MARKED_FOR_REMOVAL) != 0;
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
    if (tb == atomTable) return;

    // Notify any interested parties that the AV changed.
    if (NULL == tb and NULL != atomTable) {
        // remove, as far as the old table is concerned
        AVCHSigl& avch = atomTable->AVChangedSignal();
        avch(getHandle(), _attentionValue, AttentionValue::DEFAULT_AV());

        // UUID's belong to the atom table, not the atom. Reclaim it.
        _uuid = Handle::UNDEFINED.value();
    }
    atomTable = tb;
    if (NULL != tb) {
        // add, as far as the old table is concerned
        AVCHSigl& avch = tb->AVChangedSignal();
        avch(getHandle(), AttentionValue::DEFAULT_AV(), _attentionValue);
    }
}


/// Start tracking the incoming set for this atom.
/// An atom can't know what it's incoming set is, until this method
/// is called.  If this atom is added to any links before this call
/// is made, those links won't show up in the incoming set.
///
/// We don't automatically track incoming sets for two reasons:
/// 1) std::set takes up 48 bytes
/// 2) adding and remoiving uses up cpu cycles.
/// Thus, if the incoming set isn't needed, then don't bother
/// tracking it.
void Atom::keep_incoming_set()
{
    if (_incoming_set) return;
    _incoming_set = std::make_shared<InSet>();
}

/// Stop tracking the incoming set for this atom.
/// After this call, the incoming set for this atom can no longer
/// be queried; it si erased.
void Atom::drop_incoming_set()
{
    if (NULL == _incoming_set) return;
    std::lock_guard<std::mutex> lck (_incoming_set->_mtx);
    _incoming_set->_iset.clear();
    // delete _incoming_set;
    _incoming_set = NULL;
}

/// Add an atom to the incoming set.
void Atom::insert_atom(LinkPtr a)
{
    if (NULL == _incoming_set) return;
    std::lock_guard<std::mutex> lck (_incoming_set->_mtx);
    _incoming_set->_iset.insert(a);
    _incoming_set->_addAtomSignal(shared_from_this(), a);
}

/// Remove an atom from the incoming set.
void Atom::remove_atom(LinkPtr a)
{
    if (NULL == _incoming_set) return;
    std::lock_guard<std::mutex> lck (_incoming_set->_mtx);
    _incoming_set->_removeAtomSignal(shared_from_this(), a);
    _incoming_set->_iset.erase(a);
}

// We return a copy here, and not a reference, because the 
// set itself is not thread-safe during reading while 
// simultaneous insertion/deletion.
IncomingSet Atom::getIncomingSet() const
{
    static IncomingSet empty_set;
    if (NULL == _incoming_set) return empty_set;

    // Prevent update of set while a copy is being made.
    std::lock_guard<std::mutex> lck (_incoming_set->_mtx);
    return _incoming_set->_iset;
}
