#include "ViennaTool/interface/TNtupleAnalyzer.h"

#include <iostream>
#include <sstream>

using namespace std;

TNtupleAnalyzer::TNtupleAnalyzer()
{
  cout << "Instance of TNtupleAnalyzer" << endl;
}

TNtupleAnalyzer::~TNtupleAnalyzer()
{
}

void TNtupleAnalyzer::loadFile(TString filename, TString chain)
{
  tchain = new TChain(chain);
  tchain->Add(filename);

  if       (filename==datafile) this->curr_sample="data";
  else if  (filename==DYfile || filename==DY_NJfile) this->curr_sample="DY";
  else if  (filename==TTfile) this->curr_sample="TT";
  else if  (filename==Wjetsfile) this->curr_sample="Wjets";
  else if  (filename==QCDfile) this->curr_sample="QCD";
  else if  (filename==VVfile) this->curr_sample="VV";
  else this->curr_sample="unknown";
  
  int nev=0;
  if (chain=="TauCheck"){  event = new NtupleClass(tchain); if (DEBUG) nev=event->fChain->GetEntries(); }
  else std::cout << "unknown chain!" << std::endl;
  if (DEBUG) std::cout<<"File: "<<filename<<" loaded" << ", #events: "<< nev << std::endl;
}

void TNtupleAnalyzer::select(const TString preselectionFile, const Int_t mode)
{
  TFile *fout_file = new TFile(preselectionFile,"RECREATE");
  TTree* t_Events=new TTree("Events","Events");
  this->initOutfileTree(t_Events);
  //Commence loop over tree
  Int_t nentries = Int_t(event->fChain->GetEntries());
  Int_t pc=0;
  cout<<"Producing " << preselectionFile << " , processing "<<nentries<<" events."<< endl;
  //for (Int_t jentry=0; jentry<10000;jentry++) {
  for (Int_t jentry=0; jentry<nentries;jentry++){
    if (jentry%1000000 == 0) cout << "Event " << jentry << endl;
    event->GetEntry(jentry);
    Int_t ntau=setTreeValues(preselectionFile, mode); //preselection happens in there now
    if (ntau>=1){ t_Events->Fill(); pc++; }
    if(CHAN==kTAU && !COINFLIP){
      ntau=0;
      ntau=setTreeValues(preselectionFile, mode, 2);
      if(ntau>=1){ t_Events->Fill(); pc++; }
    }
  }// End loop over all events
  
  cout<<pc<<" events passed preselection."<< endl;
  t_Events->Write();
  fout_file->Close();
  
}

void TNtupleAnalyzer::closeFile()
{
  delete event;
  delete tchain;
}

