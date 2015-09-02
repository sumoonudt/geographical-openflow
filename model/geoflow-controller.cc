/*
 * geoflow-controller.cc
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#include "geoflow-controller.h"

NS_LOG_COMPONENT_DEFINE ("GeoFlowController");

namespace ns3
{
namespace geoflow
{

Controller::Controller (int switchNum) :
		m_topology(switchNum),
		m_routing(& m_topology)
{
}

Controller::~Controller()
{
}

Message
Controller::ReceiveMessage(Message message){
	NS_LOG_FUNCTION(this);
	Message reply;

	if (message.GetType() == MESSAGE_TYPE_STATE_UPDATE){
		m_topology.UpdateTopology(message.GetPara().nodeID,
					 message.GetPara().nodeIpAddr,
					 message.GetPara().curTime,
					 message.GetPara().nodePos,
					 message.GetPara().nodeVel,
					 message.GetPara().neighbors);
	}

	if (message.GetType() == MESSAGE_TYPE_ROUTE_REQUEST){
		reply =  m_routing.PacketIn(message);
	}

	return reply;
}

}
}
