/*
 * goeflow-topology.cpp
 *
 *  Created on: 2014年3月9日
 *      Author: sumoon
 */

#include "geoflow-multicopy-routing.h"

# define M_PI		3.14159265358979323846

NS_LOG_COMPONENT_DEFINE ("GeoFlowMulticopyRouting");

namespace ns3 {
namespace geoflow{

MulticopyRouting::MulticopyRouting() {
	// TODO Auto-generated constructor stub
	m_sid = 0;
	m_did = 10;
	m_copyNum = 3;
	m_forwardNum = 0;
	m_routingType = 3;
	m_qualityType = 2;
	m_predictHop = 0;
	m_hopNum = 3;
	m_cutPara = 0.5;
	m_distWeight = 0.5;
	m_angleWeight = 0.5;
	m_delivered = false;
	m_deliverTime = -1;
	m_range = 250;
	m_storeCost = 0;
	m_ttl = 100;
	m_bttl = 100;
}

MulticopyRouting::~MulticopyRouting() {

}

void
MulticopyRouting::UpdateTopology(std::vector<Switch> cars, std::vector <std::vector<bool> > con) {
	m_cars = cars;
	m_con = con;
	m_carNum = m_cars.size();
	if (m_trace.size()==0){
		for (int i=0; i<m_carNum; i++){
			std::vector<Forward> fw;
			m_trace.push_back(fw);
		}
	}
	RouteMessage();
}

void
MulticopyRouting::RouteMessage(){
	if (m_routingType == 0){
		EpidemicRouting();
	}
	if (m_routingType == 1){
		SprayAndWaitRouting();
	}
	if (m_routingType == 2){
		MobEyesRouting();
	}
	if (m_routingType == 3){
		SprayAndFocusRouting();
	}
	if (m_routingType == 4){
		SprayAndAllPreyRouting();
	}
	if (m_routingType == 5){
		SprayAndPartialPreyRouting();
	}
	if (m_routingType == 6){
		SprayAndRoundPreyRouting();
	}
	if (m_routingType == 7){
		SprayAndRandomRouting();
	}
	if (m_routingType == 8){
		SprayAndPredictFocusRouting();
	}
	if (m_routingType == 9){
		GlobalSprayAndFocusRouting();
	}
}

void
MulticopyRouting::EpidemicRouting(){
	/*m_storeCost += (int)m_copyCars.size() * 0.2;
	//NS_LOG_LOGIC(m_storeCost);
	double elapseTime = Simulator::Now().GetSeconds();
	if (elapseTime > m_ttl){
		m_copyCars.clear();
		return;
	}*/

	if (m_copyCars.size() == 0){
		m_copyCars.push_back(m_sid);
	}
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			break;
		}

//		if(m_copyCars.at(i) == m_did){
//			m_deliverTime = Simulator::Now().GetSeconds();
//			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "
//					//<<m_routingType<<" with store cost "<<m_storeCost);
//			m_delivered = true;
//			break;
//		}
	}
	std::vector<int> nextCopyCars = m_copyCars;
	for (std::vector<int>::size_type i=0;i!=m_copyCars.size();i++){
		int curID = m_copyCars.at(i);
		for (int j=0;j<m_carNum;j++){
			if (m_con[curID][j] == true && j!=curID){
				if (!CheckCarHasCopy(j,nextCopyCars)){
					nextCopyCars.push_back(j);
					m_forwardNum = m_forwardNum + 1;
					std::vector<Forward> fwVec = m_trace.at(curID);
					Forward fw;
					fw.setFromId(curID);
					fw.setForwardTime(Simulator::Now().GetSeconds());
					fwVec.push_back(fw);
					m_trace[j] = fwVec;
					//NS_LOG_LOGIC(m_trace[j].size());
				}
			}
		}
	}
	m_copyCars = nextCopyCars;
}

