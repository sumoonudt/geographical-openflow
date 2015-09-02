/*
 * geoflow-switch.h
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#ifndef GEOFLOW_SWITCH_H_
#define GEOFLOW_SWITCH_H_

#include "ns3/ipv4-address.h"
#include "ns3/object-factory.h"
#include "ns3/nstime.h"
#include "ns3/core-module.h"

namespace ns3
{
namespace geoflow
{

class Switch
{
public:
	Switch();
	virtual ~Switch();

	const int GetId() const{
		return m_id;
	}

	void SetId(int id) {
		this->m_id = id;
	}

	const Ipv4Address GetIp() const{
		return m_ip;
	}

	void SetIp(Ipv4Address ip) {
		this->m_ip = ip;
	}

	const Vector& GetPos() const {
		return m_pos;
	}

	void SetPos(const Vector& pos) {
		m_pos = pos;
	}

	const Time& GetTime() const {
		return m_time;
	}

	void SetTime(const Time& time) {
		m_time = time;
	}

	const Vector& GetVel() const {
		return m_vel;
	}

	void SetVel(const Vector& vel) {
		m_vel = vel;
	}

private:
	int m_id;
	Ipv4Address m_ip;
	Time m_time;
	Vector m_pos;
	Vector m_vel;
};

}
}
#endif /* GEOFLOW_SWITCH_H_ */
