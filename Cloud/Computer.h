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

#ifndef COMPUTER_H_
#define COMPUTER_H_

//#include <omnetpp/csimplemodule.h>
#include <omnetpp.h>
#include "computerMessage_m.h"
#include "MessageType.h"
#include "Constants.h"

class Computer: public omnetpp::cSimpleModule {
public:
    Computer();
    virtual ~Computer();
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void refreshDisplay() const override;
private:
    void sendMessage(ComputerMessage* msg, int dest);
    void resendLastMessage(int dest);
    void ackMessage(ComputerMessage* msg);

    ComputerMessage * timeoutHost;
    ComputerMessage * timeoutCloud;
    ComputerMessage *lastHost = NULL;
    ComputerMessage *lastCloud = NULL;
    ComputerMessage *generateNewMessage(char* str);
    int lastDest;
    int lastSeq = 0;

    float timeout = 1.0f;
    bool lastAcked = false;

    long msgSentHost;
    long msgSentCloud;
    long msgReceivedHost;
    long msgReceivedCloud;


    long ackSentHost;
    long ackSentCloud;
    long ackReceivedHost;
    long ackReceivedCloud;

};
Define_Module(Computer);
#endif /* COMPUTER_H_ */
