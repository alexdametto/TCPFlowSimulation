/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SimTesi");

class TCPFlow {
  private:
  Ptr<Socket> socket;
  Ipv4Address servAddress;
  uint16_t servPort;
  uint32_t totalTxBytes;
  uint32_t currentTxBytes;
  uint32_t writeSize;
  double startingTime;
  double endingTime;

  private:
  void WriteUntilBufferFull(Ptr<Socket> socket, uint32_t txSpace) {
    //std::cout << currentTxBytes << "\t" << totalTxBytes << "\n";
    while (currentTxBytes < totalTxBytes && socket->GetTxAvailable () > 0) 
    {
      /*uint32_t toWrite = totalTxBytes - currentTxBytes;
      if(toWrite >= writeSize)
        toWrite = writeSize;*/

      uint32_t left = totalTxBytes - currentTxBytes;
      uint32_t dataOffset = currentTxBytes % writeSize;
      uint32_t toWrite = writeSize - dataOffset;
      toWrite = std::min (toWrite, left);
      toWrite = std::min (toWrite, socket->GetTxAvailable ());
      Ptr<Packet> packet = Create<Packet>(toWrite);
      int amountSent = socket->Send (packet);
      if(amountSent < 0)
      {
        // we will be called again when new tx space becomes available.
        return;
      }
      else currentTxBytes += amountSent;
    }
    if(currentTxBytes == totalTxBytes) // il socket va chiuso SOLO se ho finito, altrimenti manderà meno pacchetti!!!! 
      socket->Close ();
  }

  public:
  TCPFlow(Ptr<Socket> socket, Ipv4Address servAddress, uint16_t servPort, uint32_t numPackets, uint32_t bytesForPacket) {
    this->socket = socket;
    this->servAddress = servAddress;
    this->servPort = servPort;
    this->totalTxBytes = numPackets * bytesForPacket;
    this->writeSize = bytesForPacket;
    this->currentTxBytes = 0;

    this->startingTime = 0;
    this->endingTime = 0;
  }

  void StartFlow () {
    NS_LOG_LOGIC ("Starting flow at time " <<  Simulator::Now ().GetSeconds ());
    //socket->Bind();
    
    this->startingTime = this->endingTime = Simulator::Now().GetSeconds();
    
    
    socket->Connect (InetSocketAddress (servAddress, servPort)); //connect

    // tell the tcp implementation to call WriteUntilBufferFull again
    // if we blocked and new tx buffer space becomes available
    socket->SetSendCallback (MakeCallback (&TCPFlow::WriteUntilBufferFull, this));
    WriteUntilBufferFull (socket, socket->GetTxAvailable());
  }

  void setTime() {
    this->endingTime = Simulator::Now().GetSeconds();
  }

  void getTimes(double ris[2]){
    ris[0] = this->startingTime;
    ris[1] = this->endingTime;
  }

};

TCPFlow** FlowArr;

