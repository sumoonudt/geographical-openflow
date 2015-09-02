/*
 * Copy.h
 *
 *  Created on: 2015年4月17日
 *      Author: sumoon
 */

#ifndef GEOFLOW_COPY_H_
#define GEOFLOW_COPY_H_

namespace ns3 {
namespace geoflow{

class Copy {
public:
	Copy();
	virtual ~Copy();
	int id;
	int sid;
	int did;
	int num;//rest number of copy it can duplicate, single copy routing set 0
	double sentTime;
	double arivTime;
};

}
} /* namespace ns3 */

#endif /* COPY_H_ */
