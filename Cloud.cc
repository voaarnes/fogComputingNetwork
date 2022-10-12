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

#include "Cloud.h"



Cloud::Cloud() {
    // TODO Auto-generated constructor stub

}



void Cloud::initialize() {
    // Create timeout message
    timeoutMsg = new ComputerMessage("timeout");
}


void Cloud::handleMessage(omnetpp::cMessage *msg) {
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
           case 1: {
               EV << "Received contents\n";
               if (!isStarted) {
                   ComputerMessage* newMsg = new ComputerMessage("Cloud ready to start");
                  newMsg->setType(2);
                  newMsg->setSource(0);
                  sendMessage(newMsg, 3);
                  isStarted = true;
               }

               break;
           }
           case 3: {
               // Reply type 4
               // Left or right
               EV << "Received request for book\n";
               ComputerMessage* newMsg = new ComputerMessage("Book is left");
               newMsg->setType(4);
               newMsg->setSource(0);
               sendMessage(newMsg, 3);

               break;
           }
           default: {
               EV << "This should not happen. case cloud: " <<  cMsg->getType() << "\n";
               break;
           }
       }
       delete cMsg;
   }

}

void Cloud::ackMessage(ComputerMessage* msg){
    ComputerMessage *ack;
    if (msg->getSource() == 1){
        ack = new ComputerMessage("ACK from Cloud to Computer");
    } else {
        ack = new ComputerMessage("ACK from Cloud to Host");
    }

    ack->setType(0);
    ack->setSource(0);

}




void Cloud::sendMessage(ComputerMessage* msg, int dest){
    delete lastMsg;
    lastMsg = msg;
    ComputerMessage *toSend = msg->dup();
    lastDest = dest;
    lastAcked = false;
    if (dest == 0){
        EV << "This should not happen";
    } else if (dest == 1){
        send(toSend, computerGate);
    } else {
        send(toSend, hostGate);
    }
    scheduleAt(omnetpp::simTime()+timeout, timeoutMsg);
}





void Cloud::resendLastMessage(){
    ComputerMessage *toSend = lastMsg->dup();

    if (lastDest == 0){
            EV << "This should not happen";
    } else if (lastDest == 1){
        send(toSend, computerGate);
    } else {
        send(toSend, hostGate);
    }
    scheduleAt(omnetpp::simTime()+timeout, timeoutMsg);
}




Cloud::~Cloud() {
    // TODO Auto-generated destructor stub
    delete timeoutMsg;
}

