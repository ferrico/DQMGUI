/****************************************************************************

****************************************************************************/

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"


#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/GEMRecHit/interface/GEMRecHit.h"
#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"
#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"


#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


#include <string>

//----------------------------------------------------------------------------------------------------
 
class GEMDQMSource: public DQMEDAnalyzer
{
  public:
    GEMDQMSource(const edm::ParameterSet& cfg);
    virtual ~GEMDQMSource();
  
  protected:
    void dqmBeginRun(edm::Run const &, edm::EventSetup const &) override;
    void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
    void analyze(edm::Event const& e, edm::EventSetup const& eSetup) override;
    void beginLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& eSetup) override;
    void endLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& eSetup) override;
    void endRun(edm::Run const& run, edm::EventSetup const& eSetup) override;

  private:
    unsigned int verbosity;
   
    int nCh;


    //edm::EDGetTokenT<  > gemDigi;
    edm::EDGetToken tagRecHit;
    edm::EDGetToken tagDigi;

    const GEMGeometry* initGeometry(edm::EventSetup const & iSetup);
    int findVFAT(float min_, float max_, float x_, int roll_);
     
    const GEMGeometry* GEMGeometry_; 

    std::vector<GEMChamber> gemChambers;

    MonitorElement* Phi;
    MonitorElement* Eta;
    
    std::unordered_map<UInt_t,  MonitorElement*> recHitME;
    
    std::unordered_map<UInt_t,  MonitorElement*> VFAT_vs_ClusterSize;
    
    std::unordered_map<UInt_t,  MonitorElement*> Digi_Strip_vs_eta;
    std::unordered_map<UInt_t,  MonitorElement*> StripsFired_vs_eta;

};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

int GEMDQMSource::findVFAT(float min_, float max_, float x_, int roll_) {
  float step = abs(max_-min_)/3.0;
  if ( x_ < (min(min_,max_)+step) ) { return 8 - roll_;}
  else if ( x_ < (min(min_,max_)+2.0*step) ) { return 16 - roll_;}
  else { return 24 - roll_;}
}

const GEMGeometry* GEMDQMSource::initGeometry(edm::EventSetup const & iSetup) {
  const GEMGeometry* GEMGeometry_ = nullptr;
  try {
    edm::ESHandle<GEMGeometry> hGeom;
    iSetup.get<MuonGeometryRecord>().get(hGeom);
    GEMGeometry_ = &*hGeom;
  }
  catch( edm::eventsetup::NoProxyException<GEMGeometry>& e) {
    edm::LogError("MuonGEMBaseValidation") << "+++ Error : GEM geometry is unavailable on event loop. +++\n";
    return nullptr;
  }

  return GEMGeometry_;
}


//----------------------------------------------------------------------------------------------------
GEMDQMSource::GEMDQMSource(const edm::ParameterSet& cfg)
{

  tagRecHit = consumes<GEMRecHitCollection>(cfg.getParameter<edm::InputTag>("recHitsInputLabel")); 
  tagDigi = consumes<GEMDigiCollection>(cfg.getParameter<edm::InputTag>("digisInputLabel")); 

}

//----------------------------------------------------------------------------------------------------

