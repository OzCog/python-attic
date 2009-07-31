/*
 * src/AtomSpace/utils.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Thiago Maia <thiago@vettatech.com>
 *            Andre Senna <senna@vettalabs.com>
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

#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <time.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/time.h>
#endif

#include "utils.h"
#include <opencog/util/platform.h>

using namespace opencog;

std::string opencog::toString(double data)
{
    char buffer[256];
    sprintf(buffer, "%f", data);
    return buffer;
}

std::string opencog::bold(int i)
{
    char buf[256];
    sprintf(buf, "[1m%d[0m", i);
    return buf;
}

std::string opencog::bold(const char* str)
{
    char buf[512];
    sprintf(buf, "[1m%s[0m", str);
    return buf;
}

std::string opencog::padstr(const char* s, unsigned int size, padAlignment a) throw (InvalidParamException)
{
    std::string answer;
    switch (a) {
    case CENTER: {
        unsigned int lpadding = (size - strlen(s)) / 2;
        unsigned int rpadding = lpadding;
        if (lpadding + rpadding + strlen(s) != size) rpadding++;
        char* charBuf = new char[size + 1];
        for (unsigned int i = 0; i < size; i++) charBuf[i] = ' ';
        for (unsigned int i = 0; i < strlen(s); i++) charBuf[i + lpadding] = s[i];
        charBuf[size] = '\0';
        answer = charBuf;
        delete[](charBuf);
        return answer;
    }
    case LEFT: {
        unsigned int rpadding = size - strlen(s);
        char* charBuf = new char[size + 1];
        for (unsigned int i = rpadding; i < size; i++) charBuf[i] = ' ';
        for (unsigned int i = 0; i < strlen(s); i++) charBuf[i] = s[i];
        charBuf[size] = '\0';
        answer = charBuf;
        delete[](charBuf);
        return answer;
    }
    case RIGHT: {
        unsigned int lpadding = size - strlen(s);
        char* charBuf = new char[size + 1];
        for (unsigned int i = 0; i < lpadding - 1; i++) charBuf[i] = ' ';
        for (unsigned int i = 0; i < strlen(s); i++) charBuf[i + lpadding] = s[i];
        charBuf[size] = '\0';
        answer = charBuf;
        delete[](charBuf);
        return answer;
    }
    default:
        throw InvalidParamException(TRACE_INFO, "utils - Invalid padAlignmet parameter.");
        break;
    }
    return (answer);
}

const char* opencog::nextLine(const char *from, std::string &line)
{
    line = "";
    if (from != NULL) {
        // Gets the first line
        const char* lineStart = from;
        int lineLength = 0;
        while ((*from != '\n') && ((*from != '\0'))) {
            from++; lineLength++;
        }
        line = std::string(lineStart, lineLength);
        // Points return value to the next line
        if (*from == '\0') {
            from = NULL;
        } else {
            from++;
        }
    }
    return(from);
}

// TODO: Review this method for both 32/64-bit processor compatibility
//int opencog::bitcount(unsigned long n)
//{
    /* works for 32-bit numbers only    */
    /* fix last line for 64-bit numbers */

 //   register unsigned long tmp;

  //  tmp = n - ((n >> 1) & 033333333333)
    //      - ((n >> 2) & 011111111111);
   // return ((tmp + (tmp >> 3)) & 030707070707) % 63;
//}

namespace opencog {

vtree MakeVirtualAtom_slow(Vertex v, const vtree& t1, const vtree& t2, const vtree& t3, const vtree& t4)
{
    vtree ret;
	try
	{
		ret.set_head(v);
		ret.append_child(ret.begin(), t1.begin());
		ret.append_child(ret.begin(), t2.begin());
		ret.append_child(ret.begin(), t3.begin());
		ret.append_child(ret.begin(), t4.begin());
	} catch(...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);
    }
    return ret;
}

vtree MakeVirtualAtom_slow(Vertex v, const vtree& t1, const vtree& t2, const vtree& t3)
{
    vtree ret;
	try {
		ret.set_head(v);
		ret.append_child(ret.begin(), t1.begin());
		ret.append_child(ret.begin(), t2.begin());
		ret.append_child(ret.begin(), t3.begin());
	} catch(...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);
    }
    return ret;
}

