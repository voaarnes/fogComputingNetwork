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




simple Visualizer
{
    parameters:
        @display("i=block/app");
    
}

simple Cloud
{
    parameters:
        @display("i=misc/cloud");
    gates:
        inout foggate;
        inout hostgate;
}


simple FogNode
{
    parameters:
        @display("i=device/pc3");
    gates:
        inout hostgate;
        inout cloudgate;
}

simple Host
{
    parameters:
        @display("i=device/pocketpc");
    gates:
        inout foggate;
        inout cloudgate;
}



network fogComputingNetwork
{
    parameters:
        @figure[library](type=rectangle; pos=375,145; size=700,505; fillColor=#d9dacc; lineWidth=3; zIndex=-1; );
        @figure[shelf1](type=rectangle; pos=500,272; size=100,375; lineColor=#f0eef3; fillColor=#f0eef3; lineWidth=1; zIndex=-1);
        @figure[shelf2](type=rectangle; pos=850,272; size=100,375; lineColor=#f0eef3; fillColor=#f0eef3; lineWidth=1; zIndex=-1);
        @figure[pillar1](type=rectangle; pos=600,272; size=25,375; fillColor=#000000; lineWidth=1; zIndex=-1);
        @figure[pillar2](type=rectangle; pos=825,272; size=25,375; fillColor=#000000; lineWidth=1; zIndex=-1);
        @figure[libraryLabel](type=text; pos=380,150; text=library);
        @display("bgb=1353,724");
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100ms;
        }
    submodules:
        cloud: Cloud {
            @display("p=1281,64");
        }
        computer: FogNode {
            @display("p=725,350");
        }
        host: Host {
            @display("p=725,610");
        }
        visualizer: Visualizer {
            @display("p=1281,166");
        }
        
    connections:
        cloud.foggate <--> Channel <--> computer.cloudgate;
        cloud.hostgate <--> Channel <--> host.cloudgate;
		computer.hostgate <--> Channel <--> host.foggate;
}
