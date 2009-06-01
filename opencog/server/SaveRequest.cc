/*
 * opencog/server/SaveRequest.cc
 *
 * Copyright (C) 2008 by Singularity Institute for Artificial Intelligence
 * All Rights Reserved
 *
 * Written by Jared Wigmore <jared.wigmore@gmail.com>
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

#include "SaveRequest.h"

#include <vector>
#include <fstream>

#include <opencog/server/CogServer.h>
//#include <opencog/xml/FileXMLBufferReader.h>
#include <opencog/xml/NMXmlExporter.h>
#include <opencog/util/Logger.h>
//#include <opencog/xml/NMXmlParser.h>
//#include <opencog/xml/XMLBufferReader.h>
#include <opencog/util/exceptions.h>
#include <opencog/atomspace/HandleSet.h>

using namespace opencog;

SaveRequest::SaveRequest()
{
}

SaveRequest::~SaveRequest()
{
}

bool SaveRequest::execute()
{
    logger().debug("[SaveRequest] execute");
    std::ostringstream oss;

    if (_parameters.empty()) {
        oss << "invalid syntax: save <filename>" << std::endl;
        send(oss.str());
        return false;
    }

    AtomSpace* atomSpace = CogServer::getAtomSpace();

    std::string& filename = _parameters.front();
    HandleEntry *handles = atomSpace->getAtomTable().getHandleSet(ATOM, true);
    NMXmlExporter *exporter = new NMXmlExporter();
    std::fstream file(filename.c_str(), std::fstream::out);
    try {
        file << exporter->toXML(handles);
        file.flush();
        file.close();
    } catch (StandardException &e) {
        oss << "Error: unable to save to XML (" << e.getMessage() << ")" << std::endl;
        send(oss.str());
        return false;
    }
    oss << "done" << std::endl;
    send(oss.str());
    return true;
}
