/*
 * geoflow-rqueueentry.h
 *
 *  Created on: 2014年3月12日
 *      Author: sumoon
 */

#ifndef GEOFLOW_PACKETENTRY_H_
#define GEOFLOW_PACKETENTRY_H_

#include "ns3/ipv4-routing-protocol.h"

namespace ns3 {
namespace geoflow{

class PacketEntry {
public:
	virtual ~PacketEntry();
	typedef Ipv4RoutingProtocol::UnicastForwardCallback UnicastForwardCallback;

	PacketEntry (Ptr<const Packet> pa = 0, Ipv4Header const & h = Ipv4Header (),
			  UnicastForwardCallback ucb = UnicastForwardCallback ()) :
	m_packet (pa), m_header (h), m_ucb (ucb)
	{
		m_time = Simulator::Now ();
	}

	UnicastForwardCallback GetUnicastForwardCallback () const { return m_ucb; }
	void SetUnicastForwardCallback (UnicastForwardCallback ucb) { m_ucb = ucb; }
	Ptr<const Packet> GetPacket () const { return m_packet; }
	void SetPacket (Ptr<const Packet> p) { m_packet = p; }
	Ipv4Header GetIpv4Header () const { return m_header; }
	void SetIpv4Header (Ipv4Header h) { m_header = h; }
	void SetTime (Time inTime) { m_time = inTime; }
	Time GetTime () const { return m_time; }

private:
	Ptr<const Packet> m_packet;
	Ipv4Header m_header;
	UnicastForwardCallback m_ucb;
	Time m_time;
};

}
} /* namespace ns3 */

#endif /* GEOFLOW_RQUEUEENTRY_H_ */
