/*
 * geoflow-flowentry.cc
 *
 *  Created on: 2013年12月22日
 *      Author: sumoon
 */

#include "geoflow-flowentry.h"

namespace ns3 {
namespace geoflow {

MatchField::MatchField() {
	// TODO Auto-generated constructor stub

}

MatchField::~MatchField() {
	// TODO Auto-generated destructor stub
}

bool
MatchField::operator== (MatchField const & o) const
{
  return (srcIpAddr == o.srcIpAddr && dstIpAddr == o.dstIpAddr);
}

Action::Action() {
	// TODO Auto-generated constructor stub

}

Action::~Action() {
	// TODO Auto-generated destructor stub
}

FlowEntry::FlowEntry() {
	// TODO Auto-generated constructor stub

}

FlowEntry::~FlowEntry() {
	// TODO Auto-generated destructor stub
}

} /* namespace geoflow */
} /* namespace ns3 */
