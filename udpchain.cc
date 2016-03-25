/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
    Brian Parks, Roman Claprood
    Lab4 CSE5462 Prof. Sinha
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  //LogComponentEnableAll (LOG_LEVEL_INFO);
  // Network Topology
  //
  //     10.1.1.0         10.1.2.0          10.1.3.0
  // A -------------- B -------------- C -------------- D
  //                    point-to-point
  //
  //


  NodeContainer nodesAB;
  nodesAB.Create (2);

  NodeContainer nodesBC;
  nodesBC.Add (nodesAB.Get (1)); //add node B (created above)
  nodesBC.Create (1);

  NodeContainer nodesCD;
  nodesCD.Add (nodesBC.Get (1));  //add node C (created above)
  nodesCD.Create(1);



  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ms"));

  NetDeviceContainer devicesAB;
  devicesAB = pointToPoint.Install (nodesAB);
  NetDeviceContainer devicesBC;
  devicesBC = pointToPoint.Install (nodesBC);
  NetDeviceContainer devicesCD;
  devicesCD = pointToPoint.Install (nodesCD);

  pointToPoint.EnablePcapAll ("udp");
  InternetStackHelper stack;
  stack.Install (nodesAB);
  stack.Install (nodesBC.Get (1));
  stack.Install (nodesCD.Get (1));

  Ipv4AddressHelper addressAB;
  addressAB.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4AddressHelper addressBC;
  addressBC.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4AddressHelper addressCD;
  addressCD.SetBase ("10.1.3.0", "255.255.255.0");

  Ipv4InterfaceContainer interfacesAB = addressAB.Assign (devicesAB);
  Ipv4InterfaceContainer interfacesBC = addressBC.Assign (devicesBC);
  Ipv4InterfaceContainer interfacesCD = addressCD.Assign (devicesCD);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodesCD.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfacesCD.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodesAB.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
