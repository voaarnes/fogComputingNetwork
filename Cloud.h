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

class Cloud: public omnetpp::cSimpleModule {
public:
    Cloud();
    virtual void initialize() override;
    virtual ~Cloud();
    virtual void handleMessage(omnetpp::cMessage *msg) override;

private:
    void sendMessage(ComputerMessage* msg, int dest);
    void resendLastMessage();
    void ackMessage(ComputerMessage* msg);

private:
    uint8_t current_status = 0;
    int currentSeq = 0;
    ComputerMessage * timeoutMsg;
    ComputerMessage *lastMsg;
    int lastDest;
    bool isStarted = false;
    float timeout = 1.0f;
    bool lastAcked = false;


    char computerGate[10];
    char hostGate[10];

};

Define_Module(Cloud);
#endif /* CLOUD_H_ */