void
MulticopyRouting::SprayAndWaitRouting(){
	m_storeCost += (int)m_copyCars.size() * 0.2;
	double elapseTime = Simulator::Now().GetSeconds();
	if (elapseTime > m_ttl){
		m_copyCars.clear();
		return;
	}
	/************************************************************************************
	 * Spray and wait routing is a routing strategy that each copy just passed to one
	 * relay car and the message will be passed directly to the destination by the relay
	 */
	if ((int)m_copyCars.size() < m_copyNum){
		/* Spray Phase
		 * Find relay cars which can communicate with the source car
		 */
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
							m_forwardNum = m_forwardNum + 1;
						}
					}
				}
			}
		}
	}
	/*Wait destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	std::vector<int>::iterator pos = m_copyCars.begin();
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size();){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			m_copyCars.erase(pos);
			m_copyNum -= 1;
		} else {
			pos++;
			i++;
		}
	}

}

void
MulticopyRouting::MobEyesRouting(){
	m_storeCost += (int)m_copyCars.size() * 0.2;
//	double elapseTime = Simulator::Now().GetSeconds();
//	if (elapseTime > m_ttl){
//		m_copyCars.clear();
//		return;
//	}
	/************************************************************************************
	 * Spray and wait routing is a routing strategy that each copy just passed to one
	 * relay car and the message will be passed directly to the destination by the relay
	 */
	bool sprayFlag = false;
	if ((int)m_copyCars.size() == 0){
		m_copyCars.push_back(m_sid);
	} else {
		if (m_hopNum > 0){
			/* Spray Phase
			 * Find relay cars which can communicate with the source car
			 */
			std::vector<int> nextCopyCars;
			for (int i=0;i<m_carNum;i++){
				for(std::vector<int>::size_type j = 0; j != m_copyCars.size();j++){
					if (m_con[m_copyCars.at(j)][i] == true){
						if (!CheckCarHasCopy(i,nextCopyCars)){
							nextCopyCars.push_back(i);
							m_forwardNum = m_forwardNum + 1;
							sprayFlag = true;
						}
					}
				}
			}
			m_copyCars = nextCopyCars;
		}
	}
	if (sprayFlag == true){
		m_hopNum -= 1;
	}

	/*Wait destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	std::vector<int>::iterator pos = m_copyCars.begin();
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size();){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			m_copyCars.erase(pos);
			m_copyNum -= 1;
		} else {
			pos++;
			i++;
		}
	}
}

void
MulticopyRouting::SprayAndFocusRouting(){
	/*m_storeCost += (int)m_copyCars.size() * 0.2;
	m_storeCost += (int)m_broadcastCars.size()* 0.05 * 0.2;
	double elapseTime = Simulator::Now().GetSeconds();
	if (elapseTime > m_bttl){
		m_broadcastCars.clear();
		return;
	}
	if (elapseTime > m_ttl){
		m_copyCars.clear();
		return;
	}
	*/
	/************************************************************************************
	 * Spray and focus routing is a routing strategy that each copy can be passed on to
	 * other better relay cars
	 */

	/*Focus destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	std::vector<int>::iterator pos = m_copyCars.begin();
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size();){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			m_broadcastCars.push_back(m_copyCars.at(i));
			m_copyCars.erase(pos);
			m_copyNum -= 1;
			m_bttl = m_deliverTime + 100;
		} else {
			pos++;
			i++;
		}

		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
//		if(m_copyCars.at(i)==m_did){
//			m_deliverTime = Simulator::Now().GetSeconds();
//			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
//			m_delivered = true;
//			m_broadcastCars.push_back(m_copyCars.at(i));
//			m_copyCars.erase(pos);
//			m_copyNum -= 1;
//			m_bttl = m_deliverTime + 100;
//		} else {
//			pos++;
//			i++;
//		}
	}
	/* clear other copies
	 *
	 */
	for(std::vector<int>::size_type i = 0; i != m_broadcastCars.size(); i++){
		int curID = m_broadcastCars.at(i);
		for (int j=0;j<m_carNum;j++){
			if (m_con[curID][j] == true){
				if (CheckCarHasCopy(j,m_copyCars)){
					std::vector<int>::iterator pos = m_copyCars.begin();
					for(std::vector<int>::size_type k = 0; k != m_copyCars.size();){
						if(m_copyCars.at(k) == j){
							m_copyCars.erase(pos);
							m_copyNum -= 1;
						} else {
							pos++;
							k++;
						}
					}
				}
				if (!CheckCarHasCopy(j,m_broadcastCars)){
					m_forwardNum = m_forwardNum + 1;
					m_broadcastCars.push_back(j);
				}
			}
		}
	}

	/*
	 * otherwise see if we can pass the message on to other relay which may be better
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		int currentID = m_copyCars.at(i);
		int nextID = -1;
		double curQuality = QualityComputation(currentID,m_did);
		for (std::vector<int>::size_type j=0; (int)j < m_carNum; j++){
			//if (m_con[currentID][j] == true and (int)j != m_sid){ // you can not pass one copy back to the source
			if (m_con[currentID][j] == true){
				double newQuality = QualityComputation((int)j,m_did);
				if (newQuality < curQuality){
					if (!CheckCarHasCopy(j,m_copyCars)){
						nextID = j;
						curQuality = newQuality;
					}
				}
			}
		}
		if(nextID != -1){
			m_copyCars[i] = nextID;
			m_forwardNum = m_forwardNum + 1;
			std::vector<Forward> fwVec = m_trace.at(currentID);
			Forward fw;
			fw.setFromId(currentID);
			fw.setForwardTime(Simulator::Now().GetSeconds());
			fwVec.push_back(fw);
			m_trace[nextID] = fwVec;
			//NS_LOG_LOGIC(m_trace[nextID].size());
		}
	}

	/* Spray Phase
	 * Find relay cars which can communicate with the source car
	 */

	if ((int)m_copyCars.size() < m_copyNum){
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
							m_forwardNum = m_forwardNum + 1;
							std::vector<Forward> fwVec = m_trace.at(m_sid);
							Forward fw;
							fw.setFromId(m_sid);
							fw.setForwardTime(Simulator::Now().GetSeconds());
							fwVec.push_back(fw);
							m_trace[i] = fwVec;
							//NS_LOG_LOGIC(m_trace[i].size());
						}
					}
				}
			}
		}
	}
}

