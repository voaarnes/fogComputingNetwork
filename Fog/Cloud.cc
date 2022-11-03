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

bool isStarted = false;


void Cloud::initialize() {
    // Create timeout message
    // Send contents to cloud
    timeoutHost = new ComputerMessage("timeoutHost");
    timeoutFog = new ComputerMessage("timeoutFog");
    msgSent = 0;
    msgReceived = 0;

    WATCH(msgSent);
    WATCH(msgReceived);
}


ComputerMessage *Cloud::generateNewMessage(char* str){
    char msgname[50];
    lastSeq++;
    sprintf(msgname, "%d-%s", lastSeq, str);
    ComputerMessage *msg = new ComputerMessage(msgname);
    msg->setSeq(lastSeq);
    msg->setSource(0);
    return msg;
}


void Cloud::handleMessage(omnetpp::cMessage *msg) {
   if (msg == timeoutHost){
       resendLastMessage(2);
       return;
   }
   if (msg == timeoutFog){
          resendLastMessage(1);
          return;
  }
   msgReceived++;
   ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
   if (cMsg != NULL){
       if (cMsg->getType() != 0){
           ackMessage(cMsg);
       }

       int type = cMsg->getType();
       int src = cMsg->getSource();
       int seq = cMsg->getSeq();
       delete cMsg;
       switch (type){
           case 0: {
               if (src == 1){
                   cancelEvent(timeoutFog);
                   delete lastFog;
               } else {
                   cancelEvent(timeoutHost);
                   delete lastHost;
               }
               lastSeq = seq;
               break;
           }
           case MSG_CONTENTS: {
               EV << "Received contents\n";
               /*
               if (!isStarted) {
                  ComputerMessage* newMsg = generateNewMessage("Cloud ready to start");
                  newMsg->setType(MSG_CLOUD_RDY);
                  sendMessage(newMsg, 1);
                  isStarted = true;
               }
                */
               break;
           }
           case MSG_WHEREIS: {
               // Reply type 4
               // Left or right
               EV << "Received request for book\n";

               int side = par("leftRightSide").intValue();
               if (side == 0){
                   char str[40] = "Book is LEFT";
                   ComputerMessage* newMsg = generateNewMessage(str);
                  newMsg->setType(MSG_FOUND_LEFT);
                  sendMessage(newMsg, 2);
               } else {
                   char str[40] = "Book is RIGHT";
                   ComputerMessage* newMsg = generateNewMessage(str);
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
    msgSent++;
    lastSeq = msg->getSeq();
    EV << "Last: " << lastSeq;
    ComputerMessage *ack;
    int source = msg->getSource();
    char msgText[30];
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

void Cloud::resendLastMessage(int dest){

    msgSent++;
    if (dest == 0){
        EV << "This should not happen";
    } else if (dest == 1){
        ComputerMessage *toSend = lastFog->dup();
        send(toSend, "foggate$o");
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);

    } else {
        ComputerMessage *toSend = lastHost->dup();
        send(toSend, "hostgate$o");
        scheduleAt(omnetpp::simTime()+timeout, timeoutHost);
    }

}


void Cloud::sendMessage(ComputerMessage* msg, int dest){
    msgSent++;
    ComputerMessage *toSend = msg->dup();

    if (dest == 0){
        EV << "This should not happen";
    } else if (dest == 1){
        send(toSend, "foggate$o");

        lastFog = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);
    } else {
        send(toSend, "hostgate$o");


        lastHost = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutHost);
    }

}



void Cloud::refreshDisplay() const{
    char buffer[20];
    sprintf(buffer, "sent: %ld rcvd: %ld", msgSent, msgReceived);
    getDisplayString().setTagArg("t", 0, buffer);
}


Cloud::~Cloud() {
    cancelEvent(timeoutHost);
    cancelEvent(timeoutFog);
    delete timeoutFog;
    delete timeoutHost;
}

