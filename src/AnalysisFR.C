#define AnalysisFR_cxx
#include "../include/AnalysisFR.h"

//------------------------------------------------------------------------------
// AnalysisFR
//------------------------------------------------------------------------------
AnalysisFR::AnalysisFR(TTree* tree, TString systematic) : AnalysisCMS(tree, systematic)
{
  SetSaveMinitree(false);
}


//------------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------------
void AnalysisFR::Loop(TString analysis, TString filename, float luminosity)
{
  if (fChain == 0) return;

  Setup(analysis, filename, luminosity);

  // Define fake rate histograms
  //----------------------------------------------------------------------------
  for (int i=0; i<ncut; i++) {
    
    TString directory = scut[i];
    
    root_output->cd();
    
    gDirectory->mkdir(directory);
    
    root_output->cd(directory);
    
    for (int j=0; j<njetet; j++) {
      
      TString muonsuffix = Form("_%.0fGeV", muonjetet[j]);
      TString elesuffix  = Form("_%.0fGeV", elejetet[j]);
      
      h_Muon_loose_pt_eta_bin[i][j] = new TH2D("h_Muon_loose_pt_eta_bin" + muonsuffix, "", nptbin, ptbins, netabin, etabins);
      h_Muon_tight_pt_eta_bin[i][j] = new TH2D("h_Muon_tight_pt_eta_bin" + muonsuffix, "", nptbin, ptbins, netabin, etabins);
      h_Ele_loose_pt_eta_bin [i][j] = new TH2D("h_Ele_loose_pt_eta_bin"  + elesuffix,  "", nptbin, ptbins, netabin, etabins);
      h_Ele_tight_pt_eta_bin [i][j] = new TH2D("h_Ele_tight_pt_eta_bin"  + elesuffix,  "", nptbin, ptbins, netabin, etabins);
      
      h_Muon_loose_pt_bin[i][j] = new TH1D("h_Muon_loose_pt_bin" + muonsuffix, "", nptbin, ptbins);
      h_Muon_tight_pt_bin[i][j] = new TH1D("h_Muon_tight_pt_bin" + muonsuffix, "", nptbin, ptbins);
      h_Ele_loose_pt_bin [i][j] = new TH1D("h_Ele_loose_pt_bin"  + elesuffix,  "", nptbin, ptbins);
      h_Ele_tight_pt_bin [i][j] = new TH1D("h_Ele_tight_pt_bin"  + elesuffix,  "", nptbin, ptbins);
      
      h_Muon_loose_eta_bin[i][j] = new TH1D("h_Muon_loose_eta_bin" + muonsuffix, "", netabin, etabins);
      h_Muon_tight_eta_bin[i][j] = new TH1D("h_Muon_tight_eta_bin" + muonsuffix, "", netabin, etabins);
      h_Ele_loose_eta_bin [i][j] = new TH1D("h_Ele_loose_eta_bin"  + elesuffix,  "", netabin, etabins);
      h_Ele_tight_eta_bin [i][j] = new TH1D("h_Ele_tight_eta_bin"  + elesuffix,  "", netabin, etabins);
	
      h_Muon_loose_pt[i][j] = new TH1D("h_Muon_loose_pt" + muonsuffix, "", 1000, 0, 200);
      h_Muon_tight_pt[i][j] = new TH1D("h_Muon_tight_pt" + muonsuffix, "", 1000, 0, 200);
      h_Ele_loose_pt [i][j] = new TH1D("h_Ele_loose_pt"  + elesuffix,  "", 1000, 0, 200);
      h_Ele_tight_pt [i][j] = new TH1D("h_Ele_tight_pt"  + elesuffix,  "", 1000, 0, 200);
      
      h_Muon_loose_mtw[i][j] = new TH1D("h_Muon_loose_mtw" + muonsuffix, "", 1000, 0, 200);
      h_Muon_tight_mtw[i][j] = new TH1D("h_Muon_tight_mtw" + muonsuffix, "", 1000, 0, 200);
      h_Ele_loose_mtw [i][j] = new TH1D("h_Ele_loose_mtw"  + elesuffix,  "", 1000, 0, 200);
      h_Ele_tight_mtw [i][j] = new TH1D("h_Ele_tight_mtw"  + elesuffix,  "", 1000, 0, 200);
	
      h_Muon_loose_m2l[i][j] = new TH1D("h_Muon_loose_m2l" + muonsuffix, "", 1000, 0, 200);
      h_Muon_tight_m2l[i][j] = new TH1D("h_Muon_tight_m2l" + muonsuffix, "", 1000, 0, 200);
      h_Ele_loose_m2l [i][j] = new TH1D("h_Ele_loose_m2l"  + elesuffix,  "", 1000, 0, 200);
      h_Ele_tight_m2l [i][j] = new TH1D("h_Ele_tight_m2l"  + elesuffix,  "", 1000, 0, 200);
      
      
      // Define effective luminosity estimation histograms
      //------------------------------------------------------------------------
      h_Muon_loose_pt_m2l[i][j] = new TH2D("h_Muon_loose_pt_m2l" + muonsuffix, "", 200, 0, 200, nptbin, ptbins);
      h_Muon_tight_pt_m2l[i][j] = new TH2D("h_Muon_tight_pt_m2l" + muonsuffix, "", 200, 0, 200, nptbin, ptbins);
      h_Ele_loose_pt_m2l [i][j] = new TH2D("h_Ele_loose_pt_m2l"  + elesuffix,  "", 200, 0, 200, nptbin, ptbins);
      h_Ele_tight_pt_m2l [i][j] = new TH2D("h_Ele_tight_pt_m2l"  + elesuffix,  "", 200, 0, 200, nptbin, ptbins);
    }
  }
  
  root_output->cd();
  
  // Loop over events
  //----------------------------------------------------------------------------
  for (Long64_t jentry=0; jentry<_nentries;jentry++) {
    
    Long64_t ientry = LoadTree(jentry);
    
    if (ientry < 0) break;
    
    fChain->GetEntry(jentry);
    
    PrintProgress(jentry, _nentries);
    
    EventSetup();
    
    _channel = (abs(Lepton1.flavour) == ELECTRON_FLAVOUR) ? e : m;
    
    _leptonPtMin  = (_channel == e) ?  13 :  10;
    _leptonEtaMax = (_channel == e) ? 2.5 : 2.4;

    if (Lepton1.v.Pt()        < _leptonPtMin)  continue;
    if (fabs(Lepton1.v.Eta()) > _leptonEtaMax) continue;


    // Get the event weight
    //--------------------------------------------------------------------------
    bool passTrigger;

    if (_ismc) {

      passTrigger = true;

      Float_t corrected_baseW = baseW; 

      if (_sample.Contains("DYJetsToLL_M-10to50")) corrected_baseW = 0.829752445221; 
      if (_sample.Contains("DYJetsToLL_M-50"))     corrected_baseW = 0.318902641535;

      _base_weight = (corrected_baseW / 1e3) * puW6p3 * GEN_weight_SM / abs(GEN_weight_SM);

      _event_weight = _base_weight;


      // Muons
      //------------------------------------------------------------------------
      if (_channel == m)
	{
	  (Lepton1.v.Pt() <= 20.) ? _event_weight *= 5.86 : _event_weight *= 163.84;
	}

      
      // Electrons
      //------------------------------------------------------------------------
      if (_channel == e)
	{
	  (Lepton1.v.Pt() <= 25.) ? _event_weight *= 8.51 : _event_weight *= 42.34;
	}

    } else {

      _event_weight = 1.0;

      passTrigger = false;


      // Muons
      //------------------------------------------------------------------------
      if (_sample.Contains("DoubleMuon") && _channel == m) {

	if (Lepton1.v.Pt() <= 20. && std_vector_trigger->at(22)) {  // HLT_Mu8_TrkIsoVVL_v*

	  passTrigger = true;

	} else if (Lepton1.v.Pt() > 20. && std_vector_trigger->at(23)) {  // HLT_Mu17_TrkIsoVVL_v*

	  passTrigger = true;
	}
      }


      // Electrons
      //------------------------------------------------------------------------
      if (_sample.Contains("DoubleEG") && _channel == e) {
	
	if (Lepton1.v.Pt() <= 25. && std_vector_trigger->at(31)) {  // HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v*

	  passTrigger = true;
	  
	} else if (Lepton1.v.Pt() > 25. && std_vector_trigger->at(33)) {  // HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v*

	  passTrigger = true;
	}
      }
    }


    // Get the jets and the W transverse mass
    //--------------------------------------------------------------------------
    GetAwayJets();
    GetMt(Lepton1, _mtw);


    // Preselection
    //--------------------------------------------------------------------------
    if (!passTrigger) continue;

    if (AnalysisJets.size() < 1) continue;


    // Get histograms for different jet pt thresholds
    //--------------------------------------------------------------------------
    for (int i=0; i<njetet; i++) {

      _inputJetEt = (_channel == e) ? elejetet[i] : muonjetet[i];

      if (AnalysisJets[0].v.Pt() < _inputJetEt) continue; 


      // QCD region
      //------------------------------------------------------------------------
      bool pass;
      pass = (_nlepton == 1);

      FillLevelHistograms(FR_00_QCD, i, pass);

    }
  }
  

  EndJob();
}


