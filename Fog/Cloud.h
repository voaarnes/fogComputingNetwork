//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef CLOUD_H_
#define CLOUD_H_

#include <omnetpp/csimplemodule.h>
#include <stdio.h>
#include <string.h>
#include "computerMessage_m.h"
#include <omnetpp.h>
#include "MessageType.h"
#include "Constants.h"

class Cloud: public omnetpp::cSimpleModule {
public:
    Cloud();
    virtual void initialize() override;
    virtual ~Cloud();
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void refreshDisplay() const override;

private:

    omnetpp::simtime_t timeout;         // timeout
    int lastSeq = 0;
    int msgLost = 0;
    virtual ComputerMessage *generateNewMessage(char* str);
    void sendMessage(ComputerMessage* msg, int dest);
    void resendLastMessage(int dest);
    void ackMessage(ComputerMessage* msg);
    ComputerMessage * timeoutFog;
    ComputerMessage * timeoutHost;
    ComputerMessage *lastFog = NULL;
    ComputerMessage *lastHost = NULL;
    ComputerMessage *processingDelay = NULL;
    ComputerMessage *cachedMessage = NULL;

    int side;
    long msgSentHost;
    long msgSentComputer;
    long msgReceivedHost;
    long msgReceivedComputer;

    long ackSentHost;
    long ackSentComputer;
    long ackReceivedHost;
    long ackReceivedComputer;

};

Define_Module(Cloud);
#endif /* CLOUD_H_ */
