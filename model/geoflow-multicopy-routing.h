/*
 * geoflow-topology.h
 *
 *  Created on: 2014年3月9日
 *      Author: sumoon
 */

#ifndef GEOFLOW_MULTICOPY_ROUTING_H_
#define GEOFLOW_MULTICOPY_ROUTING_H_

#include "geoflow-switch.h"
#include "geoflow-neighbors.h"
#include "geoflow-forward.h"
#include <vector>
#include "ns3/simulator.h"

namespace ns3 {
namespace geoflow{

class MulticopyRouting {
public:
	MulticopyRouting();
	virtual ~MulticopyRouting();

	void UpdateTopology(std::vector<Switch> cars, std::vector <std::vector<bool> > con);
	void RouteMessage();

	void EpidemicRouting();
	void SprayAndWaitRouting();
	void MobEyesRouting();
	void SprayAndFocusRouting();
	void SprayAndAllPreyRouting();
	void SprayAndPartialPreyRouting();
	void SprayAndRoundPreyRouting();
	void SprayAndRandomRouting();
	void SprayAndPredictFocusRouting();
	void GlobalSprayAndFocusRouting();
	double QualityComputation(int curID, int destID);
	double MinimumDistance (int srcID, int dstID, std::vector<Switch> cars);
	bool CheckCarHasCopy(int carID, std::vector<int> copyCars);

	void setSid(int sid) {
		m_sid = sid;
	}

	void setDid(int did) {
		m_did = did;
	}

	void setCopyNum(int copyNum) {
		m_copyNum = copyNum;
	}

	void setRoutingType(int routingType) {
		m_routingType = routingType;
	}

	void setQualityType(int qualityType) {
		m_qualityType = qualityType;
	}

	void setAngleWeight(double angleWeight) {
		m_angleWeight = angleWeight;
	}

	void setDistWeight(double distWeight) {
		m_distWeight = distWeight;
	}

	bool isDelivered() const {
		return m_delivered;
	}

	double getDeliverTime() const {
		return m_deliverTime;
	}

	double getStoreCost() const {
		return m_storeCost;
	}

	double getForwardNum() const {
		return m_forwardNum;
	}

	const std::vector<int>& getCopyCars() const {
		return m_copyCars;
	}

	const std::vector<std::vector<Forward> >& getTrace() const {
		return m_trace;
	}

private:
	int m_sid; //source car id
	int m_did; //destination car id
	int m_carNum;
	int m_copyNum;// maximized copy number
	int m_forwardNum; //the number of forwarding
	int m_routingType;// routing strategy
	int m_qualityType;// quality computation strategy
	int m_predictHop;// how many hops you want to predict
	int m_hopNum;// how many hops a copy can be spread for MobEyes
	double m_cutPara;// cut parameter which describe when to cut a bad node
	double m_distWeight;// weight of distance to the target
	double m_angleWeight; //weight of angle to existing copies
	bool m_delivered; // flag that the message has been delivered to the destination
	double m_deliverTime; // the time in seconds that the message arrive at destination
	double m_range; //communication range
	double m_storeCost; // store cost is copy number * last time
	double m_ttl; // time to live
	double m_bttl; // time to live of broadcast message
	std::vector<Switch> m_cars;
	std::vector< std::vector<bool> > m_con;
	std::vector<int> m_copyCars;
	std::vector<int> m_broadcastCars; // used to broadcast messages such as the arrival of one copy
	std::vector< std::vector<Forward> > m_trace; //used to record copy passing trace

};

}
} /* namespace ns3 */

#endif /* GEOFLOW_MULTICOPY_ROUTING_H_ */
