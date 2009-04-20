/*
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008 by Singularity Institute for Artificial Intelligence
 * All Rights Reserved
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

#include <opencog/util/exceptions.h>

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include <boost/variant/static_visitor.hpp>
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>

#include <opencog/atomspace/HandleTemporalPair.h>
#include <opencog/atomspace/ClassServer.h>
#include <opencog/atomspace/utils.h>

#include "PLN.h"

#include "rules/Rules.h"
#include "PLNEvaluator.h"
#include "AtomSpaceWrapper.h"
#include "PLNatom.h"
#include "BackInferenceTreeNode.h"

//#include "spacetime.h"
//#include "TimeStamp.h"

#include "PLNUtils.h"

#ifndef USE_PSEUDOCORE
#include "opencog/atomspace/TimeServer.h"
#endif

//class HandleEntry;
namespace test
{
extern FILE *logfile;
}

namespace haxx
{
// extern reasoning::iAtomSpaceWrapper* defaultAtomSpaceWrapper;
bool printRealAtoms = false;
pHandle VarTypes[STD_VARS];
}

/// Legacy log system from Novamente, TODO: replace
int currentDebugLevel = NORMAL;

int cprintf(int debugLevel, const char *format, ...)
{
    if (debugLevel > currentDebugLevel) return 0;
    va_list ap;
    va_start(ap, format);
    int answer = vprintf(format, ap);
    fflush(stdout);
    va_end(ap);
    return answer;
}
/// End legacy

/**
 * Implementation of methods for logging info from reasoning module
 **/
void ReasoningLog(int l, std::string m)
{
    cprintf(l, "%s\n", m.c_str());
}

void rawPrint(tree<Vertex>& t, tree<Vertex>::iterator top, int level, int _rloglevel);
void rawPrint(tree<Vertex>::iterator top, int level, int _rloglevel);

void rawPrint(tree<Vertex>& t, tree<Vertex>::iterator top, int _rloglevel)
{
    rawPrint(t, top, 0, _rloglevel);
}
void rawPrint(tree<Vertex>::iterator top, int _rloglevel)
{
    rawPrint(top, 0, _rloglevel);
}

void rawPrint(tree<Vertex>& t, tree<Vertex>::iterator top, int level, int _rloglevel)
{
    AtomSpaceWrapper *atw = GET_ATW;
    if (_rloglevel > currentDebugLevel)
        return;

    pHandle *hptr = boost::get<pHandle>(&*top);
    if (!hptr) {
        printf("null\n");
        return;
    }

    for (int i = 0; i < level; i++) {
        cprintf(_rloglevel, "   ");
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "   ");
    }

    if (hptr && !atw->isType(*hptr)) {
//puts("reasoning::printTree");
        if (!::haxx::printRealAtoms) {
            cprintf(_rloglevel, "[%u]\n", (*hptr));
            if (test::logfile && _rloglevel >= currentDebugLevel)
                fprintf(test::logfile, "[%u]\n", (*hptr));
        } else
            reasoning::printTree(*hptr, level, _rloglevel);

        return;
    }


    IntegerWrapper* iptr;

    if (hptr) {
        cprintf(_rloglevel, "%s (%d)\n", reasoning::Type2Name((Type)(*hptr)), top.number_of_children());
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "%s (%d)\n", reasoning::Type2Name((Type)(*hptr)), top.number_of_children());
    } else if ( (iptr = boost::get<IntegerWrapper>(&*top)) != NULL) {
        cprintf(_rloglevel, "%d (%d)\n", iptr->value, top.number_of_children());
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "%d (%d)\n", iptr->value, top.number_of_children());
    } else {
        cprintf(_rloglevel, "Unsupported Vertex mode\n");
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "Unsupported Vertex mode\n");
    }

    for (tree<Vertex>::sibling_iterator c = t.begin(top);
            c != t.end(top);c++)
        rawPrint(t, c, level + 1, _rloglevel);
}

void rawPrint(tree<Vertex>::iterator top, int level, int _rloglevel)
{
    AtomSpaceWrapper* atw = GET_ATW;

    if (_rloglevel > currentDebugLevel)
        return;

    pHandle *hptr = boost::get<pHandle>(&*top);
    if (!hptr) {
        printf("null\n");
        return;
    }

    for (int i = 0; i < level; i++) {
        cprintf(_rloglevel, "   ");
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "   ");
    }

    if (hptr && !atw->isType(*hptr)) {
//puts("reasoning::printTree");
        if (!::haxx::printRealAtoms) {
            cprintf(_rloglevel, "[%d]\n", (*hptr));
            if (test::logfile && _rloglevel >= currentDebugLevel)
                fprintf(test::logfile, "[%u]\n", (*hptr));
        } else
            reasoning::printTree(*hptr, level, _rloglevel);

        return;
    }


    IntegerWrapper* iptr;

    if (hptr) {
        cprintf(_rloglevel, "%s (%d)\n", reasoning::Type2Name((Type)(*hptr)), top.number_of_children());
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "%s (%d)\n", reasoning::Type2Name((Type)(*hptr)), top.number_of_children());
    } else if ( (iptr = boost::get<IntegerWrapper>(&*top)) != NULL) {
        cprintf(_rloglevel, "%d (%d)\n", iptr->value, top.number_of_children());
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "%d (%d)\n", iptr->value, top.number_of_children());
    } else {
        cprintf(_rloglevel, "Unsupported Vertex mode\n");
        if (test::logfile && _rloglevel >= currentDebugLevel)
            fprintf(test::logfile, "Unsupported Vertex mode\n");
    }

    for (tree<Vertex>::sibling_iterator c = top.begin();
            c != top.end();c++)
        rawPrint(c, level + 1, _rloglevel);

}

string repeatc(const char c, const int count)
{
    string ret;
    ret.reserve(count + 1);
    return ret.assign(count, c);
}