GEMDQMSource::~GEMDQMSource()
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSource::dqmBeginRun(edm::Run const &, edm::EventSetup const &)
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSource::bookHistograms(DQMStore::IBooker &ibooker, edm::Run const &, edm::EventSetup const & iSetup)
{

  GEMGeometry_ = initGeometry(iSetup);
  if ( GEMGeometry_ == nullptr) return ;  

  const std::vector<const GEMSuperChamber*>& superChambers_ = GEMGeometry_->superChambers();   
  for (auto sch : superChambers_){
    int n_lay = sch->nChambers();
    for (int l=0;l<n_lay;l++){
      gemChambers.push_back(*sch->chamber(l+1));
    }
  }
  nCh = gemChambers.size();
  ibooker.cd();
  ibooker.setCurrentFolder("GEM");
  Phi = ibooker.book1D("phi", "#phi", 360, -3.141592, 3.141592);
  Eta = ibooker.book1D("eta", "#eta", 10, 1.5, 2.5);
  ibooker.setCurrentFolder("GEM/recHit");
  for (auto ch : gemChambers){
    GEMDetId gid = ch.id();
    string hName = "recHit_Gemini_"+to_string(gid.superChamberId())+"_la_"+to_string(gid.layer());
    string hTitle = "recHit Gemini ID : "+to_string(gid.superChamberId())+", layer : "+to_string(gid.layer());
    recHitME[ ch.id() ] = ibooker.book1D(hName, hTitle, 24,0,24);

    string hName_2 = "VFAT_vs_ClusterSize_Gemini_"+to_string(gid.superChamberId())+"_la_"+to_string(gid.layer());
    string hTitle_2 = "VFAT vs ClusterSize Gemini ID : "+to_string(gid.superChamberId())+", layer : "+to_string(gid.layer());
    VFAT_vs_ClusterSize[ ch.id() ] = ibooker.book2D(hName_2, hTitle_2, 11, -0.5, 10.5, 24, 0.5, 24.5);
    TH2F *hist = VFAT_vs_ClusterSize[ ch.id() ]->getTH2F();
    hist->SetMarkerStyle(20);
    hist->SetMarkerSize(0.5);
    
    string hName_fired = "StripFired_Gemini_"+to_string(gid.superChamberId())+"_la_"+to_string(gid.layer());
    string hTitle_fired = "StripsFired Gemini ID : "+to_string(gid.superChamberId())+", layer : "+to_string(gid.layer());
    StripsFired_vs_eta[ ch.id() ] = ibooker.book2D(hName_fired, hTitle_fired, 384, 0.5, 384.5, 8, 0.5,8.5);
    TH2F *hist_2 = StripsFired_vs_eta[ ch.id() ]->getTH2F();
    hist_2->SetMarkerStyle(20);
    hist_2->SetMarkerSize(0.5);
  }
  
  ibooker.setCurrentFolder("GEM/digi");
  for (auto ch : gemChambers){
    GEMDetId gid = ch.id();
    string hName_digi = "Digi_Strips_Gemini_"+to_string(gid.superChamberId())+"_la_"+to_string(gid.layer());
    string hTitle_digi = "Digi Strip Gemini ID : "+to_string(gid.superChamberId())+", layer : "+to_string(gid.layer());
//     string hName_digi = "digi_"+to_string(gid.chamber());
//     string hTitle_digi = "digi "+to_string(gid.chamber());
    Digi_Strip_vs_eta[ ch.id() ] = ibooker.book2D(hName_digi, hTitle_digi, 384, 0.5, 384.5, 8, 0.5,8.5);
    TH2F *hist_3 = Digi_Strip_vs_eta[ ch.id() ]->getTH2F();
    hist_3->SetMarkerStyle(20);
    hist_3->SetMarkerSize(0.5);
  }
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSource::beginLuminosityBlock(edm::LuminosityBlock const& lumiSeg, 
                                            edm::EventSetup const& context) 
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSource::analyze(edm::Event const& event, edm::EventSetup const& eventSetup)
{
  const GEMGeometry* GEMGeometry_  = initGeometry(eventSetup);
  if ( GEMGeometry_ == nullptr) return; 

  ////////////////
  ///// Digi /////
  ////////////////
  edm::Handle<GEMDigiCollection> gemDigis;
  event.getByToken( this->tagDigi, gemDigis);
//   if (!gemDigis.isValid()){
//   		edm::LogError("GEMDQMSource") << "GEM Digi is not valid.\n";
//   		return;
//   }
  for (auto ch : gemChambers){
    GEMDetId cId = ch.id();
    for(auto roll : ch.etaPartitions()){
      GEMDetId rId = roll->id();      
      const auto& digis_in_det = gemDigis->get(rId);
      for (auto d = digis_in_det.first; d != digis_in_det.second; ++d){
      		Digi_Strip_vs_eta[ cId ]->Fill(d->strip(), rId.roll());
//       		std::cout<<d->strip()<<std::endl;
      }
    }
  }
  
  ////////////////
  //// RecHit ////
  ////////////////
  edm::Handle<GEMRecHitCollection> gemRecHits;
  event.getByToken( this->tagRecHit, gemRecHits);
//   if (!gemRecHits.isValid()) {
//     edm::LogError("GEMDQMSource") << "GEM recHit is not valid.\n";
//     return ;
//   }  
  for (GEMRecHitCollection::const_iterator recHit = gemRecHits->begin(); recHit != gemRecHits->end(); ++recHit){
    GEMDetId id((*recHit).geographicalId());
    LocalPoint recHitLP = recHit->localPosition();
    GlobalPoint recHitGP = GEMGeometry_->idToDet((*recHit).gemId())->surface().toGlobal(recHitLP);
    Float_t rhPhi = recHitGP.phi();
    Float_t rhEta = recHitGP.eta();
    Phi->Fill(rhPhi);
    Eta->Fill(rhEta);
  }
  for (auto ch : gemChambers){
    GEMDetId cId = ch.id();
    for(auto roll : ch.etaPartitions()){
      GEMDetId rId = roll->id();
      const auto& recHitsRange = gemRecHits->get(rId); 
      auto gemRecHit = recHitsRange.first;
      for ( auto hit = gemRecHit; hit != recHitsRange.second; ++hit ) {
        int nVfat = findVFAT(1.0, 385.0, hit->firstClusterStrip()+0.5*hit->clusterSize(), rId.roll());
        recHitME[ cId ]->Fill(nVfat);
        VFAT_vs_ClusterSize[ cId ]->Fill(hit->clusterSize(), nVfat);
        for(int i = hit->firstClusterStrip(); i < (hit->firstClusterStrip() + hit->clusterSize()); i++){
        	StripsFired_vs_eta[ cId ]->Fill(i, rId.roll());
        }
      }  
    }   
  }
  
//   for (auto ch : gemChambers){
//    	for(auto roll : ch.etaPartitions()){
//    	GEMDetId rId = roll->id();
//    		for(int ii = 0; ii < StripsFired_vs_eta[ ch.id() ]->getNbinsX(); ii++){
//    			int rh = StripsFired_vs_eta[ ch.id() ]->getBinContent(ii, rId.roll());
//    			int dg = Digi_Strip_vs_eta[ ch.id() ]->getBinContent(ii, rId.roll());
//    			if(dg == 0)
//     			eff_Strips_vs_eta[ ch.id() ]->setBinContent(ii, rId.roll(), 0);	
//     		else
//     			eff_Strips_vs_eta[ ch.id() ]->setBinContent(ii, rId.roll(), rh/dg);	
//     	}
//     }
//   
//   }
  
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSource::endLuminosityBlock(edm::LuminosityBlock const& lumiSeg, edm::EventSetup const& eSetup) 
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSource::endRun(edm::Run const& run, edm::EventSetup const& eSetup)
{
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(GEMDQMSource);
