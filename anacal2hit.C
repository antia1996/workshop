// Macro to produce cosmics calibration parameters (sync) from the cal root level.
// inFilepattern, tcalParFileName and syncParFileName need to be adapted prior to use.
// When used for other experiments, nPlanes and cosmtpat need to be adjusted accordingly.
// called in root using: .L anacal2hit.C ; anacal2hit(First runnumber, number of consecutive runs in addition)
// output syncParFileName: parameter file used for NeuLAND Hit level.
// output rootFileName: root file with cal level root tree.
// for details check https://wiki.r3b-nustar.de/detectors/neuland/software
//
void anacal2hit(const int RunID=362, const int nSubruns=0)
{
  //const TString tcalParFileName = TString::Format("params_tcal_%04d.root", RunID);
  //const TString syncParFileName = TString::Format("params_sync_%04d.root", RunID);
  const TString rootFileName = TString::Format("/lustre/land/Neuland/s509_anacal2hit-%04d.root", RunID);  
  //just for test Jan 2023:
  const TString tcalParFileName = TString::Format("s509_params_tcal_%04d.root", RunID);
  const TString syncParFileName = TString::Format("params_new_sync-test-%04d.root", RunID);

  const TString inFilePattern = "/lustre/r3b/202205_s509/NeuLAND_MapData/map%04d.root"; 
  //const TString inFilePattern = "/lustre/land/boretzky/s467/map%04d.root"; 
  const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
  const Int_t nPlanes = 26;       // number of planes (for TCAL calibration)
  const Int_t nev = -1;
  const Int_t trigger = -1; // 1 - onspill, 2 - offspill. -1 - all
  const Int_t cosmtpat = 0x2000; // cosmics trigger

  TStopwatch timer;
  timer.Start();
    // -------------------------------------------
    auto source = new R3BFileSource(TString::Format(inFilePattern, RunID));
    source->SetRunId(999);
    for (int i = 1; i <= nSubruns; i++)
    {
      source->AddFile(TString::Format(inFilePattern, RunID+i));
    }
    auto run = new FairRunAna();
    R3BEventHeader* EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetSource(source);
    run->SetRunId(999);
    run->SetSink(new FairRootFileSink(rootFileName));
    
    cout << " macro0 " << endl;

    // -------------------------------------------
    auto parIn = new FairParRootFileIo(false);
    //parIn->open(tcalParFileName);

    auto parOut = new FairParRootFileIo(true);
    parOut->open(syncParFileName);

    auto rtdb = run->GetRuntimeDb();

    auto parList = new TList();

    parList->Add(new TObjString("neuland_mapping_s509.root"));
    parList->Add(new TObjString(tcalParFileName));

    parIn->open(parList);
    
    rtdb->setFirstInput(parIn);
    rtdb->setOutput(parOut);
    rtdb->addRun(999);
    rtdb->getContainer("neulandMappingPar");
    rtdb->setInputVersion(999, (char*)"neulandMappingPar", 1, 1);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(999, (char*)"LandTCalPar", 1, 1);
    ((R3BNeulandHitPar*)rtdb->getContainer("NeulandHitPar"))->SetNumberOfPlanes(nPlanes);

    cout << " macro1 " << endl;
    
    // Add analysis task --------------------------------------------------------
    // Add Header copy
    R3BEventHeaderPropagator *RunIdTask = new R3BEventHeaderPropagator();
    run->AddTask(RunIdTask);

    cout << " macro2 " << endl;
    // -------------------------------------------
    auto tcal = new R3BNeulandMapped2Cal();
    tcal->SetTrigger(trigger);
    tcal->SetNofModules(nPlanes, nBarsPerPlane);
    tcal->EnableWalk(true);
    run->AddTask(tcal);

    cout << " macro3 " << endl;
    
    auto hitpar = new R3BNeulandCal2HitPar(); // "DEBUG"
    cout << " macro3.5 " << endl; 
    hitpar->SetTpat(cosmtpat);
    hitpar->SavePlots();
    run->AddTask(hitpar);

    cout << " macro4 " << endl;
    
    run->Init();
    cout << " macro5 " << endl;
    
    // -------------------------------------------
    //for (int i = 225; i <=235 ; i++) rtdb->setInputVersion(i, (char*)"LandTCalPar", 1, 1);
    //FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");

    run->Run(0,nev);
    //run->Run();

    cout << " macro6 " << endl;
    
    rtdb->saveOutput();
    // -------------------------------------------
    
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}