Int_t TNtupleAnalyzer::setTreeValues(const TString preselectionFile, const Int_t mode, Int_t whichTau)
{


  if(CHAN==kMU && !event->trg_singlemuon) return 0;
  if(CHAN==kEL && !event->trg_singleelectron) return 0;
  if(CHAN==kTAU && !(event->trg_doubletau) ) return 0;
  if(CHAN==kMU && event->Flag_badMuons) return 0;
  if(CHAN==kMU && event->Flag_duplicateMuons) return 0;
  
  TLorentzVector vec1, vec2, vec;
  //////////////////////////////////////////////////////////////////////////

  weight=1.;
  if(!preselectionFile.Contains("preselection_data"))weight = event->puweight*event->effweight*event->stitchedWeight*luminosity*event->genweight*event->antilep_tauscaling*event->topWeight_run1*event->zPtReweightWeight;

  if( CHAN == kTAU && !preselectionFile.Contains("preselection_data") ){
    if(event->gen_match_1 == 5 && event->byTightIsolationMVArun2v1DBoldDMwLT_1) weight *= 0.95;
    else if(event->gen_match_1 == 5 && event->byVLooseIsolationMVArun2v1DBoldDMwLT_1 ) weight *= 0.99;
    if(event->gen_match_2 == 5 && event->byTightIsolationMVArun2v1DBoldDMwLT_2) weight *= 0.95;
    else if(event->gen_match_2 == 5 && event->byVLooseIsolationMVArun2v1DBoldDMwLT_2 ) weight *= 0.99;
  }
  if( CHAN != kTAU && !preselectionFile.Contains("preselection_data") ){
    if(event->gen_match_2 == 5 && event->byTightIsolationMVArun2v1DBoldDMwLT_2) weight *= 0.95;
    else if(event->gen_match_2 == 5 && event->byVLooseIsolationMVArun2v1DBoldDMwLT_2 ) weight *= 0.99;
  }
  
  weight_sf=weight;
  
  if(CHAN==kTAU && !COINFLIP){
    weight=weight*0.5;
    weight_sf=weight_sf*0.5;
  }
  
  passes3LVeto=event->passesThirdLepVeto;
  if ( CHAN == kMU  ) passesDLVeto=event->passesDiMuonVeto;
  if ( CHAN == kEL  ) passesDLVeto=event->passesDiElectronVeto;
  if ( CHAN == kTAU ) passesDLVeto=1;

  bpt_1=event->bpt_1;
  bpt_2=event->bpt_2;
  nbtag=event->nbtag;
  njets=event->njets;
  mjj=event->mjj;
  mvamet=event->mvamet;
  met=event->met;
  jdeta=event->jdeta;
  njetingap20=event->njetingap20;
  mu2_iso=-999;  
  m_leplep=-999;  
  lep_dR=-999;
  mt_leplep=-999;
  n_iso_lep=0;
  n_iso_otherLep=0;

  lep_pt=event->pt_1;
  lep_eta=event->eta_1;
  lep_phi=event->phi_1;
  lep_iso=event->iso_1;
  lep_q=event->q_1;

  otherLep_pt=-999;
  otherLep_eta=-999;
  otherLep_iso=-999;
  otherLep_q=-999;

  std::vector<double> *m_otherLep_pt;
  std::vector<double> *m_otherLep_eta;
  std::vector<double> *m_otherLep_phi;
  std::vector<double> *m_otherLep_m;
  std::vector<double> *m_otherLep_iso;
  std::vector<int>    *m_otherLep_q;
  int nOtherLep;

  std::vector<double> *m_lep_pt;
  std::vector<double> *m_lep_eta;
  std::vector<double> *m_lep_phi;
  std::vector<double> *m_lep_m;
  std::vector<double> *m_lep_iso;
  std::vector<int>    *m_lep_q;
  int nLep;

  if ( CHAN == kMU || CHAN == kTAU ){  //for now: in kTAU, fill lep with muons and otherLep with electrons
    m_lep_pt=event->addmuon_pt;
    m_lep_eta=event->addmuon_eta;
    m_lep_phi=event->addmuon_phi;
    m_lep_m=event->addmuon_m;
    m_lep_iso=event->addmuon_iso;
    m_lep_q=event->addmuon_q;
    nLep=event->nadditionalMu;
    m_otherLep_pt=event->addele_pt;
    m_otherLep_eta=event->addele_eta;
    m_otherLep_phi=event->addele_phi;
    m_otherLep_m=event->addele_m;
    m_otherLep_iso=event->addele_iso;
    m_otherLep_q=event->addele_q;
    nOtherLep=event->nadditionalEle;
  } else if ( CHAN == kEL ){
    m_lep_pt=event->addele_pt;
    m_lep_eta=event->addele_eta;
    m_lep_phi=event->addele_phi;
    m_lep_m=event->addele_m;
    m_lep_iso=event->addele_iso;
    m_lep_q=event->addele_q;
    nLep=event->nadditionalEle;
    m_otherLep_pt=event->addmuon_pt;
    m_otherLep_eta=event->addmuon_eta;
    m_otherLep_phi=event->addmuon_phi;
    m_otherLep_m=event->addmuon_m;
    m_otherLep_iso=event->addmuon_iso;
    m_otherLep_q=event->addmuon_q;
    nOtherLep=event->nadditionalMu;
  }

  std::vector<double> v_otherLep_eta; std::vector<double> v_otherLep_phi;
  std::vector<double> v_lep_pt;       std::vector<double> v_lep_eta;      std::vector<double> v_lep_phi;  std::vector<double> v_lep_m; std::vector<double> v_lep_q;
  std::vector<double> v_lep_eta_iso;  std::vector<double> v_lep_phi_iso;

  double m_iso=1e6;
  for (int i=0; i<nOtherLep; i++){

    if (   m_otherLep_iso->at(i)<LEP_ISO_CUT  &&  m_otherLep_pt->at(i)>LEP_PT_CUT  &&  fabs(m_otherLep_eta->at(i)) < (TAU_ETA_CUT+0.1)    ){  //TAU eta+0.1 since we want to check overlap with taus!
      v_otherLep_eta.push_back( m_otherLep_eta->at(i) );
      v_otherLep_phi.push_back( m_otherLep_phi->at(i) );
    }

    if ( ! ( m_otherLep_pt->at(i) > LEP_PT_CUT && fabs(m_otherLep_eta->at(i)) < LEP_ETA_CUT ) ) continue;
    if (   m_otherLep_iso->at(i)<LEP_ISO_CUT  )  n_iso_otherLep++;

    if ( m_otherLep_iso->at(i) < m_iso ){
      otherLep_pt=m_otherLep_pt->at(i);
      otherLep_eta=m_otherLep_eta->at(i);
      otherLep_phi=m_otherLep_phi->at(i);
      otherLep_iso=m_otherLep_iso->at(i);
      otherLep_q=m_otherLep_q->at(i);
      m_iso=otherLep_iso;
    }
  }

  Double_t lep1_eta=-999, lep1_phi=-999, lep2_eta=-999, lep2_phi=-999;

  //for dR calculations
  if ( event->iso_1 < LEP_ISO_CUT && event->pt_1 > LEP_PT_CUT && fabs( event->eta_1 ) < (TAU_ETA_CUT+0.1) && CHAN != kTAU ){
    v_lep_eta_iso.push_back( event->eta_1 );
    v_lep_phi_iso.push_back( event->phi_1 );
  }

  //to select m(ll)~m(Z) events
  if ( event->pt_1 > LEP_PT_CUT  && fabs( event->eta_1 ) < LEP_ETA_CUT && CHAN != kTAU ){
    v_lep_pt.push_back(  event->pt_1 );
    v_lep_eta.push_back( event->eta_1 );
    v_lep_phi.push_back( event->phi_1 );
    v_lep_m.push_back(  event->pt_1 );
    v_lep_q.push_back(  event->pt_1 );
    if ( event->iso_1 < LEP_ISO_CUT ) n_iso_lep++;
  }

  for (int i=0; i<nLep; i++){

    //for dR calculations
    if (   m_lep_iso->at(i)<LEP_ISO_CUT  &&  m_lep_pt->at(i)>LEP_PT_CUT  &&  fabs(m_lep_eta->at(i)) < (TAU_ETA_CUT+0.1)    ){  //TAU eta+0.1 since we want to check overlap with taus!
      v_lep_eta_iso.push_back( m_lep_eta->at(i) );
      v_lep_phi_iso.push_back( m_lep_phi->at(i) );
    }

    //to select m(ll)~m(Z) events
    if (   m_lep_pt->at(i)>LEP_PT_CUT  &&  fabs(m_lep_eta->at(i)) < LEP_ETA_CUT  ){  //TAU eta+0.1 since we want to check overlap with taus!
      v_lep_pt.push_back(  m_lep_pt->at(i) );
      v_lep_eta.push_back( m_lep_eta->at(i) );
      v_lep_phi.push_back( m_lep_phi->at(i) );
      v_lep_m.push_back(   m_lep_m->at(i) );
      v_lep_q.push_back(   m_lep_q->at(i) );
      if ( m_lep_iso->at(i) < LEP_ISO_CUT ) n_iso_lep++;
    }

  }

  //select mZ candidate
  for (unsigned i=0; i<v_lep_eta.size(); i++){
    for (unsigned j=i+1; j<v_lep_eta.size(); j++){
      if ( ( v_lep_q.at(i) * v_lep_q.at(j) ) >= 0 ) continue;

      double m_dR=calcDR( v_lep_eta.at(i) , v_lep_phi.at(i) , v_lep_eta.at(j) , v_lep_phi.at(j) );
      if ( m_dR < DR_LEP_CUT ) continue;

      vec1.SetPtEtaPhiM( v_lep_pt.at(i) , v_lep_eta.at(i) , v_lep_phi.at(i) , v_lep_m.at(i) );
      vec2.SetPtEtaPhiM( v_lep_pt.at(j) , v_lep_eta.at(j) , v_lep_phi.at(j) , v_lep_m.at(j) );
      vec=vec1+vec2;

      if ( fabs(vec.M()-MZ) < fabs(m_leplep-MZ) ){
        m_leplep=vec.M();
        lep_dR=m_dR;
        lep1_eta=v_lep_eta.at(i);
        lep2_eta=v_lep_eta.at(j);
        lep1_phi=v_lep_phi.at(i);
        lep2_phi=v_lep_phi.at(j);

        Double_t deltaPhi=TVector2::Phi_mpi_pi( lep1_phi - lep2_phi );
        mt_leplep=sqrt(2*v_lep_pt.at(i)*v_lep_pt.at(j)*(1-TMath::Cos(deltaPhi)));
      }

    }
  }

  alltau_pt->resize(0);
  alltau_eta->resize(0);
  alltau_phi->resize(0);
  alltau_q->resize(0);
  alltau_decay->resize(0);
  alltau_beta->resize(0);
  alltau_mediumBeta->resize(0);
  alltau_vlooseMVA->resize(0);
  alltau_looseMVA->resize(0);
  alltau_mediumMVA->resize(0);
  alltau_tightMVA->resize(0);
  alltau_vtightMVA->resize(0);
  alltau_lepVeto->resize(0);
  alltau_gen_match->resize(0);
  alltau_dRToLep->resize(0);
  alltau_dRToOtherLep->resize(0);
  alltau_dRToB->resize(0);
  alltau_mvis->resize(0);
  alltau_mt->resize(0);
  alltau_mt2->resize(0);
  alltau_svfit->resize(0);
  alltau_Zpt->resize(0);

  float m_tau_pt_cut=TAU_PT_CUT;
  float m_tau_eta_cut=TAU_ETA_CUT;
  if ( CHAN == kTAU ){ m_tau_pt_cut=TAU_PT_CUT_TT; m_tau_eta_cut=TAU_ETA_CUT_TT; }

  double dR; int m;
  Double_t dR1, dR2;
  for (int i=0; i<event->nadditionalTau; i++){
    if ( ! (event->addtau_pt->at(i) > m_tau_pt_cut ) ) continue;
    if ( ! ( fabs(event->addtau_eta->at(i)) < m_tau_eta_cut ) ) continue;

    if (event->addtau_passesTauLepVetos->at(i) == 0) continue;
    m=event->addtau_decayMode->at(i);
    if ( !( (m>=0&&m<=4)||(m>=10&&m<=14) ) ) continue;
    dR=this->calcDR( event->eta_1, event->phi_1, event->addtau_eta->at(i), event->addtau_phi->at(i) );
    if ( CHAN!=kTAU && dR<0.5 ) continue;

    alltau_pt->push_back(event->addtau_pt->at(i));
    alltau_eta->push_back(event->addtau_eta->at(i));
    alltau_phi->push_back(event->addtau_phi->at(i));
    alltau_q->push_back(event->addtau_q->at(i));
    alltau_decay->push_back(m);
    alltau_beta->push_back(event->addtau_byCombinedIsolationDeltaBetaCorrRaw3Hits->at(i));
    alltau_mediumBeta->push_back(event->addtau_byMediumCombinedIsolationDeltaBetaCorr3Hits->at(i));
    alltau_vlooseMVA->push_back( event->addtau_byVLooseIsolationMVArun2v1DBoldDMwLT->at(i));
    alltau_looseMVA->push_back( event->addtau_byLooseIsolationMVArun2v1DBoldDMwLT->at(i));
    alltau_mediumMVA->push_back(event->addtau_byMediumIsolationMVArun2v1DBoldDMwLT->at(i));
    alltau_tightMVA->push_back( event->addtau_byTightIsolationMVArun2v1DBoldDMwLT->at(i));
    alltau_vtightMVA->push_back( event->addtau_byVTightIsolationMVArun2v1DBoldDMwLT->at(i));
    alltau_lepVeto->push_back(event->addtau_passesTauLepVetos->at(i));
    alltau_gen_match->push_back(event->addtau_gen_match->at(i));
    alltau_mvis->push_back(event->addtau_mvis->at(i));
    if(useMVAMET)alltau_mt->push_back(event->mt_1); //mt of lepton and MVAMET
    else alltau_mt->push_back(event->pfmt_1); //mt of lepton and PFMET
    if(useMVAMET)alltau_mt2->push_back(event->addtau_mt->at(i));
    else alltau_mt2->push_back(event->pfmt_2);  //no addtaupfmt in addtau collection
    if(use_svfit)alltau_svfit->push_back(event->m_sv); //FIXME: no svfit in addtau collection so far
    else alltau_svfit->push_back(0.);
    TLorentzVector leg2; leg2.SetPtEtaPhiM(event->addtau_pt->at(i),event->addtau_eta->at(i),event->addtau_phi->at(i),event->addtau_m->at(i));
    TLorentzVector leg1; leg1.SetPtEtaPhiM(event->pt_1,event->eta_1,event->phi_1,event->m_1);
    TLorentzVector Emiss;
    if(useMVAMET) Emiss.SetPtEtaPhiM(event->mvamet,0,event->mvametphi,0);
    else Emiss.SetPtEtaPhiM(event->met,0,event->metphi,0);
    alltau_Zpt->push_back( (leg1+leg2+Emiss).Pt() );

    dR1=1e6;
    double m_dR1=dR1;
    for (unsigned iL=0; iL<v_lep_eta_iso.size(); iL++){
      m_dR1=calcDR( event->addtau_eta->at(i),event->addtau_phi->at(i),v_lep_eta_iso.at(iL),v_lep_phi_iso.at(iL) );
      if ( dR1>m_dR1 ) dR1=m_dR1;
    }
    alltau_dRToLep->push_back( dR1 );

    dR1=1e6;
    m_dR1=dR1;
    for (unsigned iL=0; iL<v_otherLep_eta.size(); iL++){
      m_dR1=calcDR( event->addtau_eta->at(i),event->addtau_phi->at(i),v_otherLep_eta.at(iL),v_otherLep_phi.at(iL) );
      if ( dR1>m_dR1 ) dR1=m_dR1;
    }
    alltau_dRToOtherLep->push_back( dR1 );

    dR1=calcDR(event->addtau_eta->at(i),event->addtau_phi->at(i),event->beta_1,event->bphi_1);
    dR2=calcDR(event->addtau_eta->at(i),event->addtau_phi->at(i),event->beta_2,event->bphi_2);
    alltau_dRToB->push_back( min(dR1,dR2) );

  }

  dR=this->calcDR( event->eta_1, event->phi_1, event->eta_2, event->phi_2 ); //should always be >0.5 ... but to be safe
  tau_iso_ind=-1;

  //for tt: insert both _1 and _2, but in random order (i.e. not iso- or pt-ordered!!); other channels: simply insert _2
  Int_t TT_AS_LEP=1;
  if ( CHAN == kTAU ){
    if(COINFLIP) {
      if ( r3.Uniform(2)>1.0 ) TT_AS_LEP=2; //if uniform random number in (0;2) is >1, then use it as 2nd tau as lep; otherwise 1st. For et/mt, TT_AS_LEP is always =1
    }
    else if(!COINFLIP){
      if(whichTau==2) TT_AS_LEP=2;
    }
  }
    
  float decay=event->decayMode_2;
  if ( (event->passesTauLepVetos) && ( (decay>=0&&decay<=4)||(decay>=10&&decay<=14)   ) && (dR>0.5) && (event->pt_2 > m_tau_pt_cut ) && (fabs(event->eta_2) < m_tau_eta_cut) && (TT_AS_LEP==1) ){
    Int_t tpos=0;
    tau_iso_ind=tpos;
    alltau_pt->insert(alltau_pt->begin()+tpos,event->pt_2);
    alltau_eta->insert(alltau_eta->begin()+tpos,event->eta_2);
    alltau_phi->insert(alltau_phi->begin()+tpos,event->phi_2);
    alltau_q->insert(alltau_q->begin()+tpos,event->q_2);
    alltau_decay->insert(alltau_decay->begin()+tpos,decay);
    alltau_beta->insert(alltau_beta->begin()+tpos,event->byCombinedIsolationDeltaBetaCorrRaw3Hits_2);
    alltau_mediumBeta->insert(alltau_mediumBeta->begin()+tpos,event->byMediumCombinedIsolationDeltaBetaCorr3Hits_2);
    alltau_vlooseMVA->insert(alltau_vlooseMVA->begin()+tpos, event->byVLooseIsolationMVArun2v1DBoldDMwLT_2 );
    alltau_looseMVA->insert(alltau_looseMVA->begin()+tpos, event->byLooseIsolationMVArun2v1DBoldDMwLT_2 );
    alltau_mediumMVA->insert(alltau_mediumMVA->begin()+tpos, event->byMediumIsolationMVArun2v1DBoldDMwLT_2 );
    alltau_tightMVA->insert(alltau_tightMVA->begin()+tpos, event->byTightIsolationMVArun2v1DBoldDMwLT_2 );
    alltau_vtightMVA->insert(alltau_vtightMVA->begin()+tpos, event->byVTightIsolationMVArun2v1DBoldDMwLT_2 );
    alltau_lepVeto->insert(alltau_lepVeto->begin()+tpos,event->passesTauLepVetos);
    alltau_gen_match->insert(alltau_gen_match->begin()+tpos,event->gen_match_2);
    alltau_mvis->insert(alltau_mvis->begin()+tpos,event->m_vis);
    if(useMVAMET) alltau_mt->insert(alltau_mt->begin()+tpos,event->mt_1);
    else alltau_mt->insert(alltau_mt->begin()+tpos,event->pfmt_1);
    if(useMVAMET) alltau_mt2->insert(alltau_mt2->begin()+tpos,event->mt_2);
    else alltau_mt2->insert(alltau_mt2->begin()+tpos,event->pfmt_2);
    if(use_svfit)alltau_svfit->insert(alltau_svfit->begin()+tpos,event->m_sv);
    else alltau_svfit->insert(alltau_svfit->begin()+tpos,0.);
    TLorentzVector leg2; leg2.SetPtEtaPhiM(event->pt_2,event->eta_2,event->phi_2,event->m_2);
    TLorentzVector leg1; leg1.SetPtEtaPhiM(event->pt_1,event->eta_1,event->phi_1,event->m_1);
    TLorentzVector Emiss;
    if(useMVAMET) Emiss.SetPtEtaPhiM(event->mvamet,0,event->mvametphi,0);
    else Emiss.SetPtEtaPhiM(event->met,0,event->metphi,0);
    alltau_Zpt->insert( alltau_Zpt->begin()+tpos,(leg1+leg2+Emiss).Pt() );
      
    dR1=1e6;
    double m_dR1=dR1;
    for (unsigned iL=0; iL<v_lep_eta_iso.size(); iL++){
      m_dR1=calcDR( event->eta_2,event->phi_2,v_lep_eta_iso.at(iL),v_lep_phi_iso.at(iL) );
      if ( dR1>m_dR1 ) dR1=m_dR1;
    }
    alltau_dRToLep->insert(alltau_dRToLep->begin()+tpos , dR1 );

    dR1=1e6;
    m_dR1=dR1;
    for (unsigned iL=0; iL<v_otherLep_eta.size(); iL++){
      m_dR1=calcDR( event->eta_2,event->phi_2,v_otherLep_eta.at(iL),v_otherLep_phi.at(iL) );
      if ( dR1>m_dR1 ) dR1=m_dR1;
    }
    alltau_dRToOtherLep->insert(alltau_dRToOtherLep->begin()+tpos, dR1 );
    
    dR1=calcDR(event->eta_2,event->phi_2,event->beta_1,event->bphi_1);
    dR2=calcDR(event->eta_2,event->phi_2,event->beta_2,event->bphi_2);
    alltau_dRToB->insert(alltau_dRToB->begin()+tpos, min(dR1,dR2) );
  } else if ( CHAN!=kTAU ){ //if most iso tau is not ok, then do not use event if this flag is set
    return 0;
  }

  //now, for _1, the same as above for the tt chan. FIXME very ugly to duplicate code, should be moved to a dedicated method
  if ( TT_AS_LEP == 2 ){ //only possible for kTAU
    float decay=event->decayMode_1;
    if ( (event->passesTauLepVetos) && ( (decay>=0&&decay<=4)||(decay>=10&&decay<=14)   ) && (dR>0.5) && (event->pt_1 > m_tau_pt_cut ) && ( fabs(event->eta_1) < m_tau_eta_cut ) ){
      Int_t tpos=0;
      tau_iso_ind=tpos;    
      alltau_pt->insert(alltau_pt->begin()+tpos,event->pt_1);
      alltau_eta->insert(alltau_eta->begin()+tpos,event->eta_1);
      alltau_phi->insert(alltau_phi->begin()+tpos,event->phi_1);
      alltau_q->insert(alltau_q->begin()+tpos,event->q_1);
      alltau_decay->insert(alltau_decay->begin()+tpos,decay);
      alltau_beta->insert(alltau_beta->begin()+tpos,event->byCombinedIsolationDeltaBetaCorrRaw3Hits_1);
      //  alltau_looseBeta->insert(alltau_looseBeta->begin()+tpos,event->byLooseCombinedIsolationDeltaBetaCorr3Hits_1);
      alltau_mediumBeta->insert(alltau_mediumBeta->begin()+tpos,event->byMediumCombinedIsolationDeltaBetaCorr3Hits_1);
      //  alltau_tightBeta->insert(alltau_tightBeta->begin()+tpos,event->byTightCombinedIsolationDeltaBetaCorr3Hits_1);
      alltau_vlooseMVA->insert(alltau_vlooseMVA->begin()+tpos, event->byVLooseIsolationMVArun2v1DBoldDMwLT_1 );
      alltau_looseMVA->insert(alltau_looseMVA->begin()+tpos, event->byLooseIsolationMVArun2v1DBoldDMwLT_1 );
      alltau_mediumMVA->insert(alltau_mediumMVA->begin()+tpos, event->byMediumIsolationMVArun2v1DBoldDMwLT_1 );
      alltau_tightMVA->insert(alltau_tightMVA->begin()+tpos, event->byTightIsolationMVArun2v1DBoldDMwLT_1 );
      alltau_vtightMVA->insert(alltau_vtightMVA->begin()+tpos, event->byVTightIsolationMVArun2v1DBoldDMwLT_1 );
      alltau_lepVeto->insert(alltau_lepVeto->begin()+tpos,event->passesTauLepVetos);
      alltau_gen_match->insert(alltau_gen_match->begin()+tpos,event->gen_match_1);
      alltau_mvis->insert(alltau_mvis->begin()+tpos,event->m_vis);
      if(useMVAMET)alltau_mt->insert(alltau_mt->begin()+tpos,event->mt_2);
      else alltau_mt->insert(alltau_mt->begin()+tpos,event->pfmt_2);
      if(useMVAMET)alltau_mt2->insert(alltau_mt2->begin()+tpos,event->mt_1);
      else alltau_mt2->insert(alltau_mt2->begin()+tpos,event->pfmt_1);
      if(use_svfit)alltau_svfit->insert(alltau_svfit->begin()+tpos,event->m_sv);
      else alltau_svfit->insert(alltau_svfit->begin()+tpos,0.);
      TLorentzVector leg2; leg2.SetPtEtaPhiM(event->pt_2,event->eta_2,event->phi_2,event->m_2);
      TLorentzVector leg1; leg1.SetPtEtaPhiM(event->pt_1,event->eta_1,event->phi_1,event->m_1);
      TLorentzVector Emiss;
      if(useMVAMET) Emiss.SetPtEtaPhiM(event->mvamet,0,event->mvametphi,0);
      else Emiss.SetPtEtaPhiM(event->met,0,event->metphi,0);
      alltau_Zpt->insert( alltau_Zpt->begin()+tpos,(leg1+leg2+Emiss).Pt() );
      
      dR1=1e6;
      double m_dR1=dR1;
      for (unsigned iL=0; iL<v_lep_eta_iso.size(); iL++){
	m_dR1=calcDR( event->eta_1,event->phi_1,v_lep_eta_iso.at(iL),v_lep_phi_iso.at(iL) );
	if ( dR1>m_dR1 ) dR1=m_dR1;
      }
      alltau_dRToLep->insert(alltau_dRToLep->begin()+tpos , dR1 );
      
      dR1=1e6;
      m_dR1=dR1;
      for (unsigned iL=0; iL<v_otherLep_eta.size(); iL++){
	m_dR1=calcDR( event->eta_1,event->phi_1,v_otherLep_eta.at(iL),v_otherLep_phi.at(iL) );
	if ( dR1>m_dR1 ) dR1=m_dR1;
      }
      alltau_dRToOtherLep->insert(alltau_dRToOtherLep->begin()+tpos, dR1 );
      
      dR1=calcDR(event->eta_1,event->phi_1,event->beta_1,event->bphi_1);
      dR2=calcDR(event->eta_1,event->phi_1,event->beta_2,event->bphi_2);
      alltau_dRToB->insert(alltau_dRToB->begin()+tpos, min(dR1,dR2) );
    } else{
      return 0;
    }
  }

  lep_vloose = 0;
  lep_loose = 0;
  lep_medium = 0;

  if ( CHAN == kTAU ){
    //now: take "other tau" (as determined by the random number above) as "lep"
    if ( TT_AS_LEP == 2 ){
      lep_dR=-99; //needed for mZ in ee/mumu CR; not needed for tautau
      lep_pt=event->pt_2;
      lep_eta=event->eta_2;
      lep_phi=event->phi_2;
      lep_q=event->q_2;
      //lep_iso = ( (calcVTightFF==1 && event->byVTightIsolationMVArun2v1DBoldDMwLT_2==1) || (calcVTightFF==0 && event->byTightIsolationMVArun2v1DBoldDMwLT_2==1) )  ? 10 : 0;
      lep_iso = event->byTightIsolationMVArun2v1DBoldDMwLT_2==1 ? 10 : 0;
      lep_vloose = ( event->byVLooseIsolationMVArun2v1DBoldDMwLT_2 == 1 ) ? 1 : 0;
      lep_loose = ( event->byLooseIsolationMVArun2v1DBoldDMwLT_2 == 1 ) ? 1 : 0;
      lep_medium = ( event->byMediumIsolationMVArun2v1DBoldDMwLT_2 == 1 ) ? 1 : 0;
    } else{
      lep_dR=-99; //needed for mZ in ee/mumu CR; not needed for tautau
      lep_pt=event->pt_1;
      lep_eta=event->eta_1;
      lep_phi=event->phi_1;
      lep_q=event->q_1;
      //lep_iso = ( (calcVTightFF==1 && event->byVTightIsolationMVArun2v1DBoldDMwLT_1==1) || (calcVTightFF==0 && event->byTightIsolationMVArun2v1DBoldDMwLT_1==1) )  ? 10 : 0;
      lep_iso = event->byTightIsolationMVArun2v1DBoldDMwLT_1==1 ? 10 : 0;
      lep_vloose = ( event->byVLooseIsolationMVArun2v1DBoldDMwLT_1 == 1 ) ? 1 : 0;
      lep_loose = ( event->byLooseIsolationMVArun2v1DBoldDMwLT_1 == 1 ) ? 1 : 0;
      lep_medium = ( event->byMediumIsolationMVArun2v1DBoldDMwLT_1 == 1 ) ? 1 : 0;
    }
  }

  if ( !this->fitsGenCategory(mode) ) return 0;

  return alltau_pt->size(); //if 0: no tau that passes lep veto and DMF!
}