namespace reasoning
{
bool unifiesWithVariableChangeTo(const vtree & lhs_t, const vtree & rhs_t,
                                 vtree::sibling_iterator ltop, vtree::sibling_iterator rtop,
                                 map<pHandle, pHandle>& bindings);


bool unifiesWithVariableChangeTo(const vtree & lhs_t, const vtree & rhs_t,
                                 map<pHandle, pHandle>& bindings)
{
// bindings.clear();
    return unifiesWithVariableChangeTo(lhs_t, rhs_t, lhs_t.begin(), rhs_t.begin(), bindings);
}

bool unifiesWithVariableChangeTo(const vtree & lhs_t, const vtree & rhs_t,
                                 vtree::sibling_iterator ltop, vtree::sibling_iterator rtop,
                                 map<pHandle, pHandle>& bindings)
{
    AtomSpaceWrapper* atw = GET_ATW;

    if (lhs_t.empty() && rhs_t.empty()) {
        cprintf(4, "unify: Both sides empty!\n");
        return true;
    }
    if (lhs_t.empty() || rhs_t.empty()) {
        cprintf(4, "unify: One side empty!\n");
        return false;
    }

    if (!((*rtop) == (*ltop))) {
        pHandle *ph_ltop = boost::get<pHandle>(&*ltop);
        pHandle *ph_rtop = boost::get<pHandle>(&*rtop);

        if (!ph_ltop || !ph_rtop)
            return false;

        if (atw->isType(*ph_ltop) || atw->isType(*ph_rtop))
            return false;

        bool lhs_is_node = atw->inheritsType(atw->getType(*ph_ltop), NODE);
        bool lhs_is_var  = lhs_is_node && atw->inheritsType(atw->getType(*ph_ltop), FW_VARIABLE_NODE);

        if (!lhs_is_var)
            return false;

        bool rhs_is_node = atw->inheritsType(atw->getType(*ph_rtop), NODE);
        bool rhs_is_var  = rhs_is_node && atw->inheritsType(atw->getType(*ph_rtop), FW_VARIABLE_NODE);

        if (!rhs_is_var)
            return false;

        map<pHandle, pHandle>::const_iterator s = bindings.find(*ph_ltop);

        if (s == bindings.end()) {
            bindings[*ph_ltop] = *ph_rtop;
            return true;
        } else {
            return s->second == *ph_rtop;
        }
    }

    vtree::sibling_iterator rit = rhs_t.begin(rtop);
    vtree::sibling_iterator lit = lhs_t.begin(ltop);

    if (rtop.number_of_children() != ltop.number_of_children())
        return false;

    while (lit != lhs_t.end(ltop))
        if (!unifiesWithVariableChangeTo(lhs_t, rhs_t, lit++, rit++, bindings))
            return false;

    return true;
}

string make_subst_buf(const BoundVertex& a);

/*struct expand //: public unary_function<, atom, void>
{
 operator()(Rule* r)
 {
 }
};*/

pHandle getOutgoingFun::operator()(pHandle h, int i)
{
    return GET_ATW->getOutgoing(h, i);
}

bool equal_vectors(pHandle* lhs, int lhs_arity, pHandle* rhs)
{
    for (int i = 0; i < lhs_arity; i++)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

bool within(float a, float b, float diff)
{
    float d = (a - b);
    if (d < 0)
        d = -d;
    return (d <= diff);
}

//Ok, I got too excited with these conv

void convertTo(const VertexVector& args, auto_ptr<Handle>& ret)
{
    const int N = (int)args->size();
    ret = auto_ptr<Handle>(new Handle[N]);

    for (int i = 0; i < N; i++)
        ret.get()[i] = boost::get<Handle>((*args)[i]);
}

void convertTo(const vector<Vertex>& args, auto_ptr<Handle>& ret)
{
    const int N = (int)args.size();
    ret = auto_ptr<Handle>(new Handle[N]);

    for (int i = 0; i < N; i++)
        ret.get()[i] = boost::get<Handle>(args[i]);
}

void convertTo(const vector<BoundVertex>& args, auto_ptr<Handle>& ret)
{
    const int N = (int)args.size();
    ret = auto_ptr<Handle>(new Handle[N]);

    for (int i = 0; i < N; i++)
        ret.get()[i] = boost::get<Handle>(args[i].value);
}

void convertTo(const set<BoundVertex>& args, auto_ptr<Handle>& ret)
{
    const int N = (int)args.size();
    ret = auto_ptr<Handle>(new Handle[N]);
    int i = 0;
    for (set<BoundVertex>::const_iterator k = args.begin(); k != args.end(); k++, i++)
        ret.get()[i] = boost::get<Handle>(k->value);
}

void convertTo(const VertexSet& args, auto_ptr<Handle>& ret)
{
    const int N = (int)args->size();
    ret = auto_ptr<Handle>(new Handle[N]);
    int i = 0;
    for (set<Vertex>::iterator k = args->begin(); k != args->end(); k++, i++)
        ret.get()[i] = boost::get<Handle>(*k);
}

void convertTo(const vector<Handle>& args, auto_ptr<Handle>& ret)
{
    const int N = (int)args.size();
    ret = auto_ptr<Handle>(new Handle[N]);
    for (int i = 0; i < N; i++)
        ret.get()[i] = args[i];
}

void convertTo(const VertexVector& args, Handle*& ret)
{
    const int N = (int)args->size();
    ret = (new Handle[N]);

    for (int i = 0; i < N; i++)
        ret[i] = boost::get<Handle>((*args)[i]);
}

void convertTo(const vector<Vertex>& args, Handle*& ret)
{
    const int N = (int)args.size();
    ret = (new Handle[N]);

    for (int i = 0; i < N; i++)
        ret[i] = boost::get<Handle>(args[i]);
}

void convertTo(const vector<BoundVertex>& args, Handle*& ret)
{
    const int N = (int)args.size();
    ret = (new Handle[N]);

    for (int i = 0; i < N; i++)
        ret[i] = boost::get<Handle>(args[i].value);
}

void convertTo(const vector<BoundVertex>& args, HandleSeq& ret)
{
    const int N = (int)args.size();

    for (int i = 0; i < N; i++)
        ret.push_back(boost::get<Handle>(args[i].value));
}

void convertTo(const set<BoundVertex>& args, Handle*& ret)
{
    const int N = (int)args.size();
    ret = (new Handle[N]);
    int i = 0;
    for (set<BoundVertex>::const_iterator k = args.begin(); k != args.end(); k++, i++)
        ret[i] = boost::get<Handle>(k->value);
}

void convertTo(const VertexSet& args, Handle*& ret)
{
    const int N = (int)args->size();
    ret = (new Handle[N]);
    int i = 0;
    for (set<Vertex>::iterator k = args->begin(); k != args->end(); k++, i++)
        ret[i] = boost::get<Handle>(*k);
}

void convertTo(const vector<Handle>& args, Handle*& ret)
{
    const int N = (int)args.size();
    ret = (new Handle[N]);
    for (int i = 0; i < N; i++)
        ret[i] = args[i];
}


bool substitutableTo(atom& from, atom& to,
                     map<string, atom>& bindings,
                     const set<subst>& forbiddenBindings,
                     pair<string, atom>** restart_from,
                     pair<string, atom>** restart_to)
{
    /*LOG(4, "-Source");
     printAtomTree(from,0,4);
     LOG(4, "-SubstitutableTo:");
     printAtomTree(to,0,4);
     LOG(4, "vee");*/

#ifdef WIN32
#pragma warning("FW_VARIABLE_NODE substitution to the wrong direction is re-allowed!")
#endif
//#warning "FW_VARIABLE_NODE substitution to the wrong direction is re-allowed!"
    /*
     map<string, atom>::const_iterator s = bindings.find(to.name);
     assert(s == bindings.end());
    */
    /* if (atw->inheritsType(from.T, FW_VARIABLE_NODE))
      cprintf(0, "Warning! FW_VARIABLE_NODE substitution to the wrong direction was used!");*/
    /// This would complicate things...
// assert(!atw->inheritsType(from.T, FW_VARIABLE_NODE));
    AtomSpaceWrapper* atw = GET_ATW;

    if (atw->inheritsType(to.T, FW_VARIABLE_NODE) && to.name == from.name)
        return true;

    if (atw->inheritsType(to.T, FW_VARIABLE_NODE)) {
        if (!STLhas(forbiddenBindings, subst(to.name, from))) { //**next_from)))
            bindings[to.name] = from; //(from, next_from,false);
            *restart_to = new pair<string, atom>(to.name, from);

            LOG(4, "FW subst var" + to.name);

            return true;
        } else
            return false;
    }
    LOG(5, "C");
    if (from.T != to.T || from.name != to.name) {
        LOG(5, "D");
        if (atw->inheritsType(from.T, VARIABLE_NODE)) {
            LOG(5, "E");

            *restart_from = new pair<string, atom>(from.name, to);

            return true;
        } else
            return false;
    }
    LOG(5, "G");
    if (from.hs.size() != to.hs.size()) {
        return false;
    }

    for (uint i = 0; i < from.hs.size(); i++) {
        if (!substitutableTo( *from.hs[i], *to.hs[i],
                              bindings, forbiddenBindings, restart_from, restart_to)) {
            return false;
        }

        if (*restart_from || *restart_to) {
            return true;
        }
    }


    LOG(5, "I");
    return true;
}


const float MIN_CONFIDENCE = 0.0000001f;

/// index arg is not used

void TableGather::gather(tree<Vertex>& _MP, AtomLookupProvider* aprovider,
                         const Type VarT, int index)
{
    AtomSpaceWrapper* atw = GET_ATW;

    LOG(3, "BEGIN GATHER for:");
    rawPrint(_MP, _MP.begin(), 3);

    pHandle *h_ptr = boost::get<pHandle>(&*_MP.begin());
    if (!h_ptr || *h_ptr == PHANDLE_UNDEFINED ) {
        LOG(3, "No handle in vertex.");
        return;
    }

    Type T = atw->getType(*h_ptr);

    /// Needs to be handled separately.
    if (!atw->isType(*h_ptr) && T != VarT) {
        LOG(2, "TableGather:: Note: A real handle was asked for.\n");
        insert(Vertex(*h_ptr));
        return;
    }
    if (!atw->isType(*h_ptr) && T == VarT) {
        LOG(0, "Lookup for a VarT is NOT allowed!\n");
        return;
    }

    string name(atw->inheritsType(T, NODE) ? atw->getName(*h_ptr) : "");

    /// First, we fill this vector (TableGather object),
    /// then, we intersect it one by one by each new set of looup child_results.

    /* if (atw->inheritsType(T, HYPOTHETICAL_LINK)) /// Just return the query, with stub TV
     {
      insert(Vertex(aprovider->addAtom(_MP, TruthValue::TRIVIAL_TV(), false)));
     }
     else
     {*/
    Btr<set<pHandle> > lookupResults = aprovider->getHandleSet(T, name);

    cprintf(3, "%u objects matched the type %d.\n", (uint) lookupResults->size(), T);

    /// If _MP has children, they must match!

    for (set<pHandle>::iterator i = lookupResults->begin();
            i != lookupResults->end(); i++) {
        const TruthValue& tv = atw->getTV(*i);

        if (tv.isNullTv()) {
            printf("NULL TV! %d\n", *i);
            continue;
        }

        if (tv.getConfidence() < MIN_CONFIDENCE) {
            //printf("TableGather::gather; not enough confidence. Continuing...\n");
            continue;
        }

        atom fetched_a(*i);

        bindingsT* tentative_bindings = new bindingsT;

        cprintf(4, "Call unifyvector...\n");

        if (_MP.size() <= 1
                || (reasoning::MPunifyVector(_MP, _MP.begin(), fetched_a.hs,
                                             *tentative_bindings, NULL, NULL, VarT))
           ) {
            LOG(3, "Was valid search result by unification.");

            insert(BoundVertex(*i, tentative_bindings));

            /*/// DEBUG
            cprintf(2,"TableGather:\n");
                printTree(*i,0,2);
                for (bindingsT::const_iterator b = tentative_bindings->begin();
                           b!= tentative_bindings->end();
                           b++)
                {
                 LOG(2, "Next binding:");
                 printBinding(*b);
                 printTree(b->second,0,2);

                }
            /// END OF DEBUG*/
        } else {
            LOG(4, "Was invalid search result by unification.");

            delete tentative_bindings;
        }
    }
    //}

    // IF NO RESULT WAS FOUND, CHECK FOR SPECIAL CASES
    cprintf(0, "TABLEGATHER END: SIZE = %d\n", size());

#ifndef USE_PSEUDOCORE

    if (size() <= 0) {
        // Case 1:
        //     Evaluation
        //       PredicateNode "atTime"
        //       List
        //         A
        //         NumberNode "t"
        // for a given vtree A and some number t, then:
        //    find whether there is an atom with structure A and timestamp T,
        //    and if so, create a new atom with the above-mentioned EvaluationLink structure,
        //    and assign it the truthvalue <1,1>. Then, you can return it as a
        //    succesful lookup result.

        // Case 2:
        //     Evaluation
        //       PredicateNode "atInterval"
        //       List
        //         A
        //         NumberNode "t1"
        //         NumberNode "t2"
        //
        // for a given vtree A and numbers t1 and t2
        //    find whether there is an atom with structure A and timestamp T between t1 and t2.
        //    and if so, same as Case1.

        tree<Vertex>::pre_order_iterator rootNode = _MP.begin();
        pHandle rootHandle = boost::get<pHandle>(*rootNode);
        if (rootHandle == (pHandle) EVALUATION_LINK) {
            if (rootNode.number_of_children() == 2) {
                tree<Vertex>::sibling_iterator child = rootNode.begin();
                pHandle predicateHandle = boost::get<pHandle>(*child++);
                tree<Vertex>::pre_order_iterator listNode = child;
                pHandle listHandle = boost::get<pHandle>(*listNode);
                if (listHandle == (pHandle) LIST_LINK) {
                    Type t = atw->getType(predicateHandle);
                    if (t == PREDICATE_NODE) {
                        std::string nameStr = atw->getName(predicateHandle);
                        const char* name = nameStr.c_str();
                        if (!strcmp(name, "atTime")) {
                            if (listNode.number_of_children() == 2) {
                                tree<Vertex>::sibling_iterator child = listNode.begin();
                                tree<Vertex>::pre_order_iterator ANode = child++;
                                pHandle tHandle = boost::get<pHandle>(*child);
                                if (atw->getType(tHandle) == NUMBER_NODE) {
                                    // Case 1:
                                    printf("Case 1 detected\n");
                                    // Gets the A subtree as a tree<Vertex>
                                    tree<Vertex> ATree(ANode);
                                    TableGather matchingSet(ATree, aprovider, VarT);
                                    TableGather::iterator itr = matchingSet.begin();
                                    while (itr != matchingSet.end()) {
                                        pHandle matchingHandle = boost::get<pHandle>(itr->value);
                                        cout << "got a matching element for A: " << matchingHandle << endl;
                                        // Checks if the matching atom satisfy the time condition
                                        unsigned long tLong = atol(atw->getName(tHandle).c_str());
                                        //TimeStamp* ts = new TimeStamp(false, tLong);
                                        Temporal* ts = new Temporal(false, tLong, tLong);
                                        std::list<HandleTemporalPair> timeEntries;
                                        cout << "Looking for HandleTime entries with the exact timestamp: " << ts->toString() << endl;
                                        Handle realMatchingHandle = atw->fakeToRealHandle(matchingHandle).first; // what about the version handle?
                                        atw->getTimeServer().get(back_inserter(timeEntries), realMatchingHandle, *ts, TemporalTable::EXACT);
                                        if (timeEntries.size() > 0) {
                                            cout << "matched element satisfies the time condition " << timeEntries.front().toString() << endl;
                                            // Creates the link and adds it to the result
                                            char tNodeName[100];
                                            sprintf(tNodeName, "%lu", tLong);
                                            pHandle tNode = atw->addNode(NUMBER_NODE, tNodeName, TruthValue::NULL_TV(), false, true);
                                            pHandleSeq listLinkOutgoing;
                                            listLinkOutgoing.push_back(matchingHandle);
                                            listLinkOutgoing.push_back(tNode);
                                            pHandle listLink = atw->addLink(LIST_LINK, listLinkOutgoing, TruthValue::NULL_TV(), false, true);
                                            pHandle atTimePredNode = atw->addNode(PREDICATE_NODE, "atTime", TruthValue::NULL_TV(), false, true);
                                            pHandleSeq evalLinkOutgoing;
                                            evalLinkOutgoing.push_back(atTimePredNode);
                                            evalLinkOutgoing.push_back(listLink);
                                            pHandle evalLink = atw->addLink(EVALUATION_LINK, evalLinkOutgoing, SimpleTruthValue(1.0f, SimpleTruthValue::confidenceToCount(1.0f)), true, true);
                                            if (itr->bindings) {
                                                insert(BoundVertex(evalLink, itr->bindings));
                                            } else {
                                                insert(Vertex(evalLink));
                                            }
                                        }
                                        delete ts;
                                        itr++;
                                    }
                                }
                            }
                        } else if (!strcmp(name, "atInterval")) {
                            if (listNode.number_of_children() == 3) {
                                tree<Vertex>::sibling_iterator child = listNode.begin();
                                tree<Vertex>::pre_order_iterator ANode = child++;
                                pHandle t1Handle = boost::get<pHandle>(*child++);
                                pHandle t2Handle = boost::get<pHandle>(*child);
                                if (atw->getType(t1Handle) == NUMBER_NODE &&
                                        atw->getType(t2Handle) == NUMBER_NODE) {
                                    // Case 2:
                                    printf("Case 2 detected\n");
                                    tree<Vertex> ATree(ANode);
                                    TableGather matchingSet(ATree, aprovider, VarT);
                                    TableGather::iterator itr = matchingSet.begin();
                                    while (itr != matchingSet.end()) {
                                        pHandle matchingHandle = boost::get<pHandle>(itr->value);
                                        cout << "got a matching element for A: " << matchingHandle << endl;
                                        // Checks if the matching atom satisfy the time condition
                                        unsigned long t1Long = atol(atw->getName(t1Handle).c_str());
                                        unsigned long t2Long = atol(atw->getName(t2Handle).c_str());
                                        Temporal* tl = new Temporal(t1Long, t2Long);
                                        std::list<HandleTemporalPair> timeEntries;
                                        cout << "Looking for HandleTime entries inside the following temporal: " << tl->toString() << endl;
                                        //! @TODO: is STARTS_WITHIN correct?
                                        Handle realMatchingHandle = atw->fakeToRealHandle(matchingHandle).first; // TODO: what about the version handle? 
                                        atw->getTimeServer().get(back_inserter(timeEntries), realMatchingHandle, *tl, TemporalTable::STARTS_WITHIN);
                                        if (timeEntries.size() > 0) {
                                            cout << "matched element satisfies the time condition: " << timeEntries.front().toString() << endl;
                                            // Creates the link and adds it to the result
                                            char tNodeName[100];
                                            sprintf(tNodeName, "%lu", t1Long);
                                            pHandle t1Node = atw->addNode(NUMBER_NODE, tNodeName, TruthValue::NULL_TV(), false, true);
                                            sprintf(tNodeName, "%lu", t2Long);
                                            pHandle t2Node = atw->addNode(NUMBER_NODE, tNodeName, TruthValue::NULL_TV(), false, true);
                                            pHandleSeq listLinkOutgoing;
                                            listLinkOutgoing.push_back(matchingHandle);
                                            listLinkOutgoing.push_back(t1Node);
                                            listLinkOutgoing.push_back(t2Node);
                                            pHandle listLink = atw->addLink(LIST_LINK, listLinkOutgoing, TruthValue::NULL_TV(), false, true);
                                            pHandle atTimePredNode = atw->addNode(PREDICATE_NODE, "atInterval", TruthValue::NULL_TV(), false, true);
                                            pHandleSeq evalLinkOutgoing;
                                            evalLinkOutgoing.push_back(atTimePredNode);
                                            evalLinkOutgoing.push_back(listLink);
                                            pHandle evalLink = atw->addLink(EVALUATION_LINK, evalLinkOutgoing, SimpleTruthValue(1.0f, SimpleTruthValue::confidenceToCount(1.0f)), true, true);

                                            cprintf(1, "Created new atInterval link:\n");
                                            printTree(evalLink, 0, 1);
                                            if (itr->bindings) {
                                                insert(BoundVertex(evalLink, itr->bindings));
                                            } else {
                                                insert(Vertex(evalLink));
                                            }
                                        }
                                        delete tl;
                                        itr++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#endif
}

TableGather::TableGather(tree<Vertex>& _MP, AtomLookupProvider* aprovider, const Type VarT, int index)
{
//printf("Gather...\n");
    gather(_MP, (aprovider ? aprovider : ASW()), VarT, index);
// printf("----------------------\n");
}

template<>
void weak_atom<Btr<tree<Vertex> > >::apply_bindings()
{
    AtomSpaceWrapper* atw = GET_ATW;
    for (tree<Vertex>::iterator v = value->begin(); v != value->end(); v++) {
        pHandle *ph = boost::get<pHandle>(&*v);
        if (ph && (atw->inheritsType((Type)*ph, NODE))) {
            bindingsT::iterator substed = bindings->find(*ph);
            if (substed != bindings->end())
                *v = Vertex(substed->second);
        }
    }
}

/// Slow, and doesn't handle the meta-level AND,OR,NOT operations.

template<>
bool weak_atom<Btr<tree<Vertex> > >::operator()(pHandle h)
{
    ///haxx:: ATOM is the completely free variable.
    if (h == ATOM)
        return true;

    bool restart;
    //bindingsT bindings;
    set<hsubst>* forbiddenBindings = NULL;
    bindings.reset(new bindingsT);

    atom rhs(h);
    bool ok = false;
    do {
        ok = reasoning::MPunify1(*value, value->begin(), rhs, *bindings, //bindings,
                                 &forbiddenBindings,
                                 &restart, FW_VARIABLE_NODE);
    } while (restart);

    return ok;

    /* int s=0;

    bool echo=false;
    #define p(str) if (echo) puts(str);


     /// haxx:

     if (T == ATOM)
      return true;

     if (atw->inheritsType(T, RESTRICTOR))
     {
      bool unnormed_ret = false;
      bool normal_form = (T == __INSTANCEOF_N || T == __EQUALS_N);

      switch(T)
      {
      case __INSTANCEOF_N:
       unnormed_ret = atw->inheritsType(atw->getType(h), hs[0].T);

       break;

      case __EQUALS_N:

       unnormed_ret = (atom(h) == this->hs[0]);

       break;
      }

      if (!unnormed_ret && normal_form)
      {
       if (atw->inheritsType(atw->getType(h), FORALL_LINK))
        return (*this)(getOutgoing(h)[1]);
       else if (atw->inheritsType(hs[0].T, FORALL_LINK))
        return (*((MetaPredicate*)&hs[1]))(h);
       else
        return false;
      }
      else if (normal_form)
       return true;

      assert(0);
     }

     switch(T)
     {
     case AND_LINK: p("AND");
      for (s = 0; s < hs.size(); s++)
       if (!(*((MetaPredicate*)(&hs[s])))(h))
       {
        p("no");
        return false;
       }
       p("yes");
       return true;
       break;
     case OR_LINK: case __OR: p("OR");
      for (s = 0; s < hs.size(); s++)
       if ((*((MetaPredicate*)(&hs[s])))(h))
       {
        p("yes");
        return true;
       }
       p("no");
       return false;
       break;
     case NOT_LINK: for (s = 0; s < hs.size(); s++) //Actually this is "NOT AND"
          if (!(*((MetaPredicate*)(&hs[s])))(h))
          {
           p("yes");
           return true;
          }
          p("no");
          return false;
          break;
     }

     return (this->real == h || (*this) == atom(h));*/
}

bool existMPin(const vector<Btr<atom> >& hs)
{
    AtomSpaceWrapper* atw = GET_ATW;
    for (vector<Btr<atom> >::const_iterator i = hs.begin(); i != hs.end(); i++)
        if (atw->inheritsType((*i)->T, RESTRICTOR))
            return true;
        else if (!atw->inheritsType((*i)->T, NODE))
            return existMPin((*i)->hs);

    return false;
}

// TODELETE: not used anywhere
// void HandleEntry2HandleSeq(HandleEntry& src, vector<Handle>& dest);

//bool equal(const HandleWrapper& A, const HandleWrapper& B) { return A.equal(B); }
//bool isLink(int v) { return v >= 2; }

bool getLargestIntersection(const set<pHandle>& keyelem_set, const set<pHandle>& link_set, pHandle& result)
{
    AtomSpaceWrapper* atw = GET_ATW;

    int max_size = 0;

    for (set<pHandle>::const_iterator i = link_set.begin(); i != link_set.end(); i++) {
        if (*i == PHANDLE_UNDEFINED) {
            LOG(2, "getLargestIntersection(): NULL in input set");
            continue;
        }
//     const vector<Handle> inc2 = getOutgoing(*i);
//  const vector<Handle>* inc = &inc2;
        pHandleSeq inc = atw->getOutgoing(*i);
        int inc_arity = atw->getArity(*i);

//  string s = atw->getName(*keyelem_set.begin());
//  s = atw->getName(*keyelem_set.end());

        //bool is_subset = includes(keyelem_set.begin(), keyelem_set.end(), inc->begin(), inc->end());
        if (inc_arity > max_size
                &&  includes( keyelem_set.begin(),
                              keyelem_set.end(),
                              &inc[0], &inc[inc_arity])) {
            result = *i;
            max_size = inc_arity;
        }
    }

    return (max_size > 0);
}

bool getLargestIntersection2(const set<atom, lessatom>& keyelem_set,
                             const pHandleSeq& link_set, std::vector<Btr<atom> >& result)
{
    AtomSpaceWrapper* atw = GET_ATW;

    if (keyelem_set.empty())
        return false;

    uint max_size = 0;

    for (pHandleSeq::const_iterator i = link_set.begin(); i != link_set.end(); i++) {
        //const vector<Handle> inc2 = getOutgoing(*i);
        assert(!atw->isType(*i));
        atom atom_i(*i);
        vector<atom> base_set;

        foreach(Btr<atom> a, atom_i.hs)
        base_set.push_back(*a);

        bool is_subset = (base_set.empty()
                          && includes(keyelem_set.begin(), keyelem_set.end(),
                                      base_set.begin(), base_set.end()));

        if (is_subset && base_set.size() > max_size) {
            result.clear();
            foreach(const atom& a, base_set)
            result.push_back(Btr<atom>(new atom(a)));
            max_size = base_set.size();
        }
    }
    return (max_size > 0);
}

void printNode1(pHandle h, int level, int LogLevel)
{
    AtomSpaceWrapper* atw = GET_ATW;

    Type t = atw->getType(h);
    const TruthValue& tv = atw->getTV(h);

    char buf[500];

    if (!tv.isNullTv())
        sprintf(buf, "%s:%s (%d) <%.6f, %.6f>\t[%d]", atw->getName(h).c_str(), reasoning::Type2Name(t), t, tv.getMean(), tv.getConfidence(), h);
    else
        sprintf(buf, "NULL TV!");

    if (test::logfile && LogLevel >= currentDebugLevel)
        fprintf(test::logfile, "%s\n", buf);

    //if (LogLevel <= Log::getDefaultLevel())
    LOG(LogLevel, (repeatc(' ', level*3) + buf).c_str());

// LOG0(LogLevel, repeatc(' ', level*3) + buf);
}

void printTree(pHandle h, int level, int LogLevel)
{
    AtomSpaceWrapper* atw = GET_ATW;

    if (LogLevel > currentDebugLevel)
        return;

    if (level > 20) {
        level = 20;
    }

    if (h == PHANDLE_UNDEFINED) {
        //LOG0(LogLevel, repeatc(' ', level*3) + "NULL HANDLE!");
        return;
    }

    if (atw->isType(h)) {
        char buf[500];
        Type t = (Type) h;

        sprintf(buf, "Virtual %s (%d)\n", reasoning::Type2Name(t), t);

        if (test::logfile && LogLevel >= currentDebugLevel)
            fprintf(test::logfile, "%s", buf);

        printf("%s",(repeatc(' ', level*3) + buf ).c_str());
        return;
    }

    if (atw->getArity(h) == 0) {
        printNode1(h, level, LogLevel);
    } else {
        vector<pHandle> hs = atw->getOutgoing(h);
        Type t = atw->getType(h);
        const TruthValue& tv = atw->getTV(h);

        char buf[500];

        if (!tv.isNullTv())
            sprintf(buf, "%s:%s (%d) <%.6f, %.6f>\t[%d]", atw->getName(h).c_str(),
                    reasoning::Type2Name(t), t, tv.getMean(), tv.getConfidence(),
                    h);
        else
            sprintf(buf, "NULL TV!");

        if (test::logfile && LogLevel >= currentDebugLevel)
            fprintf(test::logfile, "%s\n", buf);

        LOG(LogLevel, (repeatc(' ', level*3) + buf ).c_str());

        for (vector<pHandle>::const_iterator hi = hs.begin(); hi != hs.end(); hi++)
            printTree(*hi, level + 1, LogLevel);
    }

}

map<int, Type> counter;

struct countAtom {
    static bool exec(pHandle h, int level) {
        AtomSpaceWrapper* atw = GET_ATW;
        Type t = atw->getType(h);
        counter[t]++;

        return true;
    }
};

namespace haxx {
int run_index[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                  };
}

int _rcount = 0;

string GetRandomString(int size)
{
    char s[1000];

    _rcount++;

    if (!(_rcount++ % 1000))
        cprintf(0, "rcount: %d\n", _rcount);

    ///haxx
    size = 10;

    assert(size < 30);

    for (int i = 0; i < size - 1; i++)
        s[i] = 'A' + haxx::run_index[i];
    s[size-1] = 0;

    int j = 0;
    for (j = size - 2;j >= 0 && haxx::run_index[j] == ('Z' -'A'); j--);

    if (j >= 0) {
        haxx::run_index[j]++;

        for (int k = j + 1; k < size - 1; k++)
            haxx::run_index[k] = 0;
    } else {
        printf("OUT OF RANDOM STRINGS FOR THE GIVEN SIZE %d!", size);
        throw string("OUT OF RANDOM STRINGS FOR THE GIVEN SIZE!");
    }

    return s;
}

/** The implementation for REAL RandomStrings:
namespace haxx
{
 set<string> used_strings;
 int run_index = 0;
};

string GetRandomString(int size)
{
 char s[1000];

 for (int i = 0; i < size-1; i++)
 A
  s[i] = 'A'+rand()%('Z'-'A'+1);
 s[size-1]=0;

 string ss(s);

 if (STLhas(haxx::used_strings,ss))
 {
  string next_attempt_ss = ss;
  do
  {
   next_attempt_ss = ss + i2str(haxx::run_index++);
  }
  while (STLhas(haxx::used_strings,next_attempt_ss));

  return next_attempt_ss;
 }
 else
 {
  haxx::used_strings.insert(ss);
  return ss;
 }
  //return GetRandomString(size);
}*/

/*string log_periodical_report()
{
 int u = Nodes()+Links();

 return "U-SIZE: "+i2str(u);
}*/

/// Could be more inclusive...

bool _in(set<subst>* forbid, string entry, atom a)
{
    return STLhas(*forbid, subst(entry, a));
}

string make_subst_buf(const atom& a)
{
    string subst_buf;

    if (a.bindings)
        for ( map<string, atom>::const_iterator si = a.bindings->begin();
                si != a.bindings->end(); si++) {
            if (a.forbiddenBindings) {
                assert(!_in(a.forbiddenBindings, si->first, si->second));
            }

            subst_buf += si->first + "=>" + condensed_form(si->second) + " ";
        }

    if (a.forbiddenBindings)
        for ( set<subst>::const_iterator fi = a.forbiddenBindings->begin();
                fi != a.forbiddenBindings->end(); fi++) {
            subst_buf += "NOT( " + fi->first + "=>" + condensed_form(fi->second) + " ) ";
        }

    return subst_buf;
}

void getAtomTreeString(const atom& a, string& outbuf)
{
    char buf[5000];

    if (a.hs.size() == 0) {
        sprintf(buf, "%s:%s (%d)", a.name.c_str(), reasoning::Type2Name(a.T), a.T);

        string subst_buf = make_subst_buf(a);

        outbuf += buf + string(" [   ") + subst_buf + string("]");
    } else {
        sprintf(buf, "%s( ", reasoning::Type2Name(a.T));
        outbuf += buf;

        for (vector<Btr<atom> >::const_iterator hi = a.hs.begin(); hi != a.hs.end();) {
            getAtomTreeString(**hi, outbuf);
            if (++hi != a.hs.end())
                outbuf += ", ";
        }

        outbuf += " )";
    }

}

void printNode1(const atom& a, int level, int LogLevel)
{
    char buf[500];

    sprintf(buf, "%s:%s (%d) [%d]", a.name.c_str(), reasoning::Type2Name(a.T), a.T,
            a.handle);

    string subst_buf = make_subst_buf(a);

    LOG(LogLevel, repeatc(' ', level*3) + buf + " (   " + subst_buf + ")");
}

void printAtomTree(const atom& a, int level, int LogLevel)
{
    if (a.hs.size() == 0) {
        printNode1(a, level, LogLevel);
    } else {
        char buf[500];
        sprintf(buf, "%s:%s (%d) [%d]", a.name.c_str(),
                classserver().getTypeName(a.T).c_str(), a.T, a.handle);
        string subst_buf = make_subst_buf(a);

        LOG(LogLevel, repeatc(' ', level*3) + buf + " (   " + subst_buf + ")");
    }

    for (vector<Btr<atom> >::const_iterator hi = a.hs.begin(); hi != a.hs.end(); hi++)
        printAtomTree(**hi, level + 1, LogLevel);
}

void printLeveledString(int level, string str)
{
    LOG(0, repeatc(' ', level*3) + str);
}

/*reasoning::atom atomWithNewType(Handle h, Type T)
{
 reasoning::atom ret(h);
 ret.T = T;
 return ret;
}
reasoning::atom* newAtomWithNewType(Handle h, Type T)
{
 reasoning::atom* ret = new reasoning::atom(h);
 ret->T = T;
 return ret;
}*/
/*
Handle satisfyingSet(Handle P)
{
 assert(atw->getType(P) == PREDICATE_NODE);

 map<Handle, float> members;

 Handle ret = addNode(CONCEPT_NODE, string("The "+atw->getName(P)),
  TruthValue::TRUE_TV(),
  false);

 IsAtomType<Handle> isEval(EVALUATION_LINK);

 HandleSeq evals = isEval.filterValid(getOutgoing(U));

  //getHandleSet(EVALUATION_LINK, );

 for (int i = 0; i < evals.size(); i++)
 {
  HandleSeq hs = getOutgoing((evals)[i]);
  if (equal(hs[0], P))
  {
   const TruthValue& tv = getTruthValue(hs[1]);

   HandleSeq ArgList = getOutgoing(hs[1]);

   assert(ArgList.size() == 1); //Max. arity of the predicates for sat. set!

   HandleSeq ps;
   ps.push_back(ArgList[0]);
   ps.push_back(ret);

   addLink(MEMBER_LINK, ps,
    tv,
    false);
  }
 }

 return ret;
}
*/

/* TODO: Update for tree<Vertex>*/
 /*HandleSeq constitutedSet(Handle CP,
                         float min_membershipStrength,
                         float min_membershipCount)
{
    AtomSpaceWrapper* atw = GET_ATW;

    assert(atw->getType(CP) == CONCEPT_NODE);
    
    map<Handle, float> members;
    HandleSeq ret;
    
    // HandleEntry* evals = getHandleSet(MEMBER_LINK);

    TableGather mems(atom(__INSTANCEOF_N, 1, new atom(MEMBER_LINK,0) ));
    
    for (int i = 0; i < mems.size(); i++) {
        HandleSeq hs = getOutgoing(mems[i]);
        
        assert(hs.size() == 2);
        
        if (equal(hs[1], CP)) {
            const TruthValue& tv = atw->getTV(hs[1]);
            
            if (min_membershipStrength >= tv.getMean()
                && min_membershipCount >= tv.getCount())
                ret.push_back(hs[0]);
        }
    }

    return ret;
}
 */

}

/*
#include <boost/variant/static_visitor.hpp>
template<int LOGLEVEL>
class printVisitor : public boost::static_visitor<bool>
{
public:
 bool operator()(const TimeStampWrapper &arg) { cprintf(LOGLEVEL,"TimeStampWrapper\n"); return true; }
 bool operator()(const IntegerWrapper &arg) { cprintf(LOGLEVEL,"IntegerWrapper\n"); return true; }
 bool operator()(const FloatWrapper &arg) { cprintf(LOGLEVEL,"FloatWrapper\n"); return true; }
 bool operator()(const BoolWrapper &arg) { cprintf(LOGLEVEL,"BoolWrapper\n"); return true; }
 bool operator()(const ByteWrapper &arg) { cprintf(LOGLEVEL,"ByteWrapper\n"); return true; }
 bool operator()(const CharWrapper &arg) { cprintf(LOGLEVEL,"CharWrapper\n"); return true; }
 bool operator()(const ShortIntegerWrapper &arg) { cprintf(LOGLEVEL,"ShortIntegerWrapper\n"); return true; }
// bool operator()(const ShortFloatWrapper &arg) { cprintf(LOGLEVEL,"ShortFloatWrapper\n"); return true; }
 bool operator()(const Handle &arg) { cprintf(LOGLEVEL,"Handle [%d]\n",(int)arg); return true; }
// bool operator()( &arg) { return NonHandleID; }
};
*/
namespace reasoning
{

pHandle _v2h(const Vertex& v) {
   return  boost::get<pHandle>(v);
}

bool MPunifyHandle(pHandle lhs,
                   const atom& rhs,
                   bindingsT& bindings,
                   set<hsubst>** forbiddenBindings,
                   bool* restart, const Type VarT)
{
    AtomSpaceWrapper* atw = GET_ATW;

    assert(!atw->isType(lhs));

    Type lhs_T = atw->getType(lhs);

    LOG(4, "MPunifyHandle:");
//printTree(lhs,0,3);

    bool lhs_is_node = atw->inheritsType(lhs_T, NODE);

    string lhs_name(lhs_is_node ? atw->getName(lhs) : "");
    cprintf(4, "MPunifyHandle: lhs_is_node = %d, lhs_T =%d, lhs_name: %s\n", lhs_is_node, lhs_T, lhs_name.c_str());
    if (lhs_is_node) {
        bindingsT::const_iterator s = bindings.find(lhs);

        if (s != bindings.end()) {
            /*    if (lhs->getName() == s->second.name)
                {
                 printAtomTree(lhs,0,3);
                 printAtomTree(s->second,0,3);
                }
                assert(lhs.name != s->second.name);*/
            LOG(4, "MPunifyHandle: Binding found.");
            return equal_atom_ignoreVarNameDifferences(atom(s->second), rhs);
        } else {
            LOG(4, "Binding NOT found.");
        }

        if (rhs.T != lhs_T || rhs.name != lhs_name) {
            LOG(4, "MPunifyHandle: Difference found.");

            if (atw->inheritsType(lhs_T, VarT)) {
                if (*restart)
                    return true;

                cprintf(4, "Request rhs handle...\n");

                pHandle rhs_h = rhs.attach(ASW());
                if (rhs_h == PHANDLE_UNDEFINED) {
                    cprintf(0, "Could not add handle!\n");
                    return false;
                }

                cprintf(4, "Request rhs handle ok %d\n", rhs_h);

                if (!forbiddenBindings)
                    cprintf(4, "No forbiddenBindings\n");

                if (!forbiddenBindings || !STLhas(**forbiddenBindings, hsubst(lhs_name, rhs_h))) {
                    LOG(4, "MPunifyHandle: New subst: " + lhs_name + " for:");
//printTree(rhs_h,0,3);

                    bindings[lhs] = rhs_h;

                    *restart = true;
                } else {
                    cprintf(4, "Subst was forbidden\n");
                    return false;
                }
            } else
                return false;
        } else {
//cprintf(4, "MPunifyHandle: Returning true because has no difference in node comparison\n");
            return true;
        }
    } else {
        pHandleSeq lhs_out = atw->getOutgoing(lhs);
        uint lhs_arity = atw->getArity(lhs);

        LOG(4, "MPunifyHandle: UnifyVector exec...");
        if (lhs_arity != rhs.hs.size()) {
            bindings.clear();
            cprintf(4, "MPunifyHandle: arity diff, returning (%d / %u)\n", lhs_arity, (uint) rhs.hs.size());
            return false;
        }

        for (uint i = 0; i < lhs_arity; i++) {
            LOG(4, "MPunifyHandle: next arg...");
            if (!reasoning::MPunifyHandle(lhs_out[i], *rhs.hs[i], bindings, forbiddenBindings, restart, VarT))
                return false;

            LOG(4, "MPunifyHandle: Unify1 was success!");
            if (*restart) {
                LOG(4, "MPunifyHandle: restart requested...");
                return true;
            }
        }
    }

    return true;
}

bool MPunify1(tree<Vertex>& lhs_t, tree<Vertex>::iterator lhs_ti,
              const atom& rhs,
              bindingsT& bindings,
              set<hsubst>** forbiddenBindings,
              bool* restart, const Type VarT)
{
    AtomSpaceWrapper* atw = GET_ATW;

    *restart = false; //We would not be here if restart was really pending.

    // By definition: FW_VARs only allowed on left side!
    if (rhs.T == VarT)
        return false;

//boost::apply_visitor(printVisitor<4>(), *lhs_ti);
    pHandle *lhs_ptr = boost::get<pHandle>(&*lhs_ti);
    if (!lhs_ptr)
        return false;
    pHandle lhs = *lhs_ptr;

    if (rhs.T == ATOM || lhs == (pHandle)ATOM)
        return true;

    Type lhs_T = atw->getType(lhs);

    LOG(4, "Unify1:");
    printTree(lhs, 0, 3);

    bool lhs_is_node = atw->inheritsType(lhs_T, NODE);
    string lhs_name(lhs_is_node ? atw->getName(lhs) : "");
    if (lhs_is_node) {
        cprintf(4, "Node name: %s\n", lhs_name.c_str());

        bindingsT::const_iterator s = bindings.find(lhs);

        if (s != bindings.end()) {
            /*    if (lhs->getName() == s->second.name)
                {
                 printAtomTree(lhs,0,3);
                 printAtomTree(s->second,0,3);
                }
                assert(lhs.name != s->second.name);*/
            LOG(4, "Binding found. eq...");
            bool ret = equal_atom_ignoreVarNameDifferences(atom(s->second), rhs);
            LOG(4, "equal_atom_ignoreVarNameDifferences ok");
            return ret;
        }
        LOG(4, "Binding NOT found.");
    }
    if ( // Check whether not the same type
        rhs.T != lhs_T ||
        // If a node, then check if the names are different
        (lhs_is_node && rhs.name != lhs_name) ||
        // If lhs is a real atom, check whether lhs and rhs cannot be unified
        (!atw->isType(lhs) && !MPunifyHandle(lhs, rhs, bindings,
                                             forbiddenBindings, restart, VarT)) ||
        // If lhs isn't a real atom, check whether lhs and rhs cannot be unified
        // based on the atoms pointed to by rhs.
        (atw->isType(lhs) && !MPunifyVector(lhs_t, lhs_ti, rhs.hs,
                                            bindings, forbiddenBindings, restart, VarT))) {
        LOG(4, "Difference found.");

        if (atw->inheritsType(lhs_T, VarT)) {
            if (*restart)
                return true;

            cprintf(4, "Request rhs handle...\n");

            pHandle rhs_h = rhs.attach(ASW());
            if (rhs_h == PHANDLE_UNDEFINED) {
                cprintf(0, "Could not add handle!\n");
                return false;
            }

            cprintf(4, "Request rhs handle ok %d\n", rhs_h);

            if (!forbiddenBindings)
                cprintf(4, "No forbiddenBindings\n");

            if (!forbiddenBindings || !STLhas(**forbiddenBindings, hsubst(lhs_name, rhs_h))) {
                LOG(4, "New subst: " + lhs_name + " for:");
//printTree(rhs_h,0,3);

                bindings[lhs] = rhs_h;

                *restart = true;
            } else {
                cprintf(3, "Subst was forbidden\n");
                return false;
            }
        } else
            return false;
    }

    return true;
}

bool MPunifyVector(tree<Vertex>& lhs_t, tree<Vertex>::iterator lhs_top,
                   const vector<Btr<atom> >& rhsv,
                   bindingsT& bindings,
                   set<hsubst>** forbiddenBindings,
                   bool* restart, const Type VarT)
{
    bool isroot = (NULL == restart);
    bool root_restart = false;
    if (isroot)
        restart = &root_restart;
    LOG(4, "UnifyVector exec...");
    if (lhs_top.number_of_children() != rhsv.size()) {
        bindings.clear();
        cprintf(4, "UnifyVector: arity diff, returning (%d / %u)\n", lhs_top.number_of_children(),
                (uint) rhsv.size());
        return false;
    }

    int i = 0;

    for (tree<Vertex>::sibling_iterator c = lhs_t.begin(lhs_top);
            c != lhs_t.end(lhs_top);c++, i++) {
        LOG(4, "UnifyVector: next arg...");

        if (!MPunify1(lhs_t, c, *rhsv[i], bindings, forbiddenBindings, restart, VarT))
            return false;

        LOG(4, "UnifyVector: Unify1 was success!");
//printAtomTree(lhsv[i],0,4);
        if (*restart) {
            LOG(4, "UnifyVector: restart reqsted...");
            if (!isroot) //If called from within MPunify1
                return true;
            else
                return MPunifyVector(lhs_t, lhs_top, rhsv, bindings, forbiddenBindings, NULL, VarT);
        }
    }

    return true;
}


/**
 * Check if the atom with the given properties is a template variable.
 * There are 3 types of variables:
 * 1) FWVariableNodes => anything
 * 2) Any type of node with no name => any node of that type
 * 3) Any type of link with no arity => any link of that type
 */
inline bool isVariableAtom(string name, Type type, int arity)
{
    AtomSpaceWrapper* atw = GET_ATW;
    return (atw->inheritsType(type, FW_VARIABLE_NODE) ||
            (name.empty() && atw->inheritsType(type, NODE)) ||
            (!arity && atw->inheritsType(type, LINK)));
}

string condensed_form(const atom& a)
{
    AtomSpaceWrapper* atw = GET_ATW;
    if (atw->inheritsType(a.T, NODE))
        return a.name;
    else {
        string ret = i2str(a.T) + ":(";
        for (vector<Btr<atom> >::const_iterator hi = a.hs.begin(); hi != a.hs.end();hi++)
            ret += condensed_form(**hi) + " ";
        return ret + ")";
    }
}

/*bool consistent(const bindingsT& b1, bindingsT::iterator b2start, bindingsT::iterator b2end)
{
 for (bindingsT::const_iterator b = b2start;
  b!= b2end;
  b++)
 {
  bindingsT::const_iterator b2;

  if ((b2 = b1.find(b->first)) != b1.end() &&
   b2->second != b->second) //if the same var bound different way
   return false; //it's inconsistency
 }

 return true;
}*/


class getNameOp
{
public:
    string operator()(pHandle h) {
        AtomSpaceWrapper* atw = GET_ATW;
        return string(atw->getName(h));
    }
};

/*template<typename BindKeyT, typename BindKeyOpT, typename BindItT>
bool ttsubstitutableTo(Handle from,Handle to,
      map<BindKeyT,Handle>& bindings,
      BindKeyOpT bind_key_op)
{
  return true;
}*/
/*
void foo()
{
  Handle from,to;
  map<string, Handle> bindings;

 ttsubstitutableTo<string,getNameOp,map<string,Handle>::iterator>(from, to, bindings, getNameOp());
}*/

template<typename BindKeyT, typename BindKeyOpT, typename BindItT>
bool ttsubstitutableTo(pHandle from, pHandle to,
                       map<BindKeyT, pHandle>& bindings,
                       BindKeyOpT bind_key_op)
{
    AtomSpaceWrapper* atw = GET_ATW;

    if (from == PHANDLE_UNDEFINED && to == PHANDLE_UNDEFINED)
        return true;
    if (from == PHANDLE_UNDEFINED || to == PHANDLE_UNDEFINED)
        return false;

    /*LOG(0, "-Source");
     printAtomTree(reasoning::atom(from),0,0);
     LOG(0, "-SubstitutableTo:");
     printAtomTree(reasoning::atom(to),0,0);
     LOG(0, "vee");*/

    string from_name(atw->getName(from));
    string to_name(atw->getName(to));

    BindKeyT from_key = bind_key_op(from);

    Type from_T(atw->getType(from));
    Type to_T(atw->getType(to));

#if 0
    map<string, Handle>::const_iterator s = bindings.find(to_name);
    assert(s == bindings.end());

#endif

//  Assert removed because TO side can have variables too, which is checked below
// map<string, Handle>::const_iterator s = bindings.find(to_name);
// assert(s == bindings.end());

    int to_arity = atw->getArity(to);

    // These cases would complicate things... So, templates in the TO side is not allowed to match anything
    // TODO: HOWEVER, there is at least the following case where we are not detecting here:
    // FROM = ListLink with arity 0, TO = ListLink with arity 2, but with a FW_VARIABLE_NODE in its outgoing set => TO matches FROM!!!
    //assert(!atw->inheritsType(to_T, FW_VARIABLE_NODE)); COMMENTED OUT BECAUSE TEMPLATES ARE BEING INSERTED IN REAL ATOMTABLE
    if (isVariableAtom(to_name, to_T, to_arity)) {
        cprintf(NORMAL, "substitutableTo(): TO argument is a template variable (to_name = %s, to_T = %s, to_arity = %d) => Skiping remaining checkings. Returning false...\n", to_name.c_str(), Type2Name(to_T), to_arity);
        return false;
    }

    if (atw->inheritsType(from_T, FW_VARIABLE_NODE)) {
        BindItT b = bindings.find(from_key);

        if (b != bindings.end())
            return ttsubstitutableTo< BindKeyT, BindKeyOpT, BindItT>(b->second, to, bindings, bind_key_op);
        else {
//            printf("BINDING %s TO %p\n", from_name.c_str(), to);
            bindings[from_key] = to;
            return true;
        }
    }

    if (from_T != to_T) {
        return false;
    }

    if (!from_name.empty() && from_name != to_name) {
        return false;
    }

#if 1
    int from_arity = atw->getArity(from);
    if (!from_arity) //A 0-arity atom is considered an instanceOf operator.
        return true;

    if (to_arity != from_arity)
        return false;

    vector<pHandle> from_hs = atw->getOutgoing(from);
    vector<pHandle> to_hs = atw->getOutgoing(to);

// Check if it matches with at least one combination of permutation of variable elements in outgoing set of FROM side.
// TODO: FOR NOW, UNORDERED LINKS MAY HAVE ONLY ONE VARIABLE IN THE OUTGOING SET
    if (atw->inheritsType(from_T, UNORDERED_LINK)) {
//        for (int i = 0; i < from_arity; i++) {
//            printf("%d: from_hs=%p\nto_hs=%p\n", i, from_hs[i], to_hs[i]);
//        }
        // Uses a clone of the outgoing set
        vector<pHandle> new_from_hs(from_hs);

        // Look for variables in from side
        //BindKeyT unbound_variable_key = 0; // BUG FIX: This causes basic_string initialization error!
        BindKeyT unbound_variable_key;
        bool found_unbound_variable = false;

        int variable_index = -1;
        for (int i = 0; i < from_arity; i++) {
            string name(atw->getName(new_from_hs[i]));
            Type type(atw->getType(new_from_hs[i]));
            int arity = atw->getArity(new_from_hs[i]);
            if (isVariableAtom(name, type, arity)) {
                if (variable_index == -1) {
                    variable_index = i;
                    // Check if variable is already bound to a handle
                    BindItT b = bindings.find(from_key);
                    if (b != bindings.end()) {
                        // If so, replace it at OutgoingSet
                        new_from_hs[i] = b->second;
                    } else {
                        unbound_variable_key = bind_key_op(new_from_hs[i]);
                        found_unbound_variable = true;
                    }
                } else {
                    throw new RuntimeException("Found more than one variable inside an unordered link (link type = %s,positions %d and %d)\n", classserver().getTypeName(from_T).c_str(), variable_index, i);
                }
            }
        }
        if (variable_index != -1) {
            //          printf("FOUND VARIABLE AT POSITION %d\n", variable_index);
            for (int i = 0; i < from_arity; i++) {
                // i is the place where the variable will be
//                printf("Checking with variable at position %d\n", i);
                bool result = true;
                for (int j = 0; j < from_arity; j++) {
                    int from_side_index = j;
                    if (j == i) {
                        from_side_index = variable_index;
                    } else if (variable_index < i) {
                        if (j >= variable_index && j < i) {
                            from_side_index = j + 1;
                        }
                    } else if (variable_index > i) {
                        if (j > i && j <= variable_index) {
                            from_side_index = j - 1;
                        }
                    }
//                    printf("Checking position %d against position %d\n", from_side_index, j);
                    if (!ttsubstitutableTo< BindKeyT, BindKeyOpT, BindItT>(new_from_hs[from_side_index], to_hs[j], bindings, bind_key_op)) {
                        result = false;
                        break;
                    }
                }
                if (result) {
//                    free (new_from_hs);
                    return true;
                } else {
                    // if we had an unbound variable...
//                    if (unbound_variable_key != BindKeyT(0)) {
                    if (found_unbound_variable) {
                        // Make sure it keeps unbound for next try
                        BindItT s = bindings.find(unbound_variable_key);
                        if (s != bindings.end()) {
//                            printf("UNBINDING %s\n", unbound_variable_name.c_str());
                            bindings.erase(unbound_variable_key);
                        }
                    }
                }
            }
//            free (new_from_hs);
            return false;
        }
    }

    for (int i = 0; i < from_arity; i++)
        if (!ttsubstitutableTo< BindKeyT, BindKeyOpT, BindItT>(from_hs[i], to_hs[i], bindings, bind_key_op))
            return false;
#endif
    return true;
}

/**
 Whether one can produce 'to' from 'from' by some variable substitutions.
*/

void foo2()
{
    pHandle from, to;
    map<string, pHandle> bindings;

    ttsubstitutableTo<string, getNameOp, map<string, pHandle>::iterator>(from, to, bindings, getNameOp());
}

bool substitutableTo(pHandle from, pHandle to,
                     map<string, pHandle>& bindings)
{
    return ttsubstitutableTo<string, getNameOp, map<string, pHandle>::iterator>(from, to, bindings, getNameOp());
}

class handleNoOp
{
public:
    pHandle operator()(pHandle h) {
        return h;
    }
};

/**
 Whether one can produce 'to' from 'from' by some variable substitutions.
*/

bool substitutableTo(pHandle from, pHandle to,
                     map<pHandle, pHandle>& bindings)
{
    return ttsubstitutableTo<pHandle, handleNoOp, map<pHandle, pHandle>::iterator>(from, to, bindings, handleNoOp());
}

char unnamed_type[] = "unnamed-type";
const char* Type2Name(Type t)
{

    return classserver().getTypeName(t).c_str();
    /*
     return  (STLhas(*ClassServer::class_name, t)
        ? (*ClassServer::class_name)[t]
        : unnamed_type);
    */
}

pHandle make_real(vtree& vt)
{
    //printTree(ASW()->addAtom(vt, TruthValue::TRIVIAL_TV(), false),0,0);
    return ASW()->addAtom(vt, TruthValue::TRIVIAL_TV(), false);
}

void recursiveBind(Vertex& v, const map<pHandle, pHandle>& binds)
{
    // Recursive bind steps through the chain of bindings
    // e.g. when A -> B, but a parent has C -> A
    pHandle *ph = boost::get<pHandle>(&v);
    if (ph) {
        map<pHandle, pHandle>::const_iterator it = binds.find(*ph);
        if (it != binds.end()) {
            cprintf(4, "Bound to %u", it->second);
            v = Vertex(it->second);
            recursiveBind(v, binds);
        }
    }
}

meta bind_vtree(vtree &targ, const map<pHandle, pHandle>& binds)
{
    meta thm_substed(new BoundVTree(targ));

    for (vtree::pre_order_iterator vit = thm_substed->begin(); vit != thm_substed->end(); vit++)
        recursiveBind(*vit, binds);

    return thm_substed;
}

/*
meta bind_vtree(vtree &targ, const map<Handle, Handle>& binds)
{
    meta thm_substed(new BoundVTree(targ));

    bool changes=false;
    do
    {
     changes = false;

     cprintf(4,"Next change...\n");

     for(vtree::pre_order_iterator vit = thm_substed->begin(); vit != thm_substed->end(); vit++)
     {
      cprintf(4,"Next change check...\n");

      Handle *ph = v2h(&*vit);
      if (ph)
      {
       cprintf(4,"(ph) %d\n", *ph);

       map<Handle, Handle>::const_iterator it = binds.find(*ph);
       if (it != binds.end())
       {
        cprintf(4,"Bound to %d", it->second);
        *vit = Vertex(it->second);
        changes = true;
        goto break_inner;
       }
      }
      else
       cprintf(4,"NOT (ph)");
     }
break_inner:
     cprintf(4,"1 change run ok");
    } while (changes);

 return thm_substed;
}
*/
void removeRecursionFromHandleHandleMap(bindingsT& ret_bindings)
{
    /// TODO: Simplify!
    /// Currently the only automatic way to remove recursion by looking _inside_ that set is as follows:

    /// 1. Convert handle=>handle to handle=>vtree
    bindingsVTreeT bindsWithVTree;
    foreach(hpair hp, ret_bindings)
    bindsWithVTree[hp.first] = make_vtree(hp.second);

    /// 2. Remove recursion from handle=>vtree mapping

    removeRecursionFromMap<bindingsVTreeT::iterator, vtree::iterator>(bindsWithVTree.begin(), bindsWithVTree.end());

    /// 3. Deconvert handle=>vtree to handle=>handle

    typedef pair<pHandle, vtree> phvt;
    foreach(phvt vp, bindsWithVTree)
    ret_bindings[vp.first] = make_real(vp.second);
}

void removeRecursion(std::vector<Btr<set<BoundVertex> > >& multi_input_vector)
{
    foreach(const Btr< set<BoundVertex> >& miv_set, multi_input_vector)
    foreach(const BoundVertex& bv, *miv_set)
    if (bv.bindings)
        removeRecursionFromHandleHandleMap(*bv.bindings);
}

unsigned long BoundVTree::getFingerPrint()
{
    size_t ret = 0;

    const vtree& vt = this->getStdTree();

    for (vtree::sibling_iterator i1 = vt.begin(); i1 != vt.end(); i1++) {
        ret += (unsigned long)_v2h(*i1);

        for (vtree::sibling_iterator i2 = vt.begin(i1); i2 != vt.end(i1); i2++) {
            ret += (unsigned long)_v2h(*i2) * 100;

            for (vtree::sibling_iterator i3 = vt.begin(i2); i3 != vt.end(i2); i3++)
                ret += (unsigned long)_v2h(*i3) * 10000;
        }
    }

    return ret;
}

const vtree& BoundVTree::getStdTree()
{
    if (my_std_tree.empty())
        createMyStdTree();

    return my_std_tree;
}

void BoundVTree::createMyStdTree()
{
    AtomSpaceWrapper* atw = GET_ATW;

    bindingsT varmap;

    my_std_tree = vtree(*this);

    int vars = 0;

    for (vtree::pre_order_iterator i = my_std_tree.begin(); i != my_std_tree.end(); i++) {
        pHandle *plh = boost::get<pHandle>(&*i);
        if (plh && ((long)plh) > 220 +::haxx::STD_VARS && atw->inheritsType(atw->getType(*plh), FW_VARIABLE_NODE)) {
            bindingsT::iterator old_mapping = varmap.find(*plh);
            if (old_mapping != varmap.end()) {
                *i = Vertex(old_mapping->second);
            } else {
                int wtf = ::haxx::STD_VARS;
                assert(vars < wtf);
                varmap[*plh] = ::haxx::VarTypes[vars++];
                *i = Vertex(varmap[*plh]);
            }
        }
    }
    /* rawPrint(*this, this->begin(), 0);
     puts("=>");
     rawPrint(my_std_tree,my_std_tree.begin(), 0);*/
}

bool equalVariableStructure(const vtree& lhs, const vtree& rhs)
{
    AtomSpaceWrapper* atw = GET_ATW;

    bindingsT varmap;
    if (lhs.size() != rhs.size())
        return false;


    vtree* _lhs = (const_cast<vtree*>(&lhs));
    vtree* _rhs = (const_cast<vtree*>(&rhs));

    vtree::pre_order_iterator ri = _rhs->begin();
    vtree::pre_order_iterator li = _lhs->begin();

    for (;li != _lhs->end(); li++) {
        pHandle *plh = boost::get<pHandle>(&*li);
        pHandle *prh = boost::get<pHandle>(&*ri);

        /// != operator is not supported by Vertex.
        if (!(*li == *ri)) {
            if (!plh || !prh
                    || !atw->inheritsType(atw->getType(*plh), FW_VARIABLE_NODE)
                    || !atw->inheritsType(atw->getType(*prh), FW_VARIABLE_NODE))
                return false;
            bindingsT::iterator old_mapping = varmap.find(*plh);
            if (old_mapping != varmap.end()) {
                if (old_mapping->second != *prh)
                    return false;
            } else
                varmap[*plh] = *prh;
        }
        ri++;
    }

    return true;
}

bool equalVariableStructure2(BBvtree lhs, BBvtree rhs)
{
// return false;
    return lhs->getStdTree() == rhs->getStdTree();
// return equalVariableStructure(lhs->std_tree(), rhs->std_tree());
}

void ForceVirtual(meta _target, vtree::iterator& vit)
{
    AtomSpaceWrapper* atw = GET_ATW;

    pHandle *ph = boost::get<pHandle>(&(*vit));
    if (ph && !atw->isType(*ph) && !atw->inheritsType(atw->getType(*ph), NODE)) {
        // A real link cannot have children in a vtree! That'd screw everything up.
        assert(!_target->number_of_children(vit));

        // Save the sib it because the replace() will invalidate 'vit'
//  vtree::sibling_iterator next_sib = _target->next_sibling(vit);

        cprintf(2, "ForceVirtual: [%d] (exists).\n", *ph);

        vtree virtualized_ph(make_vtree(*ph));
        _target->replace(vit, virtualized_ph.begin());

        //vit = next_sib;
        vit = _target->begin();
    } else
        ++vit;
}

meta ForceAllLinksVirtual(meta _target)
{
    /* for (vtree::sibling_iterator root = _target->begin(); root != _target->end() && root != root->end(); root = _target->begin(root))
      for (vtree::sibling_iterator vit = _target->begin(root); vit != _target->end(root);)*/

    /// @todo Don't always just iterate the whole tree again from the beginning after every change,
    /// but instead continue from where you were, using something like the above embedded loop pair for that.
    for (vtree::pre_order_iterator vit = _target->begin(); vit != _target->end();)
        ForceVirtual(_target, vit);

    return _target;
}

meta ForceRootLinkVirtual(meta _target)
{
    AtomSpaceWrapper* atw = GET_ATW;

    pHandle *ph = boost::get<pHandle>(&(*_target->begin()));

    Type t = atw->getType(*ph);
    if (ph && !atw->isType(*ph) && atw->getType(*ph) != FW_VARIABLE_NODE) {
        cprintf(2, "ForceVirtual: [%d] (exists).\n", *ph);

        return meta(new vtree(make_vtree(*ph)));
    } else
        return _target;
}

static long _ttt = 0;
void print_progress()
{
    if (currentDebugLevel <= 0) //progress reporting
        cprintf(0, "%c\b", (((_ttt++) % 2) ? '-' : '|'));
}

bool RealHandle(meta _target, Btr<set<BoundVertex> > result_set)
{
    AtomSpaceWrapper* atw = GET_ATW;

    pHandle *ph = boost::get<pHandle>(&(*_target->begin()));
    if (ph && !atw->isType(*ph)) {
        cprintf(2, "Arg [%d] exists.\n", *ph);
        const TruthValue& tv = atw->getTV(*ph);
        if (tv.isNullTv())
            cprintf(2, "NO TV!\n");
        result_set->insert(*_target->begin());

        //cprintf(3,"Arg %d now has %d direct child_results.\n", i, result_set->size());

        return true;
    } else
        return false;
}

void bind(BoundVTree& bbvt, hpair new_bind)
{
    for (vtree::iterator v = bbvt.begin(); v != bbvt.end(); v++)
        if (_v2h(*v) == new_bind.first)
            *v = Vertex(new_bind.second);
}

void bind_Bvtree(meta arg, const bindingsVTreeT& binds)
{
    for (vtree::iterator v = arg->begin();
            v != arg->end();) {
        bindingsVTreeT::const_iterator it = binds.find(_v2h(*v));
        if (it != binds.end()) {
            arg->replace(v, it->second.begin());
            v = arg->begin();
        } else
            ++v;
    }
}

/// Ok, you're not allowed to laugh at the following. It's a collection of my
/// various quick-hack binding printers from the last 6 months.

void pr2(pair<pHandle, vtree> i)
{
    cprintf(4, "%d => ", i.first);
    rawPrint(i.second, i.second.begin(), 3);
}

void print_binding(pair<Handle, vtree> i)
{
    printf("%d => ", (int)i.first.value());
    rawPrint(i.second, i.second.begin(), -1);
}

void printBinding(const pair<string, Handle> p)
{
    cprintf(3, "%s => %d\n", p.first.c_str(), (int)p.second.value());
}

void pr(pair<pHandle, pHandle> i)
{
    AtomSpaceWrapper* atw = GET_ATW;

    string s1 = atw->inheritsType(atw->getType(i.first), NODE) ? atw->getName(i.first) : i2str(i.first);
    string s2 = atw->inheritsType(atw->getType(i.second), NODE) ? atw->getName(i.second) : i2str(i.second);

    cout << s1 << " => " << s2 << "\n";
// cout << (int)i.first << " => " << (int)i.second << "\n";

}

void printSubsts(BoundVertex a, int LogLevel)
{
    LOG(LogLevel, " (   " + make_subst_buf(a) + ")");
}

string make_subst_buf(const BoundVertex& a)
{
    string subst_buf;
    AtomSpaceWrapper* atw = GET_ATW;

    if (a.bindings)
        for (bindingsT::const_iterator si = a.bindings->begin();
                si != a.bindings->end(); si++) {
            subst_buf += atw->getName(si->first) + "=>" + condensed_form(atom(si->second)) + " ";
        }

    return subst_buf;
}

template<Type TRANSITIVE_LINK_TYPE, typename containerT, typename const_iteratorT>
Vertex transitive_produce( const containerT& chain,
                           const_iteratorT next_it,
                           Vertex last_result = (Handle)NULL)
{
    vector<Vertex> rule_args;
    rule_args.push_back(_v2h(last_result) ? last_result : (*next_it++));
    rule_args.push_back(*next_it++);

    Vertex my_result = reasoning::DeductionRule<DeductionSimpleFormula>(ASW(), TRANSITIVE_LINK_TYPE).compute(rule_args);

    return (next_it != chain.end())
           ? transitive_produce<TRANSITIVE_LINK_TYPE>(chain, next_it, my_result)
           : my_result;
}

/*void testt()
{
 vector<Vertex> foo;
 transitive_produce<IMPLICATION_LINK, vector<Vertex>, vector<Vertex>::const_iterator>(foo,foo.begin());
}*/


bool IsIdenticalHigherConfidenceAtom(pHandle a, pHandle b)
{
    AtomSpaceWrapper* atw = GET_ATW;

    /// \todo Speed-optimize!

    vtree va(reasoning::make_vtree(a));
    vtree vb(reasoning::make_vtree(b));

    return va == vb &&
           (atw->getTV(b).getConfidence() - atw->getTV(a).getConfidence())
           < 0.00000001f;
}

bool unifiesTo( const vtree & lhs_t, const vtree & rhs_t,
                vtree::sibling_iterator ltop, vtree::sibling_iterator rtop,
                map<pHandle, vtree>& Lbindings,
                map<pHandle, vtree>& Rbindings,
                bool allow_rhs_binding,
                Type VarType)
{
    AtomSpaceWrapper* atw = GET_ATW;

//cprintf(0,"U: %d %d\n",
    if (lhs_t.empty() && rhs_t.empty()) {
        cprintf(4, "unifiesTo: Both sides empty!\n");
        return true;
    }
    if (lhs_t.empty() || rhs_t.empty()) {
        cprintf(4, "unifiesTo: One side empty!\n");
        return false;
    }
// if (lhs_t.number_of_children(ltop) != rhs_t.number_of_children(rtop))
//  return false;

    if (!((*rtop) == (*ltop))) {
        pHandle *ph_ltop = boost::get<pHandle>(&*ltop);
        pHandle *ph_rtop = boost::get<pHandle>(&*rtop);

        if (ph_ltop) {
            bool lhs_is_node = atw->inheritsType(atw->getType(*ph_ltop), NODE);

            /// haxx::

            bool lhs_is_var  = lhs_is_node && ( atw->inheritsType(atw->getType(*ph_ltop), VarType) || atw->inheritsType(atw->getType(*ph_ltop), FW_VARIABLE_NODE));

            if (!atw->isType(*ph_ltop) && !lhs_is_node) {
                vtree ltop_as_tree(reasoning::make_vtree(*ph_ltop));

                return unifiesTo(ltop_as_tree, rhs_t,
                                 ltop_as_tree.begin(), rtop, Lbindings, Rbindings, allow_rhs_binding, VarType);
            }

            map<pHandle, vtree>::const_iterator s = Lbindings.find(*ph_ltop);

            if (s == Lbindings.end() &&
                    (s = Rbindings.find(*ph_ltop)) == Rbindings.end()) {
                if (ph_rtop) {
                    bool rhs_is_node = atw->inheritsType(atw->getType(*ph_rtop), NODE);
                    bool rhs_is_var  = rhs_is_node && ( atw->inheritsType(atw->getType(*ph_rtop), VarType)  || atw->inheritsType(atw->getType(*ph_rtop), FW_VARIABLE_NODE));

                    if (!atw->isType(*ph_rtop) && !rhs_is_node) {
                        vtree rtop_as_tree(reasoning::make_vtree(*ph_rtop));

                        return unifiesTo(lhs_t, rtop_as_tree,
                                         rtop, rtop_as_tree.begin(), Lbindings, Rbindings, allow_rhs_binding, VarType);
                    }

                    map<pHandle, vtree>::const_iterator s = Rbindings.find(*ph_rtop);

                    if (s == Rbindings.end() &&
                            (s = Lbindings.find(*ph_rtop)) == Lbindings.end()) { /// If neither has been bound
                        if (lhs_is_var) {
                            cprintf(4, "New FW_VAR binding\n");

                            Lbindings[*ph_ltop] = vtree(rtop);
                            return true;
                        } else if (rhs_is_var && allow_rhs_binding) {
                            cprintf(4, "FW_VAR");
                            Rbindings[*ph_rtop] = vtree(ltop);
                            return true;
                        } else
                            return false;
                    } else { // A binding found from RHS
                        cprintf(4, "Binding ph_rtop\n");
                        return unifiesTo(lhs_t, s->second, ltop, s->second.begin(), Lbindings, Rbindings, allow_rhs_binding, VarType);
                    }
                }
            } else { // A binding found from LHS
                cprintf(4, "Binding ph_ltop\n");
                return unifiesTo(s->second, rhs_t, s->second.begin(), rtop, Lbindings, Rbindings, allow_rhs_binding, VarType);
            }
        }

        return false;
    }
    cprintf(4, "This node ok\n");

    vtree::sibling_iterator rit = rhs_t.begin(rtop);
    vtree::sibling_iterator lit = lhs_t.begin(ltop);

// if (rit.number_of_children() != lit.number_of_children())
    if (rtop.number_of_children() != ltop.number_of_children())
        return false;

    while (lit != lhs_t.end(ltop))
        if (!unifiesTo(lhs_t, rhs_t, lit++, rit++, Lbindings, Rbindings, allow_rhs_binding, VarType))
            return false;

    return true;
}

/*
/// Old implementation. Strict separation betw. RHS and LHS variables.

bool unifiesTo( const vtree & lhs_t, const vtree & rhs_t,
    vtree::sibling_iterator ltop, vtree::sibling_iterator rtop,
    map<Handle, vtree>& Lbindings,
    map<Handle, vtree>& Rbindings,
    Type VarType)
{
//cprintf(0,"U: %d %d\n",

 cprintf(4,"Empty checks...\n");

 if (lhs_t.empty() && rhs_t.empty())
  return true;
 if (lhs_t.empty() || rhs_t.empty())
  return false;

 cprintf(4,"Empty checks ok\n");

// if (lhs_t.number_of_children(ltop) != rhs_t.number_of_children(rtop))
//  return false;

 if (!((*rtop) == (*ltop)))
 {
  Handle *ph_ltop = v2h(&*ltop);

  if (ph_ltop)
  A
  {
   bool is_node = atw->inheritsType(atw->getType(*ph_ltop),NODE);

   /// haxx::

   bool is_var  = is_node && ( atw->inheritsType(atw->getType(*ph_ltop), VarType) || atw->inheritsType(atw->getType(*ph_ltop), FW_VARIABLE_NODE));

   if (atw->isReal(*ph_ltop) && !is_node)
   {
    vtree ltop_as_tree(make_vtree(*ph_ltop));

    return unifiesTo(ltop_as_tree, rhs_t,
     ltop_as_tree.begin(), rtop, Lbindings, Rbindings, VarType);
   }

   map<Handle, vtree>::const_iterator s = Lbindings.find(*ph_ltop);

cprintf(4,"Analyzing ph_ltop\n");
cprintf(4,"Left: %d / %d\n", *ph_ltop, atw->getType(*ph_ltop));

   if (s != Lbindings.end())
   {
cprintf(4,"Binding ph_ltop\n");
    return unifiesTo(s->second, rhs_t, s->second.begin(), rtop, Lbindings, Rbindings, VarType);
   }
   else if (is_var)
   {
cprintf(4,"FW_VAR");
    Lbindings[*ph_ltop] = vtree(rtop);
    return true;
   }
  }

  Handle *ph_rtop = v2h(&*rtop);
  if (ph_rtop)
  {
   bool is_node = atw->inheritsType(atw->getType(*ph_rtop),NODE);
   bool is_var  = is_node && ( atw->inheritsType(atw->getType(*ph_rtop),VarType)  || atw->inheritsType(atw->getType(*ph_rtop), FW_VARIABLE_NODE));

   if (atw->isReal(*ph_rtop) && !is_node)
   {
    vtree rtop_as_tree(make_vtree(*ph_rtop));

//rawPrint(rtop_as_tree,rtop_as_tree.begin(),0);

    return unifiesTo(lhs_t, rtop_as_tree,
     rtop, rtop_as_tree.begin(), Lbindings, Rbindings, VarType);
   }

   map<Handle, vtree>::const_iterator s = Rbindings.find(*ph_rtop);

cprintf(4,"Analyzing ph_rtop\n");
cprintf(4,"Right: %d / %d\n", *ph_rtop, atw->getType(*ph_rtop));

   if (s != Rbindings.end())
   {
cprintf(4,"Binding ph_rtop\n");
    return unifiesTo(lhs_t, s->second, ltop, s->second.begin(), Lbindings, Rbindings, VarType);
   }
   else if (is_var)
   {
cprintf(4,"FW_VAR");
    Rbindings[*ph_rtop] = vtree(ltop);
    return true;
   }
  }

  return false;
 }
 cprintf(4,"This node ok\n");

 vtree::sibling_iterator rit = rhs_t.begin(rtop);
 vtree::sibling_iterator lit = lhs_t.begin(ltop);

// if (rit.number_of_children() != lit.number_of_children())
 if (rtop.number_of_children() != ltop.number_of_children())
  return false;

 while (lit != lhs_t.end(ltop))
  if (!unifiesTo(lhs_t, rhs_t, lit++, rit++, Lbindings, Rbindings, VarType))
    return false;

 return true;
}*/

/*bool unifiesTo( const vtree & lhs_t, const vtree & rhs_t,
    vtree::sibling_iterator ltop, vtree::sibling_iterator rtop,
    map<Handle, vtree>& Lbindings,
    map<Handle, vtree>& Rbindings,
    bool allow_rhs_binding,
    Type VarType);*/

bool unifiesTo(const vtree& lhs, const vtree& rhs, map<pHandle, vtree>& Lbindings, map<pHandle, vtree>& Rbindings, bool allow_rhs_binding, Type VarType)
{
    return unifiesTo(lhs, rhs, lhs.begin(), rhs.begin(), Lbindings, Rbindings, allow_rhs_binding, VarType);
}

} //reasoning
