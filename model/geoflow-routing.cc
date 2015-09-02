/*
 * geoflow-application-routing.cc
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#include "geoflow-routing.h"
#include "math.h"

# define M_PI		3.14159265358979323846

NS_LOG_COMPONENT_DEFINE ("GeoFlowRouting");

namespace ns3 {
namespace geoflow {

Routing::Routing(Topology * topology) :
		m_topology(topology),
		m_routeType(ROUTE_TYPE_HOP_BASED),
		//m_routeType(ROUTE_TYPE_GEO_BASED),
		m_radius(150),
		m_priority(1),
		m_timeout(MilliSeconds(200))
{
	// TODO Auto-generated constructor stub

}

Routing::~Routing() {
	// TODO Auto-generated destructor stub
}

void
Routing::SetTopology(Topology*& topology) {
	m_topology = topology;
}

void
Routing::SetRadius(double radius) {
	m_radius = radius;
}

void
Routing::SetPriority(int priority) {
	m_priority = priority;
}

void
Routing::SetTimeout(const Time& timeout) {
	m_timeout = timeout;
}

Message
Routing::PacketIn (Message message){
	NS_LOG_FUNCTION(this);
	Message reply;

	if (message.GetType() == MESSAGE_TYPE_ROUTE_REQUEST){
		if (m_routeType == ROUTE_TYPE_HOP_BASED){
			MessagePara para = RoutingAlgorithmHop (message);
			reply.SetType(MESSAGE_TYPE_ROUTE_REPLY);
			reply.SetPara(para);
		}
		if (m_routeType == ROUTE_TYPE_GEO_BASED){
			MessagePara para = RoutingAlgorithmGeo (message);
			reply.SetType(MESSAGE_TYPE_ROUTE_REPLY);
			reply.SetPara(para);
		}
	}

	return reply;
}

MessagePara
Routing::RoutingAlgorithmHop (Message message){
	NS_LOG_FUNCTION(this);
	MessagePara para;
	int srcID = ConvertIP2ID(message.GetPara().srcIpAddr);
	int dstID = ConvertIP2ID(message.GetPara().dstIpAddr);
	if (srcID < 0 || dstID < 0){
		para.valid = false;
		return para;
	}
	int curID = message.GetPara().nodeID;
	int nextHopID = ComputeRouteHop(curID,dstID);

	m_topology->ShowTopology();
	if(!(m_topology->GetConnection())[curID][nextHopID]){
		nextHopID = -1;
	}
	para = GenerateMessageHop(srcID, dstID, nextHopID);

	return para;
}

int
Routing::ConvertIP2ID (Ipv4Address ip){
	NS_LOG_FUNCTION(this);
	int id = -1;
	NS_LOG_LOGIC("ip is "<<ip);
	for (int i=0; i<m_topology->getSwitchNum(); i++)
	{
		NS_LOG_LOGIC("switch "<<i<<"ip is "<<m_topology->GetSwitch(i).GetIp());
		if (m_topology->GetSwitch(i).GetIp() == ip){
			id = i;
			break;
		}
	}
	return id;
}

int
Routing::ComputeRouteHop (int src, int dst)
{
	NS_LOG_FUNCTION(this);
	int maxint = 1000000;

	int num = m_topology->getSwitchNum();

	double map [num][num];

	for (int i=0; i<num; i++)
	{
		for (int j=0; j<num; j++)
		{
			//map[i][j] = ComputeMinimumOptimisticTime(i,j);
			/*
			double px1 = m_topology->GetSwitch(i).GetPos().x;	double py1 = m_topology->GetSwitch(i).GetPos().y;
			double px2 = m_topology->GetSwitch(j).GetPos().x;	double py2 = m_topology->GetSwitch(j).GetPos().y;
			double d = sqrt( pow((px1 - px2),2) + pow((py1 - py2),2) );
			*/
			if (m_topology->GetConnection()[i][j]){
				map[i][j] = 1;
			} else {
				map[i][j] = maxint;//floor((d-m_radius)/m_radius)+1;
			}
		}
	}

	double dist [num];
	int used [num], pre [num];
	for (int i=0; i<num; i++)
	{
		dist[i] = map[src][i];
		used[i] = 0;
		pre [i] = src;
	}
	used[src] = 1;
	pre [src] = -1;

	for (int i=0; i<num-1; i++)
	{
		int k = 0;

		int tmin = maxint + 1;
		for (int j=0; j<num; j++)
		{
			if( !used[j] && tmin > dist[j] )
			{
				tmin = dist[j];
				k = j;
			}
		}

		used[k] = 1;
		for (int j=0; j<num; j++)
		{
			if( dist[k] + map[k][j] < dist[j] )
			{
				dist[j] = dist[k] + map[k][j];
				pre[j] = k;
			}
		}
	}

	int cur = dst;

	while (true)
	{
		if (cur == src || pre[cur] == src)
			break;
		else
			cur = pre[cur];
	}

	return cur;
}