void
MulticopyRouting::SprayAndAllPreyRouting(){
	m_qualityType = 4;
	/************************************************************************************
	 * Spray and prey routing is a routing strategy that each copy can be passed on to
	 * other better relay cars, however this better is among all relay cars not just from
	 * current relay
	 */

	/* Spray Phase
	 * Find relay cars which can communicate with the source car
	 */
	if ((int)m_copyCars.size() < m_copyNum){
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
							m_forwardNum = m_forwardNum + 1;
						}
					}
				}
			}
		}
	}
	/*Prey destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			break;
		}
	}
	/*
	 * otherwise see if we can pass the message on to other relay which may be better
	 */

	std::vector<int> nextCopyCars;
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){// check existing copies
		int candiID = m_copyCars.at(i);									// see if it needs to forward
		double candiQuality = QualityComputation(candiID,m_did);
		if (!CheckCarHasCopy(candiID,nextCopyCars)){
			if((int)nextCopyCars.size()<m_copyNum){
				nextCopyCars.push_back(candiID);
			} else {
				for(std::vector<int>::size_type j = 0; j != nextCopyCars.size(); j++){
					int curID = nextCopyCars.at(j);
					double curQuality = QualityComputation(curID,m_did);
					if (candiQuality < curQuality){
						nextCopyCars[j] = candiID;
						candiID = curID;
						candiQuality = curQuality;
					}
				}
			}
		}
		for (std::vector<int>::size_type j=0; (int)j < m_carNum; j++){	//check its neighbors
			if (m_con[m_copyCars.at(i)][j] == true){
				if(!CheckCarHasCopy(j,nextCopyCars)){
					candiID = (int) j;
					candiQuality = QualityComputation(candiID,m_did);
					if((int)nextCopyCars.size()<m_copyNum){
						nextCopyCars.push_back(candiID);
					} else {
						for(std::vector<int>::size_type k = 0; k != nextCopyCars.size(); k++){
							int curID = nextCopyCars.at(k);
							double curQuality = QualityComputation(curID,m_did);
							if (candiQuality < curQuality){
								nextCopyCars[k] = candiID;
								candiID = curID;
								candiQuality = curQuality;
							}
						}
					}
				}
			}
		}
	}
	for(std::vector<int>::size_type i = 0; i != nextCopyCars.size(); i++){
		bool forwardFlag = true;
		for(std::vector<int>::size_type j = 0; j != m_copyCars.size(); j++){
			if (nextCopyCars.at(i) == m_copyCars.at(j)){
				forwardFlag = false;
			}
		}
		if (forwardFlag){
			m_forwardNum = m_forwardNum + 1;
		}
	}
	m_copyCars = nextCopyCars;
}

