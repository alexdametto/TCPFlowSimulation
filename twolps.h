#ifndef TWOLPS_H
 #define TWOLPS_H
 
 #include "ns3/queue-disc.h"
 
 namespace ns3 { 
  class TwoLPS : public QueueDisc {
  public:
    TwoLPS();
    virtual ~TwoLPS();
    static TypeId GetTypeId (void);

  public:
    uint32_t* number;
    uint32_t numberOfFlow;
    uint32_t threshold;
    virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
    virtual Ptr<QueueDiscItem> DoDequeue (void);
    virtual bool CheckConfig (void);
    virtual void InitializeParams (void);  
    Ptr<const QueueDiscItem> DoPeek(void);
  };
 } // namespace ns3
 
 #endif /* TWOLPS */