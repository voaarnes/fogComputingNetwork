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
}


void Host::initialize(){
    timeout = 1.0;
    timeoutEvent = new omnetpp::cMessage("timeoutEvent");

}


ComputerMessage *Host::generateNewMessage(char* str){
    char msgname[20];
    sprintf(msgname, "%d-%s", message->getSeq(), str);
    ComputerMessage *msg = new ComputerMessage(msgname);
    return msg;
}

void Host::sendMessage(ComputerMessage *msg, int dest){
    ComputerMessage *copy = (ComputerMessage *)msg->dup();

    if(dest == 0){
        send(copy, "cloudgate$o");
    }else{
        send(copy, "foggate$o");
    }

    scheduleAt(omnetpp::simTime()+timeout, timeoutEvent);

}



void Host::handleMessage(omnetpp::cMessage *msg){

        if(msg == timeoutEvent){
            EV << "RESEND MESSAGE\n";
            sendMessage(message, 0);
        }
        else {
            EV << "ACK arrived\n";
            ComputerMessage *cmmsg = omnetpp::check_and_cast<ComputerMessage *>(msg);



            if(cmmsg->getType() != 0){
                char str[20] = "ACK from host to y";
                message = generateNewMessage(str);
                message->setSeq(message->getSeq()+1);
                message->setSource(2);
                message->setType(0);
                sendMessage(message, cmmsg->getSource());
            }


            switch(cmmsg->getType()) {
              case 0:
              {
                  EV << "ACK received";
                  break;
              }
              case 3:
              {
                  char str[50] = "Where is the book i am looking for?";
                  message = generateNewMessage(str);
                  message->setSeq(message->getSeq()+1);
                  message->setSource(2);
                  message->setType(3);
                  sendMessage(message, 0);
                  break;
              }
              case 4:{
                  EV << "To be animated.";
                  char str[20] = "Pay the book.";
                  message = generateNewMessage(str);
                  message->setSeq(message->getSeq()+1);
                  message->setSource(2);
                  message->setType(5);
                  sendMessage(message, 1);
                  break;
              }
              case 5:
              {
                  EV << "To be animated, walking out.";
              }
              default:
              {
                EV << "No coded response pepehands.";
              }
            }


        }

}