vtree MakeVirtualAtom_slow(Vertex v, const vtree& t1, const vtree& t2)
{
    vtree ret;
	try {
		ret.set_head(v);
		ret.append_child(ret.begin(), t1.begin());
		ret.append_child(ret.begin(), t2.begin());
	} catch(...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);
    }
    return ret;
}

vtree MakeVirtualAtom_slow(Vertex v, const vtree& t1)
{
    vtree ret;
	try {
		ret.set_head(v);
		ret.append_child(ret.begin(), t1.begin());
	} catch(...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);
    }
    return ret;
}

vtree MakeVirtualAtom_slow(Vertex v)
{
    vtree ret;
	try {
		ret.set_head(v);
	} catch(...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);
    }
    return ret;
}

}

bool less_tree_vertex::operator()(const vtree& lhs, const vtree& rhs) const
{
    return (*this)(lhs, rhs, lhs.begin(), rhs.begin());
}
bool less_tree_vertex::operator()(const vtree& lhs, const vtree& rhs,
                                  vtree::iterator ltop,
                                  vtree::iterator rtop) const
{
    if (*ltop < *rtop)
        return true;
    if (*rtop < *ltop)
        return false;

    if (ltop.number_of_children() < rtop.number_of_children())
        return true;
    if (ltop.number_of_children() > rtop.number_of_children())
        return false;

    vtree::sibling_iterator r_i = rhs.begin(rtop);
    for (vtree::sibling_iterator i = lhs.begin(ltop);
            i != lhs.end(ltop);
            i++) {
        if (less_tree_vertex()(lhs, rhs, i, r_i))
            return true;
        else
            if (less_tree_vertex()(rhs, lhs, r_i, i))
                return false;

        r_i++;
    }

    return false;
}

/* MISC UTILITIES */

StringTokenizer::StringTokenizer(const std::string &rStr, const std::string &rDelimiters)
{
    std::string::size_type lastPos(rStr.find_first_not_of(rDelimiters, 0));
    std::string::size_type pos(rStr.find_first_of(rDelimiters, lastPos));
    while (std::string::npos != pos || std::string::npos != lastPos) {
        push_back(rStr.substr(lastPos, pos - lastPos));
        lastPos = rStr.find_first_not_of(rDelimiters, pos);
        pos = rStr.find_first_of(rDelimiters, lastPos);
    }
}

std::vector<std::string> StringTokenizer::WithoutEmpty() const
{
    std::vector<std::string> ret;

    for (unsigned int i = 0; i < this->size(); i++)
        if (!(*this)[i].empty())
            ret.push_back((*this)[i]);

    return ret;
}


/**
 * Time used as reference to set/get timestamps over the code
 */
static timeval referenceTime;
static bool referenceTimeInitialized = false;

void initReferenceTime()
{
    gettimeofday(&referenceTime, NULL);
    referenceTimeInitialized = true;
}

unsigned long opencog::getElapsedMillis()
{
    cassert(TRACE_INFO, referenceTimeInitialized,
            "utils - refenceTimeInitialized should have been initialized.");
    timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return (currentTime.tv_sec -referenceTime.tv_sec)*1000 + (currentTime.tv_usec - referenceTime.tv_usec) / 1000;
}

bool less_vtree_it( const vtree & lhs_t, const vtree & rhs_t,
                    vtree::sibling_iterator ltop, vtree::sibling_iterator rtop)
{
    if ((*ltop) < (*rtop))
        return true;
    if ((*rtop) < (*ltop))
        return false;

    vtree::sibling_iterator rit = rhs_t.begin(rtop);

    for (vtree::sibling_iterator lit = lhs_t.begin(ltop);
            lit != lhs_t.end  (ltop);
            lit++, rit++)
        if (less_vtree_it(lhs_t, rhs_t, lit, rit))
            return true;
        else if (less_vtree_it(lhs_t, rhs_t, rit, lit))
            return false;

    return false;
}

bool less_vtree::operator()(const vtree& lhs, const vtree& rhs) const
{
    if (lhs.size() < rhs.size())
        return true;
    if (lhs.size() > rhs.size() || lhs.empty())
        return false;

    return less_vtree_it(lhs, rhs, lhs.begin(), rhs.begin());
}

std::string opencog::i2str(int d)
{
    char temp[20];
    sprintf(temp, "%d", d);
    return temp;
}

bool opencog::visible(char c)
{
    return c != ' ' && c != '\r' && c != '\n' && c != '\t' && c != 0;
}

