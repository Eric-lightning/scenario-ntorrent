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
 * Authors: Spyridon (Spyros) Mastorakis <mastorakis@cs.ucla.edu>
 *          Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ntorrent-consumer-app.hpp"

NS_LOG_COMPONENT_DEFINE("NTorrentConsumerApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(NTorrentConsumerApp);

TypeId
NTorrentConsumerApp::GetTypeId(void)
{
    static TypeId tid = TypeId("NTorrentConsumerApp")
      .SetParent<Application>()
      .AddConstructor<NTorrentConsumerApp>()
      .AddAttribute("StartSeq", "Initial sequence number", IntegerValue(0),
                    MakeIntegerAccessor(&NTorrentConsumerApp::m_seq), MakeIntegerChecker<int32_t>())

      .AddAttribute("Prefix", "Name of the Interest", StringValue("/prefix/sub/"),
                    MakeNameAccessor(&NTorrentConsumerApp::m_interestName), MakeNameChecker())
      .AddAttribute("LifeTime", "LifeTime for interest packet", StringValue("2s"),
                    MakeTimeAccessor(&NTorrentConsumerApp::m_interestLifeTime), MakeTimeChecker());
    return tid;
}

NTorrentConsumerApp::NTorrentConsumerApp()
{
}

NTorrentConsumerApp::~NTorrentConsumerApp()
{
}

void
NTorrentConsumerApp::StartApplication()
{
    ndn::App::StartApplication();
    ndn::FibHelper::AddRoute(GetNode(), "/prefix/sub", m_face, 0);
    Simulator::Schedule(Seconds(1.0), &NTorrentConsumerApp::SendInterest, this);
}

void
NTorrentConsumerApp::StopApplication()
{
    ndn::App::StopApplication();
}

void
NTorrentConsumerApp::SendInterest()
{
  auto interest = std::make_shared<Interest>("/prefix/sub");
  Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
  interest->setNonce(rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
  interest->setInterestLifetime(ndn::time::seconds(1));

  NS_LOG_DEBUG("Sending Interest packet for " << *interest);

  // Call trace (for logging purposes)
  m_transmittedInterests(interest, this, m_face);

  m_appLink->onReceiveInterest(*interest);
}

void
NTorrentConsumerApp::OnInterest(std::shared_ptr<const Interest> interest)
{
  ndn::App::OnInterest(interest);

  NS_LOG_DEBUG("Received Interest packet for " << interest->getName());

  // Note that Interests send out by the app will not be sent back to the app !

  auto data = std::make_shared<ndn::Data>(interest->getName());
  data->setFreshnessPeriod(ndn::time::milliseconds(1000));
  data->setContent(std::make_shared< ::ndn::Buffer>(1024));
  ndn::StackHelper::getKeyChain().sign(*data);

  NS_LOG_DEBUG("Sending Data packet for " << data->getName());

  // Call trace (for logging purposes)
  m_transmittedDatas(data, this, m_face);

  m_appLink->onReceiveData(*data);
}

void
NTorrentConsumerApp::OnData(std::shared_ptr<const Data> data)
{
    NS_LOG_DEBUG("Receiving Data packet for " << data->getName());
    std::cout << "DATA received for name " << data->getName() << std::endl;
}

} // namespace ndn
} // namespace ns3
