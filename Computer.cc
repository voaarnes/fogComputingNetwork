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

ComputerMessage *Computer::generateNewMessage(char* str){
    char msgname[20];
    sprintf(msgname, "%d-%s", lastSeq, str);
    ComputerMessage *msg = new ComputerMessage(msgname);

    return msg;
}

void Computer::initialize() {
    // Send contents to cloud
    timeoutMsg = new ComputerMessage("timeout");


    // Send contents

    char str[20] = "Book contents";
    ComputerMessage *message = generateNewMessage(str); // Creating placeholder message for access to variables in generate.
    message->setSeq(1);
    message->setSource(1);

    message->setType(MSG_CONTENTS);
    sendMessage(message, 0);



}
void Computer::handleMessage(omnetpp::cMessage *msg) {
    if (msg == timeoutMsg){
           // REsend last
           resendLastMessage();
       }

       ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
       if (cMsg != NULL){


           if (cMsg->getType() != 0){
               EV << "ACKING";
               ackMessage(cMsg);
               lastSeq = cMsg->getSeq();

           }
           switch (cMsg->getType()){
               case MSG_ACK: {
                   lastAcked = true;
                   cancelEvent(timeoutMsg);
                   break;
               }
               case MSG_BOOK_PAY: {
                   EV << "Host pay\n";
                   // Send paid
                   // Send contents


                   char str[20] = "Book contents";
                   ComputerMessage *message = generateNewMessage(str); // Creating placeholder message for access to variables in generate.
                   message->setSeq(1);
                   message->setSource(1);

                   message->setType(MSG_CONTENTS);
                   sendMessage(message, 0);

                   break;
               }
               default: {
                   EV << "This should not happen. case computer: " <<  cMsg->getType() << "\n";
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
        //send(toSend, cloudGate);
        EV << "TEST\n";
        send(toSend, "cloudgate$o");
    } else {
        //send(toSend, "hostgate$o");
    }
    scheduleAt(omnetpp::simTime()+timeout, timeoutMsg);
}

void Computer::ackMessage(ComputerMessage* msg){
    ComputerMessage *ack;
    int source = msg->getSource();
    if (source == 2){
        ack = new ComputerMessage("ACK from Computer to Host");
    } else {
        ack = new ComputerMessage("ACK from Computer to Cloud");
    }

    ack->setType(0);
    ack->setSource(1);

    if (source == 1){
       EV << "This should not happen";
   } else if (source == 2){
       send(ack, "hostgate$o");
   } else {
       send(ack, "cloudgate$o");
   }
}


void Computer::resendLastMessage(){
    ComputerMessage *toSend = lastMsg->dup();

    if (lastDest == 1){
        EV << "This should not happen";
    } else if (lastDest == 2){
        send(toSend, "hostgate$o");
    } else {
        send(toSend, "hostgate$o");
    }
    scheduleAt(omnetpp::simTime()+timeout, timeoutMsg);
}