//this typedef is put is the .cc not the .h because that provokes a vtree clash
//with combo::vtree
//typedef vtree vtree;
typedef vtree::iterator pre_it;

#if NOT_USED_ANYMORE_DELETE_ME_WHENEVER

vtree opencog::MakeVirtualAtom_slow(Handle T, const vtree& t1, const vtree& t2, const vtree& t3, const vtree& t4, const vtree& t5)
{
    //printf("MakeVirtualAtom_slow Handle, vtree, vtree, vtree, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(Vertex(T));
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());
        ret.replace(ret.append_child(head_it), t3.begin());
        ret.replace(ret.append_child(head_it), t4.begin());
        ret.replace(ret.append_child(head_it), t5.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin); return vtree();
    }
}
vtree opencog::MakeVirtualAtom_slow(Handle T, const vtree& t1, const vtree& t2, const vtree& t3, const vtree& t4)
{
    //printf("MakeVirtualAtom_slow Handle, vtree, vtree, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(Vertex(T));
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());
        ret.replace(ret.append_child(head_it), t3.begin());
        ret.replace(ret.append_child(head_it), t4.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin); return vtree();
    }
}
vtree opencog::MakeVirtualAtom_slow(Handle T, const vtree& t1, const vtree& t2, const vtree& t3)
{
    //printf("MakeVirtualAtom_slow Handle, vtree, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(Vertex(T));
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());
        ret.replace(ret.append_child(head_it), t3.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}

vtree opencog::MakeVirtualAtom_slow(Handle T, const vtree& t1, const vtree& t2)
{
    //printf("MakeVirtualAtom_slow Handle, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(Vertex(T));
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}

vtree opencog::MakeVirtualAtom_slow(Handle T, const vtree& t1)
{
    //printf("MakeVirtualAtom_slow Handle, vtree\n");
    try {
        vtree ret;
        ret.set_head(Vertex(T));
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}

vtree opencog::MakeVirtualAtom_slow(Handle T)
{
    //printf("MakeVirtualAtom_slow Handle\n");
    try {
        vtree ret;
        ret.set_head(Vertex(T));

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}
vtree opencog::MakeVirtualAtom_slow(Vertex T, const vtree& t1, const vtree& t2, const vtree& t3, const vtree& t4, const vtree& t5)
{
    //printf("MakeVirtualAtom_slow Vertex, vtree, vtree, vtree, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(T);
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());
        ret.replace(ret.append_child(head_it), t3.begin());
        ret.replace(ret.append_child(head_it), t4.begin());
        ret.replace(ret.append_child(head_it), t5.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}
vtree opencog::MakeVirtualAtom_slow(Vertex T, const vtree& t1, const vtree& t2, const vtree& t3, const vtree& t4)
{
    //printf("MakeVirtualAtom_slow Vertex, vtree, vtree, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(T);
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());
        ret.replace(ret.append_child(head_it), t3.begin());
        ret.replace(ret.append_child(head_it), t4.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}
vtree opencog::MakeVirtualAtom_slow(Vertex T, const vtree& t1, const vtree& t2, const vtree& t3)
{
    //printf("MakeVirtualAtom_slow Vertex, vtree, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(T);
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());
        ret.replace(ret.append_child(head_it), t3.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}

vtree opencog::MakeVirtualAtom_slow(Vertex T, const vtree& t1, const vtree& t2)
{
    //printf("MakeVirtualAtom_slow Vertex, vtree, vtree\n");
    try {
        vtree ret;
        ret.set_head(T);
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());
        ret.replace(ret.append_child(head_it), t2.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}

vtree opencog::MakeVirtualAtom_slow(Vertex T, const vtree& t1)
{
    //printf("MakeVirtualAtom_slow Vertex, vtree\n");
    try {
        vtree ret;
        ret.set_head(T);
        pre_it head_it = ret.begin();
        ret.replace(ret.append_child(head_it), t1.begin());

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}

vtree opencog::MakeVirtualAtom_slow(Vertex T)
{
    //printf("MakeVirtualAtom_slow Vertex\n");
    try {
        vtree ret;
        ret.set_head(T);

        return ret;

    } catch (...) {
        puts("MakeVirtualAtom_slow exception."); getc(stdin);  return vtree();
    }
}

#endif /* NOT_USED_ANYMORE_DELETE_ME_WHENEVER */

