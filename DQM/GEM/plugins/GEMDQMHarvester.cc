#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//DQM services
#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <memory>
#include <vector>

#include "TString.h"


using namespace std;

class GEMDQMHarvester: public DQMEDHarvester
{
  
 public:

  GEMDQMHarvester(const edm::ParameterSet&);
  virtual ~GEMDQMHarvester();
  
 
//   virtual void beginJob(){return;};
//   
//   virtual void endJob(){return;};  
//  
//   virtual void analyze(const edm::Event&, const edm::EventSetup&){return;};
//   
//   virtual void endRun(const edm::Run&, const edm::EventSetup&){return;};
  
protected:
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override {}
  
  void dqmEndLuminosityBlock(DQMStore::IBooker &, DQMStore::IGetter &, const edm::LuminosityBlock &, const edm::EventSetup &) override;

  
private:
	void prova(DQMStore::IBooker&, DQMStore::IGetter&);
  std::string fName;
  int verbosity;
  DQMStore *dbe;

};


GEMDQMHarvester::GEMDQMHarvester(const edm::ParameterSet& ps)
{
//   fName = ps.getUntrackedParameter<std::string>("Name");

  //dbe_path_ = std::string("GEMDQM/");
  //outputFile_ = ps.getUntrackedParameter<std::string>("outputFile", "myfile.root");
}

GEMDQMHarvester::~GEMDQMHarvester()
{

}

void GEMDQMHarvester::prova(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter)
{
// 	MonitorElement* eta_1 = igetter.get("/GEM/testEta"); 
// 	MonitorElement* eta_2 = igetter.get("/GEM/testEta_2"); 
// 	MonitorElement* eff = igetter.get("/GEM/prova/eff");
// 	
// 	for(int i = 0; i < eta_1->getNbinsX(); i++){
// 		if(eta_2->getBinContent(i) == 0)
// 			eff->setBinContent(i, 0);
// 		else{
// 			double r = eta_1->getBinContent(i) / eta_2->getBinContent(i);
// 			eff->setBinContent(i, r);
// 		}
// 	}
}

void GEMDQMHarvester::dqmEndLuminosityBlock(DQMStore::IBooker &ibooker, DQMStore::IGetter &igetter, const edm::LuminosityBlock &, const edm::EventSetup &)
{
}

//void GEMDQMHarvestor::dqmEndJob(DQMStore::IBooker & ibooker, DQMStore::IGetter &ig )
//{
  //ig.setCurrentFolder(dbe_path_.c_str());

//}
DEFINE_FWK_MODULE(GEMDQMHarvester);