// Metodo di callback quando arriva un pacchetto
void PacketSink::HandleRead(Ptr<Socket> socket){
  Ptr<Packet> packet;
	Address from;
	while (packet = socket->RecvFrom (from))
	{
		//uint8_t *msg;
		//msg = new uint8_t[packet->GetSize()];
		//byteRicevuti += packet->GetSize() - packet->RemoveHeader();
    //packet->CopyData(msg, packet->GetSize());


    //uint8_t buffer[20];

    //Ipv4Address address = InetSocketAddress::ConvertFrom (from).GetIpv4 ();

    //from.CopyTo(buffer);

    //std::cout << packet->GetUid() << "\t" << InetSocketAddress::ConvertFrom (from).GetIpv4 () << "\n";

    std::stringstream buffer;
    InetSocketAddress::ConvertFrom(from).GetIpv4().Print(buffer);

    const char* ip = buffer.str().c_str();

    char num[4];

    uint punti = 0;
    uint j = 0;
    for(uint i = 0; i < sizeof(ip)/sizeof(char); i++){
      char c = ip[i];

      if(c == '.')
        punti++;
      else if(punti == 2){
        num[j] = c;
        num[j+1] = '\n';
        j++;
      }
    }

    uint nFlow = atoi(num);

    // -2 perchè inizio da due
    FlowArr[nFlow-2]->setTime();

    //std::cout << ip << "\n";

    /*char* token = new char[1];
    token[0] = '.';*/ 

    //std::ostringstream convert;
    for (int a = 0; a < 20; a++) {
      //std::cout << buffer[a];
    }

    //std::cout << "\n";

    //std::string ip = convert.str();

    //std::cout << ip << "\n";

    //std::string ip = InetSocketAddress::ConvertFrom (from).GetIpv4 () << "";

    //uint32_t n = ip.Get();

    //std::cout << Simulator::Now().GetSeconds() << "\n";

    //std::cout << packet->GetSize() << "\n";

    //byteRicevuti += packet->GetSize();

    //std::cout << InetSocketAddress::ConvertFrom (from).GetIpv4 () << "\n";

    //std::cout << packet->GetSize() << "\n";

    //byteRicevuti += packet->GetSize();

		/*NS_LOG_INFO (m_local << "Received " << packet->GetSize () << " bytes from " <<
				InetSocketAddress::ConvertFrom (from).GetIpv4 ());
		NS_LOG_INFO ("PACKET ID: " << packet->GetUid() << "====> CONTENT:" << msg << " SIZE: " << packet->GetSize());
    */
  }
  //std::cout << byteRicevuti << "\n";
} 

/*static void CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}*/

/*static void closeSocket(){
    std::cout << Simulator::Now().GetSeconds() << "\n";
}*/

void printRoutingTable (Ptr<Node> node) {
        Ipv4StaticRoutingHelper helper;
        Ptr<Ipv4> stack = node -> GetObject<Ipv4>();
        Ptr<Ipv4StaticRouting> staticrouting = helper.GetStaticRouting
(stack);
        uint32_t numroutes=staticrouting->GetNRoutes();
        Ipv4RoutingTableEntry entry;
        std::cout << "Routing table for device: " <<
Names::FindName(node) <<
"\n";
        std::cout << "Destination\tMask\t\tGateway\t\tIface\n";

        for (uint32_t i =0 ; i<numroutes;i++) {
                entry =staticrouting->GetRoute(i);
                std::cout << entry.GetDestNetwork()  << "\t"
                                << entry.GetDestNetworkMask() << "\t"
                                << entry.GetGateway() << "\t\t"
                                << entry.GetInterface() << "\n";
        }
        return;

}

