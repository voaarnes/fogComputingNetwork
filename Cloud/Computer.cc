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
    char msgname[40];
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
    processingDelay = new ComputerMessage("processingDelay");

    msgSentHost = 0;
    msgSentCloud = 0;
    msgReceivedHost = 0;
    msgReceivedCloud = 0;

    ackSentHost = 0;
    ackSentCloud= 0;
    ackReceivedHost = 0;
    ackReceivedCloud = 0;


    WATCH(msgSentHost);
    WATCH(msgSentCloud);
    WATCH(msgReceivedHost);
    WATCH(msgReceivedCloud);


    WATCH(ackSentHost);
    WATCH(ackSentCloud);
    WATCH(ackReceivedHost);
    WATCH(ackReceivedCloud);

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

       ComputerMessage *cMsg = dynamic_cast<ComputerMessage *>(msg);
       if (cMsg != NULL){
           if(cMsg->getSource() == 0  && cMsg->getType() != 0) {msgReceivedCloud++;}
           else {msgReceivedHost++;}

           if (cMsg->getType() != 0){
               EV << "ACKING";
               ackMessage(cMsg);
           }

           int src = cMsg->getSource();
           // ADD RECIVING DELAY
           if (msg == processingDelay){
               cMsg = cachedMessage;

           } else if (cMsg->getType() != MSG_ACK) {
               // Ignore this for ACKS
               cachedMessage = cMsg;
               if (src == 0){
                   scheduleAt(omnetpp::simTime()+(R_DELAY_CLOUD_TO_FOG/1000.0), processingDelay);
               } else {
                   scheduleAt(omnetpp::simTime()+(R_DELAY_HOST_TO_FOG/1000.0), processingDelay);
               }
               return;
           }



           int type = cMsg->getType();
           src = cMsg->getSource();
           delete cMsg;
           switch (type){
               case MSG_ACK: {
                   if(src == 0) {ackReceivedCloud++;}
                   else {ackReceivedHost++;}

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
                  // Account for ack that comes in return
                  lastSeq++;

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
    ComputerMessage *toSend = msg->dup();

    if (dest == 1){
        EV << "This should not happen";
    } else if (dest == 0){
        msgSentCloud++;
        sendDelayed(toSend, (S_DELAY_FOG_TO_CLOUD / 1000.0), "cloudout");

        lastCloud = msg;
        scheduleAt(omnetpp::simTime()+timeout+(S_DELAY_FOG_TO_CLOUD / 1000.0), timeoutCloud);
    } else {
        msgSentHost++;
        sendDelayed(toSend, (S_DELAY_FOG_TO_HOST / 1000.0), "hostout");


        lastHost = msg;
        scheduleAt(omnetpp::simTime()+timeout+(S_DELAY_FOG_TO_HOST / 1000.0), timeoutHost);
    }

}

void Computer::ackMessage(ComputerMessage* msg){
    lastSeq = msg->getSeq();
    ComputerMessage *ack;
    int source = msg->getSource();
    char msgText[30];
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
       ackSentHost++;
       send(ack, "hostout");
   } else {
       ackSentCloud++;
       send(ack, "cloudout");
   }
}


void Computer::resendLastMessage(int dest){
    if (dest == 1){
        EV << "This should not happen";
    } else if (dest == 2){
        msgSentHost++;
        ComputerMessage *toSend = lastHost->dup();
        send(toSend, "hostout");
        scheduleAt(omnetpp::simTime()+timeout, timeoutHost);

    } else {
        msgSentCloud++;
        ComputerMessage *toSend = lastCloud->dup();
        send(toSend, "cloudout");
        scheduleAt(omnetpp::simTime()+timeout, timeoutCloud);
    }

}

void Computer::refreshDisplay() const{
    char buffer[20];
    sprintf(buffer, "sent: %ld rcvd: %ld", msgSentCloud+msgSentHost+ackSentCloud+ackSentHost, msgReceivedCloud+msgReceivedHost+ackReceivedCloud+ackReceivedHost);
    getDisplayString().setTagArg("t", 0, buffer);

    char label[70];
    // Change total sent/received cloud
    sprintf(label, "Total number of messages sent/received by the computer: %ld", msgSentCloud+msgSentHost+ackSentCloud+ackSentHost+msgReceivedCloud+msgReceivedHost+ackReceivedCloud+ackReceivedHost);
    omnetpp::cCanvas *canvas = getParentModule()->getCanvas();
    omnetpp::cTextFigure *textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("computersr"));
    textFigure->setText(label);


    // Change power spent sending.
    sprintf(label, "Computer (from computer to smartphone): %ld", (msgSentHost+ackSentHost)*S_POWER_FOG_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendcs"));
    textFigure->setText(label);

    sprintf(label, "Computer (from computer to cloud): %ld", (msgSentCloud+ackSentCloud)*S_POWER_FOG_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentsendccl"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change power spent receiving.
    sprintf(label, "Computer (computer from smartphone): %ld", (msgReceivedHost+ackReceivedHost)*R_POWER_HOST_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceivecs"));
    textFigure->setText(label);

    sprintf(label, "Computer (computer from cloud): %ld", (msgReceivedCloud+ackReceivedCloud)*R_POWER_CLOUD_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("powerspentreceiveccl"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change total sending delay..
    sprintf(label, "Computer (from computer to smartphone): %ld", (msgSentHost+ackSentHost)*P_DELAY_HOST_FOG+msgSentHost*S_DELAY_FOG_TO_HOST);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendcs"));
    textFigure->setText(label);


    sprintf(label, "Computer (from computer to cloud): %ld", (msgSentCloud+ackSentCloud)*P_DELAY_FOG_CLOUD+msgSentCloud*S_DELAY_FOG_TO_CLOUD);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delaysendccl"));
    textFigure->setText(label);

    /////////////////////////////////////

    // Change total receiving delay.
    sprintf(label, "Computer (computer from smartphone): %ld", (msgReceivedHost+ackReceivedHost)*P_DELAY_HOST_FOG+msgReceivedHost*R_DELAY_HOST_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceivecs"));
    textFigure->setText(label);


    sprintf(label, "Computer (computer from cloud): %ld", (msgReceivedCloud+ackReceivedCloud)*P_DELAY_FOG_CLOUD+msgReceivedCloud*R_DELAY_CLOUD_TO_FOG);
    textFigure = omnetpp::check_and_cast<omnetpp::cTextFigure*>(canvas->getFigure("delayreceiveccl"));
    textFigure->setText(label);
}
