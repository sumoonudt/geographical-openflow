/*
 * geoflow-header.h
 *
 *  Created on: 2014年1月21日
 *      Author: sumoon
 */

#ifndef GEOFLOW_HEADER_H_
#define GEOFLOW_HEADER_H_

#include "ns3/geoflow-flowentry.h"
#include "ns3/header.h"
#include "ns3/core-module.h"

namespace ns3 {
namespace geoflow {

enum PacketType
{
	GEOFLOW_TYPE_HELLO = 1,	//Hello
	GEOFLOW_TYPE_SCAST = 2,	//Single Cast
	GEOFLOW_TYPE_MCREQ = 3,	//Multiple Cast Request
	GEOFLOW_TYPE_MCREP = 4,	//Multiple Cast Reply
	GEOFLOW_TYPE_MCACK = 5,	//Multiple Cast ACK
	GEOFLOW_TYPE_BCREQ = 6,	//Broad Cast Request
};

class TypeHeader : public Header
{
public:
	TypeHeader();
	virtual ~TypeHeader();
	static TypeId GetTypeId ();
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (std::ostream &os) const;

	PacketType GetType() const {
		return m_type;
	}

	void SetType(PacketType type) {
		m_type = type;
	}

	bool IsValid() const {
		return m_valid;
	}

	void SetValid(bool valid) {
		m_valid = valid;
	}

private:
	PacketType m_type;
	bool m_valid;
};

class HelloHeader : public Header
{
public:
	HelloHeader();
	virtual ~HelloHeader();
	static TypeId GetTypeId ();
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (std::ostream &os) const;

	const Vector& GetCoord() const {
		return m_coord;
	}

	void SetCoord(const Vector& coord) {
		m_coord = coord;
	}

	const Time& GetTime() const {
		return m_time;
	}

	void SetTime(const Time& time) {
		m_time = time;
	}

	bool IsValid() const {
		return m_valid;
	}

	void SetValid(bool valid) {
		m_valid = valid;
	}

	Ipv4Address getIp() const {
		return m_ip;
	}

	void setIp(Ipv4Address ip) {
		m_ip = ip;
	}

	int getNodeId() const {
		return m_nodeId;
	}

	void setNodeId(int nodeId) {
		m_nodeId = nodeId;
	}

	const Vector& getVel() const {
		return m_vel;
	}

	void setVel(const Vector& vel) {
		m_vel = vel;
	}

private:
	bool m_valid;
	int m_nodeId;
	Ipv4Address m_ip;
	Vector m_coord;
	Vector m_vel;
	Time m_time;
};

class SCastHeader : public Header
{
public:
	SCastHeader();
	virtual ~SCastHeader();
	static TypeId GetTypeId ();
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (std::ostream &os) const;

	double GetDirection() const {
		return m_direction;
	}

	void SetDirection(const double direction) {
		m_direction = direction;
	}

	Ipv4Address GetDstIp() const {
		return m_dstIp;
	}

	void SetDstIp(Ipv4Address dstIp) {
		m_dstIp = dstIp;
	}

	Ipv4Address GetSrcIp() const {
		return m_srcIp;
	}

	void SetSrcIp(Ipv4Address srcIp) {
		m_srcIp = srcIp;
	}

	const Time& GetTimeout() const {
		return m_timeout;
	}

	void SetTimeout(const Time& timeout) {
		m_timeout = timeout;
	}

	bool IsValid() const {
		return m_valid;
	}

	void SetValid(bool valid) {
		m_valid = valid;
	}

private:
	bool m_valid;
	Ipv4Address m_srcIp;
	Ipv4Address m_dstIp;
	double m_direction;
	Time m_timeout;
};

}
} /* namespace ns3 */

#endif /* GEOFLOW_HEADER_H_ */
