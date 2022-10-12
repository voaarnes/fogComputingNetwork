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

#ifndef FOGNODE_H_
#define FOGNODE_H_

//#include <omnetpp/csimplemodule.h>
#include <omnetpp.h>
#include "MessageType.h"
#include "computerMessage_m.h"

class FogNode: public omnetpp::cSimpleModule {
public:
    FogNode();
    virtual ~FogNode();
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;

private:
    void sendMessage(ComputerMessage *msg); // Sends a message and starts timeout sequence
    void SendContentsCloud(); // Send the library contents to the cloud
    void LocateBook(); // Locates the book
    void RecievePaymentForBook(); // Sells a book
    void MarkBookAsSold(); // Will mark book as sold and updated the cloudd
};
Define_Module(FogNode);
#endif /* FOGNODE_H_ */
