/*
 * geoflow-topology.h
 *
 *  Created on: 2014年3月9日
 *      Author: sumoon
 */

#ifndef GEOFLOW_TOPOLOGY_H_
#define GEOFLOW_TOPOLOGY_H_

#include "geoflow-switch.h"
#include "geoflow-neighbors.h"
#include "geoflow-multicopy-routing.h"
#include <vector>
#include "ns3/simulator.h"

namespace ns3 {
namespace geoflow{

class Topology {
public:
	Topology(int switchNum);
	virtual ~Topology();

	void SetUpTopology();
	void SetUpMultiCopyRouting(int sid, int did, int copyNum, int routingType, int qualityType);

	std::vector<Switch>& GetTopology();
	Switch& GetSwitch(int switchID);
	int getSwitchNum();

	void UpdateTopology(int switchID, Ipv4Address addr, Time time, Vector pos, Vector vel, std::vector<int> neighbors);
	void ShowTopology();

	std::vector<std::vector<bool> >& GetConnection();

private:
	int m_switchNum;
	std::vector<Switch> m_switches;
	std::vector< std::vector<bool> > m_connection;
	MulticopyRouting m_mcRouting;

	std::ofstream m_of;
	Time m_showTopoInterval;
};

}
} /* namespace ns3 */

#endif /* GEOFLOW_TOPOLOGY_H_ */