//assumes v_val is ordered in pt; otherwise it will not work!
Int_t TNtupleAnalyzer::findPos(const Double_t val, const vector<Double_t> *v_val)
{
  for (unsigned i=0; i<v_val->size(); i++){
    if ( val>v_val->at(i) ) return i; //beginning or in between
  } 
  return v_val->size(); //at the end, can also be 0
}

//checks preselection and (for DY) gen matches
Int_t TNtupleAnalyzer::fitsGenCategory(const Int_t mode)
{

  if ( ! alltau_gen_match->size() ) return 0;
  Int_t gm=alltau_gen_match->at(0);
 
  if        (mode & _DY && mode & _TTAU) {
    if (gm==tauH) return 1;
  } else if (mode & _DY && mode & _JTAU) {
    if (gm==realJet) return 1;
  } else if (mode & _DY && mode & _LTAU) {
    if ( (gm!=tauH)     && (gm!=realJet)  ) return 1;
  } else if (mode & _TT && mode & _TTAU) {
    if (gm==tauH) return 1;
  } else if (mode & _TT && mode & _JTAU) {
    if (gm==realJet) return 1;
  } else if (mode & _TT && mode & _LTAU) {
    if ( (gm!=tauH)     && (gm!=realJet)  ) return 1;
  } else if (mode & _VV && mode &_TTAU) {
    if (gm==tauH) return 1;
  } else if (mode & _VV && mode & _JTAU) {
    if (gm==realJet) return 1;
  } else if (mode & _VV && mode & _LTAU) {
    if ( (gm!=tauH)     && (gm!=realJet)  ) return 1;
  } else if (mode & _QCD) {
    return 1;
  } else {
    return 1;
  }

  return 0;
}

