/*
 * geoflow-packetqueue.h
 *
 *  Created on: 2014年4月23日
 *      Author: sumoon
 */

#ifndef GEOFLOW_PACKETQUEUE_H_
#define GEOFLOW_PACKETQUEUE_H_

#include "geoflow-packetentry.h"

namespace ns3 {
namespace geoflow{

class PacketQueue {
public:
	PacketQueue(Time checkInterval);
	virtual ~PacketQueue();
	typedef Ipv4RoutingProtocol::UnicastForwardCallback UnicastForwardCallback;

	bool Enqueue (PacketEntry entry);

	int HasPacket(Ipv4Address dstIP);

	void Dequeue (int index);

	void CheckQueue();

	PacketEntry GetEntryAt (uint32_t pos);

	uint32_t GetSize ();

private:
	std::vector<PacketEntry> m_packetQueue;
	Time m_expireTime;
};

}
} /* namespace ns3 */

#endif /* GEOFLOW_PACKETQUEUE_H_ */
