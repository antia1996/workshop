void neuland_ana(const int RunID=490)
{
    const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
    const Int_t nPlanes = 24;       // number of planes (for TCAL calibration)
    const double distanceToTarget = 1522.5;

    const TString inFilePattern = "/lustre/land/gasparic/workshop/s515_neuland_mapped_%04d.root";
    const TString rootFileName = TString::Format("/lustre/land/gasparic/workshop/s515_hit_%04d.root", RunID);  
    //const TString rootFileName = TString::Format("/lustre/land/your_name/your_directory/s515_hit_%04d.root", RunID);
    
    const Int_t nev = -1;
    const Int_t cosmtpat = 0x800; // cosmics trigger
    
    // -------------------------------------------
    auto source = new R3BFileSource(TString::Format(inFilePattern, RunID));
    source->SetRunId(RunID);

    auto run = new FairRunAna();
    R3BEventHeader* EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetSource(source);
    run->SetRunId(RunID);
    run->SetSink(new FairRootFileSink(rootFileName));
    // -------------------------------------------
    
    auto parList = new TList();
    parList->Add(new TObjString("neuland_mapping_s515.root"));
    parList->Add(new TObjString("los_trigger.root"));
    parList->Add(new TObjString("s515_params_sync_0490.root"));
    
    auto parIn = new FairParRootFileIo(false);
    parIn->open(parList);

    auto rtdb = run->GetRuntimeDb();
    rtdb->setFirstInput(parIn);
    rtdb->addRun(RunID);
    rtdb->getContainer("neulandMappingPar");
    rtdb->setInputVersion(RunID, (char*)"neulandMappingPar", 1, 1);
    rtdb->getContainer("LosTCalPar");
    rtdb->setInputVersion(RunID, (char*)"LosTCalPar", 1, 1);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(RunID, (char*)"LandTCalPar", 1, 1);
    rtdb->getContainer("NeulandHitPar");
    rtdb->setInputVersion(RunID, (char*)"NeulandHitPar", 1, 1);
    
    // Add analysis task --------------------------------------------------------
    // Add Header copy
    R3BEventHeaderPropagator *RunIdTask = new R3BEventHeaderPropagator();
    run->AddTask(RunIdTask);

    auto losMapped2Cal = new R3BLosMapped2Cal();
    losMapped2Cal->SetNofModules(1, 8);
    run->AddTask(losMapped2Cal);
    
    run->AddTask(new R3BLosProvideTStart());
    
    auto tcal = new R3BNeulandMapped2Cal();
    tcal->SetNofModules(nPlanes, nBarsPerPlane);
    tcal->EnableWalk(true);
    //tcal->SubtractTriggerTime(false);
    run->AddTask(tcal);
    
    auto nlhit = new R3BNeulandCal2Hit();
    nlhit->SetDistanceToTarget(distanceToTarget);
    nlhit->SetGlobalTimeOffset(400.8);
    nlhit->SetEnergyCutoff(0.0);
    run->AddTask(nlhit);

    //auto r3bNeulandClusterFinder = new R3BNeulandClusterFinder(2. * 7.5, 2. * 7.5, 2. * 7.5, 5.);
    //run->AddTask(r3bNeulandClusterFinder);
    
    auto r3bNeulandOnlineSpectra = new R3BNeulandOnlineSpectra();
    r3bNeulandOnlineSpectra->SetDistanceToTarget(distanceToTarget);
    r3bNeulandOnlineSpectra->SetCosmicTpat(0x800);
    run->AddTask(r3bNeulandOnlineSpectra);
    
    // Go!
    // -------------------------------------------
    run->Init();
    
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    run->Run(0,0);
    //rtdb->saveOutput();
 
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << rootFileName << endl;
}
