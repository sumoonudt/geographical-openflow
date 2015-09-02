/*
 * geoflow-protocol.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_ROUTING_PROTOCOL_H
#define GEOFLOW_ROUTING_PROTOCOL_H

#include <map>

#include "geoflow-packetentry.h"
#include "geoflow-packetqueue.h"
#include "geoflow-agent.h"
#include "geoflow-neighbors.h"
#include "geoflow-flowtable.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/node.h"
#include "ns3/random-variable-stream.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include <ns3/traced-callback.h>

namespace ns3
{
namespace geoflow
{

class Protocol : public Ipv4RoutingProtocol
{
public:
	static TypeId GetTypeId (void);
	static const uint32_t GEOFLOW_PORT;

	Protocol ();
	virtual ~Protocol();
	virtual void DoDispose ();

	Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);
	bool RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
				   UnicastForwardCallback ucb, MulticastForwardCallback mcb,
				   LocalDeliverCallback lcb, ErrorCallback ecb);
	virtual void NotifyInterfaceUp (uint32_t interface);
	virtual void NotifyInterfaceDown (uint32_t interface);
	virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
	virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);
	virtual void SetIpv4 (Ptr<Ipv4> ipv4);
	virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const;

	Ptr<Ipv4Route> LoopbackRoute (const Ipv4Header & header, Ptr<NetDevice> oif) const;
	void DeferredRouteOutput (Ptr<const Packet> p, const Ipv4Header & header, UnicastForwardCallback ucb, ErrorCallback ecb);
	bool Forwarding (Ptr<const Packet> p, const Ipv4Header & header, UnicastForwardCallback ucb, ErrorCallback ecb);

	void SetUp(Ptr<Node> node, ns3::geoflow::Controller * controller);
	void ScheduleTimer ();
	void UpdateState (void);
	Ptr<Ipv4Route> ConvertFlow2Route (FlowEntry flow);
	void FlowMiss(Ptr<const Packet> p, const Ipv4Header & header);
	void ReceiveMessage(Message message);
	Ipv4Address ComputeNextHop(Ipv4Address dstIp, double direction);
	void NotifyFlowInsert(FlowEntry flow);
	void AddPacketToQueue(Ptr<const Packet> p, const Ipv4Header & header, UnicastForwardCallback ucb);
	void PopPacketQueue();
	bool TestFlowEntry(FlowEntry flow);

	void SendHello();
	void SendSingleCast(Ipv4Address srcIP, Ipv4Address dstIP, double direction, Time timeout);
	void SendTo(Ptr<Socket> socket, Ptr<Packet> packet, Ipv4Address destination);
	void RecvGeoFlow(Ptr<Socket> socket);
	void RecvHello (Ptr<Packet> p, Ipv4Address receiver, Ipv4Address src);
	void RecvSingleCast (Ptr<Packet> p, Ipv4Address receiver, Ipv4Address src);
private:

	Ptr<Ipv4> m_ipv4;
	Ptr<NetDevice> m_lo;
	Ptr<Socket> m_socket;
	Ptr<Node> m_node;

	Controller * m_controller;

	Time m_interval;
	Time m_helloInterval;
	Time m_checkNeighborsInterval;
	Time m_updateInterval;
	Time m_checkFlowTableInterval;
	Time m_checkPacketQueueInterval;
	Time m_expireTime;
	Time m_popPacketQueueInterval;
	Time m_controlDelay;

	int m_popCount;

	FlowTable m_flowtable;
	Neighbors m_neighbors;
	PacketQueue m_packetQueue;

	Ptr<UniformRandomVariable> m_uniformRandomVariable;

	TracedCallback<uint32_t> m_txPacketTrace;
	TracedCallback<uint32_t> m_rxPacketTrace;

};

}
}

#endif
