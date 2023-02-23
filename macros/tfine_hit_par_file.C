void tfine_hit_par_file(Int_t runid=132)
{
  const TString runString = TString::Format("%04d",runid);

  const TString parInFilename = "params_ig_fine5.root";
  const TString parOutFilename = "params_ig_fine6.root";
  
  auto rtdb = FairRuntimeDb::instance();

  auto parIn = new FairParRootFileIo(false);
  parIn->open(parInFilename);
  rtdb->setFirstInput(parIn);

  auto parOut = new FairParRootFileIo(true);
  parOut->open(parOutFilename);
  rtdb->setOutput(parOut);

  auto hpars = (R3BNeulandHitPar*)rtdb->getContainer("NeulandHitPar");

  //rtdb->addRun(runid);
  //rtdb->setInputVersion(runid, (char*)"NeulandHitPar", 1, 1);  
  //rtdb->initContainers(runid, 999);
  rtdb->initContainers(999);
  //rtdb->addRun(runid);

  rtdb->print();
  
  R3BNeulandHitModulePar* hp[1300];
  ifstream tcorr_file("tfine_run132_030123.txt");

  Double_t tcorr;
  Int_t j;

  for (int i=0; i<1300; i++) {

    tcorr_file >> j >> tcorr;

    hp[j-1] = hpars->GetModuleParAt(j-1);
    hp[j-1]->SetTSync(hp[j-1]->GetTSync()+tcorr-50.);
  }

  //  hpars->setChanged();          // writes a new version
  //  rtdb->writeContainers();      // doesn't write FairRtdbRun

  //rtdb->removeRun("999");
  
  rtdb->saveOutput();               // writes FairRtdbRun
  
}