//------------------------------------------------------------------------------
// FillLevelHistograms
//------------------------------------------------------------------------------
void AnalysisFR::FillLevelHistograms(int icut, int i, bool pass)
{
  if (!pass) return;

  FillAnalysisHistograms(icut, i);
}


//------------------------------------------------------------------------------   
// FillanalysisHistograms
//------------------------------------------------------------------------------                                                                        
void AnalysisFR::FillAnalysisHistograms(int icut, int i)
{
  float lep1eta = fabs(Lepton1.v.Eta());

  if (_channel == m) {

    h_Muon_loose_pt_eta_bin[icut][i]->Fill(Lepton1.v.Pt(), lep1eta, _event_weight);

    h_Muon_loose_mtw    [icut][i]->Fill(_mtw,            _event_weight);
    h_Muon_loose_pt     [icut][i]->Fill(Lepton1.v.Pt(),  _event_weight);
    h_Muon_loose_pt_bin [icut][i]->Fill(Lepton1.v.Pt(),  _event_weight);
    h_Muon_loose_eta_bin[icut][i]->Fill(lep1eta,         _event_weight);

  } else if (_channel == e) {

    h_Ele_loose_pt_eta_bin[icut][i]->Fill(Lepton1.v.Pt(), lep1eta, _event_weight);

    h_Ele_loose_mtw    [icut][i]->Fill(_mtw,            _event_weight);
    h_Ele_loose_pt     [icut][i]->Fill(Lepton1.v.Pt(),  _event_weight);
    h_Ele_loose_pt_bin [icut][i]->Fill(Lepton1.v.Pt(),  _event_weight);
    h_Ele_loose_eta_bin[icut][i]->Fill(lep1eta,         _event_weight);
  }
    
  if (Lepton1.type == Tight) {
   
    if (_channel == m) {

      h_Muon_tight_pt_eta_bin[icut][i]->Fill(Lepton1.v.Pt(), lep1eta, _event_weight);

      h_Muon_tight_mtw    [icut][i]->Fill(_mtw,           _event_weight);	
      h_Muon_tight_pt     [icut][i]->Fill(Lepton1.v.Pt(), _event_weight);
      h_Muon_tight_pt_bin [icut][i]->Fill(Lepton1.v.Pt(), _event_weight);
      h_Muon_tight_eta_bin[icut][i]->Fill(lep1eta,        _event_weight);

    } else if (_channel == e) {

      h_Ele_tight_pt_eta_bin[icut][i]->Fill(Lepton1.v.Pt(), lep1eta, _event_weight);

      h_Ele_tight_mtw    [icut][i]->Fill(_mtw,           _event_weight);
      h_Ele_tight_pt     [icut][i]->Fill(Lepton1.v.Pt(), _event_weight);
      h_Ele_tight_pt_bin [icut][i]->Fill(Lepton1.v.Pt(), _event_weight);
      h_Ele_tight_eta_bin[icut][i]->Fill(lep1eta,        _event_weight);
    }
  }
}


