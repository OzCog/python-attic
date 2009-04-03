/*
 * opencog/embodiment/Control/MessagingSystem/PetMessageSender.h
 *
 * Copyright (C) 2007-2008 Carlos Lopes
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
#ifndef PETMESSAGESENDER_H_
#define PETMESSAGESENDER_H_

#include "MessageSender.h"
#include "NetworkElement.h"

namespace OperationalPetController
{

class PetMessageSender : public MessageSender
{

private:

    /**
     * A network element object used to send the messages to LS
     */
    MessagingSystem::NetworkElement * ne;

public:

    /**
     * Constructor
     */
    ~PetMessageSender();
    PetMessageSender(MessagingSystem::NetworkElement * ne);

    /**
     *
     */
    bool sendReward(const std::string &schema, const std::vector<std::string> & schemaArguments, const std::string &triedSchema, const double reward);

    /**
     *
     */
    bool sendExemplar(const std::string &schema, const std::vector<std::string> & schemaArguments, const std::string &ownerId, const std::string &avatarId, AtomSpace &atomSpace);

    /**
     *
     */
    bool sendCommand(const std::string &command, const std::string &schema);

    /**
     *
     */
    bool sendFeedback(const std::string &ownerId, const std::string &feedback);

    /**
      *
      */
    bool sendTrySchema(const std::string &schemaName, const std::vector<std::string> & schemaArgs);

    /**
     *
     */
    bool sendStopLearning(const std::string &schemaName, const std::vector<std::string> & schemaArgs);


}; // class
}  // namespace


#endif

