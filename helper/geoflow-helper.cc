/*
 * geoflow-helper.cc
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#include "geoflow-helper.h"
#include "ns3/geoflow-module.h"
#include "ns3/ptr.h"
#include <iostream>

namespace ns3
{
	GeoFlowHelper::GeoFlowHelper(ns3::geoflow::Controller * controller) :
	Ipv4RoutingHelper ()
	{
		m_agentFactory.SetTypeId ("ns3::geoflow::Protocol");
		m_controller = controller;
	}

	GeoFlowHelper*
	GeoFlowHelper::Copy (void) const
	{
		return new GeoFlowHelper (*this);
	}

	Ptr<Ipv4RoutingProtocol>
	GeoFlowHelper::Create (Ptr<Node> node) const
	{
		Ptr<geoflow::Protocol> protocol = m_agentFactory.Create<geoflow::Protocol> ();
		protocol->SetUp(node,m_controller);
		node->AggregateObject (protocol);
		return protocol;
	}
}
