/*
 * Composition.cc
 *
 * Copyright (C) 2015 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  aPRIL 2015
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

#include <opencog/atoms/bind/BetaRedex.h>
#include <opencog/atoms/bind/SatisfactionLink.h>

#include "PatternMatchEngine.h"
#include "PatternMatchCallback.h"

using namespace opencog;

// Uncomment below to enable debug print
// #define DEBUG
#ifdef DEBUG
	#define dbgprt(f, varargs...) printf(f, ##varargs)
#else
	#define dbgprt(f, varargs...)
#endif

/* ================================================================= */

/* Reset the current variable grounding to the last grounding pushed
 * onto the stack. */
#define POPGND(soln,stack) {         \
	OC_ASSERT(not stack.empty(), "Unbalanced grounding stack"); \
	soln = stack.top();               \
	stack.pop();                      \
}

/* ================================================================= */

void PatternMatchEngine::push_clauses(void)
{
}

void PatternMatchEngine::pop_clauses(void)
{
}

bool PatternMatchEngine::redex_compare(const LinkPtr& lp,
                                       const LinkPtr& lg)
{
	// If we are here, the pattern is defined in a DefineLink. We
	// must match to that. There seem to be two strategies for doing
	// that:  Method A: rename all of the variables in the defined
	// pattern to be the variables we are actually using in the
	// top-level search.  This seems easy, but it is wrong, for two
	// reasons. One reason is that, after renaming, we will have
	// created a pattern that is probably not in the atomspace.
	// That means that the pattern will have atoms with invalid UUID's
	// in them, causing trouble down the line. The other problem is
	// that the variables in the defined target now look like perfectly
	// good grounding candidates, and so get found and reported as valid
	// grounds. So, for these two reasons, the simple, "obvious" method
	// A is out. Instead, we implement method B: we rename the variables
	// that the match engine is carrying, to correspond with the variable
	// names that are native to the definition. This way, insde the body
	// of the definition, everything looks "normal", and should thus
	// proceed as formal.  Of course, on exit, we have to unmasquerade. 
	//
	// By "everything looks normal", we really mean "treat this as if it
	// was a brand-new pattern matching problem".  To do this, it is very
	// empting to just create a new PME, and let it run. The problem with
	// that is that we have no particularly good way of integrating the
	// new pme state, with the existing pme state. So we don't.  Instead,
	// we push all pme state, clear the decks, (almost as if strting from
	// scratch) and then pop all pme state when we are done.
	all_stacks_push();

	BetaRedexPtr cpl(BetaRedexCast(lp));

	// To explore the defined pattern, we've got to get
	// into its local frame. Do this by masquerading
	// any grounded variables we may have so far.
	const HandleSeq& local_args(cpl->get_local_args());
	const HandleSeq& redex_args(cpl->get_args());

// XXX TODO respect  the type definitions, too!!!!
// XXX TODO handle clause_grounding as well

	SolnMap local_grounding;
	size_t sz = redex_args.size();
	for (size_t i=0; i< sz; i++)
	{
		// Relabel (masquerade) the grounded vars.
		auto iter = var_grounding.find(redex_args[i]);
		if (iter == var_grounding.end()) continue;
		local_grounding.insert({local_args[i], iter->second});
	}
	var_grounding = local_grounding;

	// Now, get the set of clauses to be grounded. We expect
	// the redex body to be a SatisfactionLink; we have already
	// remapped its variable declarations; now get its body.
	Handle hsat(cpl->get_definition());
	SatisfactionLinkPtr sat_link(SatisfactionLinkCast(hsat));
	if (NULL == sat_link)
		throw InvalidParamException(TRACE_INFO,
			"Expecting SatisfactionLink, got %s",
				hsat->toString().c_str());

	const HandleSeq& local_clauses(sat_link->get_clauses());
	if (1 != local_clauses.size())
		throw InvalidParamException(TRACE_INFO,
			"More than one clause - not implemented");

	Handle local_pattern(local_clauses[0]);
printf("duuuude ready to compare pat=%s to gnd=%s\n",
local_pattern->toString().c_str(), lg->toString().c_str());

	// Now, proceed as normal.
	std::set<Handle> saved_vars = _bound_vars;
	_bound_vars = cpl->get_local_argset();
	bool have_match = tree_compare(local_pattern, Handle(lg));
	_bound_vars = saved_vars;

	// No match; restore original grounding and quit
	if (not have_match)
	{
		all_stacks_pop();
		return false;
	}

	// If there is a match, then maybe we grounded some variables.
	// If so, we need to unmasquerade them.
	local_grounding = var_grounding;
	all_stacks_pop();
	for (size_t i=0; i< sz; i++)
	{
		auto iter = local_grounding.find(local_args[i]);
		if (iter != local_grounding.end())
			var_grounding.insert({redex_args[i], iter->second});
	}

	return true;
}

/* ===================== END OF FILE ===================== */
