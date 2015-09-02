/*
 * geoflow-neighbors.c
 *
 *  Created on: 2014年4月23日
 *      Author: sumoon
 */

#include "geoflow-neighbors.h"
#include "ns3/core-module.h"

NS_LOG_COMPONENT_DEFINE ("GeoFlowNeighbors");

namespace ns3{
namespace geoflow {

Neighbors::Neighbors (Time checkInterval) :
		m_checkInterval(checkInterval)
{
}

Neighbors::~Neighbors()
{
	// TODO Auto-generated destructor stub
}

bool
Neighbors::IsNeighbor(Ipv4Address ip){
	NS_LOG_FUNCTION(this);
	for(std::vector<Switch>::const_iterator i = m_neighbors.begin (); i != m_neighbors.end (); ++i){
		if (i->GetIp().IsEqual(ip)) {
			return true;
		}
	}
	return false;
}

uint32_t
Neighbors::NumOfNeighbors(){
	NS_LOG_FUNCTION(this);
	return m_neighbors.size();
}

Switch
Neighbors::GetNeighborAt(uint32_t pos){
	NS_LOG_FUNCTION(this);
	return m_neighbors.at(pos);
}

std::vector<int>
Neighbors::GetNeighbors() {
	NS_LOG_FUNCTION(this);
	std::vector<int> neighborsID;
	for(std::vector<Switch>::const_iterator i = m_neighbors.begin (); i != m_neighbors.end (); ++i){
		int id = i->GetId();
		neighborsID.push_back(id);
	}

	return neighborsID;
}

void
Neighbors::UpdateNeighbors(Switch neighbor){
	NS_LOG_FUNCTION(this);
	ShowNeighbors();
	NS_LOG_LOGIC("begin update neighbors");
	std::vector<Switch>::size_type i;
	for(i = 0; i != m_neighbors.size(); ++i){
		if (m_neighbors[i].GetId() == neighbor.GetId()) {
			m_neighbors[i].SetPos(neighbor.GetPos());
			m_neighbors[i].SetVel(neighbor.GetVel());
			m_neighbors[i].SetTime(neighbor.GetTime());
			break;
		}
	}

	if (i == m_neighbors.size()){
		/*Switch newNeighbor;
		newNeighbor.SetId(neighbor.GetId());
		newNeighbor.SetIp(neighbor.GetIp());
		newNeighbor.SetPos(neighbor.GetPos());
		newNeighbor.SetVel(neighbor.GetVel());
		newNeighbor.SetTime(neighbor.GetTime());*/
		m_neighbors.push_back(neighbor);
	}
	NS_LOG_LOGIC("end update neighbors");
	ShowNeighbors();
}

void
Neighbors::CheckNeighbors(){
	NS_LOG_FUNCTION(this);
	ShowNeighbors();
	NS_LOG_LOGIC("begin check neighbors");
	std::vector<Switch>::iterator pos = m_neighbors.begin();
	for(std::vector<Switch>::size_type i = 0; i < m_neighbors.size();){
		NS_LOG_LOGIC("neighbor pos' id is "<<pos->GetId()<<" ip is "<<pos->GetIp()<<" time is "<<pos->GetTime());
		Time expireTime = m_neighbors.at(i).GetTime();
		NS_LOG_LOGIC("Neighbor "<<i<<"expire time is "<<Simulator::Now().GetMilliSeconds() - expireTime.GetMilliSeconds());
		if (Simulator::Now().GetMilliSeconds() - expireTime.GetMilliSeconds() > 2*m_checkInterval.GetMilliSeconds()){
			pos = m_neighbors.erase(pos);
		} else {
			pos++;
			i++;
		}
	}
	NS_LOG_LOGIC("end check neighbors");
	ShowNeighbors();
}

void
Neighbors::ShowNeighbors(){
	NS_LOG_FUNCTION(this);
	NS_LOG_LOGIC("Show Neighbors");
	for(std::vector<Switch>::size_type i = 0; i != m_neighbors.size(); ++i){
		NS_LOG_LOGIC("Neighbor "<<i<<": "
				<<"NodeID "<<m_neighbors[i].GetId()
				<<" NodeIP "<<m_neighbors[i].GetIp()
				<<" NodeCoordX "<<m_neighbors[i].GetPos().x
				<<" NodeCoordY "<<m_neighbors[i].GetPos().y
				<<" NodeVelX "<<m_neighbors[i].GetVel().x
				<<" NodeVelY "<<m_neighbors[i].GetVel().y
				<<" Time "<<m_neighbors[i].GetTime());
	}
}

}
}
