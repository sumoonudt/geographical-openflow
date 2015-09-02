/*
 * geoflow-helper.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_HELPER_H
#define GEOFLOW_HELPER_H

#include "ns3/geoflow-module.h"
#include "ns3/geoflow-message.h"
#include "ns3/object-factory.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/ptr.h"

namespace ns3
{
	class GeoFlowHelper : public Ipv4RoutingHelper
	{
	public:
		GeoFlowHelper(ns3::geoflow::Controller * controller);

		GeoFlowHelper* Copy (void) const;

		virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;

	private:
		ObjectFactory m_agentFactory;
		ns3::geoflow::Controller * m_controller;
	};
}

#endif
