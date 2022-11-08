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





simple Cloud
{
    parameters:
        @display("i=misc/cloud");
        int leftRightSide = default(1);
    gates:
        input fogin;
        input hostin;
        output fogout;
        output hostout;
}


simple Computer
{
    parameters:
        @display("i=device/pc3");
        int leftRightSide = default(1);
    gates:
        input cloudin;
        input hostin;
        output cloudout;
        output hostout;
}

simple Host 
{
    parameters:
        @display("i=device/pocketpc");
    gates:
        input fogin;
        input cloudin;
        output fogout;
        output cloudout;
}

module HostShell extends StandardHost
{
	gates:
        input fogin;
        input cloudin;
        output fogout;
        output cloudout;
	submodules:
		host: Host;
	connections allowunconnected:
		fogin --> host.fogin;
		cloudin --> host.cloudin; 
		fogout <-- host.fogout;
		cloudout <-- host.cloudout;
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
        @display("bgb=1353,770");
        
        
        @figure[cloudsr](type=text; pos=5,50; font=,,bold; text="Total number of messages sent/received by the cloud: N/A");
        @figure[computersr](type=text; pos=5,70; font=,,bold; text="Total number of messages sent/received by the computer: N/A");
        @figure[smartphonesr](type=text; pos=5,90; font=,,bold; text="Total number of messages sent/received by the smartphone: N/A");
        
        
        @figure[titlepowerspentsend](type=text; pos=5,130; font=,,bold; text="Total power spent when sending messages by:");
        @figure[powerspentsendcls](type=text; pos=5,150; font=,,bold; text="Cloud (from cloud to smartphone): N/A");
        @figure[powerspentsendclc](type=text; pos=5,170; font=,,bold; text="Cloud (from cloud to computer): N/A");
        @figure[powerspentsendccl](type=text; pos=5,190; font=,,bold; text="Computer (from computer to cloud): N/A");
        @figure[powerspentsendcs](type=text; pos=5,210; font=,,bold; text="Computer (from computer to smartphone): N/A");
        @figure[powerspentsendscl](type=text; pos=5,230; font=,,bold; text="Smartphone (from smartphone to cloud): N/A");
        @figure[powerspentsendsc](type=text; pos=5,250; font=,,bold; text="Smartphone (from smartphone to computer): N/A");
        
        @figure[titlepowerspentreceive](type=text; pos=5,290; font=,,bold; text="Total power spent when receiving messages by:");
        @figure[powerspentreceivecls](type=text; pos=5,310; font=,,bold; text="Cloud (cloud from smartphone): N/A");
        @figure[powerspentreceiveclc](type=text; pos=5,330; font=,,bold; text="Cloud (cloud from computer): N/A");
        @figure[powerspentreceiveccl](type=text; pos=5,350; font=,,bold; text="Computer (computer from cloud): N/A");
        @figure[powerspentreceivecs](type=text; pos=5,370; font=,,bold; text="Computer (computer from smartphone): N/A");
        @figure[powerspentreceivescl](type=text; pos=5,390; font=,,bold; text="Smartphone (smartphone from cloud): N/A");
        @figure[powerspentreceivesc](type=text; pos=5,410; font=,,bold; text="Smartphone (smartphone from computer): N/A"); 
        
        @figure[titledelaysend](type=text; pos=5,450; font=,,bold; text="Total delay when sending messages by:");
        @figure[delaysendcls](type=text; pos=5,470; font=,,bold; text="Cloud (cloud to smartphone): N/A");
        @figure[delaysendclc](type=text; pos=5,490; font=,,bold; text="Cloud (cloud to computer): N/A");
        @figure[delaysendccl](type=text; pos=5,510; font=,,bold; text="Computer (computer to cloud): N/A");
        @figure[delaysendcs](type=text; pos=5,530; font=,,bold; text="Computer (computer to smartphone): N/A");
        @figure[delaysendscl](type=text; pos=5,550; font=,,bold; text="Smartphone (smartphone to cloud): N/A");
        @figure[delaysendsc](type=text; pos=5,570; font=,,bold; text="Smartphone (smartphone to computer): N/A"); 
        
        @figure[titledelayreceive](type=text; pos=5,610; font=,,bold; text="Total delay when received messages by:");
        @figure[delayreceivecls](type=text; pos=5,630; font=,,bold; text="Cloud (cloud from smartphone): N/A");
        @figure[delayreceiveclc](type=text; pos=5,650; font=,,bold; text="Cloud (cloud from computer): N/A");
        @figure[delayreceiveccl](type=text; pos=5,670; font=,,bold; text="Computer (computer from cloud): N/A");
        @figure[delayreceivecs](type=text; pos=5,690; font=,,bold; text="Computer (computer from smartphone): N/A");
        @figure[delayreceivescl](type=text; pos=5,710; font=,,bold; text="Smartphone (smartphone from cloud): N/A");
        @figure[delayreceivesc](type=text; pos=5,730; font=,,bold; text="Smartphone (smartphone from computer): N/A"); 

    types:
        
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
        cloud.fogout --> {delay = 300ms;} --> computer.cloudin;
        cloud.hostout --> {delay = 400ms;} --> host.cloudin;
        host.fogout --> {delay = 50ms;} --> computer.hostin;
        host.cloudout --> {delay = 400ms;} --> cloud.hostin;
        computer.cloudout --> {delay = 300ms;} --> cloud.fogin;
        computer.hostout --> {delay = 50ms;} --> host.fogin;
}
