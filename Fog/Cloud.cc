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
    processingDelay = new ComputerMessage("processingDelay");

    side = par("leftRightSide").intValue();

    msgSentHost = 0;
    msgSentComputer = 0;
    msgReceivedHost = 0;
    msgReceivedComputer = 0;



    ackSentHost = 0;
    ackSentComputer = 0;
    ackReceivedHost = 0;
    ackReceivedComputer = 0;

    WATCH(side);
    WATCH(msgSentHost);
    WATCH(msgSentComputer);
    WATCH(msgReceivedHost);
    WATCH(msgReceivedComputer);


    WATCH(ackSentHost);
    WATCH(ackSentComputer);
    WATCH(ackReceivedHost);
    WATCH(ackReceivedComputer);
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

   ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
   if (cMsg != processingDelay){
       if(cMsg->getType() != 0){
           if(cMsg->getSource() == 1) {msgReceivedComputer++;}
           else {msgReceivedHost++;}
       }
   }


   if (cMsg != NULL){
       if (cMsg->getType() != 0){
           ackMessage(cMsg);
       }


       int src = cMsg->getSource();
      // ADD RECIVING DELAY
      if (msg == processingDelay){
          cMsg = cachedMessage;

      } else if (cMsg->getType() != MSG_ACK) {
          // Ignore this for ACKS
          cachedMessage = cMsg;
          if (src == 1){
              scheduleAt(omnetpp::simTime()+(R_DELAY_FOG_TO_CLOUD/1000.0), processingDelay);
          } else {
              scheduleAt(omnetpp::simTime()+(R_DELAY_HOST_TO_CLOUD/1000.0), processingDelay);
          }
          return;
      }



       int type = cMsg->getType();
       src = cMsg->getSource();
       int seq = cMsg->getSeq();
       delete cMsg;
       switch (type){
           case MSG_ACK: {

               if(cMsg->getSource() == 1) {ackReceivedComputer++;}
               else {ackReceivedHost++;}

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
       ackSentComputer++;
       send(ack, "fogout");
   } else {
       ackSentHost++;
       send(ack, "hostout");
   }

}

void Cloud::resendLastMessage(int dest){
    if (dest == 0){
        EV << "This should not happen";
    } else if (dest == 1){
        msgSentComputer++;
        ComputerMessage *toSend = lastFog->dup();
        send(toSend, "fogout");
        scheduleAt(omnetpp::simTime()+timeout, timeoutFog);

    } else {
        msgSentHost++;
        ComputerMessage *toSend = lastHost->dup();
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
        sendDelayed(toSend, S_DELAY_CLOUD_TO_FOG / 1000.0, "fogout");

        lastFog = msg;
        scheduleAt(omnetpp::simTime()+timeout+(S_DELAY_CLOUD_TO_FOG / 1000.0), timeoutFog);
    } else {
        msgSentHost++;
        sendDelayed(toSend, S_DELAY_CLOUD_TO_HOST / 1000.0, "hostout");


        lastHost = msg;
        scheduleAt(omnetpp::simTime()+timeout+(S_DELAY_CLOUD_TO_HOST / 1000.0), timeoutHost);
    }

}



void Cloud::refreshDisplay() const{
    char buffer[20];
    sprintf(buffer, "sent: %ld rcvd: %ld", msgSentComputer+msgSentHost+ackSentComputer+ackSentHost, msgReceivedComputer+msgReceivedHost+ackReceivedComputer+ackReceivedHost);
    getDisplayString().setTagArg("t", 0, buffer);

    char label[60];

    // Change total sent/received cloud
    sprintf(label, "Total number of messages sent/received by the cloud: %ld", msgSentComputer+msgSentHost+ackSentComputer+ackSentHost+msgReceivedComputer+msgReceivedHost);
    omnetpp::cCanvas *canvas = getParentModule()->getCanvas();
    omnetpp::cTextFigure *textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("cloudsr"));
    textFigure->setText(label);


    if (side == 0){
        sprintf(label, "Library - edge based version - LEFT");
    }else{
        sprintf(label, "Library - edge based version - RIGHT");
    }

    // Set title
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("title"));
    textFigure->setText(label);


    // Change power spent sending.
    sprintf(label, "Cloud (from cloud to smartphone): %ld", (msgSentHost+ackSentHost)*S_POWER_CLOUD_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendcls"));
    textFigure->setText(label);

    sprintf(label, "Cloud (from cloud to computer): %ld", (msgSentComputer+ackSentComputer)*S_POWER_CLOUD_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendclc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change power spent receiving.
    sprintf(label, "Cloud (cloud from smartphone): %ld", (msgReceivedHost+ackReceivedHost)*R_POWER_HOST_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceivecls"));
    textFigure->setText(label);

    sprintf(label, "Cloud (cloud from computer): %ld", (msgReceivedComputer+ackReceivedComputer)*R_POWER_FOG_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceiveclc"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change total sending delay..
    sprintf(label, "Cloud (from cloud to smartphone): %ld", (msgSentHost+ackSentHost)*P_DELAY_HOST_CLOUD+msgSentHost*S_DELAY_CLOUD_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendcls"));
    textFigure->setText(label);


    sprintf(label, "Cloud (from cloud to computer): %ld", (msgSentComputer+ackSentComputer)*P_DELAY_FOG_CLOUD+msgSentComputer*S_DELAY_CLOUD_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendclc"));
    textFigure->setText(label);

   /////////////////////////////////////

    // Change total receiving delay.
    sprintf(label, "Cloud (cloud from smartphone): %ld", (msgReceivedHost+msgReceivedHost)*P_DELAY_HOST_CLOUD+msgReceivedHost*R_DELAY_HOST_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceivecls"));
    textFigure->setText(label);

    sprintf(label, "Cloud (cloud from computer): %ld", (msgReceivedComputer+msgReceivedComputer)*P_DELAY_FOG_CLOUD+msgReceivedComputer*R_DELAY_FOG_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceiveclc"));
    textFigure->setText(label);
}


Cloud::~Cloud() {
    cancelEvent(timeoutHost);
    cancelEvent(timeoutFog);
    delete timeoutFog;
    delete timeoutHost;
}

