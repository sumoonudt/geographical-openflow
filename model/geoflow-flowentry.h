/*
 * geoflow-flowentry.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_FLOWENTRY_H_
#define GEOFLOW_FLOWENTRY_H_

#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/core-module.h"

namespace ns3 {
namespace geoflow {

class MatchField {
public:
	MatchField();
	virtual ~MatchField();

	bool operator== (MatchField const & o) const;

	Ipv4Address GetDstIpAddr() const {
		return dstIpAddr;
	}

	void SetDstIpAddr(Ipv4Address dstIpAddr) {
		this->dstIpAddr = dstIpAddr;
	}

	Ipv4Address GetSrcIpAddr() const {
		return srcIpAddr;
	}

	void SetSrcIpAddr(Ipv4Address srcIpAddr) {
		this->srcIpAddr = srcIpAddr;
	}

private:
	Ipv4Address srcIpAddr;
	Ipv4Address dstIpAddr;
};

enum ActionType
{
	ACTION_TYPE_DROP  = 1,   //Drop the packet
	ACTION_TYPE_FORWARD  = 2,   //Forward the packet
	ACTION_TYPE_UPWARD  = 3,   //My packet
	ACTION_TYPE_BROADCAST  = 4,   //Broadcast the packet
	ACTION_TYPE_GEOCAST = 5,	//Broadcast the packet in a direction
};

struct ActionPara
{
	Ipv4Address nextIpAddr;
	double direction;
};

class Action {
public:
	Action();
	virtual ~Action();

	ActionPara GetPara() const {
		return para;
	}

	void SetPara(ActionPara para) {
		this->para = para;
	}

	ActionType GetType() const {
		return type;
	}

	void SetType(ActionType type) {
		this->type = type;
	}

private:
	ActionType type;
	ActionPara para;
};

class FlowEntry {
public:
	FlowEntry();
	virtual ~FlowEntry();

	const Action& GetAction() const {
		return action;
	}

	void SetAction(const Action& action) {
		this->action = action;
	}

	int GetCounter() const {
		return counter;
	}

	void SetCounter(int counter) {
		this->counter = counter;
	}

	const MatchField& GetMatch() const {
		return match;
	}

	void SetMatch(const MatchField& match) {
		this->match = match;
	}

	int GetPriority() const {
		return priority;
	}

	void SetPriority(int priority) {
		this->priority = priority;
	}

	const Time& GetTimeout() const {
		return timeout;
	}

	void SetTimeout(const Time& timeout) {
		this->timeout = timeout;
	}

	bool IsValid() const {
		return valid;
	}

	void SetValid(bool valid) {
		this->valid = valid;
	}

private:
	bool valid;
	MatchField match;
	int priority;
	int counter;
	Action action;
	Time timeout;
};

} /* namespace geoflow */
} /* namespace ns3 */
#endif /* GEOFLOW_FLOWENTRY_H_ */
