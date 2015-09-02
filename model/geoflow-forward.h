/*
 * geoflow-forward.h
 *
 *  Created on: 2014年12月19日
 *      Author: sumoon
 */

#ifndef GEOFLOW_FORWARD_H_
#define GEOFLOW_FORWARD_H_

namespace ns3 {

class Forward {
public:
	Forward();
	virtual ~Forward();

	double getForwardTime() const {
		return m_forwardTime;
	}

	void setForwardTime(double forwardTime) {
		m_forwardTime = forwardTime;
	}

	int getFromId() const {
		return m_fromID;
	}

	void setFromId(int fromId) {
		m_fromID = fromId;
	}

private:
	int m_fromID;
	double m_forwardTime;
};

} /* namespace ns3 */

#endif /* GEOFLOW_FORWARD_H_ */
