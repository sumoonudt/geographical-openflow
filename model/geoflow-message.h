/*
 * geoflow-message.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_MESSAGE_H_
#define GEOFLOW_MESSAGE_H_

#include "geoflow-flowentry.h"
#include "geoflow-neighbors.h"
#include "ns3/core-module.h"
#include "ns3/nstime.h"
#include "ns3/ipv4-address.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"

namespace ns3 {
namespace geoflow {

enum MessageType
{
	MESSAGE_TYPE_STATE_UPDATE  = 1,   //Update node state
	MESSAGE_TYPE_STATE_REPLY = 2,	  //Reply node neighbors
	MESSAGE_TYPE_ROUTE_REQUEST  = 3,  //Request a route
	MESSAGE_TYPE_ROUTE_REPLY  = 4,    //Reply a route
};

enum RouteType
{
	ROUTE_TYPE_HOP_BASED = 1,
	ROUTE_TYPE_GEO_BASED = 2,
};

struct MessagePara
{
	int nodeID;
	Vector nodePos;
	Vector nodeVel;
	Time curTime;
	Ipv4Address nodeIpAddr;
	Ipv4Address srcIpAddr;
	Ipv4Address dstIpAddr;
	std::vector<int> neighbors;
	RouteType routeType;
	Time timeout;
	double direction;
	Ipv4Address nextIpAddr;
	bool valid;
};

class Message {
public:
	Message();
	virtual ~Message();

	const MessagePara& GetPara() const {
		return para;
	}

	void SetPara(const MessagePara& para) {
		this->para = para;
	}

	MessageType GetType() const {
		return type;
	}

	void SetType(MessageType type) {
		this->type = type;
	}

	uint32_t GetSize(){
		uint32_t size = 0;
		if (type == MESSAGE_TYPE_STATE_UPDATE){
			size = 53 + para.neighbors.size();
		}
		if (type == MESSAGE_TYPE_STATE_REPLY){
			size = 0;
		}
		if (type == MESSAGE_TYPE_ROUTE_REQUEST){
			size = 53;
		}
		if (type == MESSAGE_TYPE_ROUTE_REPLY){
			if (para.routeType == ROUTE_TYPE_HOP_BASED){
				size = 22;
			}
			if (para.routeType == ROUTE_TYPE_GEO_BASED){
				size = 26;
			}
		}
		return size;
	}

private:
	MessageType type;
	MessagePara para;
};

} /* namespace geoflow */
} /* namespace ns3 */

#endif /* GEOFLOW_MESSAGE_H_ */
