/*
 * geoflow-controller.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_CONTROLLER_H
#define GEOFLOW_CONTROLLER_H

#include <vector>

#include "ns3/geoflow-routing.h"
#include "ns3/geoflow-message.h"
#include "ns3/geoflow-switch.h"
#include "ns3/geoflow-topology.h"
#include "ns3/nstime.h"
#include "ns3/core-module.h"

namespace ns3
{
namespace geoflow
{

class Controller : public Object
{
public:

	Controller(int switchNum);

	~Controller();

	Message ReceiveMessage(Message message);

private:
	Topology m_topology;
	Routing m_routing;
};

}
}

#endif
