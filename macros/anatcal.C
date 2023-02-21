//void anatcal(const int RunID, const int nSubruns)
void anatcal(const int RunID=490)
{
    TStopwatch timer;
    timer.Start();
    const TString inFilePattern = "/lustre/land/gasparic/workshop/s515_neuland_mapped_%04dx.root";
    const TString parFileName = TString::Format("s515_params_tcal_%04d.root", RunID);
    const TString histFileName = TString::Format("/lustre/land/gasparic/s515_anatcal-out-%04d.root", RunID);

    const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
    const Int_t nPlanes = 24;       // number of planes (for TCAL calibration)
    const Int_t minStats = 1000;
    const Int_t trigger = -1; // 1 - onspill, 2 - offspill. -1 - all
    const Int_t nev = -1;
    // -------------------------------------------
    auto source = new R3BFileSource(TString::Format(inFilePattern, RunID));

    source->SetRunId(999);
    //for (int i = 1; i <= nSubruns; i++)
    //{
    //     source->AddFile(TString::Format(inFilePattern, RunID+i));
    //}
    
    auto run = new FairRunAna();
    run->SetSource(source);
    run->SetRunId(999);

    auto EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetSink(new FairRootFileSink(histFileName));
    // -------------------------------------------
    auto parIO = new FairParRootFileIo(true);
    parIO->open(parFileName);
    auto rtdb = run->GetRuntimeDb();
    //rtdb->setFirstInput(parIO);
    rtdb->setOutput(parIO);
    rtdb->initContainers(999);
    
    // Add analysis task --------------------------------------------------------
    auto tcalpar = new R3BNeulandMapped2CalPar();
    tcalpar->SetMinStats(minStats);
    //tcalpar->SetTrigger(trigger);
    tcalpar->SetNofModules(nPlanes, nBarsPerPlane);
    run->AddTask(tcalpar);

    // -------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    run->Run(0,0);
    rtdb->saveOutput();
    // -------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output files are " <<  parFileName << " and " << histFileName  <<  endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;
}