void
MulticopyRouting::SprayAndPartialPreyRouting(){
	m_qualityType = 4;
	/************************************************************************************
	 * Spray and partial prey routing is a routing strategy that each copy can be passed on to
	 * other better relay cars in a global fashion, however this will happen under some conditions
	 * The condition is that bad utility is some time less of good utility
	 */

	/* Spray Phase
	 * Find relay cars which can communicate with the source car
	 */
	if ((int)m_copyCars.size() < m_copyNum){
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
						}
					}
				}
			}
		}
	}
	/*Prey destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			break;
		}
	}

	/*
	 * otherwise see if we can pass the message on to other relay which may be better
	 */

	std::vector<int> nextCopyCars = m_copyCars;
	for(std::vector<int>::size_type i = 0; i != nextCopyCars.size(); i++){	//select next hop for each copy
		int currentID = nextCopyCars.at(i);
		int nextID = -1;
		double curQuality = QualityComputation(currentID,m_did);
		for (std::vector<int>::size_type j=0; (int)j < m_carNum; j++){
			if (m_con[currentID][j] == true and (int)j != m_sid){ // you can not pass one copy back to the source
				double newQuality = QualityComputation((int)j,m_did);
				if (newQuality < curQuality){
					if (!CheckCarHasCopy(j,m_copyCars)){
						nextID = j;
						curQuality = newQuality;
					}
				}
			}
		}
		if(nextID != -1){
			nextCopyCars[i] = nextID;
		}
	}

	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){// check existing copies
		for (std::vector<int>::size_type j=0; (int)j < m_carNum; j++){	//check its neighbors
			if (m_con[m_copyCars.at(i)][j] == true){
				if(!CheckCarHasCopy(j,nextCopyCars)){
					int candiID = (int) j;
					double candiQuality = QualityComputation(candiID,m_did);
					if((int)nextCopyCars.size()<m_copyNum){
						nextCopyCars.push_back(candiID);
					} else {
						for(std::vector<int>::size_type k = 0; k != nextCopyCars.size(); k++){
							int curID = nextCopyCars.at(k);
							double curQuality = QualityComputation(curID,m_did);
							if (candiQuality < curQuality * m_cutPara){
								nextCopyCars[k] = candiID;
								candiID = curID;
								candiQuality = curQuality;
							}
						}
					}
				}
			}
		}
	}
	m_copyCars = nextCopyCars;
}

