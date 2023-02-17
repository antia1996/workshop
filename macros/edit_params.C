struct EXT_STR_h101_t
{
  EXT_STR_h101_unpack_t unpack;
};

void edit_params()
{
  const Int_t nev = 1; /* number of events to read, -1 - until CTRL+C */
  const Int_t trigger = -1;              // 1 - onspill, 2 - offspill. -1 - all
  
  /* Create source using ucesb for input ------------------ */
  
  //TString filename = "stream://lxir123:7000"; // :7000 for standalone DAQ

  Int_t RunId = 999;
  TString runNumber=Form ("%04d", RunId);

  const TString filename = "/lustre/r3b/202006_s444/lmd/main0011_*.lmd";
  TString ntuple_options = "RAW";

  const TString ucesb_path = "/u/land/fake_cvmfs/9.13/upexps/202006_s444/202006_s444 --allow-errors --input-buffer=47Mi";
  
  const Int_t nBarsPerPlane = 50;    // number of scintillator bars per plane
  const Int_t nPlanes = 26;           // number of planes (for TCAL calibration)

  EXT_STR_h101 ucesb_struct;
  R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
					      ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
  
  source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack_t *)&ucesb_struct,
  					offsetof(EXT_STR_h101, unpack)));

  source->SetMaxEvents(nev);
 
  const Int_t refresh = 1;                 // refresh rate for saving 
 
  /* Create online run ------------------------------------ */
  FairRunOnline* run = new FairRunOnline(source);
  run->SetRunId(RunId);
  /* ------------------------------------------------------ */

  //run->SetSink(new FairRootFileSink("deleteme.root"));
  
   // ----- Runtime DataBase info -----------------------------------------------
  auto rtdb = run->GetRuntimeDb();

  auto parIn = new FairParRootFileIo(false);
  parIn->open("params_sync_s522_1.root");
  
  auto parOut = new FairParRootFileIo(true);
  parOut->open("params_ig_fine.root");
  
  rtdb->setFirstInput(parIn);
  //rtdb->print();
  rtdb->addRun(RunId);
  rtdb->getContainer("LandTCalPar");
  rtdb->setInputVersion(RunId, (char*)"LandTCalPar", 1, 1);

  rtdb->getContainer("NeulandHitPar");
  rtdb->setInputVersion(RunId, (char*)"NeulandHitPar", 1, 1);

  rtdb->setOutput(parOut);
  // ---------------------------------------------------------------------------
 
  // calibrated data in the output file

  /* Initialize ------------------------------------------- */
  run->Init(); // InitContainers()
  //FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
  /* ------------------------------------------------------ */
  
  auto hpars = (R3BNeulandHitPar*) rtdb->getContainer("NeulandHitPar");

  R3BNeulandHitModulePar* hp[1300];

  ifstream tcorr_file("tfine.txt");
  
  Double_t tcorr;

  Int_t j;
  
  for (int i=0; i<1300; i++) {

    tcorr_file >> j >> tcorr;
    
    hp[j-1] = hpars->GetModuleParAt(j-1);
    //hp[i]->SetTDiff(0.);
    hp[j-1]->SetTSync(hp[j-1]->GetTSync()+tcorr-50.);
  }

  tcorr_file.close();
  
  //hpars->SetDistanceToTarget(1520.);
  //hpars->SetGlobalTimeOffset(3150.);
  
  /* Run -------------------------------------------------- */
  run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
  rtdb->saveOutput();  // save tcal parameters
  //delete run;
  /* ------------------------------------------------------ */
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;

}
