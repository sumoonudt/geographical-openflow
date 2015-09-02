/*
 * goeflow-topology.cpp
 *
 *  Created on: 2014年3月9日
 *      Author: sumoon
 */

#include "geoflow-topology.h"
#include "ns3/ipv4-address-helper.h"

NS_LOG_COMPONENT_DEFINE ("GeoFlowTopology");

namespace ns3 {
namespace geoflow{

Topology::Topology(int switchNum) {
	// TODO Auto-generated constructor stub
	m_switchNum = switchNum;
	m_showTopoInterval = MilliSeconds(200);
	m_of.open("trace.txt");
	SetUpTopology();
}

Topology::~Topology() {
	m_switches.clear();
	m_of.close();
}

void
Topology::SetUpTopology()
{
	NS_LOG_FUNCTION(this);
	Ipv4AddressHelper iphelper;
	iphelper.SetBase ("192.168.1.0", "255.255.255.0");
	for (int i=0; i<m_switchNum; i++)
	{
		Switch s;
		s.SetId(i);
		s.SetIp(iphelper.NewAddress());
		s.SetTime(Simulator::Now());
		s.SetPos(Vector());
		s.SetVel(Vector());
		m_switches.push_back(s);
	}
	for (int i=0; i<m_switchNum; i++){
		std::vector<bool> row;
		for (int j=0; j<m_switchNum; j++){
			if (i == j){
				row.push_back(true);
			} else {
				row.push_back(false);
			}
		}
		m_connection.push_back(row);
	}
	ShowTopology();
}

void
Topology::SetUpMultiCopyRouting(int sid, int did, int copyNum, int routingType, int qualityType){
	m_mcRouting.setSid(sid);
	m_mcRouting.setDid(did);
	m_mcRouting.setCopyNum(copyNum);
	m_mcRouting.setRoutingType(routingType);
	m_mcRouting.setQualityType(qualityType);
}

std::vector<Switch>&
Topology::GetTopology(){
	return m_switches;
}

Switch&
Topology::GetSwitch(int switchID){
	return m_switches[switchID];
}

int
Topology::getSwitchNum(){
	return m_switchNum;
}

void
Topology::UpdateTopology(int switchID, Ipv4Address addr, Time time, Vector pos, Vector vel, std::vector<int> neighbors)
{
	NS_LOG_FUNCTION(this);
	NS_LOG_LOGIC("Update Network Topology with: "
				<<"switchID "<<switchID
				<<" address "<<addr
				<<" time "<<time
				<<" pos.x "<<pos.x
				<<" pox.y "<<pos.y
				<<" vel.x "<<vel.x
				<<" vel.y "<<vel.y
				<<" neighbors.size "<<neighbors.size());
	for (std::vector<int>::size_type i = 0; i != neighbors.size(); ++i){
		NS_LOG_LOGIC("neighbor "<<i<<" is switchID "<<neighbors.at(i));
	}

	m_switches[switchID].SetIp(addr);
	m_switches[switchID].SetTime(time);
	m_switches[switchID].SetPos(pos);
	m_switches[switchID].SetVel(vel);
	for (int i=0; i<m_switchNum; i++){
		if (i != switchID){
			m_connection[switchID][i] = false;
			m_connection[i][switchID] = false;
		}
	}
	for (std::vector<int>::size_type i = 0; i != neighbors.size(); ++i){
		int other = neighbors.at(i);
		m_connection.at(switchID).at(other) = true;
		m_connection.at(other).at(switchID) = true;
	}
	//ShowTopology();
}

void
Topology::ShowTopology()
{
	NS_LOG_FUNCTION(this);
	NS_LOG_LOGIC("Network State is ");
	m_mcRouting.UpdateTopology(m_switches,m_connection);
	double curTime = Simulator::Now().GetSeconds();
	for (int i=0;i<m_switchNum;i++)
	{
		NS_LOG_LOGIC(m_switches[i].GetId()<<" "<<m_switches[i].GetIp()<<" "<<m_switches[i].GetTime()<<" "<<m_switches[i].GetPos().x<<" "<<m_switches[i].GetPos().y<<" "<<m_switches[i].GetVel().x<<" "<<m_switches[i].GetVel().y);
		m_of<<m_switches[i].GetId()<<" "<<curTime<<" "<<m_switches[i].GetPos().x<<" "<<m_switches[i].GetPos().y<<'\n';
	}
	NS_LOG_LOGIC("Network Topology is ");
	for (int i=0;i<m_switchNum;i++)
	{
		for (int j=0;j<m_switchNum;j++)
		{
			NS_LOG_LOGIC(m_connection[i][j]);
			m_of<<m_connection[i][j];
		}
		m_of<<'\n';
	}
	Time nextTime = m_showTopoInterval;
	NS_LOG_LOGIC("next time is "<<nextTime.GetMilliSeconds());
	Simulator::Schedule (nextTime, & Topology::ShowTopology,this);
}

std::vector<std::vector<bool> >&
Topology::GetConnection(){
	return m_connection;
}

}
} /* namespace ns3 */
