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

uint8_t current_status = 0;
int currentSeq = 0;
ComputerMessage * timeoutEvent;
ComputerMessage *lastMsg;
int lastDest;
bool isStarted = false;
int timeout = 1;
bool lastAcked = false;


char computerGate[10];
char hostGate[10];


void Cloud::initialize() {
    // Create timeout message
    timeoutEvent = new omnetpp::cMessage("timeoutEvent");
    timeout = 1.0;

    EV << "Initialized cloud";
    /*
    char str[20] = "Hello";
    ComputerMessage *message = new ComputerMessage(str); // Creating placeholder message for access to variables in generate.
    message->setSeq(1);
    message->setSource(0);
    message->setType(3);
    sendMessage(message, 2);
    */
}


ComputerMessage *Cloud::generateNewMessage(char* str, int seq){
    char msgname[40];

    //sprintf(msgname, "%d-%s", lastMsg->getSeq(), str);
    sprintf(msgname, "%d-%s", seq+1, str);
    ComputerMessage *msg = new ComputerMessage(msgname);
    return msg;
}


void Cloud::handleMessage(omnetpp::cMessage *msg) {

   EV << "HIIIIIT";

   if (msg == timeoutEvent){
       // REsend last
       ComputerMessage *toSend = lastMsg->dup();
       EV << "TIMEOUT!\n";
       if (lastDest == 0){
           EV << "This should not happen";
       } else if (lastDest == 1){
           send(toSend, "foggate$o");
       } else {
           send(toSend, "hostgate$o");
       }
       scheduleAt(omnetpp::simTime()+timeout, timeoutEvent);
   }

   //ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
   ComputerMessage *cMsg = omnetpp::check_and_cast<ComputerMessage *>(msg);
   if (cMsg != NULL){

       if (cMsg->getType() != 0){
           ackMessage(cMsg);

       }

       switch (cMsg->getType()){
           case 0: {
               lastAcked = true;
               cancelEvent(timeoutEvent);
               break;
           }
           case 1: {
               EV << "Received contents\n";
               if (!isStarted) {
                  ComputerMessage* newMsg = new ComputerMessage("Cloud ready to start");
                  newMsg->setSeq(cMsg->getSeq()+1);
                  newMsg->setType(2);
                  newMsg->setSource(0);
                  sendMessage(newMsg, 2);
               }

               break;
           }
           case 3: {
               // Reply type 4
               // Left or right
               EV << "Received request for book\n";
               ComputerMessage* newMsg = new ComputerMessage("Book is left");
               newMsg->setSeq(cMsg->getSeq()+1);
               newMsg->setType(4);
               newMsg->setSource(0);
               sendMessage(newMsg, 2);

               break;
           }
           default: {
               EV << "This should not happen. case cloud: " <<  cMsg->getType() << "\n";
               break;
           }
           delete cMsg;
       }

   }


}

void Cloud::ackMessage(ComputerMessage* msg){

    char str[40] = "ACK from host to y";

    int source = msg->getSource();
    if (source == 1){
        char str[40] = "ACK from Cloud to Computer";
    } else {
        char str[40] = "ACK from Cloud to Computer";
    }
    ComputerMessage *ack = generateNewMessage(str, msg->getSeq());
    ack->setType(0);
    ack->setSource(0);
    if (source == 0){
       EV << "This should not happen";
   } else if (source == 1){
       send(ack, "foggate$o");
   } else {
       send(ack, "hostgate$o");
   }

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
    scheduleAt(omnetpp::simTime()+timeout, timeoutEvent);
}




Cloud::~Cloud() {
    // TODO Auto-generated destructor stub
    delete timeoutEvent;
}