double
Routing::ComputeMinimumOptimisticTime (int n1, int n2)
{
	NS_LOG_FUNCTION(this);
	double px1 = m_topology->GetSwitch(n1).GetPos().x;	double py1 = m_topology->GetSwitch(n1).GetPos().y;
	double px2 = m_topology->GetSwitch(n2).GetPos().x;	double py2 = m_topology->GetSwitch(n2).GetPos().y;
	double vx1 = m_topology->GetSwitch(n1).GetVel().x;	double vy1 = m_topology->GetSwitch(n1).GetVel().y;
	double vx2 = m_topology->GetSwitch(n2).GetVel().x;	double vy2 = m_topology->GetSwitch(n2).GetVel().y;

	double vxSum = vx1 - vx2;	double vySum = vy1 - vy2;	double speedSum = sqrt(pow(vxSum,2) + pow(vySum,2));
	double pxSum = px2 - px1;	double pySum = py2 - py1;	double distSum = sqrt(pow(pxSum,2) + pow(pySum,2));

	double vAngle = (vxSum==0)?((vySum>0)?(M_PI/2):(-1*M_PI/2)):atan(vySum/vxSum);
	double pAngle = (pxSum==0)?((pySum>0)?(M_PI/2):(-1*M_PI/2)):atan(pySum/pxSum);

	double v2pAngle = vAngle - pAngle;
	double v2pSpeed = speedSum * cos(v2pAngle);
	//double v2pCrossSpeed = speedSum * sin(v2pAngle);
	//v2pSpeed = v2pSpeed * abs(v2pSpeed/(abs(v2pSpeed) + abs(v2pCrossSpeed)));
	double time = (v2pSpeed==0)?-1:abs(distSum - m_radius)/v2pSpeed;

	if (m_topology->GetConnection()[n1][n2] && distSum <= m_radius){
		return 0;
	} else {
		if (time < 0){
			return 100000;
		} else {
			return time;
		}
	}
}

MessagePara
Routing::GenerateMessageHop(int src, int dst, int nextHopID){
	NS_LOG_FUNCTION(this);
	MessagePara para;

	para.valid = false;
	para.srcIpAddr = m_topology->GetSwitch(src).GetIp();
	para.dstIpAddr = m_topology->GetSwitch(dst).GetIp();
	para.timeout = m_timeout;
	para.routeType = ROUTE_TYPE_HOP_BASED;

	Ipv4Address nextHopIp = Ipv4Address("1.1.1.1");
	if (nextHopID != -1){
		nextHopIp = m_topology->GetSwitch(nextHopID).GetIp();
		para.valid = true;
	}

	para.nextIpAddr = nextHopIp;

	return para;
}

MessagePara
Routing::RoutingAlgorithmGeo (Message message){
	NS_LOG_FUNCTION(this);
	MessagePara para;
	int srcID = ConvertIP2ID(message.GetPara().srcIpAddr);
	int dstID = ConvertIP2ID(message.GetPara().dstIpAddr);
	if (srcID < 0 || dstID < 0){
		para.valid = false;
		return para;
	}
	double direction = ComputeRouteGeo(srcID,dstID);
	para = GenerateMessageGeo(srcID, dstID, direction);

	return para;
}

double
Routing::ComputeRouteGeo (int src, int dst)
{
	NS_LOG_FUNCTION(this);
	double px1 = m_topology->GetSwitch(src).GetPos().x;	double py1 = m_topology->GetSwitch(src).GetPos().y;
	double px2 = m_topology->GetSwitch(dst).GetPos().x;	double py2 = m_topology->GetSwitch(dst).GetPos().y;

	double diffx = px2 - px1;
	double diffy = py2 - py1;

	double direction;
	if(diffx != 0){
		direction = atan(diffy/diffx);
		if(diffx < 0){
			direction += M_PI;
		}
	}else{
		if(diffy >= 0){
			direction = M_PI/2;
		}else{
			direction = -M_PI/2;
		}
	}
	direction += M_PI/2;

	if(direction >= M_PI*15/8 or direction < M_PI/8){
		direction = 0;
	}
	if(direction >= M_PI/8 and direction < M_PI*3/8){
		direction = M_PI*2/8;
	}
	if(direction >= M_PI*3/8 and direction < M_PI*5/8){
		direction = M_PI*4/8;
	}
	if(direction >= M_PI*5/8 and direction < M_PI*7/8){
		direction = M_PI*6/8;
	}
	if(direction >= M_PI*7/8 and direction < M_PI*9/8){
		direction = M_PI;
	}
	if(direction >= M_PI*9/8 and direction < M_PI*11/8){
		direction = M_PI*10/8;
	}
	if(direction >= M_PI*11/8 and direction < M_PI*13/8){
		direction = M_PI*12/8;
	}
	if(direction >= M_PI*13/8 and direction < M_PI*15/8){
		direction = M_PI*14/8;
	}

	return direction;
}

MessagePara
Routing::GenerateMessageGeo(int src, int dst, double direction){
	NS_LOG_FUNCTION(this);
	MessagePara para;

	para.srcIpAddr = m_topology->GetSwitch(src).GetIp();
	para.dstIpAddr = m_topology->GetSwitch(dst).GetIp();
	para.direction = direction;
	para.timeout = m_timeout;
	para.routeType = ROUTE_TYPE_GEO_BASED;
	para.valid = true;

	return para;
}

} /* namespace geoflow */
} /* namespace ns3 */
