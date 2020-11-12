#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

#include <vector>
#include <iostream>

enum nu_flavours { 
  FHC_NUMU, FHC_NUMU_BAR, FHC_NUE, FHC_NUE_BAR,
  RHC_NUMU, RHC_NUMU_BAR, RHC_NUE, RHC_NUE_BAR
};

const std::vector<TString> nu_names {
  "fhc_numu", "fhc_numubar", "fhc_nue", "fhc_nuebar",
  "rhc_numu", "rhc_numubar", "rhc_nue", "rhc_nuebar"
};

const std::vector<TString> nu_labels {
  "#nu_{#mu} FHC", "#bar{#nu}_{#mu} FHC", "#nu_{e} FHC", "#bar{#nu}_{e} FHC",
  "#nu_{#mu} RHC", "#bar{#nu}_{#mu} RHC", "#nu_{e} RHC", "#bar{#nu}_{e} RHC"
};

TFile * OpenFile(const TString filename) {
  TFile * f = new TFile(filename);
  if(!f->IsOpen()) {
    std::cout<<"Could not open "<<filename<<"!\n";
    exit(1);
  }
  return f;
}

void propagate_uncertainty(std::vector <TH1D*> spectra, TH2D* correlation, nu_flavours nominator, nu_flavours denominator) {
  /*
   * 
   */
  const int nbins = spectra[nominator]->GetNbinsX();
  
  //first bins of the relevant ranges of the correlation matrix 
  const int first_nominator_bin = nbins * nominator + 1;
  const int first_denominator_bin = nbins * denominator + 1;
  
  /*
   * TODO
   * 1. integrate both spectra over given range
   * 2. Divide them
   * 3. Calculate uncertainty using correlation matrix... I'm not yet sure how to do this with integrated spectra
   */
  
}

void weigh_spectrum(TH1D * h) {
  /**
   * Weigh spectrum by detector efficiency and interaction cross-section
   */
  for(int bin = 1; bin <= h->GetNbinsX(); bin++) {
    const double E = h->GetBinCenter(bin);
    
    double weight = 1; //TODO: this should be some function of E
    
    h->SetBinContent(bin, weight * h->GetBinContent(bin));
  }
}


void process(const TString covariance_name, const TString spectra_FHC_name, const TString spectra_RHC_name) {
  TFile * file_covariance = OpenFile(covariance_name);
  TFile * file_spectra_FHC = OpenFile(spectra_FHC_name);
  TFile * file_spectra_RHC = OpenFile(spectra_RHC_name);
  
  TH2D * correlation = (TH2D*)(file_covariance->Get("hcor_total"));
  if(!correlation) { std::cout<<"Could not read correlation matrix!\n"; exit(1); }
  
  std::vector<TH1D*> spectra;
  for(unsigned int i = 0; i < nu_names.size(); i++) {
    TFile * f = i < 4 ? file_spectra_FHC : file_spectra_RHC;
    TString hname = "total/htotal_" + nu_names[i](4,10) + "_calculated";
    spectra.push_back((TH1D*)(f->Get(hname)));
    if(!spectra[i]) { std::cout<<"Could not read "<<hname<<"!\n";  exit(1); }
    
    weigh_spectrum(spectra[i]);
  }
  
  propagate_uncertainty(spectra, correlation, FHC_NUMU, FHC_NUE);
  
  file_covariance->Close();
  file_spectra_FHC->Close();
  file_spectra_RHC->Close();
}

int main() {
  const TString covariance_name = "../covariance_matrices.root";
  const TString spectra_FHC_name = "../out_center_15_FHC.root";
  const TString spectra_RHC_name = "../out_center_15_FHC.root";
  
  process(covariance_name, spectra_FHC_name, spectra_RHC_name);
  
}
