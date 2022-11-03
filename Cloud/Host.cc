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
    payBookDelay = new ComputerMessage("payBook");

    msgSent = 0;
    msgReceived = 0;

    WATCH(msgSent);
    WATCH(msgReceived);
}


ComputerMessage *Host::generateNewMessage(char* str){
    char msgname[50];
    lastSeq++;
    sprintf(msgname, "%d-%s", lastSeq, str);
    ComputerMessage *msg = new ComputerMessage(msgname);
    msg->setSeq(lastSeq);
    msg->setSource(2);
    return msg;
}

void Host::sendMessage(ComputerMessage* msg, int dest){
    msgSent++;
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

        if (msg == payBookDelay){
                    char str[20] = "Pay the book.";
                    message = generateNewMessage(str);
                    message->setType(MSG_BOOK_PAY);
                    sendMessage(message, 0);
                }
        else {


            if (msgLost < 3){
                msgLost++;
                getParentModule()->bubble("Message lost");
                delete msg;
                return;
            }

            msgReceived++;
            ComputerMessage *cmmsg = omnetpp::check_and_cast<ComputerMessage *>(msg);
            if(cmmsg->getType() != 0){
                ackMessage(cmmsg);
            }
            int type = cmmsg->getType();
            int src = cmmsg->getSource();
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

                 break;
              }
              case MSG_CLOUD_RDY:
              {

                  getParentModule()->bubble("Ready to start");
                  char str[50] = "Where is the book i am looking for?";
                  message = generateNewMessage(str);
                  message->setType(MSG_WHEREIS);
                  sendMessage(message, 0);
                  return;
                  break;
              }
              case MSG_FOUND_LEFT:{
                  EV << "To be animated.";
                  getParentModule()->bubble("Book is left");

                  scheduleAt(33.0, payBookDelay);
                  break;
              }
              case MSG_FOUND_RIGHT:{
                   EV << "To be animated.";
                   getParentModule()->bubble("Book is RIGHT");
                   scheduleAt(33.0, payBookDelay);
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
    msgSent++;
    lastSeq = msg->getSeq();
    ComputerMessage *ack;
    int source = msg->getSource();
    char msgText[30];
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

void Host::refreshDisplay() const{
    char buffer[40];
    sprintf(buffer, "sent: %ld rcvd: %ld lost: %i", msgSent, msgReceived, msgLost);
    getParentModule()->getDisplayString().setTagArg("t", 0, buffer);
}
