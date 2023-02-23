// NeuLAND Online Monitoring
// run: root -l -q -b neuland_online.C

struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_TPAT tpat;
    EXT_STR_h101_LOS_t los;
    EXT_STR_h101_raw_nnp_tamex_onion_t raw_nnp;
};

void neuland_lmd()
{
    TStopwatch timer;
    timer.Start();
  
    const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
    const Int_t nPlanes = 24;       // number of planes (for TCAL calibration)
    const double distanceToTarget = 1520.+2.5;

    const Int_t nev = -1;     /* number of events to read, -1 - until CTRL+C */
    const Int_t trigger = -1; // 1 - onspill, 2 - offspill. -1 - all

    const TString filename = "/lustre/r3b/202104_s515/stitched/main0497_*.lmd";
    
    const TString ucesbPath = "/u/land/fake_cvmfs/9.13/upexps/202104_s515/202104_s515";

    const TString usesbCall = ucesbPath + " --allow-errors --input-buffer=100Mi";
    
    const TString outputFileName = "/lustre/land/Neuland/run497_testx_trig.root";

    // Event IO Setup
    // -------------------------------------------
    EXT_STR_h101 ucesbStruct;
    auto source = new R3BUcesbSource(filename, "RAW", usesbCall, &ucesbStruct, sizeof(ucesbStruct));
    //auto source = new R3BUcesbSource(filename, "RAW,time-stitch=1000", usesbCall, &ucesbStruct, sizeof(ucesbStruct));
    source->SetMaxEvents(nev);

    source->AddReader(new R3BUnpackReader(&ucesbStruct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BTrloiiTpatReader((EXT_STR_h101_TPAT_t*)&ucesbStruct.tpat, offsetof(EXT_STR_h101, tpat)));

    auto losreader = new R3BLosReader(&ucesbStruct.los, offsetof(EXT_STR_h101, los));
    //losreader->SetSkipTriggerTimes();
    source->AddReader(losreader);

    auto nlreader = new R3BNeulandTamexReader(&ucesbStruct.raw_nnp, offsetof(EXT_STR_h101, raw_nnp));
    nlreader->SetMaxNbPlanes(24);
    //nlreader->SetSkipTriggerTimes();
    source->AddReader(nlreader);
                      
    auto run = new FairRunOnline(source);
    run->SetRunId(999);
    //run->ActivateHttpServer(1, 8897);
    run->SetSink(new FairRootFileSink(outputFileName));

    auto EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);                                                                           

    // Parameter IO Setup
    // -------------------------------------------
    auto rtdb = run->GetRuntimeDb();

    auto parList = new TList();

    parList->Add(new TObjString("neuland_mapping_s515.root"));
    parList->Add(new TObjString("los_trigger.root"));
    //parList->Add(new TObjString("params_ig_out.root"));
    //parList->Add(new TObjString("params_ig_zeroes.root"));
    parList->Add(new TObjString("params_sync_s515_0497_trig.root"));
    
    auto parIO = new FairParRootFileIo(false);
    
    //parIO->open("params_tcal.root");
    //parIO->open("params_sync_s515_0999.root");
    
    parIO->open(parList);
    rtdb->setFirstInput(parIO);

    rtdb->addRun(999);
    rtdb->getContainer("neulandMappingPar");
    rtdb->setInputVersion(999, (char*)"neulandMappingPar", 1, 1);
    rtdb->getContainer("LosTCalPar");
    rtdb->setInputVersion(999, (char*)"LosTCalPar", 1, 1);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(999, (char*)"LandTCalPar", 1, 1);
    rtdb->getContainer("NeulandHitPar");
    rtdb->setInputVersion(999, (char*)"NeulandHitPar", 1, 1);
    
    auto parIOout = new FairParRootFileIo(true);
    parIOout->open("djdj.root");
    rtdb->setOutput(parIOout);
    
    auto losMapped2Cal = new R3BLosMapped2Cal();
    losMapped2Cal->SetNofModules(1, 8);
    losMapped2Cal->SetTrigger(trigger);
    run->AddTask(losMapped2Cal);
    
    run->AddTask(new R3BLosProvideTStart());
    
    auto tcal = new R3BNeulandMapped2Cal();
    tcal->SetTrigger(trigger);
    tcal->SetNofModules(nPlanes, nBarsPerPlane);
    tcal->SetNhitmin(1);
    tcal->EnableWalk(true);
    //tcal->SubtractTriggerTime(false);
    run->AddTask(tcal);
    
    auto nlhit = new R3BNeulandCal2Hit();
    nlhit->SetDistanceToTarget(distanceToTarget);
    //nlhit->SetGlobalTimeOffset(10461.6);
    nlhit->SetGlobalTimeOffset(9931.8);
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
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
    rtdb->saveOutput();
 
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;
}