void
MulticopyRouting::SprayAndRoundPreyRouting(){
	m_qualityType = 4;
	/************************************************************************************
	 * Spray and round prey routing is a routing strategy that each copy can be passed on to
	 * other better relay cars in a global fashion, however all copies will form a round and
	 * this round will focus on the target
	 */

	/* Spray Phase
	 * Find relay cars which can communicate with the source car
	 */
	if ((int)m_copyCars.size() < m_copyNum){
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
						}
					}
				}
			}
		}
	}
	/*Prey destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			break;
		}
	}

	/*
	 * otherwise see if we can pass the message on to other relay which may be better
	 */

	double maxDist = 0;
	std::vector<int> allCandiCopyCars; // find all candidate from current copies
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		int currentID = m_copyCars.at(i);
		for (int j=0; j < m_carNum; j++){
			if (m_con[currentID][j] == true and (int)j != m_sid){ // you can not pass one copy back to the source
				if (!CheckCarHasCopy(j,allCandiCopyCars)){
					double dist = QualityComputation(j,m_did);
					if (dist > maxDist) {
						maxDist = dist;
					}
					allCandiCopyCars.push_back(j);
				}
			}
		}
	}

	std::vector<int> nextCopyCars;
	////////form a round using target as the heart, maximize distance of copies
	for(int i=0; i<m_copyNum; i++){ //generate next copyNum new copies
		int candiID = -1;
		double candiQuality = 100000000;
		std::vector<int>::iterator pos = allCandiCopyCars.begin();
		int posCount = 0;
		for(std::vector<int>::size_type j = 0; j != allCandiCopyCars.size(); j++){	//find the nearest one
			int curID = allCandiCopyCars.at(j);
			double curQuality = 0;
			double distPart = QualityComputation(curID,m_did);
			double anglePart = 0;
			if (nextCopyCars.size()!=0){ // already choose some copies
				double curX = m_cars.at(curID).GetPos().x; double curY = m_cars.at(curID).GetPos().y;
				double didX = m_cars.at(m_did).GetPos().x; double didY = m_cars.at(m_did).GetPos().y;

				for(std::vector<int>::size_type k = 0; k != nextCopyCars.size(); k++){
					int existID = nextCopyCars.at(k);
					double existX = m_cars.at(existID).GetPos().x; double existY = m_cars.at(existID).GetPos().y;
					double dot = (curX-didX)*(existX-didX)+(curY-didY)*(existY-didY);
					double norm = sqrt(pow((curX-didX),2)+pow((curY-didY),2)) * sqrt(pow((curX-didX),2)+pow(existY-didY,2));

					double temp = dot/norm;

					if (temp > 1)
						temp = 1;
					if (temp < -1)
						temp = -1;

					double angle = acos(temp);

					anglePart = anglePart + (M_PI - angle)/M_PI;//sum of all normalized angles
				}
				anglePart = anglePart/nextCopyCars.size();//average of all angles (0,1)
				distPart = distPart/maxDist; //normalization of distance (0,1)
				curQuality = distPart*m_distWeight + anglePart*m_angleWeight;
			} else {
				curQuality = distPart;
			}
			if (curQuality < candiQuality){
				candiID = curID;
				candiQuality = curQuality;
				posCount = (int)j;
			}
		}
		for (int j=0; j<posCount; j++){
			pos++;
		}
		if (candiID != -1){ //there is a nearest copy
			nextCopyCars.push_back(candiID);
			pos = allCandiCopyCars.erase(pos);
		}
	}

	m_copyCars = nextCopyCars;
}

void
MulticopyRouting::SprayAndRandomRouting(){
	/* Spray Phase
	 * Find relay cars which can communicate with the source car
	 */
	if ((int)m_copyCars.size() < m_copyNum){
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
						}
					}
				}
			}
		}
	}
	/*Prey destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			break;
		}
	}
	/*
	 * otherwise see if we can pass the message on to other relay which may be better
	 */
	//add all carrier candidates
	std::vector<int> candidateCopyCars;
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		for (std::vector<int>::size_type j=0; (int)j < m_carNum; j++){
			if (m_con[m_copyCars.at(i)][j] == true){
				if(!CheckCarHasCopy(j,candidateCopyCars)){
					candidateCopyCars.push_back((int) j);
				}
			}
		}
	}
	//rank by quality
	for(std::vector<int>::size_type i = 0; i != candidateCopyCars.size(); i++){
		int xid = candidateCopyCars.at(i);
		double xqua = QualityComputation(xid,m_did);
		for(std::vector<int>::size_type j = i+1; j != candidateCopyCars.size(); j++){
			int yid = candidateCopyCars.at(j);
			double yqua = QualityComputation(yid,m_did);
			if (yqua < xqua){
				candidateCopyCars[i] = yid;
				candidateCopyCars[j] = xid;
				xid = yid;
				xqua = yqua;
			}
		}
	}
	//randomly choose copyNum next copy
	int nextCopyNum = m_copyNum;
	if (nextCopyNum > (int)candidateCopyCars.size()){
		nextCopyNum = (int)candidateCopyCars.size();
	}
	std::vector<int> nextCopyCars;
	int i=0;
	while(i<nextCopyNum){
		int id = random()%(int)candidateCopyCars.size();
		int nextID = candidateCopyCars.at(id);
		if(!CheckCarHasCopy(nextID,nextCopyCars)){
			nextCopyCars.push_back(nextID);
			i++;
		}
	}

	m_copyCars = nextCopyCars;
}

