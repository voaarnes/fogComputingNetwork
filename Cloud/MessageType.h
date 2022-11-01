/*
 * MessageType.h
 *
 *  Created on: 6. okt. 2022
 *      Author: Halvor
 */

#ifndef MESSAGETYPE_H_
#define MESSAGETYPE_H_

// TEMP
//int networktype = 0;


enum {
    MSG_ACK = 0,
    MSG_CLOUD_RDY,
    MSG_CONTENTS,
    MSG_WHEREIS,
    MSG_FOUND_LEFT,
    MSG_FOUND_RIGHT,
    MSG_BOOK_PAY,
    MSG_BOOK_PAYED,
    MSG_BOOK_SOLD // Sent to the cloud to mark sold book
};


#endif /* MESSAGETYPE_H_ */
