/*
 * geoflow-agent.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_AGENT_H_
#define GEOFLOW_AGENT_H_

#include "ns3/geoflow-flowtable.h"
#include "ns3/geoflow-controller.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/ipv4-address.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"

namespace ns3 {
namespace geoflow {

class Agent {
public:
	Agent();
	virtual ~Agent();

private:

	std::vector<Switch> m_neighbors;

};

} /* namespace geoflow */
} /* namespace ns3 */

#endif /* GEOFLOW_AGENT_H_ */