int main (int argc, char *argv[])
{
    LogComponentEnable("SimTesi", LOG_LEVEL_INFO);

    int number = 5;

    int seed = 10;

    FlowArr = (TCPFlow**)malloc(number * sizeof(TCPFlow*));

    std::string lat = "2ms"; // 2 ms
    std::string datarate = "500000"; // 500 kb/s

    SeedManager::SetSeed (seed);

    CommandLine cmd;
    /*cmd.AddValue("Delay", "Latenza collegamento P2P in millisecondi", lat);
    cmd.AddValue("DataRate", "P2P data rate in bps", datarate);
    cmd.AddValue("n_tcp", "Number of TCP Flow", number);
    cmd.AddValue("seed", "Number of seed", seed);*/
    cmd.Parse (argc, argv);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue (datarate)); // B [bps] è la banda del canale
    p2p.SetChannelAttribute ("Delay", StringValue (lat)); // lambda^(-1)

    Time::SetResolution (Time::NS);

    NodeContainer routers;
    routers.Create(1);

    NodeContainer hosts;
    hosts.Create(number);

    NodeContainer endHosts;
    endHosts.Create(1);

    Ipv4AddressHelper ipv4;
    Ipv4Mask mask = Ipv4Mask("255.255.255.0");

    InternetStackHelper internet;
    internet.Install(hosts);
    internet.Install(endHosts);
    internet.Install(routers);

    for(int i = 0; i < number; ++i){
        NodeContainer nr = NodeContainer(routers.Get(0), hosts.Get(i));
        NetDeviceContainer n0r_connection = p2p.Install (nr);

        std::string bas = "10.1." + std::to_string(i+2) + ".0";
        const char* c = bas.c_str();

        Ipv4Address base = Ipv4Address(c);
        ipv4.SetBase(base, mask);

        NodeContainer connection = NodeContainer(routers.Get(0), hosts.Get(i));
        NetDeviceContainer ndc = p2p.Install(connection);

        ipv4.Assign(ndc);
    }

    NodeContainer e1r = NodeContainer(routers.Get(0), endHosts.Get(0));
    NetDeviceContainer e1r_connetction;
    e1r_connetction = p2p.Install(e1r);

    Ipv4Address base = Ipv4Address("10.1.1.0");
    ipv4.SetBase(base, mask);

    NodeContainer connection = NodeContainer(routers.Get(0), endHosts.Get(0));
    NetDeviceContainer ndc = p2p.Install(connection);

    Ipv4InterfaceContainer ipInterfs = ipv4.Assign(ndc);

    Ipv4GlobalRoutingHelper::RecomputeRoutingTables();

    uint16_t servPort = 50000;

    // Creato un packet sink per ricevere i pacchetti sull'endHost
    /*PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), servPort)); 
    ApplicationContainer apps = sink.Install (endHosts.Get(0));
    apps.Start (Seconds (0.0));

    PacketSink app = apps.Get(0);*/

    // ....HandleRead()

    /*Ptr<MyrcevApp> app1 = CreateObject<MyrcevApp> ();
    routers.Get(0)->AddApplication (app1);*/
    //app1->HandleRead(recvsocket);

    PacketSinkHelper sink ("ns3::TcpSocketFactory",
                     InetSocketAddress (Ipv4Address::GetAny (), servPort));
    ApplicationContainer sinkApps = sink.Install (endHosts.Get (0));
    sinkApps.Start (Seconds (0.0));

    double mean = 1;
    double bound = 2;
    //genera una distribuzione con valori da 0 a bound con media mean
   
    Ptr<ExponentialRandomVariable> exp = CreateObject<ExponentialRandomVariable> ();
    exp->SetAttribute("Mean", DoubleValue(mean));
    exp->SetAttribute("Bound", DoubleValue(bound));

    for(int i = 0; i < number; ++i){
        Ptr<Socket> localSocket = Socket::CreateSocket (hosts.Get (i), TcpSocketFactory::GetTypeId ());
        localSocket->Bind ();
        TCPFlow* app = new TCPFlow (localSocket, ipInterfs.GetAddress (1), servPort, 1000, 1024);

        FlowArr[i] = app;

        Time delay = Seconds(exp->GetValue());

        Simulator::Schedule(delay, &TCPFlow::StartFlow, app); // avvio la mia app in background


        // Le funzioni di callback vogliono il metodo e, nel caso di metodo di una classe, l'oggetto a cui applicarlo !!

        /*Ptr<Socket> localSocket = Socket::CreateSocket (hosts.Get (i), TcpSocketFactory::GetTypeId ());
        localSocket->Bind ();

        Simulator::ScheduleNow (&StartFlow, localSocket,
                          ipInterfs.GetAddress (1), servPort);
        */
        // QUESTO FUNZIONAAA!!!!
        /*BulkSendHelper source ("ns3::TcpSocketFactory",
        InetSocketAddress (ipInterfs.GetAddress (1), servPort));
        // Set the amount of data to send in bytes.  Zero is unlimited.
        source.SetAttribute ("SendSize", UintegerValue (1024));
        source.SetAttribute ("MaxBytes", UintegerValue(1025));
        ApplicationContainer sourceApps = source.Install (hosts.Get (i));
        sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (1000.0));*/
    }

    Simulator::Run ();
    Simulator::Destroy ();

    double tempoTot = 0;
    for(int i = 0; i < number; i++){
      double ris[2];
      FlowArr[i]->getTimes(ris);

      std::cout << "Simulazione iniziata a " << ris[0] << " secondi e finita a " << ris[1] << " secondi.\n";

      tempoTot += (ris[1] - ris[0]);
    }

    std::cout << "\n";

    std::cout << "Tempo medio della simulazione: " << tempoTot/number << " secondi.\n";


    return 0;
}