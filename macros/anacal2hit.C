void anacal2hit(const int RunID=490)
{
  const TString inFilePattern = "/lustre/land/gasparic/workshop/s515_neuland_mapped_%04d.root";
  const TString rootFileName = TString::Format("/lustre/land/gasparic/workshop/s515_anacal2hit_%04d.root", RunID);
  //const TString rootFileName = TString::Format("/lustre/land/your_name/your_directory/s515_anacal2hit_%04d.root", RunID);
  const TString tcalParFileName = TString::Format("s515_params_tcal_%04d.root", RunID);
  const TString syncParFileName = TString::Format("s515_params_sync_%04d.root", RunID);
  const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
  const Int_t nPlanes = 24;       // number of planes (for TCAL calibration)
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
    parList->Add(new TObjString(tcalParFileName));
    auto parIn = new FairParRootFileIo(false);
    parIn->open(parList);
    auto parOut = new FairParRootFileIo(true);
    parOut->open(syncParFileName);
    auto rtdb = run->GetRuntimeDb();

    rtdb->setFirstInput(parIn);
    rtdb->setOutput(parOut);
    rtdb->addRun(RunID);
    rtdb->getContainer("neulandMappingPar");
    rtdb->setInputVersion(RunID, (char*)"neulandMappingPar", 1, 1);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(RunID, (char*)"LandTCalPar", 1, 1);
    ((R3BNeulandHitPar*)rtdb->getContainer("NeulandHitPar"))->SetNumberOfPlanes(nPlanes);

    // Add analysis task --------------------------------------------------------
    // Add Header copy
    R3BEventHeaderPropagator *RunIdTask = new R3BEventHeaderPropagator();
    run->AddTask(RunIdTask);
    // -------------------------------------------
    auto tcal = new R3BNeulandMapped2Cal();
    tcal->SetNofModules(nPlanes, nBarsPerPlane);
    tcal->EnableWalk(true);
    run->AddTask(tcal);

    auto hitpar = new R3BNeulandCal2HitPar(); // "DEBUG"
    hitpar->SetTpat(cosmtpat);
    hitpar->SavePlots();
    run->AddTask(hitpar);

    run->Init();
    // -------------------------------------------
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");

    run->Run(0,nev);
    rtdb->saveOutput();
    // -------------------------------------------
    
    cout << "Macro finished succesfully." << endl;
    cout << "Output files are " <<  syncParFileName << " and " << rootFileName  <<  endl;
}
