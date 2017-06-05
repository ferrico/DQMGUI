import FWCore.ParameterSet.Config as cms

GEMDQMSourceDigi = cms.EDAnalyzer("GEMDQMSourceDigi",
    digisInputLabel = cms.InputTag("simMuonGEMDigis", "", "HLT")     
  
)