void
MulticopyRouting::SprayAndPredictFocusRouting(){
	m_qualityType = 6;
	/************************************************************************************
	 * Spray and predict focus routing is a routing strategy that first copy we use current nearest, second copy we use one hop predict nearest.
	 */

	/* Spray Phase
	 * Find relay cars which can communicate with the source car
	 */
	if ((int)m_copyCars.size() < m_copyNum){
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
						}
					}
				}
			}
		}
	}
	/*Prey destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			break;
		}
	}
	/*
	 * otherwise see if we can pass the message on to other relay which may be better
	 */

	std::vector<int> candidateCopyCars;
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		for (std::vector<int>::size_type j=0; (int)j < m_carNum; j++){
			if (m_con[m_copyCars.at(i)][j] == true){
				if(!CheckCarHasCopy(j,candidateCopyCars)){
					candidateCopyCars.push_back((int) j);
				}
			}
		}
	}

	std::vector<int> nextCopyCars;
	int nextCopyNum = m_copyNum;
	if((int)candidateCopyCars.size()<m_copyNum){
		nextCopyNum = (int)candidateCopyCars.size();
	}

	for(int n=0;n<nextCopyNum;n++){
		//rank by quality
		m_predictHop = n;
		for(std::vector<int>::size_type i = 0; i != candidateCopyCars.size(); i++){
			int xid = candidateCopyCars.at(i);
			double xqua = QualityComputation(xid,m_did);
			for(std::vector<int>::size_type j = i+1; j != candidateCopyCars.size(); j++){
				int yid = candidateCopyCars.at(j);
				double yqua = QualityComputation(yid,m_did);
				if (yqua < xqua){
					candidateCopyCars[i] = yid;
					candidateCopyCars[j] = xid;
					xid = yid;
					xqua = yqua;
				}
			}
		}
		//choose the best one
		for(std::vector<int>::size_type i = 0; i != candidateCopyCars.size(); i++){
			int candid = candidateCopyCars.at(i);
			if (!CheckCarHasCopy(candid,nextCopyCars)){
				nextCopyCars.push_back(candid);
				break;
			}
		}
	}

	m_copyCars = nextCopyCars;

}

void
MulticopyRouting::GlobalSprayAndFocusRouting(){
	m_storeCost += (int)m_copyCars.size() * 0.2;
	double elapseTime = Simulator::Now().GetSeconds();
	if (elapseTime > m_ttl){
		m_copyCars.clear();
		return;
	}
	m_qualityType = 4;
	/************************************************************************************
	 * Global Spray and focus routing is a routing strategy that each copy can be passed on to
	 * other better relay cars, however, we use global information to choose the global best
	 */

	/* Spray Phase
	 * Find relay cars which can communicate with the source car
	 */
	if ((int)m_copyCars.size() < m_copyNum){
		for (int i=0;i<m_carNum;i++){
			if (i!=m_sid){
				if(m_con[m_sid][i] == true){			//Test each neighbor cars of source car
					if((int)m_copyCars.size() < m_copyNum){	//If we still need more copy
						if (!CheckCarHasCopy(i,m_copyCars)){				//And this neighbor has no copy
							m_copyCars.push_back(i);	//Then we pass one copy to this neighbor car
						}
					}
				}
			}
		}
	}
	/*Focus destination phase
	 *Test if any relay cars is neighbor of destination car
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		//NS_LOG_LOGIC("m_copyCars["<<i<<"]: "<<m_copyCars[i]);
		if(m_con[m_copyCars.at(i)][m_did] == true){
			m_deliverTime = Simulator::Now().GetSeconds();
			//NS_LOG_LOGIC("Message has been delivered to destination at "<<m_deliverTime<<" using routing type "<<m_routingType);
			m_delivered = true;
			m_copyNum = 0;
			m_copyCars.clear();
			break;
		}
	}
	/*
	 * otherwise see if we can pass the message on to other relay which may be better
	 */
	for(std::vector<int>::size_type i = 0; i != m_copyCars.size(); i++){
		int currentID = m_copyCars.at(i);
		int nextID = -1;
		double curQuality = QualityComputation(currentID,m_did);
		for (std::vector<int>::size_type j=0; (int)j < m_carNum; j++){
			if (m_con[currentID][j] == true and (int)j != m_sid){ // you can not pass one copy back to the source
				double newQuality = QualityComputation((int)j,m_did);
				if (newQuality < curQuality){
					if (!CheckCarHasCopy(j,m_copyCars)){
						nextID = j;
						curQuality = newQuality;
					}
				}
			}
		}
		if(nextID != -1){
			m_copyCars[i] = nextID;
		}
	}
}

