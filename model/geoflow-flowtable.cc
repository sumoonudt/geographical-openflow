/*
 * geoflow-flowtable.cc
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#include "geoflow-flowtable.h"

NS_LOG_COMPONENT_DEFINE ("GeoFlowTable");

namespace ns3 {
namespace geoflow {

FlowTable::FlowTable(Time checkInterval) :
		m_checkInterval (checkInterval),
		m_prolongTime (checkInterval)
{
}

FlowTable::~FlowTable() {
	// TODO Auto-generated destructor stub
}

FlowEntry
FlowTable::GetFlowEntry(Ipv4Address srcIp, Ipv4Address dstIp){
	NS_LOG_FUNCTION(this);
	MatchField match = GetMatchField(srcIp, dstIp);
	FlowEntry entry = LookUpFlowEntry(match);
	return entry;
}

MatchField
FlowTable::GetMatchField(Ipv4Address srcIp, Ipv4Address dstIp){
	NS_LOG_FUNCTION(this);
	MatchField match;
	match.SetSrcIpAddr(srcIp);
	match.SetDstIpAddr(dstIp);
	return match;
}

FlowEntry
FlowTable::LookUpFlowEntry(MatchField match){
	NS_LOG_FUNCTION(this);
	NS_LOG_LOGIC("Begin looking up flow entry for srcIP "<<match.GetSrcIpAddr()<<" and dstIP "<<match.GetDstIpAddr());

	FlowEntry flow;
	flow.SetValid(false);

	for(std::vector<FlowEntry>::size_type i = 0; i != m_flowtable.size(); ++i){
		if (m_flowtable[i].GetMatch() == match) {
			flow.SetMatch(m_flowtable[i].GetMatch());
			flow.SetPriority(m_flowtable[i].GetPriority());
			flow.SetCounter(m_flowtable[i].GetCounter());
			flow.SetAction(m_flowtable[i].GetAction());
			flow.SetTimeout(m_flowtable[i].GetTimeout());
			flow.SetValid(m_flowtable[i].IsValid());

			m_flowtable[i].SetCounter(m_flowtable[i].GetCounter()+1);

			break;
		}
	}
	NS_LOG_LOGIC("End looking up flow entry with result: "
			<<"MatchField( srcIP "<<flow.GetMatch().GetSrcIpAddr()
			<<" dstIP "<<flow.GetMatch().GetDstIpAddr()
			<<"); Priority "<<flow.GetPriority()
			<<"; Counter "<<flow.GetCounter()
			<<"; Action( nextIP "<<flow.GetAction().GetPara().nextIpAddr
			<<"; Direction "<<flow.GetAction().GetPara().direction
			<<"; Timeout "<<flow.GetTimeout()
			<<"); Valid "<<flow.IsValid());

	return flow;
}

bool
FlowTable::InsertFlowEntry(FlowEntry flow){
	NS_LOG_FUNCTION(this);
	NS_LOG_LOGIC("Begin inserting a new flow entry that is: "
				<<"MatchField( srcIP "<<flow.GetMatch().GetSrcIpAddr()
				<<" dstIP "<<flow.GetMatch().GetDstIpAddr()
				<<"); Priority "<<flow.GetPriority()
				<<"; Counter "<<flow.GetCounter()
				<<"; Action( nextIP "<<flow.GetAction().GetPara().nextIpAddr
				<<"; Direction "<<flow.GetAction().GetPara().direction
				<<"; Timeout "<<flow.GetTimeout()
				<<"); Valid "<<flow.IsValid());
	m_flowtable.push_back(flow);
	NS_LOG_LOGIC("End inserting flow entry");
	ShowTable();
	return true;
}

void
FlowTable::DropFlowEntry(FlowEntry flow){
	NS_LOG_FUNCTION(this);
	MatchField match = flow.GetMatch();
	std::vector<FlowEntry>::iterator pos = m_flowtable.begin();
	for(std::vector<FlowEntry>::size_type i = 0; i != m_flowtable.size();){
		if (m_flowtable[i].GetMatch() == match){
			pos = m_flowtable.erase(pos);
		} else {
			pos++;
			i++;
		}
	}
}

void
FlowTable::ProlongFlowEntry(FlowEntry flow){
	NS_LOG_FUNCTION(this);
	MatchField match = flow.GetMatch();
	for(std::vector<FlowEntry>::size_type i = 0; i != m_flowtable.size();i++){
		if (m_flowtable[i].GetMatch() == match){
			Time timeout = m_flowtable[i].GetTimeout();
			m_flowtable[i].SetTimeout(MilliSeconds (timeout.GetMilliSeconds() + m_prolongTime.GetMilliSeconds()));
		}
	}
}

void
FlowTable::CheckTable(){
	NS_LOG_FUNCTION(this);
	ShowTable();
	NS_LOG_LOGIC("Begin checking flow table");
	std::vector<FlowEntry>::iterator pos = m_flowtable.begin();
	for(std::vector<FlowEntry>::size_type i = 0; i != m_flowtable.size();){
		Time timeout = m_flowtable[i].GetTimeout();
		NS_LOG_LOGIC("timeout is "<<timeout.GetMilliSeconds()<<" checkInterval is "<<m_checkInterval.GetMilliSeconds());
		if (timeout.GetMilliSeconds() <= m_checkInterval.GetMilliSeconds()){
			pos = m_flowtable.erase(pos);
		} else {
			m_flowtable[i].SetTimeout(MilliSeconds (timeout.GetMilliSeconds() - m_checkInterval.GetMilliSeconds()));
			pos++;
			i++;
		}
	}
	NS_LOG_LOGIC("End checking flow table");
	ShowTable();
}

void
FlowTable::ShowTable(){
	NS_LOG_FUNCTION(this);
	NS_LOG_LOGIC("Show Flow Table");
	for(std::vector<FlowEntry>::size_type i = 0; i != m_flowtable.size(); ++i){
		NS_LOG_LOGIC("FlowEntry "<<i<<": "
				<<"MatchField( srcIP "<<m_flowtable[i].GetMatch().GetSrcIpAddr()
				<<" dstIP "<<m_flowtable[i].GetMatch().GetDstIpAddr()
				<<"); Priority "<<m_flowtable[i].GetPriority()
				<<"; Counter "<<m_flowtable[i].GetCounter()
				<<"; Action( nextIP "<<m_flowtable[i].GetAction().GetPara().nextIpAddr
				<<"; Direction "<<m_flowtable[i].GetAction().GetPara().direction
				<<"; Timeout "<<m_flowtable[i].GetTimeout()
				<<"); Valid "<<m_flowtable[i].IsValid());
	}
}

} /* namespace geoflow */
} /* namespace ns3 */
