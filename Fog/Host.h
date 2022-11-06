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

#ifndef HOST_H_
#define HOST_H_

#include <omnetpp.h>
#include <omnetpp/csimplemodule.h>
#include "computerMessage_m.h"
#include "MessageType.h"

class Host: public omnetpp::cSimpleModule {
private:
    omnetpp::simtime_t timeout;         // timeout
    omnetpp::cMessage *timeoutMsg;    // holds pointer to the timeout self-message
    omnetpp::cMessage *doneWalkingMsg;    // holds pointer to the timeout self-message
    ComputerMessage *message;  // message that has to be re-sent on timeout
public:
    Host();
    virtual ~Host();
    virtual void initialize() override;
    virtual ComputerMessage *generateNewMessage(char* str);
    virtual void sendMessage(ComputerMessage *msg, int dest);
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void resendLastMessage(int dest);
    virtual void refreshDisplay() const override;
private:
    void ackMessage(ComputerMessage* msg);
    ComputerMessage * timeoutFog;
    ComputerMessage * timeoutCloud;
    ComputerMessage * payBookDelay;
    ComputerMessage *lastFog = NULL;
    ComputerMessage *lastCloud = NULL;
    int lastSeq = 0;
    int msgLost = 0;

    long msgSentCloud;
    long msgSentComputer;
    long msgReceivedCloud;
    long msgReceivedComputer;

    const int S_POWER_HOST_TO_CLOUD = 300;
    const int S_POWER_HOST_TO_FOG = 300;
    const int R_POWER_CLOUD_TO_HOST = 250;
    const int R_POWER_FOG_TO_HOST = 250;
    const int S_DELAY_HOST_TO_CLOUD = 400;
    const int S_DELAY_HOST_TO_FOG = 50;
    const int R_DELAY_CLOUD_TO_HOST= 400;
    const int R_DELAY_FOG_TO_HOST = 50;
};
Define_Module(Host);
#endif /* HOST_H_ */
