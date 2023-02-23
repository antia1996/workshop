void anatcal(const int RunID=490)
{
    const TString inFilePattern = "/lustre/land/gasparic/workshop/s515_neuland_mapped_%04d.root";
    const TString parFileName = TString::Format("s515_params_tcal_%04d.root", RunID);
    const TString histFileName = TString::Format("/lustre/land/gasparic/workshop/s515_anatcal_out_%04d.root", RunID);
    //const TString histFileName = TString::Format("/lustre/land/your_name/your_directory/s515_anatcal_out_%04d.root", RunID);
    
    const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
    const Int_t nPlanes = 24;       // number of planes (for TCAL calibration)
    const Int_t minStats = 1000;
    const Int_t nev = -1;
    // -------------------------------------------
    auto source = new R3BFileSource(TString::Format(inFilePattern, RunID));
    source->SetRunId(RunID);
    
    auto run = new FairRunAna();
    run->SetSource(source);
    run->SetRunId(RunID);

    auto EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetSink(new FairRootFileSink(histFileName));
    // -------------------------------------------
    auto parIO = new FairParRootFileIo(false);
    parIO->open(parFileName,"recreate");
    auto rtdb = run->GetRuntimeDb();
    rtdb->setOutput(parIO);
    rtdb->initContainers(RunID);
    
    // Add analysis task --------------------------------------------------------
    auto tcalpar = new R3BNeulandMapped2CalPar();
    tcalpar->SetMinStats(minStats);
    tcalpar->SetNofModules(nPlanes, nBarsPerPlane);
    run->AddTask(tcalpar);

    // -------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    run->Run(0,0);
    rtdb->saveOutput();
    // -------------------------------------------

    cout << "Macro finished succesfully." << endl;
    cout << "Output files are " <<  parFileName << " and " << histFileName  <<  endl;
}
