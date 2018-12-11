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
#include <sstream>


#include <iostream>
#include <fstream>

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
  uint32_t numPackets;
  double startingTime;
  double endingTime;


  private:
  void WriteUntilBufferFull(Ptr<Socket> socket, uint32_t txSpace) {
    //std::cout << currentTxBytes << "\t" << totalTxBytes << "\n";
    while (currentTxBytes < totalTxBytes && socket->GetTxAvailable () >= this->writeSize) 
    {
      /*uint32_t toWrite = totalTxBytes - currentTxBytes;
      if(toWrite >= writeSize)
        toWrite = writeSize;*/

      /*uint32_t left = totalTxBytes - currentTxBytes;
      uint32_t dataOffset = currentTxBytes % writeSize;
      uint32_t toWrite = writeSize - dataOffset;


      toWrite = std::min (toWrite, left);
      toWrite = std::min (toWrite, socket->GetTxAvailable ());

      if(toWrite < this->writeSize) {
        return;
      }*/
      Ptr<Packet> packet = Create<Packet>(this->writeSize);
      int amountSent = socket->Send (packet);
      if(amountSent < 0)
      {
        std::cout << "uhm....\n";
        // we will be called again when new tx space becomes available.
        return;
      }
      else currentTxBytes += amountSent;

      //std::cout << "Inviati: " << amountSent << "\t Stato: " << currentTxBytes << "\t/\t" << totalTxBytes << "\n"; 
    }
    if(currentTxBytes == totalTxBytes) // il socket va chiuso SOLO se ho finito, altrimenti manderà meno pacchetti!!!! 
      socket->Close ();

    //std::cout << currentTxBytes << "\t/\t" << totalTxBytes << "\n"; 
  }

  public:
  TCPFlow(Ptr<Socket> socket, Ipv4Address servAddress, uint16_t servPort, uint32_t numPackets, uint32_t bytesForPacket) {
    this->socket = socket;
    this->servAddress = servAddress;
    this->servPort = servPort;
    this->totalTxBytes = numPackets * bytesForPacket;
    this->writeSize = bytesForPacket;
    this->currentTxBytes = 0;
    this->numPackets = numPackets;

    this->startingTime = 0;
    this->endingTime = 0;
  }

  ~TCPFlow() {
  }

  void StartFlow () {
    //NS_LOG_LOGIC ("Starting flow at time " <<  Simulator::Now ().GetSeconds ());
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

  uint32_t getFlowDim() {
    return this->numPackets;
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
  }
} 



double geometric(double mean) {

  //double x = (double)rand() / (double)RAND_MAX;

  double p = 1/mean;
  double value = (log(1 - rand() * 1.0 / RAND_MAX) / log(1 - p));

  return value;
}

double exponential(double mean) {
  //double x = (double)rand() / (double)RAND_MAX;

  return -log(rand() * 1.0 / RAND_MAX) / mean;
}

int main (int argc, char *argv[])
{
    LogComponentEnable("SimTesi", LOG_LEVEL_INFO);

    int number = 200;

    int simNumber = 1;

    int seed = 100;

    CommandLine cmd;
    cmd.AddValue("SimNumber", "Number of the simulation.", simNumber);
    cmd.AddValue("FlowNumber", "Number of TCP Flow.", number);
    cmd.AddValue("Seed", "Seed of the simulation.", seed);
    /*cmd.AddValue("DataRate", "P2P data rate in bps", datarate);
    cmd.AddValue("n_tcp", "Number of TCP Flow", number);
    cmd.AddValue("seed", "Number of seed", seed);*/
    cmd.Parse (argc, argv);

    //std::cout << simNumber << "\t" << number << "\t" << seed << "\n";

    //return 0;

    FlowArr = (TCPFlow**)malloc(number * sizeof(TCPFlow*));

    srand(seed+1);
    SeedManager::SetSeed (seed+1);

    /*if(number > 250) {
      number = 250;
      std::cout << "IL NUMERO MASSIMO DI TCP FLOW È 250. LIMITE IMPOSTATO A 250.";
    }*/

    std::string lat = "2"; // 2 ms
    std::string datarate = "1000000000"; // 1 Gb/s

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", DataRateValue ( DataRate(1000000000))); // B [bps] è la banda del canale
    p2p.SetChannelAttribute ("Delay", TimeValue (MilliSeconds(2))); // lambda^(-1)

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

        std::string bas = "10." +  std::to_string((int)i/250+2) + "." + std::to_string(i%250+2) + ".0";
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

    std::string datarateDest = "1000000"; // 1 Gb/s

    PointToPointHelper speciapP2P;
    speciapP2P.SetDeviceAttribute ("DataRate", DataRateValue ( DataRate(1000000))); // B [bps] è la banda del canale
    speciapP2P.SetChannelAttribute ("Delay", TimeValue (MilliSeconds(2))); // lambda^(-1)

    NodeContainer connection = NodeContainer(routers.Get(0), endHosts.Get(0));
    NetDeviceContainer ndc = speciapP2P.Install(connection);

    Ipv4InterfaceContainer ipInterfs = ipv4.Assign(ndc);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    uint16_t servPort = 50000;

    PacketSinkHelper sink ("ns3::TcpSocketFactory",
                     InetSocketAddress (Ipv4Address::GetAny (), servPort));
    ApplicationContainer sinkApps = sink.Install (endHosts.Get (0));
    sinkApps.Start (Seconds (0.0));

    uint32_t dimPack = 1522;
    
    double packetMean = 2000;
    double expMean = 0.95 * 1000000 / dimPack; // 100 Mb/s

    Time lastDelay = Seconds(0);

    // pacchetti in modo geometrico media = 2000 pacchetti

    for(int i = 0; i < number; i++){
        Ptr<Socket> localSocket = Socket::CreateSocket (hosts.Get (i), TcpSocketFactory::GetTypeId ());
        localSocket->Bind ();

        TCPFlow* app = new TCPFlow (localSocket, ipInterfs.GetAddress (1), servPort, geometric(packetMean), dimPack); // 1522 ethernet

        FlowArr[i] = app;

        if(i == 0) {
          Simulator::ScheduleNow(&TCPFlow::StartFlow, app); // avvio la mia app in BACKGROUND
        }
        else {
          Time newDelay = Seconds(lastDelay.GetSeconds() + exponential(expMean));
          Simulator::Schedule(newDelay,&TCPFlow::StartFlow, app); // avvio la mia app in BACKGROUND
          lastDelay = newDelay;
        }
    }

    Simulator::Run ();
    Simulator::Destroy ();

    std::string path = "scratch/TCPFlowSimulation/OutputFiles/ris" + std::to_string(simNumber) + ".txt";
    std::fstream txtFile;
    txtFile.open(path, std::fstream::out);

    double tempoTot = 0;
    for(int i = 0; i < number; i++){
      double ris[2];
      FlowArr[i]->getTimes(ris);

      std::cout << "Simulazione iniziata a " << ris[0] << " secondi e finita a " << ris[1] << " secondi.\n";

      tempoTot += (ris[1] - ris[0]);

      std::stringstream data;
      data << FlowArr[i]-> getFlowDim() << "," << ris[1] - ris[0];

      //std::cout << data.str() << "\n";


      //free(FlowArr[i]);

      //delete FlowArr[i];

      txtFile << data.str() << "\n";

      //std::string data(std::to_string(tempoTot/number));
    }

    free(FlowArr);

    //delete [] exp;

    std::cout << "Tempo medio della simulazione: " << tempoTot/number << " secondi.\n";

    //free(&(*exp));

    txtFile.close();

    std::cout << "\n";

    return 0;
}