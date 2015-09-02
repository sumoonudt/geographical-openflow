/*
 * geoflow-header.cc
 *
 *  Created on: 2014年1月21日
 *      Author: sumoon
 */

#include "geoflow-header.h"

NS_LOG_COMPONENT_DEFINE ("GeoFlowHeader");

namespace ns3 {
namespace geoflow {

TypeHeader::TypeHeader()
{

}

TypeHeader::~TypeHeader() {

}

TypeId
TypeHeader::GetTypeId ()
{
	static TypeId tid = TypeId ("ns3::geoflow::TypeHeader")
		.SetParent<Header> ()
		.AddConstructor<TypeHeader> ()
		;
	return tid;
}

TypeId
TypeHeader::GetInstanceTypeId () const
{
	return GetTypeId ();
}

uint32_t
TypeHeader::GetSerializedSize () const
{
	return 1;
}

void
TypeHeader::Serialize (Buffer::Iterator i) const
{
	i.WriteU8 ((uint8_t) m_type);
}

uint32_t
TypeHeader::Deserialize (Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;
	uint8_t type = i.ReadU8 ();
	m_valid = true;
	switch (type)
	{
		case GEOFLOW_TYPE_HELLO:	//Single Cast Request
		case GEOFLOW_TYPE_SCAST:	//Single Cast Request
		case GEOFLOW_TYPE_MCREQ:	//Single Cast Request
		case GEOFLOW_TYPE_MCREP:	//Single Cast Reply
		case GEOFLOW_TYPE_MCACK:	//Single Cast ACK
		case GEOFLOW_TYPE_BCREQ:	//Broad Cast Request
		{
			m_type = (PacketType) type;
			break;
		}
		default:
			m_valid = false;
	}
	uint32_t dist = i.GetDistanceFrom (start);
	NS_ASSERT (dist == GetSerializedSize ());
	return dist;
}

void
TypeHeader::Print (std::ostream &os) const
{
	os << m_type << m_valid;
}


HelloHeader::HelloHeader() {

}

HelloHeader::~HelloHeader() {

}

TypeId
HelloHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::geoflow::HelloHeader")
    .SetParent<Header> ()
    .AddConstructor<HelloHeader> ()
  ;
  return tid;
}

TypeId
HelloHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
HelloHeader::GetSerializedSize () const
{
  return 45;
}

void
HelloHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8((uint8_t) m_nodeId);
  i.WriteU32((uint32_t) m_ip.Get());
  i.WriteU64((uint64_t) m_coord.x);
  i.WriteU64((uint64_t) m_coord.y);
  i.WriteU64((uint64_t) m_vel.x);
  i.WriteU64((uint64_t) m_vel.y);
  i.WriteU64((uint64_t) m_time.GetDouble());
}

uint32_t
HelloHeader::Deserialize (Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;
	m_valid = true;
	uint8_t nodeId = i.ReadU8();
	m_nodeId = nodeId;
	uint32_t ip = i.ReadU32();
	m_ip = (Ipv4Address) ip;
	uint64_t coordX = i.ReadU64();
	m_coord.x = (double) coordX;
	uint64_t coordY = i.ReadU64();
	m_coord.y = (double) coordY;
	uint64_t velX = i.ReadU64();
	m_vel.x = (double) velX;
	uint64_t velY = i.ReadU64();
	m_vel.y = (double) velY;
	uint64_t time = i.ReadU64();
	m_time = (Time) time;

	uint32_t dist = i.GetDistanceFrom (start);
	NS_ASSERT (dist == GetSerializedSize ());
	return dist;
}

void
HelloHeader::Print (std::ostream &os) const
{
	os << m_nodeId << m_ip << m_coord << m_vel << m_time;
}

SCastHeader::SCastHeader() {

}

SCastHeader::~SCastHeader() {

}

TypeId
SCastHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::geoflow::SCastHeader")
    .SetParent<Header> ()
    .AddConstructor<SCastHeader> ()
  ;
  return tid;
}

TypeId
SCastHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
SCastHeader::GetSerializedSize () const
{
  return 24;
}

void
SCastHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU32((uint32_t) m_srcIp.Get());
  i.WriteU32((uint32_t) m_dstIp.Get());
  i.WriteU64((uint64_t) m_direction);
  i.WriteU64((uint64_t) m_timeout.GetDouble());
}

uint32_t
SCastHeader::Deserialize (Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;
	m_valid = true;
	uint32_t srcIp = i.ReadU32();
	m_srcIp = (Ipv4Address)srcIp;
	uint32_t dstIp = i.ReadU32();
	m_dstIp = (Ipv4Address)dstIp;
	uint64_t direction = i.ReadU64();
	m_direction = (double) direction;
	uint64_t timeout = i.ReadU64();
	m_timeout = (Time) timeout;

	uint32_t dist = i.GetDistanceFrom (start);
	NS_ASSERT (dist == GetSerializedSize ());
	return dist;
}

void
SCastHeader::Print (std::ostream &os) const
{
	os << m_srcIp << m_dstIp << m_direction << m_timeout;
}

}
} /* namespace ns3 */
