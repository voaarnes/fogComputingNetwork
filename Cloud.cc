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

}

void Cloud::initialize() {
    char msgname[20];
    sprintf(msgname, "poggy");
    omnetpp::cMessage *msg = new omnetpp::cMessage(msgname);
    omnetpp::cMessage *msg2 = new omnetpp::cMessage(msgname);
    send(msg, "inCloud");
    send(msg2, "outFog");
}


Cloud::~Cloud() {
    // TODO Auto-generated destructor stub
}

