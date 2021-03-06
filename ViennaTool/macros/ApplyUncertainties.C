#include "ViennaTool/interface/FFCalculator.h"
#include "ViennaTool/src/FFCalculator.cc"

#include <iostream>
#include <string>

using namespace std;

void ApplyFF() {

  std::cout << std::endl << "*******************************************************" << std::endl;
  std::cout << "*Applying uncertainties to FakeFactors                *" << std::endl;
  std::cout << "*******************************************************" << std::endl << std::endl;

  
  TString p, pi;
  TString m_preselection_data;
  TString m_SR_data;
  TString m_label;
  if      ( DOMC  &&  DOQCD                 ){ pi=path_img_mc;         p=path_ff_mc;         m_preselection_data=preselection_MCsum;       m_SR_data=SR_MCsum;       m_label="sim";  }
  else if ( !DOMC                           ){ pi=path_img_data;       p=path_ff_data;       m_preselection_data=preselection_data;        m_SR_data=SR_data;        m_label="data"; }
  else if ( DOMC  && ( !DOQCD || CHAN!=kMU) ){ pi=path_img_mc_woQCD;   p=path_ff_mc_woQCD;   m_preselection_data=preselection_MCsum_woQCD; m_SR_data=SR_MCsum_woQCD; m_label="sim";  }

  TString pmc=path_ff_mc;             if ( ( !DOQCD && DOMC ) || CHAN!=kMU ) pmc=path_ff_mc_woQCD;
  TString psmc=preselection_MCsum;    if ( ( !DOQCD && DOMC ) || CHAN!=kMU ) psmc=preselection_MCsum_woQCD;

  int m_gen_match=0;
  if (requireGenMatch_whenCalcFF) m_gen_match=GEN_MATCH;



  
  FFCalculator* Analyzer = new FFCalculator(N_p_Wjets,N_p_DY,N_p_TT,N_p_QCD,
                                            N_e_Wjets,N_e_DY,N_e_TT,N_e_QCD,
                                            N_t_Wjets,N_t_DY,N_t_TT,N_t_QCD,
                                            N_m_Wjets,N_m_DY,N_m_TT,N_m_QCD,
                                            N_j_Wjets,N_j_DY,N_j_TT,N_j_QCD,
                                            Pt_cuts_Wjets,Pt_cuts_DY,Pt_cuts_TT,Pt_cuts_QCD,
                                            Eta_cuts_Wjets,Eta_cuts_DY,Eta_cuts_TT,Eta_cuts_QCD,
                                            Decay_cuts_Wjets,Decay_cuts_DY,Decay_cuts_TT,Decay_cuts_QCD,
                                            Mt_cuts_Wjets,Mt_cuts_DY,Mt_cuts_TT,Mt_cuts_QCD,
                                            Njet_cuts_Wjets,Njet_cuts_DY,Njet_cuts_TT,Njet_cuts_QCD
                                            );

  Analyzer->init();
  

  if (doApplyUncertainties) {
    cout << endl << "################### Applying uncertainties  ###############" << endl << endl;

    for(Int_t icat=0; icat<nCAT; icat++){
      //if(icat==2 || icat==3 || icat==4 || icat==6) continue;      
    //for(Int_t icat=2; icat<5; icat++){
      if(inclusive_selection && icat>0) continue;
      Analyzer->loadFile(m_preselection_data,"Events");
      vector<Int_t> mode_comb; mode_comb.push_back(NO_SR|MT); mode_comb.push_back(MVIS); mode_comb.push_back(NO_SR|PT); if(use_svfit){mode_comb.push_back(SVFIT);}
      //vector<Int_t> mode_comb; mode_comb.push_back(SVFIT);;
      //vector<Int_t> mode_comb; mode_comb.push_back(NO_SR|MT); mode_comb.push_back(MVIS);
      //vector<Int_t> mode_comb; mode_comb.push_back(NO_SR|PT);
      Int_t categoryMode=0;
      if(inclusive_frac_mt) categoryMode=_INCLFRAC_MT;
      if(inclusive_frac_et) categoryMode=_INCLFRAC_ET;
      if(inclusive_frac_tt) categoryMode=_INCLFRAC_TT;
      if(!inclusive_selection) {
        if(inclusive_frac_mt) categoryMode=catMode[icat]|_INCLFRAC_MT;
        if(inclusive_frac_et) categoryMode=catMode[icat]|_INCLFRAC_ET;
        if(inclusive_frac_tt) categoryMode=catMode[icat]|_INCLFRAC_TT;
        if(cat_frac_mt) categoryMode=catMode[icat]|_CATFRAC_MT;
        if(cat_frac_et) categoryMode=catMode[icat]|_CATFRAC_ET;
        if(cat_frac_tt) categoryMode=catMode[icat]|_CATFRAC_TT;
      }
      Analyzer->applyFF_wUncertainties(p+"FFestimate_wUncertainties", mode_comb, categoryMode);
      //Analyzer->applyFF_wUncertainties(p+"FFestimate_wUncertainties_SS_SR", mode_comb, categoryMode);
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    
    
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  ///////////////4. Plotting
  ///////////////////////////////////////////////////////////////////////////////////////
  if (doFinalPlots){
    cout << endl << "################### Doing final plots  ###############" << endl << endl;
    Analyzer->loadFile(preselection_data,"Events");

    std::vector<TString> vlabel_all;
    std::vector<TString> vlabel_bkg;
    std::vector<TString> vlabel_ff;
    std::vector<TString> vlabel_bkg_ff;
    std::vector<TString> vlabel_bkg_check;
    std::vector<TString> vname_ff;
    std::vector<TString> vsuff_ff;
    for (unsigned il=0; il< (sizeof(vlabel) / sizeof(vlabel[0]) ); il++){
      vlabel_all.push_back(vlabel[il]);
      if ( !vlabel[il].Contains("ZTT") ) vlabel_bkg.push_back(vlabel[il]);
    }
    for (unsigned il=0; il< (sizeof(vlabel_w_ff) / sizeof(vlabel_w_ff[0]) ); il++){
      vlabel_bkg_ff.push_back(vlabel_w_ff[il]);
    }
    for (unsigned il=0; il< (sizeof(vlabel_check) / sizeof(vlabel_check[0]) ); il++){
      vlabel_bkg_check.push_back(vlabel_check[il]);
    }
    for (unsigned i=0; i<n_ff; i++){
      vlabel_ff.push_back(vlabel[ vlabel_i_ff[i] ]);
      vname_ff.push_back(vname[ vlabel_i_ff[i] ]);
      vsuff_ff.push_back(vsuff[ vlabel_i_ff[i] ]);
    }

    for(Int_t icat=0; icat<nCAT; icat++){
      if(inclusive_selection && icat>0) continue;
      std::vector<TString> proc_list_mt;
      proc_list_mt.push_back(p+"FFestimate_Wjets_mt.root");
      proc_list_mt.push_back(p+"FFestimate_tt_J_mt.root");
      proc_list_mt.push_back(SR_TT_T_mt_sim);
      proc_list_mt.push_back(SR_TT_L_mt_sim);
      proc_list_mt.push_back(p+"FFestimate_DY_J_mt.root");
      proc_list_mt.push_back(SR_DY_TT_mt_sim);
      proc_list_mt.push_back(SR_DY_L_mt_sim);
      if (DOQCD)   proc_list_mt.push_back(p+"FFestimate_QCD_mt.root");
      if (useVV)   proc_list_mt.push_back(SR_VV_T_mt_sim);
      if (useVV)   proc_list_mt.push_back(SR_VV_L_mt_sim);
      if(!inclusive_selection) {
        for( Int_t ilist=0;ilist<proc_list_mt.size();ilist++ ) proc_list_mt.at(ilist).ReplaceAll( ".root", categories[icat]+".root" );
      }
      /*/if( (CHAN==kTAU || CHAN==kEL) && inclusive_frac_mt){
        for( Int_t ilist=0;ilist<proc_list_mt.size();ilist++ ){
          if( proc_list_mt.at(ilist).Contains("Wjets") || proc_list_mt.at(ilist).Contains("tt_J") || proc_list_mt.at(ilist).Contains("DY_J") || proc_list_mt.at(ilist).Contains("QCD") ){
            proc_list_mt.at(ilist).ReplaceAll( ".root", "_mtfrac.root" );
          }
        }
        }*/
        
      Int_t categoryMode=0;
      if(inclusive_frac_mt) categoryMode=_INCLFRAC_MT;
        if(inclusive_frac_et) categoryMode=_INCLFRAC_ET;
        if(inclusive_frac_tt) categoryMode=_INCLFRAC_TT;
      if(!inclusive_selection) {
        if(inclusive_frac_mt) categoryMode=catMode[icat]|_INCLFRAC_MT;
        if(inclusive_frac_et) categoryMode=catMode[icat]|_INCLFRAC_ET;
        if(inclusive_frac_tt) categoryMode=catMode[icat]|_INCLFRAC_TT;
        if(cat_frac_mt) categoryMode=catMode[icat]|_CATFRAC_MT;
        if(cat_frac_et) categoryMode=catMode[icat]|_CATFRAC_ET;
        if(cat_frac_tt) categoryMode=catMode[icat]|_CATFRAC_TT;
      }
      TString fracString = Analyzer->getFracString(categoryMode);
      cout << "fracString: " << fracString << endl;
      for( Int_t ilist=0;ilist<proc_list_mt.size();ilist++ ){
        if(CHAN==kTAU){
          if( proc_list_mt.at(ilist).Contains("Wjets") || proc_list_mt.at(ilist).Contains("QCD") ){
            proc_list_mt.at(ilist).ReplaceAll( ".root", fracString+".root" );
          }
        }
        else{
          if( proc_list_mt.at(ilist).Contains("Wjets") || proc_list_mt.at(ilist).Contains("tt_J") || proc_list_mt.at(ilist).Contains("DY_J") || proc_list_mt.at(ilist).Contains("QCD") ){
            proc_list_mt.at(ilist).ReplaceAll( ".root", fracString+".root" );
          }
        }
      }
      
      std::vector<TString> proc_list_mt_ff;
      proc_list_mt_ff.push_back(SR_TT_T_mt_sim);
      //proc_list_mt_ff.push_back("sim/mt_backup/SR_TT_T_mt.root");
      proc_list_mt_ff.push_back(SR_TT_L_mt_sim);
      proc_list_mt_ff.push_back(SR_DY_TT_mt_sim);
      //proc_list_mt_ff.push_back("sim/mt_backup/SR_DY_TT_mt.root");
      proc_list_mt_ff.push_back(SR_DY_L_mt_sim);
      proc_list_mt_ff.push_back(p+"FFestimate_wUncertainties_mt.root");
      if(useVV)   proc_list_mt_ff.push_back(SR_VV_T_mt_sim);
      if(useVV)   proc_list_mt_ff.push_back(SR_VV_J_mt_sim);
      if(!inclusive_selection){
        for( Int_t ilist=0;ilist<proc_list_mt_ff.size();ilist++ ) proc_list_mt_ff.at(ilist).ReplaceAll( ".root", categories[icat]+".root" );
      }
      for( Int_t ilist=0;ilist<proc_list_mt_ff.size();ilist++ ){
        if(CHAN==kTAU){
        }
        /* if( proc_list_mt_ff.at(ilist).Contains("FFestimate_") ){
            proc_list_mt_ff.at(ilist).ReplaceAll( "_wUncertainties", "" );
            proc_list_mt_ff.at(ilist).ReplaceAll( ".root", "_raw.root" );
          }
        }
        else if(CHAN==kTAU){
          if( proc_list_mt_ff.at(ilist).Contains("FFestimate_") ){
            proc_list_mt_ff.at(ilist).ReplaceAll( "_wUncertainties", "" );
          }
          }*/
        else{
          if( proc_list_mt_ff.at(ilist).Contains("FFestimate_") ){
            proc_list_mt_ff.at(ilist).ReplaceAll( ".root", fracString+".root" );
          }
        }
      }
      //TFile f = f(p+"FFestimate_wUncertainties_mt.root");
      //TH1F *h = dynamic_cast<TH1F*>f.Get("hh_t_mt");
      
      std::vector<TString> proc_list_mvis(proc_list_mt);
      std::vector<TString> proc_list_pt(proc_list_mt);
      for (unsigned iv=0; iv<proc_list_mt.size(); iv++){
        if( proc_list_mvis.at(iv).Contains("mt.") && !proc_list_mvis.at(iv).Contains("catfrac") )proc_list_mvis.at(iv).ReplaceAll("mt.","mvis.");
        else proc_list_mvis.at(iv).ReplaceAll("_mt_","_mvis_");
        if( proc_list_pt.at(iv).Contains("mt.") && !proc_list_pt.at(iv).Contains("catfrac") )proc_list_pt.at(iv).ReplaceAll("mt.","pt.");
        else proc_list_pt.at(iv).ReplaceAll("_mt_","_pt_");
      }
      
      std::vector<TString> proc_list_mvis_ff(proc_list_mt_ff);
      std::vector<TString> proc_list_pt_ff(proc_list_mt_ff);
      std::vector<TString> proc_list_svfit_ff(proc_list_mt_ff);
      for (unsigned iv=0; iv<proc_list_mt_ff.size(); iv++){
        if( proc_list_mvis_ff.at(iv).Contains("mt.") && !proc_list_mvis_ff.at(iv).Contains("catfrac") )proc_list_mvis_ff.at(iv).ReplaceAll("mt.","mvis.");
        else proc_list_mvis_ff.at(iv).ReplaceAll("_mt_","_mvis_");
      }
      for (unsigned iv=0; iv<proc_list_mt_ff.size(); iv++){
        if( proc_list_pt_ff.at(iv).Contains("mt.") && !proc_list_pt_ff.at(iv).Contains("catfrac") )proc_list_pt_ff.at(iv).ReplaceAll("mt.","pt.");
        else proc_list_pt_ff.at(iv).ReplaceAll("_mt_","_pt_");
      }
      for (unsigned iv=0; iv<proc_list_mt_ff.size(); iv++){
        if( proc_list_svfit_ff.at(iv).Contains("mt.") && !proc_list_svfit_ff.at(iv).Contains("catfrac") )proc_list_svfit_ff.at(iv).ReplaceAll("mt.","svfit.");
        else proc_list_svfit_ff.at(iv).ReplaceAll("_mt_","_svfit_");
      }
      
      
      std::vector<TString> proc_list_bkg_mt;
      for (unsigned iv=0; iv<proc_list_mt.size(); iv++) proc_list_bkg_mt.push_back( proc_list_mt.at(iv) ); 
      
      std::vector<TString> proc_list_bkg_mvis(proc_list_bkg_mt);
      std::vector<TString> proc_list_bkg_pt(proc_list_bkg_mt);
      for (unsigned iv=0; iv<proc_list_bkg_mt.size(); iv++){
        cout << proc_list_bkg_mvis.at(iv) << endl;
        if( proc_list_bkg_mvis.at(iv).Contains("mt.") && !proc_list_bkg_mvis.at(iv).Contains("catfrac") )proc_list_bkg_mvis.at(iv).ReplaceAll("mt.","mvis.");
        else proc_list_bkg_mvis.at(iv).ReplaceAll("_mt_","_mvis_");
        cout << proc_list_bkg_mvis.at(iv) << endl;
        if( proc_list_bkg_pt.at(iv).Contains("mt.") && !proc_list_bkg_pt.at(iv).Contains("catfrac") ) proc_list_bkg_pt.at(iv).ReplaceAll("mt.","pt.");
        else proc_list_bkg_pt.at(iv).ReplaceAll("_mt_","_pt_");
      }

      TString plotEnding="";
      if(!inclusive_selection) plotEnding=categories[icat];
      plotEnding+=fracString;
      //plot bkg sum estimate a la run 1 vs FF
      if ( true ){
        TString bsum=SR_MCsum_mt;
        if(!inclusive_selection) bsum.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(bsum,"MC sum",proc_list_bkg_mt,  vlabel_all,pi+"MCsum_vs_ff_"+tvar[0]+plotEnding,"MC sum run 1 vs FF "+plotEnding,tvar_l[0],_COLOR);

        std::vector<TString> proc_list_bkg_mt;
        for (unsigned iv=0; iv<proc_list_mt.size(); iv++){ if ( !proc_list_mt.at(iv).Contains("DY_TT") ) proc_list_bkg_mt.push_back( proc_list_mt.at(iv) ); }
        
        bsum=SR_MCsum_mt;
        if(!inclusive_selection) bsum.ReplaceAll( ".root",categories[icat]+".root" );
        bsum.ReplaceAll( "SR_MCsum", "SR_Bkgsum_run1" ); //bsum.ReplaceAll( "_"+s_mt , "_"+tvar[0] );
        
        //Analyzer->plotBgStackedAgainstData(bsum,"Bkg a la run 1",proc_list_bkg_mt,  vlabel_bkg,pi+"run1_vs_ff_"+tvar[0]+plotEnding,"Bkg sum run 1 vs FF "+plotEnding,tvar_l[0],_COLOR_RUN1);
        //control plots for et channel////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<TString> proc_list_mt_check;
        proc_list_mt_check.push_back(p+"FFestimate_Wjets_mt"+fracString+".root"); proc_list_mt_check.push_back(p+"FFestimate_tt_J_mt"+fracString+".root"); proc_list_mt_check.push_back(p+"FFestimate_DY_J_mt"+fracString+".root"); proc_list_mt_check.push_back(p+"FFestimate_QCD_mt"+fracString+".root");
        std::vector<TString> proc_list_mvis_check;
        proc_list_mvis_check.push_back(p+"FFestimate_Wjets_mvis"+fracString+".root"); proc_list_mvis_check.push_back(p+"FFestimate_tt_J_mvis"+fracString+".root"); proc_list_mvis_check.push_back(p+"FFestimate_DY_J_mvis"+fracString+".root"); proc_list_mvis_check.push_back(p+"FFestimate_QCD_mvis"+fracString+".root");
        std::vector<TString> proc_list_pt_check;
        proc_list_pt_check.push_back(p+"FFestimate_Wjets_pt"+fracString+".root"); proc_list_pt_check.push_back(p+"FFestimate_tt_J_pt"+fracString+".root"); proc_list_pt_check.push_back(p+"FFestimate_DY_J_pt"+fracString+".root"); proc_list_pt_check.push_back(p+"FFestimate_QCD_pt"+fracString+".root");
        if(inclusive_selection && s_chan[CHAN]=="et"){
        
	  //plotting et samples with mt fakefactor et fractions vs mt fractions
	  /*TString check="fakefactor/data_et/FFestimate_wUncertainties_mt.root";
	  Analyzer->plotBgStackedAgainstData(check,"etfrac",proc_list_mt_check,  vlabel_bkg_check,pi+"mtfrac_ff_"+tvar[0],"mt/et frac for et FFs",tvar_l[0],_COLOR_CHECK);
                
	  check="fakefactor/data_et/FFestimate_wUncertainties_mvis.root";
	  Analyzer->plotBgStackedAgainstData(check,"etfrac",proc_list_mvis_check,  vlabel_bkg_check,pi+"mtfrac_ff_"+tvar[1],"mt/et frac for et FFs",tvar_l[1],_COLOR_CHECK);
        
	  //plotting et vs mt fake factors
	  Analyzer->plotFF("fakefactor/mc_woQCD_et/FF_DY_J_only.root",    "fakefactor/mc_mt/FF_DY_J_only.root",    _DY, pi+"FF_DY_mt_vs_et"   ,"DY m_{T}(#mu#mu), e#tau", "DY m_{T}(#mu#mu), #mu#tau",PTDM_BINS,nFIT_BINS+1);
	  Analyzer->plotFF("fakefactor/mc_woQCD_et/FF_TT_J_only.root",    "fakefactor/mc_mt/FF_TT_J_only.root",    _TT, pi+"FF_TT_mt_vs_et"   ,"TT , e#tau", "TT, #mu#tau",PTDM_BINS,nPTDM_BINS+1);
	  Analyzer->plotFF("fakefactor/mc_woQCD_et/FF_Wjets_only.root",    "fakefactor/mc_mt/FF_Wjets_only.root",    _W_JETS, pi+"FF_Wjets_mt_vs_et"   ,"Wjets , e#tau", "Wjets, #mu#tau",PTDM_BINS,nFIT_BINS+1);
	  Analyzer->plotFF("fakefactor/data_et/FF_corr_Wjets_MCsum_noGen.root",    "fakefactor/data_mt/FF_corr_Wjets_MCsum_noGen.root",    _W_JETS, pi+"FF_data_Wjets_mt_vs_et"   ,"Wjets , e#tau", "Wjets, #mu#tau",PTDMJET_BINS,nPTDMJET_BINS+1);
	  Analyzer->plotFF("fakefactor/data_et/FF_corr_TT_MCsum_noGen.root",    "fakefactor/data_mt/FF_corr_TT_MCsum_noGen.root",    _TT, pi+"FF_data_TT_mt_vs_et"   ,"TT , e#tau", "TT, #mu#tau",PTDMJET_BINS,nPTDMJET_BINS+1);
	  Analyzer->plotFF("fakefactor/data_et/FF_corr_DY_MCsum_noGen.root",    "fakefactor/data_mt/FF_corr_DY_MCsum_noGen.root",    _DY, pi+"FF_data_DY_J_mt_vs_et"   ,"DY , e#tau", "DY, #mu#tau",PTDMJET_BINS,nPTDMJET_BINS+1);
	  Analyzer->plotFF("fakefactor/data_et/FF_corr_QCD_MCsum_noGen.root",    "fakefactor/data_mt/FF_corr_QCD_MCsum_noGen.root",    _QCD, pi+"FF_data_QCD_mt_vs_et"   ,"QCD , e#tau", "QCD, #mu#tau",PTDMJET_BINS,nPTDMJET_BINS+1);*/
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TString toCompare="sim/tt/SR_cuts_1p_data_mt.root";
        std::vector<TString> others;
        std::vector<TString> others_name;
        others.push_back("sim/tt/SR_cuts_1p_Wjets_mt.root");
        others_name.push_back("MC Wjets");
        //Analyzer->makeComparePlots(toCompare,"data",others, others_name,pi+"SR_loose_mt_data_vs_WjetsMC_1p","loose_mt","m_T",_COLOR);
        //Analyzer->makeComparePlots(toCompare,"data",others, others_name,pi+"SR_tight_mt_data_vs_WjetsMC_1p","tight_mt","m_T",_COLOR);
        //Analyzer->makeComparePlots(toCompare,"data",others, others_name,pi+"SR_antiiso_mt_data_vs_WjetsMC_1p","antiiso_mt","m_T",_COLOR);
        toCompare="sim/tt/SR_cuts_3p_data_mt.root";
        others.clear();
        others_name.clear();
        others.push_back("sim/tt/SR_cuts_3p_Wjets_mt.root");
        others_name.push_back("MC Wjets");
        //Analyzer->makeComparePlots(toCompare,"data",others, others_name,pi+"SR_loose_mt_data_vs_WjetsMC_3p","loose_mt","m_T",_COLOR);
        //Analyzer->makeComparePlots(toCompare,"data",others, others_name,pi+"SR_tight_mt_data_vs_WjetsMC_3p","tight_mt","m_T",_COLOR);
        //Analyzer->makeComparePlots(toCompare,"data",others, others_name,pi+"SR_antiiso_mt_data_vs_WjetsMC_3p","antiiso_mt","m_T",_COLOR);
        

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        TString data=SR_data_mt;
        if(DOMC)data=SR_MCsum_mt;
        //if(DOMC)data="sim/mt_backup/SR_MCsum_mt.root";
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );       
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_mt_ff,  vlabel_bkg_ff,pi+"bkg_ff_"+tvar[0]+plotEnding,"Bkg estimation with FF "+plotEnding,tvar_l[0],_COLOR_FF);

        if(!inclusive_selection) bsum.ReplaceAll( ".root",categories[icat]+".root" );
        bsum=SR_MCsum_mvis; 
        //Analyzer->plotBgStackedAgainstData(bsum,"MC sum",proc_list_bkg_mvis,  vlabel_all,pi+"MCsum_vs_ff_"+tvar[1]+plotEnding,"MC sum vs FF "+plotEnding,tvar_l[1],_COLOR);
        bsum=SR_MCsum_pt; 
        //Analyzer->plotBgStackedAgainstData(bsum,"MC sum",proc_list_bkg_pt,  vlabel_all,pi+"MCsum_vs_ff_"+tvar[2]+plotEnding,"MC sum vs FF "+plotEnding,tvar_l[2],_COLOR);

        std::vector<TString> proc_list_bkg_mvis(proc_list_bkg_mt);
        std::vector<TString> proc_list_bkg_pt(proc_list_bkg_mt);
        for (unsigned iv=0; iv<proc_list_bkg_mt.size(); iv++){
          if( proc_list_bkg_mvis.at(iv).Contains("mt.") && !proc_list_bkg_mvis.at(iv).Contains("catfrac") ) proc_list_bkg_mvis.at(iv).ReplaceAll("mt.","mvis.");
          else proc_list_bkg_mvis.at(iv).ReplaceAll("_mt_","_mvis_");
        }
        for (unsigned iv=0; iv<proc_list_bkg_mt.size(); iv++){
          if( proc_list_bkg_pt.at(iv).Contains("mt.") && !proc_list_bkg_pt.at(iv).Contains("catfrac") ) proc_list_bkg_pt.at(iv).ReplaceAll("mt.","pt.");
          else proc_list_bkg_pt.at(iv).ReplaceAll("_mt_","_pt_");
        }
        
        bsum=SR_MCsum_mvis;
        if(!inclusive_selection) bsum.ReplaceAll( ".root",categories[icat]+".root" );
        bsum.ReplaceAll( "SR_MCsum", "SR_Bkgsum_run1" ); //bsum.ReplaceAll( "_"+s_mvis , "_"+tvar[1] );
        //Analyzer->plotBgStackedAgainstData(bsum,"Bkg a la run 1",proc_list_bkg_mvis,  vlabel_bkg,pi+"run1_vs_ff_"+tvar[1]+plotEnding,"Bkg sum run 1 vs FF "+plotEnding,tvar_l[1],_COLOR_RUN1);

        bsum=SR_MCsum_pt;
        if(!inclusive_selection) bsum.ReplaceAll( ".root",categories[icat]+".root" );
        bsum.ReplaceAll( "SR_MCsum", "SR_Bkgsum_run1" ); //bsum.ReplaceAll( "_"+s_mvis , "_"+tvar[1] );
        //Analyzer->plotBgStackedAgainstData(bsum,"Bkg a la run 1",proc_list_bkg_pt,  vlabel_bkg,pi+"run1_vs_ff_"+tvar[2]+plotEnding,"Bkg sum run 1 vs FF "+plotEnding,tvar_l[2],_COLOR_RUN1);
        
        data=SR_data_mvis;
        //if(DOMC)data=SR_MCsum_mvis;
        if(DOMC)data="sim/mt/SR_MCsum_mvis.root";
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_mvis_ff,  vlabel_bkg_ff,pi+"bkg_ff_"+tvar[1]+plotEnding,"Bkg estimation with FF "+plotEnding,tvar_l[1],_COLOR_FF);

        data=SR_data_pt;
        if(DOMC)data=SR_MCsum_pt;
        //if(DOMC)data="sim/mt_backup/SR_MCsum_pt.root";
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_pt_ff,  vlabel_bkg_ff,pi+"bkg_ff_"+tvar[2]+plotEnding,"Bkg estimation with FF "+plotEnding,tvar_l[2],_COLOR_FF);

        data=SR_data_svfit;
        if(DOMC)data=SR_MCsum_svfit;
        //if(DOMC)data="sim/mt_backup/SR_MCsum_pt.root";
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_svfit_ff,  vlabel_bkg_ff,pi+"bkg_ff_"+s_svfit+plotEnding,"Bkg estimation with FF "+plotEnding,"m_{SV} [GeV]",_COLOR_FF);
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        cout << "---------------------------------------------------" << endl;
        cout << "Plotting splitted FFs" << endl;
        proc_list_mt_ff.clear();
        proc_list_mvis_ff.clear();
        proc_list_pt_ff.clear();
        proc_list_svfit_ff.clear();
        std::vector<TString> proc_list_mt_subtract; std::vector<TString> proc_list_mvis_subtract; std::vector<TString> proc_list_pt_subtract; std::vector<TString> proc_list_svfit_subtract;
        proc_list_mt_ff.push_back(p+"FFestimate_Wjets_mt"+fracString+".root");
        if (DOQCD)   proc_list_mt_ff.push_back(p+"FFestimate_QCD_mt"+fracString+".root");
        proc_list_mt_ff.push_back(p+"FFestimate_tt_J_mt"+fracString+".root");
        proc_list_mt_subtract.push_back(SR_TT_T_mt_sim);
        proc_list_mt_subtract.push_back(SR_TT_L_mt_sim);
        proc_list_mt_ff.push_back(p+"FFestimate_DY_J_mt"+fracString+".root");
        proc_list_mt_subtract.push_back(SR_DY_TT_mt_sim);
        proc_list_mt_subtract.push_back(SR_DY_L_mt_sim);
        if (useVV)   proc_list_mt_subtract.push_back(SR_VV_T_mt_sim);
        if (useVV)   proc_list_mt_subtract.push_back(SR_VV_L_mt_sim);
        if(!inclusive_selection) {
          for( Int_t ilist=0;ilist<proc_list_mt_ff.size();ilist++ ) proc_list_mt_ff.at(ilist).ReplaceAll( "_catfrac_mt", categories[icat]+"_catfrac_mt" );
          for( Int_t ilist=0;ilist<proc_list_mt_ff.size();ilist++ ) proc_list_mt_ff.at(ilist).ReplaceAll( "_catfrac_et", categories[icat]+"_catfrac_et" );
          for( Int_t ilist=0;ilist<proc_list_mt_ff.size();ilist++ ) proc_list_mt_ff.at(ilist).ReplaceAll( "_catfrac_tt", categories[icat]+"_catfrac_tt" );
          for( Int_t ilist=0;ilist<proc_list_mt_subtract.size();ilist++ ) proc_list_mt_subtract.at(ilist).ReplaceAll( ".root", categories[icat]+".root" );
        }
        if(DOMC)data=SR_MCsum_mt;
        else data=SR_data_mt;
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_mt_ff,  vlabel_bkg_check, proc_list_mt_subtract, pi+"data_vs_ff_"+tvar[0]+plotEnding,"Bkg estimation with FF "+plotEnding,tvar_l[0],_COLOR_CHECK);

        for (unsigned iv=0; iv<proc_list_mt_ff.size(); iv++){
          proc_list_mvis_ff.push_back(proc_list_mt_ff.at(iv)); proc_list_pt_ff.push_back(proc_list_mt_ff.at(iv)); proc_list_svfit_ff.push_back(proc_list_mt_ff.at(iv));
          if( proc_list_mvis_ff.at(iv).Contains("mt") && !proc_list_mvis_ff.at(iv).Contains("catfrac") )proc_list_mvis_ff.at(iv).ReplaceAll("mt.","mvis.");
          else proc_list_mvis_ff.at(iv).ReplaceAll("_mt_","_mvis_");
          if( proc_list_pt_ff.at(iv).Contains("mt") && !proc_list_pt_ff.at(iv).Contains("catfrac") ) proc_list_pt_ff.at(iv).ReplaceAll("mt.","pt.");
          else proc_list_pt_ff.at(iv).ReplaceAll("_mt_","_pt_");
          if( proc_list_svfit_ff.at(iv).Contains("mt") && !proc_list_svfit_ff.at(iv).Contains("catfrac") ) proc_list_svfit_ff.at(iv).ReplaceAll("mt.","svfit.");
          else proc_list_svfit_ff.at(iv).ReplaceAll("_mt_","_svfit_");
        }

        for (unsigned iv=0; iv<proc_list_mt_subtract.size(); iv++){
          proc_list_mvis_subtract.push_back(proc_list_mt_subtract.at(iv)); proc_list_pt_subtract.push_back(proc_list_mt_subtract.at(iv)); proc_list_svfit_subtract.push_back(proc_list_mt_subtract.at(iv));
          if( proc_list_mvis_subtract.at(iv).Contains("mt.") && !proc_list_mvis_subtract.at(iv).Contains("catfrac") )proc_list_mvis_subtract.at(iv).ReplaceAll("mt.","mvis.");
          else proc_list_mvis_subtract.at(iv).ReplaceAll("_mt_","_mvis_");
          if( proc_list_pt_subtract.at(iv).Contains("mt.") && !proc_list_pt_subtract.at(iv).Contains("catfrac") ) proc_list_pt_subtract.at(iv).ReplaceAll("mt.","pt.");
          else proc_list_pt_subtract.at(iv).ReplaceAll("_mt_","_pt_");
          if( proc_list_svfit_subtract.at(iv).Contains("mt.") && !proc_list_svfit_subtract.at(iv).Contains("catfrac") ) proc_list_svfit_subtract.at(iv).ReplaceAll("mt.","svfit.");
          else proc_list_svfit_subtract.at(iv).ReplaceAll("_mt_","_svfit_");
        }

        if(DOMC)data=SR_MCsum_mvis;
        else data=SR_data_mvis;
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_mvis_ff,  vlabel_bkg_check, proc_list_mvis_subtract, pi+"data_vs_ff_"+tvar[1]+plotEnding,"Bkg estimation with FF "+plotEnding,tvar_l[1],_COLOR_CHECK);
        if(DOMC)data=SR_MCsum_pt;
        else data=SR_data_pt;
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_pt_ff,  vlabel_bkg_check, proc_list_pt_subtract, pi+"data_vs_ff_"+tvar[2]+plotEnding,"Bkg estimation with FF "+plotEnding,tvar_l[2],_COLOR_CHECK);
        if(DOMC)data=SR_MCsum_svfit;
        else data=SR_data_svfit;
        if(!inclusive_selection) data.ReplaceAll( ".root",categories[icat]+".root" );
        Analyzer->plotBgStackedAgainstData(data,"data",proc_list_svfit_ff,  vlabel_bkg_check, proc_list_svfit_subtract, pi+"data_vs_ff_"+s_svfit+plotEnding,"Bkg estimation with FF "+plotEnding,"m_{SV} [GeV]",_COLOR_CHECK);
        
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      
    }

  }

  if(doComparePlots){
    cout << "Making compare plots for " << s_chan[CHAN] << endl;
    std::vector<TString> proc_bkg; std::vector<TString> proc_bkg2;
    proc_bkg.push_back("W"); proc_bkg2.push_back("W");
    proc_bkg.push_back("TT"); proc_bkg2.push_back("TT");
    proc_bkg.push_back("ZJ"); proc_bkg2.push_back("ZJ");
    proc_bkg.push_back("ZTT");
    proc_bkg.push_back("ZL");
    proc_bkg.push_back("VV");
    proc_bkg.push_back("QCD"); proc_bkg2.push_back("QCD");

    std::vector<TString> vlabel_compareBkg; std::vector<TString> vlabel_compareBkg2;
    for (unsigned il=0; il< (sizeof(vlabel_compare) / sizeof(vlabel_compare[0]) ); il++){
      vlabel_compareBkg.push_back(vlabel_compare[il]);
    }
    for (unsigned il=0; il< (sizeof(vlabel_compare2) / sizeof(vlabel_compare2[0]) ); il++){
      vlabel_compareBkg2.push_back(vlabel_compare2[il]);
    }

    TString file="compareFiles/Imperial/htt_"+s_chan[CHAN]+".inputs-mssm-13TeV-mvis.root"; TString directory=s_chan[CHAN]+"_inclusive";
    TString compareFile="compareFiles/Imperial/htt_"+s_chan[CHAN]+".inputs-mssm-13TeV-mvis.root"; TString compareDirectory=s_chan[CHAN]+"_inclusive"; TString compareName="data_obs"; TString description="MSSMdatacards";
    Analyzer->plotCompBgStackedAgainstData(compareFile,compareDirectory,compareName,file,directory,proc_bkg,vlabel_compareBkg,description,"data","Obs/Pred","Images/compareFiles/Imperial-mssm_"+s_chan[CHAN]+"_mvis",_COMPARE);
    compareFile=p+"FFestimate_wUncertainties_mvis.root"; compareDirectory="";compareName="hh_t_mvis";description="FF vs bkg estimate";
    Analyzer->plotCompBgStackedAgainstData(compareFile,compareDirectory,compareName,file,directory,proc_bkg2,vlabel_compareBkg2,description,"FF","Ratio","Images/compareFiles/Imperial-mssm_"+s_chan[CHAN]+"_mvis_vs_FF",_COMPARE2);
    
    
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(doSSSRPlots){

    cout << "---------------------------------------------------" << endl;
    cout << "Plotting same sign FFs" << endl;
        
    std::vector<TString> proc_list_mt_ff;
    std::vector<TString> vlabel_bkg_ff;
    for (unsigned il=0; il< (sizeof(vlabel_w_ff) / sizeof(vlabel_w_ff[0]) ); il++){
      vlabel_bkg_ff.push_back(vlabel_w_ff[il]);
    }
    proc_list_mt_ff.push_back(SS_SR_TT_T_mt_sim);
    proc_list_mt_ff.push_back(SS_SR_TT_L_mt_sim);
    proc_list_mt_ff.push_back(SS_SR_DY_TT_mt_sim);
    proc_list_mt_ff.push_back(SS_SR_DY_L_mt_sim);
    proc_list_mt_ff.push_back(p+"FFestimate_wUncertainties_SS_SR_mt.root");
    if(useVV)   proc_list_mt_ff.push_back(SS_SR_VV_T_mt_sim);
    if(useVV)   proc_list_mt_ff.push_back(SS_SR_VV_J_mt_sim);

    TString data=SS_SR_data_mt;
    Analyzer->plotBgStackedAgainstData(data,"data",proc_list_mt_ff,  vlabel_bkg_ff,pi+"bkg_ff_SS_SR_"+tvar[0],"Bkg SS estimation with FF",tvar_l[0],_COLOR_FF);

    std::vector<TString> proc_list_mvis_ff(proc_list_mt_ff);
    std::vector<TString> proc_list_pt_ff(proc_list_mt_ff);
    for (unsigned iv=0; iv<proc_list_mt_ff.size(); iv++){
      if( proc_list_mvis_ff.at(iv).Contains("mt.") && !proc_list_mvis_ff.at(iv).Contains("catfrac") )proc_list_mvis_ff.at(iv).ReplaceAll("mt.","mvis.");
      else proc_list_mvis_ff.at(iv).ReplaceAll("_mt_","_mvis_");
    }
    for (unsigned iv=0; iv<proc_list_mt_ff.size(); iv++){
      if( proc_list_pt_ff.at(iv).Contains("mt.") && !proc_list_pt_ff.at(iv).Contains("catfrac") )proc_list_pt_ff.at(iv).ReplaceAll("mt.","pt.");
      else proc_list_pt_ff.at(iv).ReplaceAll("_mt_","_pt_");
    }

    data=SS_SR_data_mvis;
    Analyzer->plotBgStackedAgainstData(data,"data",proc_list_mvis_ff,  vlabel_bkg_ff,pi+"bkg_ff_SS_SR_"+tvar[1],"Bkg SS estimation with FF",tvar_l[1],_COLOR_FF);

    data=SS_SR_data_pt;
    Analyzer->plotBgStackedAgainstData(data,"data",proc_list_pt_ff,  vlabel_bkg_ff,pi+"bkg_ff_SS_SR_"+tvar[2],"Bkg SS estimation with FF",tvar_l[2],_COLOR_FF);
    
    
    
  }


  
  
  delete Analyzer;

  
}
  
#ifndef __CINT__
int main(int argc, char* argv[]) {
  ApplyFF();    
}
#endif
