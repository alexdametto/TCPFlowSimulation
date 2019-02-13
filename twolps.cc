/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007, 2014 University of Washington
 *               2015 Universita' degli Studi di Napoli Federico II
 *
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

#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/uinteger.h"
#include "ns3/ptr.h"
#include "ns3/object-vector.h"
#include "ns3/packet.h"
#include "ns3/socket.h"
#include "ns3/unused.h"
#include "ns3/simulator.h"
#include <ns3/drop-tail-queue.h>
#include "twolps.h"
#include "ns3/queue-disc.h"
#include "ns3/net-device-queue-interface.h"
#include <string>
#include <sstream>
#include <stdlib.h>

namespace ns3 {

  int getFlowNumber(const char* ip, uint32_t size) {
    char num1[4];
    char num2[4];

    uint punti = 0;
    uint j = 0;
    uint k = 0;

    for(uint i = 0; i < size; i++){
      char c = ip[i];

      if(c == '.')
        punti++;
      else if(punti == 1) {
        num1[k] = c;
        num1[k+1] = '\n';
        k++;
      }
      else if(punti == 2){
        num2[j] = c;
        num2[j+1] = '\n';
        j++;
      }
    }

    return (atoi(num1)-2) * 250 + (atoi(num2)-2);
  }

NS_OBJECT_ENSURE_REGISTERED (TwoLPS);

TwoLPS::TwoLPS() {
  numberOfFlow = 1;
  threshold = 150;
  number = new uint32_t[1] {0};
}

TwoLPS::~TwoLPS() {
}

TypeId TwoLPS::GetTypeId (void) {
  static TypeId tid = TypeId("ns3::TwoLPS").SetParent<QueueDisc>().AddConstructor<TwoLPS>().SetGroupName("TrafficControl");
  return tid;
}
bool TwoLPS::DoEnqueue(Ptr<QueueDiscItem> item) {
  std::ostringstream buffer;
  buffer.str(std::string());
  item->Print(buffer);

  std::string s = buffer.str();
  
  char *cstr = &s[0u];

  char temp[17] = "";
  bool flag = true;
  bool spazio = false;
  uint32_t index = 0;
  for(uint32_t i = 0; flag; i++) {
    char c = cstr[i];

    if(c == ' ') {
      spazio = true;
    }
    else if(c == '>') {
      flag = false;
    }
    else {
      if(spazio) {
        for(uint32_t j = 0; j < 17; j++) {
          temp[j] = ' ';
        }
        index = 0;
        spazio = false;
      }

      temp[index] = c;
      index++;
    }
  }


  temp[index] = '\0';

  int n = getFlowNumber(temp, 17);
  
  if(n < 0) {
    return false;
  }

  if(n > (int)numberOfFlow) {
    uint32_t* x = new uint32_t[n+1];

    for(int i = 0; i < (n+1); i++) {
      x[i] = 0;
    }

    for(int i = 0; i < (int)numberOfFlow; i++) {
      x[i] = number[i];
    }

    delete [] number;

    number = x;

    numberOfFlow = n+1;
  }

  if(number[n] > threshold) {
    // metto nella seconda
    number[n]++;
    return GetInternalQueue(1)->Enqueue(item);
  }
  else {
    // metto nella prima
    number[n]++;
    return GetInternalQueue(0)->Enqueue(item);
  }
}

Ptr<const QueueDiscItem> TwoLPS::DoPeek(void) {
  Ptr<const QueueDiscItem> item = GetInternalQueue(0)->Peek();

  if(!item) {
    item = GetInternalQueue(1)->Peek();
  }

  if(!item) {
    return 0;
  }

  return item;
}

Ptr<QueueDiscItem> TwoLPS::DoDequeue() {
  Ptr<QueueDiscItem> item = GetInternalQueue(0)->Dequeue();
  if(!item) {
    item = GetInternalQueue(1)->Dequeue();
  }
  
  if(!item)
    return 0;

  return item;
}

void TwoLPS::InitializeParams() {
  uint32_t total = GetNInternalQueues();
  for(uint32_t i = 0; i < (3 - total); i++) {
      AddInternalQueue(CreateObjectWithAttributes<DropTailQueue<QueueDiscItem>>("MaxSize", QueueSizeValue(GetMaxSize())));
  }
}

bool TwoLPS::CheckConfig() {
  if(GetNInternalQueues() != 2) {
    uint32_t total = GetNInternalQueues();
    for(uint32_t i = 0; i < (3 - total); i++) {
      AddInternalQueue(CreateObjectWithAttributes<DropTailQueue<QueueDiscItem>>("MaxSize", QueueSizeValue(GetMaxSize())));
    }
  }

  return true;
}
} // namespace ns3
