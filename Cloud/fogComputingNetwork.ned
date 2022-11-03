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


import inet.node.inet.StandardHost;
import inet.visualizer.integrated.IntegratedVisualizer;




simple Visualizer
{
    parameters:
        @display("i=block/app");
    
}

simple Cloud
{
    parameters:
        @display("i=misc/cloud");
        int leftRightSide = default(1);
    gates:
        inout foggate;
        inout hostgate;
}


simple Computer
{
    parameters:
        @display("i=device/pc3");
        int leftRightSide = default(1);
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

module HostShell extends StandardHost
{
	gates:
		inout foggate;
        inout cloudgate;
	submodules:
		host: Host;
	connections allowunconnected:
		foggate <--> host.foggate;
		cloudgate <--> host.cloudgate;
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
        channel Channel2 extends ned.DelayChannel
        {
            delay = 200ms;
        }
        channel Channel3 extends ned.DelayChannel
        {
            delay = 250ms;
        }
    submodules:
        cloud: Cloud {
        	
            @display("p=1281,64;r=700");
        }
        computer: Computer {
            @display("p=725,350;r=250");
        }
        host: HostShell {
            @display("p=725,610;r=200");
        }
        
        visualizer: IntegratedVisualizer {
            @display("p=1281,166");
        }
        
    connections allowunconnected:
        cloud.foggate <--> Channel2 <--> computer.cloudgate;
        cloud.hostgate <--> Channel3 <--> host.cloudgate;
		computer.hostgate <--> Channel <--> host.foggate;
}
