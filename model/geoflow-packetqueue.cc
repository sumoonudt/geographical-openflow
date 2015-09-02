/*
 * geoflow-packetqueue.cc
 *
 *  Created on: 2014年4月23日
 *      Author: sumoon
 */

#include "geoflow-packetqueue.h"
#include "ns3/core-module.h"

NS_LOG_COMPONENT_DEFINE ("GeoFlowPacketQueue");

namespace ns3 {
namespace geoflow{

PacketQueue::PacketQueue(Time expireTime) :
		m_expireTime (expireTime)
{
}

PacketQueue::~PacketQueue() {
	// TODO Auto-generated destructor stub
}

bool
PacketQueue::Enqueue (PacketEntry entry){
	NS_LOG_FUNCTION(this);
	m_packetQueue.push_back(entry);
	return true;
}

void
PacketQueue::Dequeue (int index){
	NS_LOG_FUNCTION(this);
	std::vector<PacketEntry>::iterator pos = m_packetQueue.begin();
	for(std::vector<PacketEntry>::size_type i = 0; i != m_packetQueue.size();){
		if((int)i == index){
			NS_LOG_LOGIC("Packet "<<m_packetQueue.at(i).GetPacket()->GetUid()<<" is sent out at "<<Simulator::Now().GetMilliSeconds());
			pos = m_packetQueue.erase(pos);
			break;
		}else {
			pos++;
			i++;
		}
	}
}

PacketEntry
PacketQueue::GetEntryAt (uint32_t pos){
	NS_LOG_FUNCTION(this);
	return m_packetQueue.at(pos);
}

uint32_t
PacketQueue::GetSize (){
	NS_LOG_FUNCTION(this);
	return m_packetQueue.size();
}

void
PacketQueue::CheckQueue(){
	NS_LOG_FUNCTION(this);
	std::vector<PacketEntry>::iterator pos = m_packetQueue.begin();
	for(std::vector<PacketEntry>::size_type i = 0; i != m_packetQueue.size();){
		Time expireTime = m_packetQueue.at(i).GetTime();
		NS_LOG_LOGIC("The expire time of packet "<<m_packetQueue.at(i).GetPacket()->GetUid()<<" is "
				<<m_expireTime.GetMilliSeconds() + expireTime.GetMilliSeconds() - Simulator::Now().GetMilliSeconds());
		if (m_expireTime.GetMilliSeconds() + expireTime.GetMilliSeconds() - Simulator::Now().GetMilliSeconds() < 0){
			pos = m_packetQueue.erase(pos);
		} else {
			pos++;
			i++;
		}
	}
}

int
PacketQueue::HasPacket(Ipv4Address dstIP){
	NS_LOG_FUNCTION(this);
	for(std::vector<PacketEntry>::size_type i = 0; i != m_packetQueue.size();){
		Ipv4Address dstIPtemp = m_packetQueue.at(i).GetIpv4Header().GetDestination();
		if (dstIPtemp.IsEqual(dstIP)){
			return (int)i;
		}
	}
	return -1;
}

}
} /* namespace ns3 */
