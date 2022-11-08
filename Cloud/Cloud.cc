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

    msgSentHost = 0;
    msgSentComputer = 0;
    msgReceivedHost = 0;
    msgReceivedComputer = 0;

    WATCH(msgSentHost);
    WATCH(msgSentComputer);
    WATCH(msgReceivedHost);
    WATCH(msgReceivedComputer);
}


ComputerMessage *Cloud::generateNewMessage(char* str){
    char msgname[60];
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
   ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);

   if (cMsg != NULL){

       if(cMsg->getSource() == 1) {msgReceivedComputer++;}
       else {msgReceivedHost++;}

       if (cMsg->getType() != 0){
           ackMessage(cMsg);
       }

       int type = cMsg->getType();
       int src = cMsg->getSource();

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
               break;
           }
           case MSG_CONTENTS: {
               EV << "Received contents\n";
               if (!isStarted) {
                  char str[40] = "Cloud ready to start";
                  ComputerMessage* newMsg = generateNewMessage(str);
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
       msgSentComputer++;
       //send(ack, "foggate$o");
       send(ack, "fogout");
   } else {
       msgSentHost++;
       //send(ack, "hostgate$o");
       send(ack, "hostout");
   }

}

void Cloud::resendLastMessage(int dest){
    if (dest == 0){
        EV << "This should not happen";
    } else if (dest == 1){
        ComputerMessage *toSend = lastFog->dup();
        msgSentComputer++;
        //send(toSend, "foggate$o");
        send(toSend, "fogout");
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);

    } else {
        ComputerMessage *toSend = lastHost->dup();
        msgSentHost++;
        //send(toSend, "hostgate$o");
        send(toSend, "hostout");
        scheduleAt(omnetpp::simTime()+timeout, timeoutHost);
    }

}


void Cloud::sendMessage(ComputerMessage* msg, int dest){
    ComputerMessage *toSend = msg->dup();

    if (dest == 0){
        EV << "This should not happen";
    } else if (dest == 1){
        msgSentComputer++;
        send(toSend, "fogout");

        lastFog = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);
    } else {
        msgSentHost++;
        send(toSend, "hostout");


        lastHost = msg;
        scheduleAt(omnetpp::simTime()+timeout, timeoutHost);
    }

}



Cloud::~Cloud() {
    cancelEvent(timeoutHost);
    cancelEvent(timeoutFog);
    delete timeoutFog;
    delete timeoutHost;
}

void Cloud::refreshDisplay() const{
    char buffer[20];
    sprintf(buffer, "sent: %ld rcvd: %ld", msgSentComputer+msgSentHost, msgReceivedComputer+msgReceivedHost);
    getDisplayString().setTagArg("t", 0, buffer);

    char label[60];

    // Change total sent/received cloud
    sprintf(label, "Total number of messages sent/received by the cloud: %ld", msgSentComputer+msgSentHost+msgReceivedComputer+msgReceivedHost);
    omnetpp::cCanvas *canvas = getParentModule()->getCanvas();
    omnetpp::cTextFigure *textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("cloudsr"));
    textFigure->setText(label);


    // Change power spent sending.
    sprintf(label, "Cloud (from cloud to smartphone): %ld", msgSentHost*S_POWER_CLOUD_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendcls"));
    textFigure->setText(label);

    sprintf(label, "Cloud (from cloud to computer): %ld", msgSentComputer*S_POWER_CLOUD_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendclc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change power spent receiving.
    sprintf(label, "Cloud (cloud from smartphone): %ld", msgReceivedHost*R_POWER_HOST_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceivecls"));
    textFigure->setText(label);

    sprintf(label, "Cloud (cloud from computer): %ld", msgReceivedComputer*R_POWER_FOG_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceiveclc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change total sending delay..
    sprintf(label, "Cloud (from cloud to smartphone): %ld", msgSentHost*S_DELAY_CLOUD_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendcls"));
    textFigure->setText(label);


    sprintf(label, "Cloud (from cloud to computer): %ld", msgSentComputer*S_DELAY_CLOUD_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendclc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change total receiving delay.
    sprintf(label, "Cloud (cloud from smartphone): %ld", msgReceivedHost*R_DELAY_HOST_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceivecls"));
    textFigure->setText(label);

    sprintf(label, "Cloud (cloud from computer): %ld", msgReceivedComputer*R_DELAY_FOG_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceiveclc"));
    textFigure->setText(label);

}