//------------------------------------------------------------------------------
// GetAwayJets
//------------------------------------------------------------------------------
void AnalysisFR::GetAwayJets()
{
  AnalysisJets.clear();
   
  int jetsize = std_vector_jet_pt->size();
        
  for (int j=0; j<jetsize; j++) {
      
    Jet jet_;
      
    jet_.index = j; 
      
    float pt  = std_vector_jet_pt ->at(j);
    float eta = std_vector_jet_eta->at(j);
    float phi = std_vector_jet_phi->at(j);

    if (pt < 10.) continue;

    TLorentzVector tlv;

    tlv.SetPtEtaPhiM(pt, eta, phi, 0);
      
    jet_.v = tlv;

    float dR = tlv.DeltaR(Lepton1.v);

    if (dR > 1) AnalysisJets.push_back(jet_);
  }
}


/*

   [2016/09/02] Total luminosity = 12.892/fb

   export PATH=$HOME/.local/bin:/afs/cern.ch/cms/lumi/brilconda-1.0.3/bin:$PATH

   brilcalc lumi -b "STABLE BEAMS" --normtag /afs/cern.ch/user/l/lumipro/public/normtag_file/normtag_DATACERT.json -u /pb -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-276811_13TeV_PromptReco_Collisions16_JSON.txt --hltpath "HLT_Mu8_TrkIsoVVL_v*" 

   +----------------------+-------+------+-------+-------------------+------------------+
   | hltpath              | nfill | nrun | ncms  | totdelivered(/pb) | totrecorded(/pb) |
   +----------------------+-------+------+-------+-------------------+------------------+
   | HLT_Mu8_TrkIsoVVL_v3 | 23    | 65   | 29182 | 4.285             | 4.092            |
   | HLT_Mu8_TrkIsoVVL_v4 | 30    | 111  | 69602 | 1.842             | 1.766            |
   +----------------------+-------+------+-------+-------------------+------------------+

   brilcalc lumi -b "STABLE BEAMS" --normtag /afs/cern.ch/user/l/lumipro/public/normtag_file/normtag_DATACERT.json -u /pb -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-276811_13TeV_PromptReco_Collisions16_JSON.txt --hltpath "HLT_Mu17_TrkIsoVVL_v*" 

   +-----------------------+-------+------+-------+-------------------+------------------+
   | hltpath               | nfill | nrun | ncms  | totdelivered(/pb) | totrecorded(/pb) |
   +-----------------------+-------+------+-------+-------------------+------------------+
   | HLT_Mu17_TrkIsoVVL_v2 | 23    | 65   | 29182 | 55.504            | 53.084           |
   | HLT_Mu17_TrkIsoVVL_v3 | 30    | 111  | 69602 | 114.926           | 110.757          |
   +-----------------------+-------+------+-------+-------------------+------------------+

   brilcalc lumi -b "STABLE BEAMS" --normtag /afs/cern.ch/user/l/lumipro/public/normtag_file/normtag_DATACERT.json -u /pb -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-276811_13TeV_PromptReco_Collisions16_JSON.txt --hltpath "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v*" 

   +---------------------------------------------+-------+------+-------+-------------------+------------------+
   | hltpath                                     | nfill | nrun | ncms  | totdelivered(/pb) | totrecorded(/pb) |
   +---------------------------------------------+-------+------+-------+-------------------+------------------+
   | HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v3 | 23    | 65   | 29182 | 3.184             | 3.057            |
   | HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v4 | 4     | 12   | 7544  | 0.675             | 0.646            |
   | HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v5 | 4     | 16   | 8540  | 0.751             | 0.725            |
   | HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v6 | 23    | 83   | 53518 | 4.240             | 4.084            |
   +---------------------------------------------+-------+------+-------+-------------------+------------------+

   brilcalc lumi -b "STABLE BEAMS" --normtag /afs/cern.ch/user/l/lumipro/public/normtag_file/normtag_DATACERT.json -u /pb -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-276811_13TeV_PromptReco_Collisions16_JSON.txt --hltpath "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v*" 

   +---------------------------------------------+-------+------+-------+-------------------+------------------+
   | hltpath                                     | nfill | nrun | ncms  | totdelivered(/pb) | totrecorded(/pb) |
   +---------------------------------------------+-------+------+-------+-------------------+------------------+
   | HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v3 | 23    | 65   | 29182 | 17.578            | 16.838           |
   | HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v4 | 4     | 12   | 7544  | 3.247             | 3.112            |
   | HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v5 | 4     | 16   | 8540  | 3.584             | 3.459            |
   | HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v6 | 23    | 83   | 53518 | 19.652            | 18.937           |
   +---------------------------------------------+-------+------+-------+-------------------+------------------+

*/
