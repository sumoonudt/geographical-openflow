/*
 * geoflow-flowtable.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_FLOWTABLE_H_
#define GEOFLOW_FLOWTABLE_H_

#include <vector>

#include "ns3/geoflow-flowentry.h"
#include "ns3/packet.h"
#include "ns3/internet-module.h"
#include "ns3/event-id.h"

namespace ns3 {
namespace geoflow {

class FlowTable {
public:
	FlowTable(Time checkInterval);
	virtual ~FlowTable();

	FlowEntry GetFlowEntry(Ipv4Address srcIp, Ipv4Address dstIp);

	MatchField GetMatchField(Ipv4Address srcIp, Ipv4Address dstIp);

	FlowEntry LookUpFlowEntry(MatchField match);

	bool InsertFlowEntry(FlowEntry flow);

	void DropFlowEntry(FlowEntry flow);

	void ProlongFlowEntry(FlowEntry flow);

	void CheckTable();

	void ShowTable();

private:
	std::vector<FlowEntry> m_flowtable;
	Time m_checkInterval;
	Time m_prolongTime;
};

} /* namespace geoflow */
} /* namespace ns3 */

#endif /* GEOFLOW_FLOWTABLE_H_ */
