/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Akshay Raman <akshay.raman@cs.ucla.edu>
 */

#ifndef NTORRENT_STRATEGY_HPP
#define NTORRENT_STRATEGY_HPP
#include "src/torrent-file.hpp"
#include "src/file-manifest.hpp"
#include "src/torrent-manager.hpp"
#include "src/util/shared-constants.hpp"
#include "src/util/simulation-constants.hpp"
#include "src/util/io-util.hpp"

namespace ndn_ntorrent = ndn::ntorrent;

#include <boost/random/mersenne_twister.hpp>
#include "face/face.hpp"
#include "fw/strategy.hpp"
#include "fw/algorithm.hpp"
#include <sys/time.h>

namespace nfd {
namespace fw {

class NTorrentStrategy : public Strategy {
public:
  NTorrentStrategy(Forwarder& forwarder, const Name& name = getStrategyName());

  virtual ~NTorrentStrategy() override;

  virtual void
  afterReceiveInterest (const Face& inFace, const Interest& interest,
                       const shared_ptr<pit::Entry>& pitEntry) override;

  //TODO: Add afterReceiveData
    
  virtual void
  beforeSatisfyInterest (const shared_ptr< pit::Entry > &pitEntry, 
                        const Face &inFace, const Data &data) override;

  virtual void
  beforeExpirePendingInterest (const shared_ptr< pit::Entry > &pitEntry) override;

  virtual void  
  afterReceiveNack (const Face &inFace, const lp::Nack &nack, 
          const shared_ptr< pit::Entry > &pitEntry) override;

  virtual void  
  onDroppedInterest (const Face &outFace, const Interest &interest)
  override;

  virtual void
  afterReceiveData (const shared_ptr<pit::Entry>& pitEntry,
          const Face& inFace, const Data& data);
  
  static const Name&
  getStrategyName();

  static long int getTimestamp(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int us = tp.tv_sec*1000000 + tp.tv_usec;
    return us;
  }

  /* dump out everything from face_name_incoming_time */
  static void dump_hash(std::string label, std::unordered_map<int,std::unordered_map<Name, long int>> face_name_incoming_time){  
    std::cout << label;
  for(auto i=face_name_incoming_time.begin(); i!=face_name_incoming_time.end(); ++i)
  {
    std::cout << i->first << ": { ";
    for(auto j=i->second.begin(); j!=i->second.end(); ++j)
    {
        std::cout << j->first << ": " << j->second << ", ";
    }
    std::cout << "}, " << std::endl;
  }
  std::cout << std::endl;
}

  /* Function to compare delay in face_average_delay map*/
  static bool compareDelay(std::pair<int, std::pair<int,int>> a, std::pair<int, std::pair<int,int>> b) {
        float delay1=a.second.first;
        int freq1=a.second.second;
        float delay2=b.second.first;
        int freq2=b.second.second;
        return delay1/freq1 < delay2/freq2; 
  }

protected:
  boost::random::mt19937 m_randomGenerator;

  typedef std::unordered_map<Name,long int> name_incoming_time;
  std::unordered_map<int, name_incoming_time> face_name_incoming_time;
  
  std::unordered_map<int, std::pair<int,int>> face_average_delay;
  std::unordered_map<int, std::pair<int,int>> face_satisfaction_rate;
  
  std::unordered_map<Name,std::vector<int>> nackedname_nexthop;
};

} // namespace fw
} // namespace nfd

#endif // NTORRENT_STRATEGY_HPP
