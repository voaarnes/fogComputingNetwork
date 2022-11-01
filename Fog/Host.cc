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

#include "Host.h"

Host::Host() {
    // TODO Auto-generated constructor stub

}

Host::~Host() {
    // TODO Auto-generated destructor stub
    cancelEvent(timeoutCloud);
    cancelEvent(timeoutFog);
    delete timeoutFog;
    delete timeoutCloud;
}


void Host::initialize(){
    timeout = 1.0;
    timeoutFog = new ComputerMessage("timeoutFog");
    timeoutCloud = new ComputerMessage("timeoutCloud");

}


ComputerMessage *Host::generateNewMessage(char* str){
    char msgname[20];
    lastSeq++;
    sprintf(msgname, "%d-%s", lastSeq, str);
    ComputerMessage *msg = new ComputerMessage(msgname);
    msg->setSeq(lastSeq);
    msg->setSource(2);
    return msg;
}

void Host::sendMessage(ComputerMessage* msg, int dest){

    ComputerMessage *toSend = msg->dup();

    if (dest == 2){
        EV << "This should not happen";
    } else if (dest == 0){
        send(toSend, "cloudgate$o");
        lastCloud = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutCloud);
    } else {
        send(toSend, "foggate$o");
        lastFog = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);
    }

}




void Host::handleMessage(omnetpp::cMessage *msg){

        if(msg == timeoutCloud){
            resendLastMessage(0);
            delete msg;
            return;
        }
        if(msg == timeoutFog){
                    resendLastMessage(1);
                    delete msg;
                    return;
                }
        else {


            if (msgLost < 3){
                msgLost++;
                bubble("Message lost");
                delete msg;
                return;
            }
            ComputerMessage *cmmsg = omnetpp::check_and_cast<ComputerMessage *>(msg);
            if(cmmsg->getType() != 0){
                ackMessage(cmmsg);
            }
            int type = cmmsg->getType();
            int src = cmmsg->getSource();
            int seq = cmmsg->getSeq();
            delete cmmsg;

            switch(type) {
              case 0:
              {
                  if (src == 0){
                     cancelEvent(timeoutCloud);
                     delete lastCloud;
                 } else {
                     cancelEvent(timeoutFog);
                     delete lastFog;
                 }
                  lastSeq = seq;
                 break;
              }
              case MSG_CLOUD_RDY:
              {

                  bubble("Ready to start");
                  char str[50] = "Where is the book i am looking for?";
                  message = generateNewMessage(str);
                  message->setType(MSG_WHEREIS);
                  sendMessage(message, 1);
                  return;
                  break;
              }
              case MSG_FOUND_LEFT:{
                  EV << "To be animated.";
                  bubble("Book is left");

                  char str[20] = "Pay the book.";
                  message = generateNewMessage(str);
                  message->setType(MSG_BOOK_PAY);
                  sendMessage(message, 1);
                  break;
              }
              case MSG_FOUND_RIGHT:{
                   EV << "To be animated.";
                   bubble("Book is RIGHT");

                   char str[20] = "Pay the book.";
                   message = generateNewMessage(str);
                   message->setType(MSG_BOOK_PAY);
                   sendMessage(message, 1);
                   break;
               }
              default:
              {
                EV << "No coded response pepehands.";
              }
            }
        }


}




void Host::ackMessage(ComputerMessage* msg){
    lastSeq = msg->getSeq();
    ComputerMessage *ack;
    int source = msg->getSource();
    char msgText[25];
    if (source == 2){
        sprintf(msgText, "ACK from Host to Computer");
    } else {
        sprintf(msgText, "ACK from Host to Cloud");
    }
    ack = generateNewMessage(msgText);
    ack->setType(MSG_ACK);
    ack->setSource(2);
    if (source == 2){
       EV << "This should not happen";
   } else if (source == 1){
       send(ack, "foggate$o");
   } else {
       send(ack, "cloudgate$o");
   }

}


void Host::resendLastMessage(int dest){


    if (dest == 2){
        EV << "This should not happen";
    } else if (dest == 1){
        ComputerMessage *toSend = lastFog->dup();
        send(toSend, "foggate$o");
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);

    } else {
        ComputerMessage *toSend = lastCloud->dup();
        send(toSend, "cloudgate$o");
        scheduleAt(omnetpp::simTime()+timeout, timeoutCloud);
    }

}