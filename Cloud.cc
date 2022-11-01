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
    timeout = 1.0;
}

uint8_t current_status = 0;
int currentSeq = 0;
ComputerMessage * timeoutMsg;
ComputerMessage *lastMsg;
int lastDest;
bool isStarted = false;
bool lastAcked = false;


char computerGate[10];
char hostGate[10];


void Cloud::initialize() {
    // Create timeout message
    timeoutMsg = new ComputerMessage("timeout");

}


ComputerMessage *Cloud::generateNewMessage(char* str){
    char msgname[20];
    lastSeq++;
    sprintf(msgname, "%d-%s", lastSeq, str);
    ComputerMessage *msg = new ComputerMessage(msgname);
    msg->setSeq(lastSeq);
    msg->setSource(0);
    return msg;
}


void Cloud::handleMessage(omnetpp::cMessage *msg) {
   if (msg == timeoutMsg){
       resendLastMessage();
       return;
   }

   ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
   if (cMsg != NULL){
       if (cMsg->getType() != 0){
           ackMessage(cMsg);
       }

       int type = cMsg->getType();
       delete cMsg;
       switch (type){
           case 0: {
               lastAcked = true;
               cancelEvent(timeoutMsg);
               break;
           }
           case MSG_CONTENTS: {
               EV << "Received contents\n";
               if (!isStarted) {
                  ComputerMessage* newMsg = generateNewMessage("Cloud ready to start");
                  newMsg->setType(MSG_CLOUD_RDY);
                  sendMessage(newMsg, 2);
                  isStarted = true;
               }

               break;
           }
           case 3: {
               // Reply type 4
               // Left or right
               EV << "Received request for book\n";

               int side = par("leftRightSide").intValue();
               if (side == 0){
                   ComputerMessage* newMsg = generateNewMessage("Book is left");
                  newMsg->setType(MSG_FOUND_LEFT);
                  sendMessage(newMsg, 2);
               } else {
                   ComputerMessage* newMsg = generateNewMessage("Book is RIGHT");
                  newMsg->setType(MSG_FOUND_RIGHT);
                  sendMessage(newMsg, 2);
               }

               break;
           }
           default: {
               EV << "This should not happen. case cloud";
               break;
           }
       }

   } else {
       delete msg;
   }


}

void Cloud::ackMessage(ComputerMessage* msg){
    lastSeq = msg->getSeq();
    EV << "Last: " << lastSeq;
    ComputerMessage *ack;
    int source = msg->getSource();
    char msgText[25];
    if (source == 2){
        sprintf(msgText, "ACK from Cloud to Host");
    } else {
        sprintf(msgText, "ACK from Cloud to Computer");
    }
    ack = generateNewMessage(msgText);
    ack->setType(MSG_ACK);

    ack->setSource(0);
    if (source == 0){
       EV << "This should not happen";
   } else if (source == 1){
       send(ack, "foggate$o");
   } else {
       send(ack, "hostgate$o");
   }

}

void Cloud::resendLastMessage(){
    ComputerMessage *toSend = lastMsg->dup();

    if (lastDest == 0){
        EV << "This should not happen";
    } else if (lastDest == 1){
        send(toSend, "foggate$o");
        EV << "RESEND0";
    } else {
        send(toSend, "hostgate$o");
        EV << "RESEND2";
    }
    EV << "RESEND";
    scheduleAt(omnetpp::simTime()+timeout, timeoutMsg);
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
        send(toSend, "foggate$o");
    } else {
        send(toSend, "hostgate$o");
    }
    scheduleAt(omnetpp::simTime()+1, timeoutMsg);
}




Cloud::~Cloud() {
    cancelEvent(timeoutMsg);
    delete timeoutMsg;
}

