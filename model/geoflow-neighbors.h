/*
 * geoflow-neighbors.h
 *
 *  Created on: 2014年4月23日
 *      Author: sumoon
 */

#ifndef GEOFLOW_NEIGHBORS_H_
#define GEOFLOW_NEIGHBORS_H_

#include "geoflow-switch.h"

namespace ns3 {
namespace geoflow{

class Neighbors {
public:
	Neighbors(Time checkInterval);
	virtual ~Neighbors();

	bool IsNeighbor(Ipv4Address ip);

	uint32_t NumOfNeighbors();

	Switch GetNeighborAt(uint32_t pos);

	std::vector<int> GetNeighbors();

	void UpdateNeighbors(Switch neighbor);

	void CheckNeighbors();

	void ShowNeighbors();

private:
	std::vector<Switch> m_neighbors;
	Time m_checkInterval;
};

}
} /* namespace ns3 */

#endif /* GEOFLOW_NEIGHBORS_H_ */
