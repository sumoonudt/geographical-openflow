/*
 * geoflow-application-routing.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_APPLICATION_ROUTING_H_
#define GEOFLOW_APPLICATION_ROUTING_H_

#include "ns3/geoflow-message.h"
#include "ns3/geoflow-topology.h"
#include "ns3/simulator.h"

namespace ns3 {
namespace geoflow {

class Routing : public Object
{
public:
	Routing(Topology * topology);
	~Routing();

	void SetTopology(Topology*& topology);
	void SetRadius(double radius);
	void SetPriority(int priority);
	void SetDirectDiff(double directDiff);
	void SetTimeout(const Time& timeout);

	Message PacketIn (Message message);

	MessagePara	RoutingAlgorithmHop (Message message);
	int ConvertIP2ID (Ipv4Address ip);
	int ComputeRouteHop (int src, int dst);
	double ComputeMinimumOptimisticTime (int n1, int n2);
	MessagePara GenerateMessageHop (int src, int dst, int nextHopID);

	MessagePara	RoutingAlgorithmGeo (Message message);
	double ComputeRouteGeo (int src, int dst);
	MessagePara GenerateMessageGeo (int src, int dst, double direction);


private:
	Topology * m_topology;

	RouteType m_routeType;
	double m_radius;
	int m_priority;
	Time m_timeout;
};

} /* namespace geoflow */
} /* namespace ns3 */

#endif /* GEOFLOW_APPLICATION_ROUTING_H_ */
