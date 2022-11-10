/*
 * Constants.h
 *
 *  Created on: 10. nov. 2022
 *      Author: Halvor
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

    const int S_POWER_FOG_TO_CLOUD = 300;
    const int S_POWER_FOG_TO_HOST = 200;
    const int R_POWER_CLOUD_TO_FOG = 300;
    const int R_POWER_HOST_TO_FOG = 300;

    const int S_POWER_HOST_TO_CLOUD = 300;
    const int S_POWER_HOST_TO_FOG = 300;
    const int R_POWER_CLOUD_TO_HOST = 250;
    const int R_POWER_FOG_TO_HOST = 250;

    const int S_POWER_CLOUD_TO_HOST = 200;        // In figure 1: total=1600, cloud->host msg=8
    const int S_POWER_CLOUD_TO_FOG = 100;         // total=200, msg=2
    const int R_POWER_HOST_TO_CLOUD = 300;        // total=900, msg=3
    const int R_POWER_FOG_TO_CLOUD = 300;         // total=600, msg=2


    // Fog uses same time to ready a packet to each node
    const int S_DELAY_FOG_TO_CLOUD = 20;
    const int S_DELAY_FOG_TO_HOST = 20;

    // This implies the Cloud did the processing
    const int R_DELAY_CLOUD_TO_FOG = 10;

    // This implies the Fog nodes needs to do processing
    const int R_DELAY_HOST_TO_FOG = 25;

    // Same time to ready packet, big fast cloud
    const int S_DELAY_CLOUD_TO_HOST = 5;
    const int S_DELAY_CLOUD_TO_FOG = 5;

    //  Low latency with processing a packet, big fast cloud
    const int R_DELAY_HOST_TO_CLOUD = 5;
    const int R_DELAY_FOG_TO_CLOUD = 5;

    // Same delay to ready and process a packet (same message)
    const int S_DELAY_HOST_TO_CLOUD = 25;
    const int S_DELAY_HOST_TO_FOG = 25;

    // Same time to process message
    const int R_DELAY_CLOUD_TO_HOST = 10;
    const int R_DELAY_FOG_TO_HOST = 10;


    const int P_DELAY_HOST_CLOUD = 400;
    const int P_DELAY_HOST_FOG = 50;
    const int P_DELAY_FOG_CLOUD = 300;


#endif /* CONSTANTS_H_ */
