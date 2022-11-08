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

    msgSentComputer = 0;
    msgSentCloud = 0;
    msgReceivedComputer = 0;
    msgReceivedCloud = 0;

    WATCH(msgSentComputer);
    WATCH(msgSentCloud);
    WATCH(msgReceivedComputer);
    WATCH(msgReceivedCloud);
}


ComputerMessage *Host::generateNewMessage(char* str){
    char msgname[60];
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
        msgSentCloud++;
        send(toSend, "cloudout");
        lastCloud = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutCloud);
    } else {
        msgSentComputer++;
        send(toSend, "fogout");
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
            sendMessage(message, 1);
        }
        else {


            if (msgLost < 3){
                msgLost++;
                getParentModule()->bubble("Message lost");
                delete msg;
                return;
            }

            ComputerMessage *cmmsg = omnetpp::check_and_cast<ComputerMessage *>(msg);
            if(cmmsg->getSource() == 0) {msgReceivedCloud++;}
            else {msgReceivedComputer++;}

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

                  getParentModule()->bubble("Ready to start");
                  char str[50] = "Where is the book i am looking for?";
                  message = generateNewMessage(str);
                  message->setType(MSG_WHEREIS);
                  sendMessage(message, 1);
                  return;
                  break;
              }
              case MSG_FOUND_LEFT:{
                  getParentModule()->bubble("Book is left");
                  scheduleAt(33.0, payBookDelay);
                  break;
              }
              case MSG_FOUND_RIGHT:{
                  getParentModule()->bubble("Book is right");
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
       msgSentComputer++;
       send(ack, "fogout");
   } else {
       msgSentCloud++;
       send(ack, "cloudout");
   }

}


void Host::resendLastMessage(int dest){
    if (dest == 2){
        EV << "This should not happen";
    } else if (dest == 1){
        msgSentComputer++;
        ComputerMessage *toSend = lastFog->dup();
        send(toSend, "fogout");
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);

    } else {
        msgSentCloud++;
        ComputerMessage *toSend = lastCloud->dup();
        send(toSend, "cloudout");
        scheduleAt(omnetpp::simTime()+timeout, timeoutCloud);
    }

}

void Host::refreshDisplay() const{
    char buffer[40];
    sprintf(buffer, "sent: %ld rcvd: %ld lost: %i", msgSentComputer+msgSentCloud, msgReceivedComputer+msgReceivedCloud, msgLost);
    getParentModule()->getDisplayString().setTagArg("t", 0, buffer);

    char label[70];

    // Change total sent/received cloud
    sprintf(label, "Total number of messages sent/received by the smartphone: %ld", msgSentCloud+msgSentCloud+msgReceivedComputer+msgReceivedComputer);
    omnetpp::cCanvas *canvas = getParentModule()->getParentModule()->getCanvas();
    omnetpp::cTextFigure *textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("smartphonesr"));
    textFigure->setText(label);


    // Change power spent sending.
    sprintf(label, "Smartphone (from smartphone to cloud): %ld", msgReceivedCloud*S_POWER_HOST_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendscl"));
    textFigure->setText(label);

    sprintf(label, "Smartphone (from smartphone to computer): %ld", msgSentComputer*S_POWER_HOST_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendsc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change power spent receiving.
    sprintf(label, "Smartphone (smartphone from cloud): %ld", msgReceivedCloud*R_POWER_CLOUD_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceivescl"));
    textFigure->setText(label);

    sprintf(label, "Smartphone (smartphone from computer): %ld", msgReceivedComputer*R_POWER_FOG_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceivesc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change total sending delay..
    sprintf(label, "Smartphone (from smartphone to cloud): %ld", msgReceivedCloud*S_DELAY_HOST_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendscl"));
    textFigure->setText(label);


    sprintf(label, "Smartphone (from smartphone to computer): %ld", msgSentComputer*S_DELAY_HOST_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendsc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change total receiving delay.
    sprintf(label, "Smartphone (smartphone from cloud): %ld", msgReceivedCloud*R_DELAY_CLOUD_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceivescl"));
    textFigure->setText(label);

    sprintf(label, "Smartphone (smartphone from computer): %ld", msgReceivedComputer*R_DELAY_FOG_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceivesc"));
    textFigure->setText(label);
}
