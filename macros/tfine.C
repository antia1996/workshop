void tfine()
{
  
  auto ff = new TFile("/lustre/land/Neuland/nearline_out_s522.root");

  auto fdir = (TDirectoryFile*)ff->Get("R3BNeulandOnlineSpectra");

  fdir->cd();

  for (int i=1; i<=1300; i++)
    {

      auto cch = (TH1F*)hTofcvsBar->ProjectionY("cch",i,i);
      Int_t ntot = cch->GetEntries();

      Double_t y=0.;
   
      if (ntot>0) {

        auto f1 = new TF1("f1", "gaus");
        cch->Fit("f1", "R", "", 128., 130.);
        y = f1->GetParameter(1); 
        
        delete f1;

      }
    }
}
