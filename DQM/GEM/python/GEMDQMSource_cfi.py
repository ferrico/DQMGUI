import FWCore.ParameterSet.Config as cms

GEMDQMSource = cms.EDAnalyzer("GEMDQMSource",
    recHitsInputLabel = cms.InputTag("gemRecHits", "", "RECO"),
    digisInputLabel = cms.InputTag("simMuonGEMDigis", "", "HLT")     

  
)
