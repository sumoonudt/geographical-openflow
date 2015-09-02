/*
 * geoflow-protocol.cc
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#include "geoflow-protocol.h"
#include "ns3/geoflow-header.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/random-variable-stream.h"
#include "ns3/inet-socket-address.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/wifi-net-device.h"
#include "ns3/adhoc-wifi-mac.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/mobility-module.h"
#include <algorithm>
#include <limits>

# define M_PI		3.14159265358979323846

NS_LOG_COMPONENT_DEFINE ("GeoFlowProtocol");

namespace ns3
{
namespace geoflow
{
NS_OBJECT_ENSURE_REGISTERED (Protocol);
const uint32_t Protocol::GEOFLOW_PORT = 334;

//-----------------------------------------------------------------------------
Protocol::Protocol () :
		m_interval(MilliSeconds(100)),
		m_helloInterval(MilliSeconds(100)),
		m_checkNeighborsInterval(MilliSeconds(1000)),
		m_updateInterval(MilliSeconds(1000)),
		m_checkFlowTableInterval(MilliSeconds(200)),
		m_checkPacketQueueInterval(MilliSeconds(1000)),
		m_expireTime(MilliSeconds(10000)),
		m_controlDelay(MilliSeconds(0)),
		m_popCount(0),
		m_flowtable(m_checkFlowTableInterval),
		m_neighbors(m_checkNeighborsInterval),
		m_packetQueue(m_expireTime)
{
}

TypeId
Protocol::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::geoflow::Protocol")
		.SetParent<Ipv4RoutingProtocol> ()
		.AddConstructor<Protocol> ()
		.AddAttribute ("UniformRv",
					   "Access to the underlying UniformRandomVariable",
					   StringValue ("ns3::UniformRandomVariable"),
					   MakePointerAccessor (&Protocol::m_uniformRandomVariable),
					   MakePointerChecker<UniformRandomVariable> ())
		.AddTraceSource ("Tx", "Send GeoFlow packet.",
					   MakeTraceSourceAccessor (&Protocol::m_txPacketTrace))
		.AddTraceSource ("Rx", "Receive GeoFlow packet.",
					   MakeTraceSourceAccessor (&Protocol::m_rxPacketTrace))
	;
	return tid;
}

Protocol::~Protocol ()
{

}

void
Protocol::DoDispose ()
{
	m_ipv4 = 0;
	Ipv4RoutingProtocol::DoDispose ();
}

void
Protocol::PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const
{
}

Ptr<Ipv4Route>
Protocol::RouteOutput (Ptr<Packet> p, const Ipv4Header &header,
							  Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
	NS_LOG_FUNCTION (this);
	return LoopbackRoute (header, oif);
}

bool
Protocol::RouteInput (Ptr<const Packet> p, const Ipv4Header &header,
							 Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
							 MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb)
{
	NS_LOG_FUNCTION (this);
	int32_t iif = m_ipv4->GetInterfaceForDevice (idev);

	Ipv4Address dst = header.GetDestination ();

	if (idev == m_lo)// Deferred route request
	{
		DeferredRouteOutput (p, header, ucb, ecb);
		return true;
	}

	if (m_ipv4->IsDestinationAddress (dst, iif))// Unicast local delivery
	{
		lcb (p, header, iif);
		return true;
	}

	return Forwarding (p, header, ucb, ecb);// Forwarding
}

void
Protocol::SetIpv4 (Ptr<Ipv4> ipv4)
{
	NS_LOG_FUNCTION (this);
	NS_ASSERT (ipv4 != 0);
	NS_ASSERT (m_ipv4 == 0);
	m_ipv4 = ipv4;
	NS_ASSERT (m_ipv4->GetNInterfaces () == 1 && m_ipv4->GetAddress (0, 0).GetLocal () == Ipv4Address ("127.0.0.1"));
	m_lo = m_ipv4->GetNetDevice (0);
}

void
Protocol::NotifyInterfaceUp (uint32_t i)
{
	NS_LOG_FUNCTION (this << m_ipv4->GetAddress (i, 0).GetLocal ());
	Ptr<Ipv4L3Protocol> l3 = m_ipv4->GetObject<Ipv4L3Protocol> ();
	if (l3->GetNAddresses (i) > 1)
	{
		NS_LOG_WARN ("GeoFlow does not work with more then one address per each interface.");
	}
	Ipv4InterfaceAddress iface = l3->GetAddress (i, 0);
	if (iface.GetLocal () == Ipv4Address ("127.0.0.1"))
		return;

	// Create a socket to listen only on this interface
	Ptr<Socket> socket = Socket::CreateSocket (GetObject<Node> (),
										 UdpSocketFactory::GetTypeId ());
	NS_ASSERT (socket != 0);
	socket->SetRecvCallback (MakeCallback (&Protocol::RecvGeoFlow, this));
	socket->BindToNetDevice (l3->GetNetDevice (i));
	socket->Bind (InetSocketAddress (Ipv4Address::GetAny (), GEOFLOW_PORT));
	socket->SetAllowBroadcast (true);
	socket->SetAttribute ("IpTtl", UintegerValue (1));
	m_socket = socket;
}

void
Protocol::NotifyInterfaceDown (uint32_t i)
{
	NS_ASSERT (m_socket);
	m_socket->Close ();
}

void
Protocol::NotifyAddAddress (uint32_t i, Ipv4InterfaceAddress address)
{
}

void
Protocol::NotifyRemoveAddress (uint32_t i, Ipv4InterfaceAddress address)
{
}

Ptr<Ipv4Route>
Protocol::LoopbackRoute (const Ipv4Header & hdr, Ptr<NetDevice> oif) const
{
	NS_LOG_FUNCTION (this);
	NS_ASSERT (m_lo != 0);
	Ptr<Ipv4Route> rt = Create<Ipv4Route> ();
	rt->SetDestination (hdr.GetDestination ());
	rt->SetSource(m_ipv4->GetAddress(1,0).GetLocal());
	rt->SetGateway (Ipv4Address ("127.0.0.1"));
	rt->SetOutputDevice (m_lo);
	return rt;
}

void
Protocol::AddPacketToQueue(Ptr<const Packet> p, const Ipv4Header & header, UnicastForwardCallback ucb){
	MatchField match;
	match.SetSrcIpAddr(header.GetSource ());
	match.SetDstIpAddr(header.GetDestination ());
	FlowEntry flow = m_flowtable.LookUpFlowEntry(match);
	if (flow.IsValid()){
		if(TestFlowEntry(flow)){
			m_flowtable.ProlongFlowEntry(flow);
			Ptr<Ipv4Route> route = ConvertFlow2Route(flow);
			ucb (route, p, header);
		} else {
			m_flowtable.DropFlowEntry(flow);
			PacketEntry newEntry (p, header, ucb);
			m_packetQueue.Enqueue(newEntry);
			NS_LOG_LOGIC ("Add packet " << p->GetUid () << " to queue.");
			FlowMiss(p,header);
		}
	} else {
		PacketEntry newEntry (p, header, ucb);
		m_packetQueue.Enqueue(newEntry);
		NS_LOG_LOGIC ("Add packet " << p->GetUid () << " to queue.");
		FlowMiss(p,header);
	}
}

void
Protocol::DeferredRouteOutput (Ptr<const Packet> p, const Ipv4Header & header,
                                      UnicastForwardCallback ucb, ErrorCallback ecb)
{
	NS_LOG_FUNCTION (this);
	AddPacketToQueue(p,header,ucb);
}

bool
Protocol::Forwarding (Ptr<const Packet> p, const Ipv4Header & header,
                             UnicastForwardCallback ucb, ErrorCallback ecb)
{
	NS_LOG_FUNCTION (this);
	AddPacketToQueue(p,header,ucb);
	return true;
}

bool
Protocol::TestFlowEntry(FlowEntry flow){
	return m_neighbors.IsNeighbor(flow.GetAction().GetPara().nextIpAddr);
}

void
Protocol::SetUp(Ptr<Node> node, Controller * controller)
{
	NS_LOG_FUNCTION (this);
	m_node = node;
	m_controller = controller;
	m_uniformRandomVariable->SetStream (node->GetId());

	ns3::PacketMetadata::Enable ();
	ScheduleTimer();
}

void
Protocol::ScheduleTimer (){
	int curTime = Simulator::Now().GetMilliSeconds();
	NS_LOG_LOGIC("cur time is "<<curTime);

	//PopPacketQueue();

	if (m_popCount % (m_helloInterval.GetMilliSeconds()/m_interval.GetMilliSeconds()) == 0){
		if (m_socket != 0){
			SendHello();
		}
	}
	if (m_popCount % (m_checkNeighborsInterval.GetMilliSeconds()/m_interval.GetMilliSeconds()) == 0){
		m_neighbors.CheckNeighbors();
	}
	if (m_popCount % (m_updateInterval.GetMilliSeconds()/m_interval.GetMilliSeconds()) == 0){
		UpdateState();
	}
	if (m_popCount % (m_checkFlowTableInterval.GetMilliSeconds()/m_interval.GetMilliSeconds()) == 0){
		m_flowtable.CheckTable();
	}
	if (m_popCount % (m_checkPacketQueueInterval.GetMilliSeconds()/m_interval.GetMilliSeconds()) == 0){
		m_packetQueue.CheckQueue();
	}

	m_popCount++;

	Time nextTime = Seconds(m_interval.GetSeconds()+0.0001*(m_uniformRandomVariable->GetInteger (0, 10)));
	NS_LOG_LOGIC("next time is "<<nextTime.GetMilliSeconds());
	Simulator::Schedule (nextTime, & Protocol::ScheduleTimer,this);
}

void
Protocol::UpdateState (void)
{
	NS_LOG_FUNCTION (this);
	Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel>();
	Vector pos = mm->GetPosition(); // Get position
	Vector vel = mm->GetVelocity(); // Get velocity
	Time time = Simulator::Now();

	Message message;
	message.SetType(MESSAGE_TYPE_STATE_UPDATE);
	struct MessagePara para;
	para.nodeID = m_node->GetId();
	if(m_ipv4 == 0){
		para.nodeIpAddr = Ipv4Address("192.168.1.1");
	} else {
		para.nodeIpAddr = m_ipv4->GetAddress(1,0).GetLocal();
	}
	para.nodePos = pos;
	para.nodeVel = vel;
	para.curTime = time;
	para.neighbors = m_neighbors.GetNeighbors();
	message.SetPara(para);

	m_txPacketTrace (message.GetSize());

	Message reply;
	reply = m_controller->ReceiveMessage(message);

	Simulator::Schedule (m_controlDelay, & Protocol::ReceiveMessage,this,reply);
}

Ptr<Ipv4Route>
Protocol::ConvertFlow2Route (FlowEntry flow){
	NS_LOG_FUNCTION (this);
	Ptr<Ipv4Route> rt = Create<Ipv4Route> ();
	rt->SetDestination (flow.GetMatch().GetDstIpAddr());
	rt->SetSource(flow.GetMatch().GetSrcIpAddr());
	rt->SetGateway (flow.GetAction().GetPara().nextIpAddr);
	rt->SetOutputDevice (m_node->GetDevice(0));

	return rt;
}

void
Protocol::FlowMiss(Ptr<const Packet> p, const Ipv4Header & header){
	NS_LOG_FUNCTION (this);

	Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel>();
	Vector pos = mm->GetPosition(); // Get position
	Vector vel = mm->GetVelocity(); // Get velocity
	Time time = Simulator::Now();
	Ipv4Address srcIP = header.GetSource();
	Ipv4Address dstIP = header.GetDestination();

	Message request;
	request.SetType(MESSAGE_TYPE_ROUTE_REQUEST);
	struct MessagePara para;
	para.nodeID = m_node->GetId();
	para.nodeIpAddr = m_ipv4->GetAddress(1,0).GetLocal();
	para.nodePos = pos;
	para.nodeVel = vel;
	para.curTime = time;
	para.srcIpAddr = srcIP;
	para.dstIpAddr = dstIP;
	request.SetPara(para);

	m_txPacketTrace (request.GetSize());

	Message reply;
	reply = m_controller->ReceiveMessage(request);

	m_txPacketTrace (reply.GetSize());

	Simulator::Schedule (m_controlDelay, & Protocol::ReceiveMessage,this, reply);
}

void
Protocol::ReceiveMessage(Message message){
	NS_LOG_FUNCTION (this);
	if (message.GetType() == MESSAGE_TYPE_ROUTE_REPLY){
		MessagePara para = message.GetPara();
		if (para.valid){
			if(para.routeType == ROUTE_TYPE_HOP_BASED){
				MatchField match;
				match.SetSrcIpAddr(para.srcIpAddr);
				match.SetDstIpAddr(para.dstIpAddr);
				Action action;
				ActionPara apara;
				apara.nextIpAddr = para.nextIpAddr;
				action.SetType(ACTION_TYPE_FORWARD);
				action.SetPara(apara);
				FlowEntry flow;
				flow.SetMatch(match);
				flow.SetAction(action);
				flow.SetCounter(0);
				flow.SetPriority(1);
				flow.SetTimeout(para.timeout);
				flow.SetValid(true);

				m_flowtable.InsertFlowEntry(flow);

				NotifyFlowInsert(flow);
			}

			if(para.routeType == ROUTE_TYPE_GEO_BASED){
				Ipv4Address nextHopIp = ComputeNextHop(para.dstIpAddr,para.direction);
				if (nextHopIp != "1.1.1.1"){
					SendSingleCast(para.srcIpAddr,para.dstIpAddr,para.direction,para.timeout);

					MatchField match;
					match.SetSrcIpAddr(para.srcIpAddr);
					match.SetDstIpAddr(para.dstIpAddr);
					Action action;
					ActionPara apara;
					apara.nextIpAddr = nextHopIp;
					apara.direction = para.direction;
					action.SetType(ACTION_TYPE_FORWARD);
					action.SetPara(apara);
					FlowEntry flow;
					flow.SetMatch(match);
					flow.SetAction(action);
					flow.SetCounter(0);
					flow.SetPriority(1);
					flow.SetTimeout(para.timeout);
					flow.SetValid(true);

					m_flowtable.InsertFlowEntry(flow);

					NotifyFlowInsert(flow);
				}
			}
		}
	}
}

Ipv4Address
Protocol::ComputeNextHop(Ipv4Address dstIp, double direction){
	NS_LOG_FUNCTION (this);
	Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel>();
	Vector pos = mm->GetPosition(); // Get position
	double px1 = pos.x;	double py1 = pos.y;

	Ipv4Address nextHopIp = Ipv4Address("1.1.1.1");
	int nextHopID = -1;
	if (m_neighbors.IsNeighbor(dstIp)){
		nextHopIp = dstIp;
		return nextHopIp;
	}

	for (uint32_t i=0; i<m_neighbors.NumOfNeighbors(); i++){
		double px2 = m_neighbors.GetNeighborAt(i).GetPos().x;	double py2 = m_neighbors.GetNeighborAt(i).GetPos().y;
		double diffx = px2 - px1;
		double diffy = py2 - py1;
		double angle;
		if(diffx != 0){
			angle = atan(diffy/diffx);
			if(diffx < 0){
				angle += M_PI;
			}
		}else{
			if(diffy >= 0){
				angle = M_PI/2;
			}else{
				angle = -M_PI/2;
			}
		}
		angle += M_PI/2;

		double newDiff = abs(angle - direction);
		if (newDiff > abs(newDiff - 2*M_PI))
			newDiff = abs(newDiff - 2*M_PI);
		if (newDiff <= M_PI/8){
			nextHopID = i;
			break;
		}
	}
	if(nextHopID != -1){
		nextHopIp = m_neighbors.GetNeighborAt(nextHopID).GetIp();
	}

	return nextHopIp;
}

void
Protocol::NotifyFlowInsert(FlowEntry flow){
	NS_LOG_FUNCTION (this);
	Ipv4Address dst = flow.GetMatch().GetDstIpAddr();
	int pos = m_packetQueue.HasPacket(dst);
	while (pos != -1)
	{
		if(TestFlowEntry(flow)){
			m_flowtable.ProlongFlowEntry(flow);
			Ptr<Ipv4Route> route = ConvertFlow2Route(flow);
			PacketEntry packetEntry = m_packetQueue.GetEntryAt(pos);
			Ptr<Packet> p = ConstCast<Packet> (packetEntry.GetPacket());
			UnicastForwardCallback ucb = packetEntry.GetUnicastForwardCallback();
			Ipv4Header header = packetEntry.GetIpv4Header ();
			header.SetSource (route->GetSource ());
			header.SetTtl (header.GetTtl () + 1);
			ucb (route, p, header);
			m_packetQueue.Dequeue(pos);
			pos = m_packetQueue.HasPacket(dst);
		} else {
			m_flowtable.DropFlowEntry(flow);
			break;
		}
	}
}

void
Protocol::PopPacketQueue(){
	uint32_t size = m_packetQueue.GetSize();
	int index = 0;
	if (size != 0){
		PacketEntry entry = m_packetQueue.GetEntryAt(index);
		MatchField match;
		match.SetSrcIpAddr(entry.GetIpv4Header().GetSource());
		match.SetDstIpAddr(entry.GetIpv4Header().GetDestination());
		FlowEntry flow = m_flowtable.LookUpFlowEntry(match);
		if (flow.IsValid()){
			Ptr<Packet> p = ConstCast<Packet> (entry.GetPacket ());
			UnicastForwardCallback ucb = entry.GetUnicastForwardCallback ();
			Ipv4Header header = entry.GetIpv4Header ();
			if(TestFlowEntry(flow)){
				m_flowtable.ProlongFlowEntry(flow);
				Ptr<Ipv4Route> route = ConvertFlow2Route(flow);

				header.SetSource (route->GetSource ());
				header.SetDestination(route->GetDestination());
				header.SetTtl (header.GetTtl () + 1); // compensate extra TTL decrement by fake loopback routing
				ucb (route, p, header);

				m_packetQueue.Dequeue(index);

			} else {
				m_flowtable.DropFlowEntry(flow);
				FlowMiss(p,header);
			}
		} else {
			FlowMiss(entry.GetPacket(),entry.GetIpv4Header());
		}
	}
}

void
Protocol::SendHello(){
	NS_LOG_FUNCTION (this);

	Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel>();
	Vector pos = mm->GetPosition(); // Get position
	Vector vel = mm->GetVelocity(); // Get velocity
	Time time = Simulator::Now();

	HelloHeader helloHeader;
	helloHeader.setNodeId(m_node->GetId());
	helloHeader.setIp(m_ipv4->GetAddress(1,0).GetLocal());
	helloHeader.SetCoord(pos);
	helloHeader.setVel(vel);
	helloHeader.SetTime(time);

	Ptr<Packet> packet = Create<Packet> ();
	packet->AddHeader (helloHeader);
	TypeHeader typeHeader;
	typeHeader.SetType(GEOFLOW_TYPE_HELLO);
	packet->AddHeader (typeHeader);
	// Send to all-hosts broadcast if on /32 addr, subnet-directed otherwise
	Ipv4Address destination = Ipv4Address ("255.255.255.255");

	SendTo(m_socket,packet,destination);
}

void
Protocol::SendSingleCast(Ipv4Address srcIP, Ipv4Address dstIP, double direction, Time timeout){
	NS_LOG_FUNCTION (this);

	SCastHeader scastHeader;
	scastHeader.SetSrcIp(srcIP);
	scastHeader.SetDstIp(dstIP);
	scastHeader.SetDirection(direction);
	scastHeader.SetTimeout(timeout);

	Ptr<Packet> packet = Create<Packet> ();
	packet->AddHeader (scastHeader);
	TypeHeader typeHeader;
	typeHeader.SetType(GEOFLOW_TYPE_SCAST);
	packet->AddHeader (typeHeader);
	// Send to all-hosts broadcast if on /32 addr, subnet-directed otherwise
	Ipv4Address destination = Ipv4Address ("255.255.255.255");

	SendTo(m_socket,packet,destination);
}

void
Protocol::SendTo (Ptr<Socket> socket, Ptr<Packet> packet, Ipv4Address destination)
{
	NS_LOG_FUNCTION (this);
    socket->SendTo (packet, 0, InetSocketAddress (destination, GEOFLOW_PORT));
    m_txPacketTrace (packet->GetSize());
}

void
Protocol::RecvGeoFlow (Ptr<Socket> socket)
{
	NS_LOG_FUNCTION (this << socket);
	Address sourceAddress;
	Ptr<Packet> packet = socket->RecvFrom (sourceAddress);
	InetSocketAddress inetSourceAddr = InetSocketAddress::ConvertFrom (sourceAddress);
	Ipv4Address sender = inetSourceAddr.GetIpv4 ();
	Ipv4Address receiver = m_ipv4->GetAddress (1, 0).GetLocal ();
	NS_LOG_DEBUG ("GeoFlow node " << m_node->GetId() << " received a GeoFlow packet from " << sender << " to " << receiver);

	//m_rxPacketTrace (packet->GetSize());

	TypeHeader typeHeader;
	packet->RemoveHeader (typeHeader);
	if (!typeHeader.IsValid ())
	{
		NS_LOG_DEBUG ("GeoFlow message " << packet->GetUid () << " with unknown type received: " << typeHeader.GetType() << ". Drop");
		return; // drop
	}
	switch (typeHeader.GetType())
	{
		case GEOFLOW_TYPE_HELLO:
		{
			RecvHello (packet, receiver, sender);
			break;
		}
		case GEOFLOW_TYPE_SCAST:
		{
			RecvSingleCast (packet, receiver, sender);
			break;
		}
		default:
		{
			break;
		}
	}
}

void
Protocol::RecvHello (Ptr<Packet> p, Ipv4Address receiver, Ipv4Address src){
	NS_LOG_FUNCTION (this);
	HelloHeader helloHeader;
	p->RemoveHeader (helloHeader);
	if (helloHeader.IsValid()){
		int nodeId = helloHeader.getNodeId();
		Ipv4Address ip = helloHeader.getIp();
		Vector coord = helloHeader.GetCoord();
		Vector vel = helloHeader.getVel();
		Time time = helloHeader.GetTime();
		NS_LOG_LOGIC ("Reveive hello from node " << nodeId << " with IP " << ip);

		Switch neighbor;
		neighbor.SetId(nodeId);
		neighbor.SetIp(ip);
		neighbor.SetPos(coord);
		neighbor.SetVel(vel);
		neighbor.SetTime(time);

		m_neighbors.UpdateNeighbors(neighbor);
	}
}

void
Protocol::RecvSingleCast (Ptr<Packet> p, Ipv4Address receiver, Ipv4Address src){
	NS_LOG_FUNCTION (this);
	SCastHeader scastHeader;
	p->RemoveHeader (scastHeader);
	if (scastHeader.IsValid()){
		Ipv4Address srcIP = scastHeader.GetSrcIp();
		Ipv4Address dstIP = scastHeader.GetDstIp();
		double direction = scastHeader.GetDirection();
		Time timeout = scastHeader.GetTimeout();
		Ipv4Address nextHopIp = ComputeNextHop(dstIP,direction);
		NS_LOG_LOGIC ("Reveive SingleCast for srcIP " << srcIP << " to dstIP " << dstIP);

		MatchField match;
		match.SetSrcIpAddr(srcIP);
		match.SetDstIpAddr(dstIP);
		Action action;
		ActionPara apara;
		apara.nextIpAddr = nextHopIp;
		apara.direction = direction;
		action.SetType(ACTION_TYPE_FORWARD);
		action.SetPara(apara);
		FlowEntry flow;
		flow.SetMatch(match);
		flow.SetAction(action);
		flow.SetCounter(0);
		flow.SetPriority(1);
		flow.SetTimeout(timeout);
		flow.SetValid(true);
		if (nextHopIp != "1.1.1.1"){
			m_flowtable.InsertFlowEntry(flow);
			//NotifyFlowInsert(flow);
		}
	}
}

}
}