double
MulticopyRouting::QualityComputation(int curID, int destID){
	double quality = 0.0;
	if (m_qualityType == 1){ //quality is computed with distance
		quality = sqrt(pow((m_cars.at(curID).GetPos().x - m_cars.at(destID).GetPos().x),2) +
				  pow((m_cars.at(curID).GetPos().y - m_cars.at(destID).GetPos().y),2));
	}
	if (m_qualityType == 2){
		/*movement prediction
		 * using constant moving pattern
		 */
		double nextCurX = m_cars.at(curID).GetPos().x + m_cars.at(curID).GetVel().x * 0.2;
		double nextCurY = m_cars.at(curID).GetPos().y + m_cars.at(curID).GetVel().y * 0.2;
		double nextDestX = m_cars.at(destID).GetPos().x + m_cars.at(destID).GetVel().x * 0.2;
		double nextDestY = m_cars.at(destID).GetPos().y + m_cars.at(destID).GetVel().y * 0.2;
		quality = sqrt(pow((nextCurX - nextDestX),2) + pow((nextCurY - nextDestY),2));
	}
	if (m_qualityType == 3){
		/*movement prediction
		 * using triangle moving pattern, that we predict that the car will go with changing direction
		 */
		int predictTime = 10;
		for (int i=1; i<predictTime; i++){
			double nextCurX = m_cars.at(curID).GetPos().x + m_cars.at(curID).GetVel().x * 0.2 * i;
			double nextCurY = m_cars.at(curID).GetPos().y + m_cars.at(curID).GetVel().y * 0.2 * i;
			double nextDestLeftX = m_cars.at(destID).GetPos().x;
			double nextDestLeftY = m_cars.at(destID).GetPos().y + m_cars.at(destID).GetVel().y * 0.2 * i;
			double nextDestMidX = m_cars.at(destID).GetPos().x + m_cars.at(destID).GetVel().x * 0.2 * i;
			double nextDestMidY = m_cars.at(destID).GetPos().y + m_cars.at(destID).GetVel().y * 0.2 * i;
			double nextDestRightX = m_cars.at(destID).GetPos().x + m_cars.at(destID).GetVel().x * 0.2 * i;
			double nextDestRightY = m_cars.at(destID).GetPos().y;

			double qualityLeft = sqrt(pow((nextCurX - nextDestLeftX),2) + pow((nextCurY - nextDestLeftY),2));
			double qualityMid = sqrt(pow((nextCurX - nextDestMidX),2) + pow((nextCurY - nextDestMidY),2));
			double qualityRight = sqrt(pow((nextCurX - nextDestRightX),2) + pow((nextCurY - nextDestRightY),2));

			if(qualityLeft<qualityMid)
				quality = qualityLeft;
			else
				quality = qualityMid;

			if (qualityRight < quality)
				quality = qualityRight;

		}
	}

	if (m_qualityType == 4){
		/* using minimum graph distance instead of direct distance
		 *
		 */
		quality = MinimumDistance (curID,destID,m_cars);
	}

	if (m_qualityType == 5){
		/* using predicted distance with hop number
		 *
		 */
		quality = sqrt(pow((m_cars.at(curID).GetPos().x - m_cars.at(destID).GetPos().x),2) +
				  pow((m_cars.at(curID).GetPos().y - m_cars.at(destID).GetPos().y),2));
		int n = 0;
		std::vector<Switch> predictCars = m_cars;
		while(n<m_predictHop){
			for(int i=0; i<m_carNum; i++){
				Vector pos = predictCars.at(i).GetPos(); // Get position
				double nextX = pos.x + pos.x * 0.2;
				double nextY = pos.y + pos.y * 0.2;
				pos.x = nextX;
				pos.y = nextY;
				predictCars.at(i).SetPos(pos);
			}
			int nextID = curID;
			double nextQua = sqrt(pow((predictCars.at(curID).GetPos().x - predictCars.at(destID).GetPos().x),2) +
					  pow((predictCars.at(curID).GetPos().y - predictCars.at(destID).GetPos().y),2));
			for(int i=0; i<m_carNum; i++){
				double dist = sqrt(pow((predictCars.at(curID).GetPos().x - predictCars.at(i).GetPos().x),2) +
								  pow((predictCars.at(curID).GetPos().y - predictCars.at(i).GetPos().y),2));
				if (dist <= m_range){
					double qua = sqrt(pow((predictCars.at(i).GetPos().x - predictCars.at(destID).GetPos().x),2) +
							  pow((predictCars.at(i).GetPos().y - predictCars.at(destID).GetPos().y),2));
					if (qua < nextQua){
						nextID = i;
						nextQua = qua;
					}
				}
			}
			curID = nextID;
			quality = nextQua;
			n++;
		}
	}

	if (m_qualityType == 6){
		/* using predicted distance with hop number
		 *
		 */
		quality = MinimumDistance (curID, destID, m_cars);
		int n = 0;
		std::vector<Switch> predictCars = m_cars;
		while(n<m_predictHop){
			for(int i=0; i<m_carNum; i++){
				Vector pos = predictCars.at(i).GetPos(); // Get position
				double nextX = pos.x + pos.x * 0.2;
				double nextY = pos.y + pos.y * 0.2;
				pos.x = nextX;
				pos.y = nextY;
				predictCars.at(i).SetPos(pos);
			}
			int nextID = curID;
			double nextQua = MinimumDistance (curID, destID, predictCars);
			for(int i=0; i<m_carNum; i++){
				double dist = sqrt(pow((predictCars.at(curID).GetPos().x - predictCars.at(i).GetPos().x),2) +
								  pow((predictCars.at(curID).GetPos().y - predictCars.at(i).GetPos().y),2));
				if (dist <= m_range){
					double qua = MinimumDistance (i, destID, predictCars);
					if (qua < nextQua){
						nextID = i;
						nextQua = qua;
					}
				}
			}
			curID = nextID;
			quality = nextQua;
			n++;
		}
	}

	return quality;
}

