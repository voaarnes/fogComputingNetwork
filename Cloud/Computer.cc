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
    timeout = 1.0;

}

Computer::~Computer() {
    // TODO Auto-generated destructor stub
    cancelEvent(timeoutHost);
    cancelEvent(timeoutCloud);
    delete timeoutCloud;
    delete timeoutHost;
}

ComputerMessage *Computer::generateNewMessage(char* str){
    char msgname[20];
    lastSeq++;
    sprintf(msgname, "%d-%s", lastSeq, str);
    ComputerMessage *msg = new ComputerMessage(msgname);
    msg->setSeq(lastSeq);
    msg->setSource(1);
    return msg;
}

void Computer::initialize() {
    // Send contents to cloud
    timeoutHost = new ComputerMessage("timeoutHost");
    timeoutCloud = new ComputerMessage("timeoutCloud");

    msgSent = 0;
    msgReceived = 0;

    WATCH(msgSent);
    WATCH(msgReceived);
    // Send contents

    char str[20] = "Book contents";
    ComputerMessage *message = generateNewMessage(str); // Creating placeholder message for access to variables in generate.
    message->setType(MSG_CONTENTS);
    sendMessage(message, 0);


}
void Computer::handleMessage(omnetpp::cMessage *msg) {
    if (msg == timeoutHost){
           // REsend last
           resendLastMessage(2);
           return;
       }
    if (msg == timeoutCloud){
               // REsend last
               resendLastMessage(0);
               return;
           }
    msgReceived++;
       ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
       if (cMsg != NULL){


           if (cMsg->getType() != 0){
               EV << "ACKING";
               ackMessage(cMsg);

           }
           int type = cMsg->getType();
           int src = cMsg->getSource();
           delete cMsg;
           switch (type){
               case MSG_ACK: {

                   if (src == 0){
                       cancelEvent(timeoutCloud);
                       delete lastCloud;
                   } else {
                       cancelEvent(timeoutHost);
                       delete lastHost;
                   }

                   break;
               }
               case MSG_BOOK_PAY: {
                   EV << "Host pay\n";
                   // Send paid
                   // Send contents
                   bubble("Book payed");
                   char str[20] = "Book payed";
                  ComputerMessage *message = generateNewMessage(str); // Creating placeholder message for access to variables in generate.
                  message->setType(MSG_BOOK_PAYED);
                  sendMessage(message, 2);


                   char str1[20] = "Book contents";
                   message = generateNewMessage(str1); // Creating placeholder message for access to variables in generate.

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

}

void Computer::sendMessage(ComputerMessage* msg, int dest){
    msgSent++;
    ComputerMessage *toSend = msg->dup();

    if (dest == 1){
        EV << "This should not happen";
    } else if (dest == 0){
        send(toSend, "cloudgate$o");

        lastCloud = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutCloud);
    } else {
        send(toSend, "hostgate$o");


        lastHost = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutHost);
    }

}

void Computer::ackMessage(ComputerMessage* msg){
    msgSent++;
    lastSeq = msg->getSeq();
    ComputerMessage *ack;
    int source = msg->getSource();
    char msgText[25];
    if (source == 2){
        sprintf(msgText, "ACK from Computer to Host");
    } else {
        sprintf(msgText, "ACK from Computer to Cloud");
    }
    ack = generateNewMessage(msgText);
    ack->setType(MSG_ACK);
    ack->setSource(1);

    if (source == 1){
       EV << "This should not happen";
   } else if (source == 2){
       send(ack, "hostgate$o");
   } else {
       send(ack, "cloudgate$o");
   }
}


void Computer::resendLastMessage(int dest){
    if (dest == 1){
        EV << "This should not happen";
    } else if (dest == 2){
        ComputerMessage *toSend = lastHost->dup();
        send(toSend, "hostgate$o");
        scheduleAt(omnetpp::simTime()+timeout, timeoutHost);

    } else {
        ComputerMessage *toSend = lastCloud->dup();
        send(toSend, "cloudgate$o");
        scheduleAt(omnetpp::simTime()+timeout, timeoutCloud);
    }

}

void Computer::refreshDisplay() const{
    char buffer[20];
    sprintf(buffer, "sent: %ld rcvd: %ld", msgSent, msgReceived);
    getDisplayString().setTagArg("t", 0, buffer);
}
