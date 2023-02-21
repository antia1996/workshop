void anacal2hit(const int RunID=490)
{
  const TString rootFileName = TString::Format("/lustre/land/gasparic/workshop/s515_anacal2hit_%04dx.root", RunID);  

  const TString tcalParFileName = TString::Format("s515_params_tcal_%04d.root", RunID);
  const TString syncParFileName = TString::Format("s515_params_sync_%04d.root", RunID);

  const TString inFilePattern = "/lustre/land/gasparic/workshop/s515_neuland_mapped_%04dx.root"; 

  const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
  const Int_t nPlanes = 24;       // number of planes (for TCAL calibration)
  const Int_t nev = -1;
  const Int_t trigger = -1; // 1 - onspill, 2 - offspill. -1 - all
  const Int_t cosmtpat = 0x800; // cosmics trigger

  TStopwatch timer;
  timer.Start();
    // -------------------------------------------
    auto source = new R3BFileSource(TString::Format(inFilePattern, RunID));
    source->SetRunId(RunID);
    /*for (int i = 1; i <= nSubruns; i++)
    {
      source->AddFile(TString::Format(inFilePattern, RunID+i));
    }
    */
    auto run = new FairRunAna();
    R3BEventHeader* EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetSource(source);
    run->SetRunId(RunID);
    run->SetSink(new FairRootFileSink(rootFileName));
    
    cout << " macro0 " << endl;

    // -------------------------------------------
    auto parIn = new FairParRootFileIo(false);
    //parIn->open(tcalParFileName);

    auto parOut = new FairParRootFileIo(true);
    parOut->open(syncParFileName);

    auto rtdb = run->GetRuntimeDb();

    auto parList = new TList();

    parList->Add(new TObjString("neuland_mapping_s515.root"));
    parList->Add(new TObjString(tcalParFileName));

    parIn->open(parList);
    
    rtdb->setFirstInput(parIn);
    rtdb->setOutput(parOut);
    rtdb->addRun(RunID);
    rtdb->getContainer("neulandMappingPar");
    rtdb->setInputVersion(RunID, (char*)"neulandMappingPar", 1, 1);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(RunID, (char*)"LandTCalPar", 1, 1);
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