double
MulticopyRouting::MinimumDistance (int src, int dst, std::vector<Switch> cars)
{
	int maxint = 1000000;

	int num = m_carNum;

	double map [num][num];

	for (int i=0; i<num; i++)
	{
		for (int j=0; j<num; j++)
		{
			double px1 = cars.at(i).GetPos().x;	double py1 = cars.at(i).GetPos().y;
			double px2 = cars.at(j).GetPos().x;	double py2 = cars.at(j).GetPos().y;
			double d = sqrt( pow((px1 - px2),2) + pow((py1 - py2),2) );
			if (d <= m_range){			//***************** use m_con[i][j] if we test in geoflow
				map[i][j] = 0;
			} else {
				map[i][j] = d - m_range;
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
	double distance = 0;
	//std::cout<<"dst is "<<cur<<" ";
	while (true)
	{
		if (pre[cur]!= -1){
			distance += map[cur][pre[cur]];
			distance++; // to let the distance from nodes to destination on the routing increase if they are all zero
		}
		//std::cout<<pre[cur]<<" ";
		if (cur == src || pre[cur] == src)
			break;
		else
			cur = pre[cur];
	}
	//std::cout<<std::endl;

	return distance;
}

bool
MulticopyRouting::CheckCarHasCopy(int carID, std::vector<int> copyCars){
	bool existFlag = false;
	for(std::vector<int>::size_type i=0; i!=copyCars.size(); i++){
		if(copyCars.at(i) == carID)
			existFlag = true;
	}
	return existFlag;
}

}
} /* namespace ns3 */
