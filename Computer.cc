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

#include "Computer.h"




Computer::Computer() {
    // TODO Auto-generated constructor stub

}

Computer::~Computer() {
    // TODO Auto-generated destructor stub
    delete timeoutMsg;
}

void Computer::initialize() {
    // Send contents to cloud
    timeoutMsg = new ComputerMessage("timeout");


    // Send contents


}
void Computer::handleMessage(omnetpp::cMessage *msg) {
    if (msg == timeoutMsg){
           // REsend last
           resendLastMessage();
       }

       ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
       if (cMsg != NULL){


           if (cMsg->getType() != 0){
               ackMessage(cMsg);

           }
           switch (cMsg->getType()){
               case 0: {
                   lastAcked = true;
                   cancelEvent(timeoutMsg);
                   break;
               }
               case 5: {
                   EV << "Host pay\n";
                   // Send paid
                   // Send contents

                   break;
               }
               default: {
                   EV << "This should not happen. case cloud: " <<  cMsg->getType() << "\n";
                   break;
               }
           }
       }
       delete cMsg;

}

void Computer::sendMessage(ComputerMessage* msg, int dest){
    if (lastMsg != NULL){
        delete lastMsg;
    }
    lastMsg = msg;
    ComputerMessage *toSend = msg->dup();
    lastDest = dest;
    lastAcked = false;

    if (dest == 1){
        EV << "This should not happen";
    } else if (dest == 0){
        send(toSend, cloudGate);
    } else {
        send(toSend, hostGate);
    }
    scheduleAt(omnetpp::simTime()+timeout, timeoutMsg);
}

void Computer::ackMessage(ComputerMessage* msg){
    ComputerMessage *ack;
    if (msg->getSource() == 2){
        ack = new ComputerMessage("ACK from Computer to Host");
    } else {
        ack = new ComputerMessage("ACK from Computer to Cloud");
    }

    ack->setType(0);
    ack->setSource(1);

}


void Computer::resendLastMessage(){
    ComputerMessage *toSend = lastMsg->dup();

    if (lastDest == 1){
        EV << "This should not happen";
    } else if (lastDest == 2){
        send(toSend, hostGate);
    } else {
        send(toSend, cloudGate);
    }
    scheduleAt(omnetpp::simTime()+timeout, timeoutMsg);
}