Double_t TNtupleAnalyzer::calcDR(const Double_t eta1, const Double_t phi1, const Double_t eta2, const Double_t phi2)
{
  Double_t dEta=eta1-eta2;
  Double_t dPhi=TVector2::Phi_mpi_pi(phi1-phi2);
  return(TMath::Sqrt(dEta*dEta+dPhi*dPhi));
}

 
void TNtupleAnalyzer::initOutfileTree(TTree* tree)
{
  tree->Branch("weight",&weight);
  tree->Branch("sf",&sf);
  tree->Branch("weight_sf",&weight_sf);
  tree->Branch("passes3LVeto",&passes3LVeto);
  tree->Branch("passesDLVeto",&passesDLVeto);
  tree->Branch("bpt_1",&bpt_1);
  tree->Branch("bpt_2",&bpt_2);
  tree->Branch("njets",&njets);
  tree->Branch("nbtag",&nbtag);
  tree->Branch("mjj",&mjj);
  tree->Branch("jdeta",&jdeta);
  tree->Branch("njetingap20", &njetingap20);
  tree->Branch("mvamet",&mvamet);
  tree->Branch("met",&met);
  tree->Branch("m_leplep",&m_leplep);
  tree->Branch("lep_dR",&lep_dR);
  tree->Branch("mt_leplep",&mt_leplep);
  tree->Branch("otherLep_pt" ,&otherLep_pt);
  tree->Branch("otherLep_eta",&otherLep_eta);  
  tree->Branch("otherLep_q"  ,&otherLep_q);
  tree->Branch("otherLep_iso",&otherLep_iso);
  tree->Branch("lep_pt" ,&lep_pt);
  tree->Branch("lep_eta",&lep_eta);
  tree->Branch("lep_phi",&lep_phi);
  tree->Branch("lep_q"  ,&lep_q);
  tree->Branch("lep_iso",&lep_iso);
  tree->Branch("lep_vloose",&lep_vloose);
  tree->Branch("lep_loose",&lep_loose);
  tree->Branch("lep_medium",&lep_medium);
  tree->Branch("n_iso_lep",&n_iso_lep);
  tree->Branch("n_iso_otherLep",&n_iso_otherLep);
  tree->Branch("alltau_pt",&alltau_pt);
  tree->Branch("alltau_eta",&alltau_eta);
  tree->Branch("alltau_phi",&alltau_phi);
  tree->Branch("alltau_q",&alltau_q);
  tree->Branch("alltau_decay",&alltau_decay);
  tree->Branch("alltau_beta",&alltau_beta);
  tree->Branch("alltau_mediumBeta",&alltau_mediumBeta);
  tree->Branch("alltau_vlooseMVA",&alltau_vlooseMVA);
  tree->Branch("alltau_looseMVA",&alltau_looseMVA);
  tree->Branch("alltau_mediumMVA",&alltau_mediumMVA);
  tree->Branch("alltau_tightMVA",&alltau_tightMVA);
  tree->Branch("alltau_vtightMVA",&alltau_vtightMVA);
  tree->Branch("alltau_lepVeto",&alltau_lepVeto);
  tree->Branch("alltau_gen_match",&alltau_gen_match);
  tree->Branch("alltau_dRToLep",&alltau_dRToLep);
  tree->Branch("alltau_dRToOtherLep",&alltau_dRToOtherLep);
  tree->Branch("alltau_dRToB",&alltau_dRToB);
  tree->Branch("alltau_mvis",&alltau_mvis);
  tree->Branch("alltau_mt",&alltau_mt);
  tree->Branch("alltau_mt2",&alltau_mt2);
  tree->Branch("alltau_svfit",&alltau_svfit);
  tree->Branch("alltau_Zpt", &alltau_Zpt);
  tree->Branch("tau_iso_ind",&tau_iso_ind);

  alltau_pt=new vector<Double_t>;
  alltau_eta=new vector<Double_t>;
  alltau_phi=new vector<Double_t>;
  alltau_q=new vector<Double_t>;
  alltau_decay=new vector<Int_t>;
  alltau_beta=new vector<Double_t>;
  alltau_mediumBeta=new vector<Int_t>;
  alltau_vlooseMVA=new vector<Int_t>;
  alltau_looseMVA=new vector<Int_t>;
  alltau_mediumMVA=new vector<Int_t>;
  alltau_tightMVA=new vector<Int_t>;
  alltau_vtightMVA=new vector<Int_t>;
  alltau_lepVeto=new vector<Int_t>;
  alltau_gen_match=new vector<Int_t>;
  alltau_dRToLep=new vector<Double_t>;
  alltau_dRToOtherLep=new vector<Double_t>;
  alltau_dRToB=new vector<Double_t>;
  alltau_mvis=new vector<Double_t>;
  alltau_mt=new vector<Double_t>;
  alltau_mt2=new vector<Double_t>;
  alltau_svfit=new vector<Double_t>;
  alltau_Zpt=new vector<Double_t>;

}
