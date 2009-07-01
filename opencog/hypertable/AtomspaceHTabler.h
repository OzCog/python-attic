/*
 * opencog/hypertable/AtomspaceHTabler.h
 *
 * Copyright (C) 2009 Jeremy Schlatter <jeremy.schlatter@gmail.com>
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

#ifndef _OPENCOG_ATOMSPACE_HTABLER_H
#define _OPENCOG_ATOMSPACE_HTABLER_H

#include "Common/Compat.h"

#include <opencog/atomspace/HandleEntry.h>
#include <opencog/atomspace/HandleSet.h>

#include "Hypertable/Lib/Client.h"

namespace opencog
{

/**
 * Persistent Atom storage, backed by Hypertable.
 */
class AtomspaceHTabler
{
private:
        Client *client_ptr;
public:
        AtomspaceHTabler(){
            client_ptr = new Client();
        }      
        
        virtual ~AtomspaceHTabler(){
            delete client_ptr;
        }

		/**
		 * Recursively store the atom and anything in its outgoing set.
		 * If the atom is already in storage, this will update it's 
		 * truth value, etc. 
		 */
		virtual void storeAtom(Handle);
		
		/** 
		 * Return a pointer to an Atom associated with the given
		 * handle, if it exists; else return NULL.
		 */
		virtual Atom * getAtom(Handle) const;

};

} // namespace opencog

#endif // _OPENCOG_ATOMSPACE_HTABLER_H
