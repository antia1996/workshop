// Macro to produce mapped data level from stitched lmd data.
// storage, lmdfile, outstorage and outFile need to be adapted prior to use.
// When used for other experiments, also ucesbPath and Readers need to be adapted.
//	called in root using: .L lmd2root.C ; lmd2root(runnumber)
//	output is mapped root tree for NeuLAND
//
struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_TPAT tpat;
    EXT_STR_h101_LOS_t los;
    EXT_STR_h101_raw_nnp_tamex_onion_t raw_nnp;
    EXT_STR_h101_WRMASTER_t wrmaster;
};

void lmd2root(const int runID)
{
    TStopwatch timer;
    timer.Start();
      
    const TString storage = "/lustre/r3b/202205_s509/lmd_stitched/";
    const TString lmdfile = storage + TString::Format("main%04d_*.lmd", runID);
    const TString outstorage = "/lustre/land/Neuland/";
    const TString outFile = outstorage + TString::Format("s509_xmap%04d.root", runID);
    const TString ucesbPath = "/u/land/fake_cvmfs/10/jan23/upexps/202205_s509/202205_s509";
    const TString usesbCall = ucesbPath + " --allow-errors --input-buffer=100Mi";

    EXT_STR_h101 ucesbStruct;
    auto source = new R3BUcesbSource(lmdfile, "RAW", usesbCall, &ucesbStruct,          sizeof(ucesbStruct));

    source->AddReader(new R3BUnpackReader(&ucesbStruct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BTrloiiTpatReader(&ucesbStruct.tpat, offsetof(EXT_STR_h101, tpat)));
    source->AddReader(new R3BLosReader(&ucesbStruct.los, offsetof(EXT_STR_h101, los)));
    source->AddReader(new R3BNeulandTamexReader(&ucesbStruct.raw_nnp, offsetof(EXT_STR_h101, raw_nnp)));
    source->AddReader(new R3BWhiterabbitMasterReader((EXT_STR_h101_WRMASTER*)&ucesbStruct.wrmaster, offsetof(EXT_STR_h101, wrmaster), 0x1000));

    auto run = new FairRunOnline(source);
    auto EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetRunId(999);
    run->SetSink(new FairRootFileSink(outFile));

    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    //run->Run(-1,0);

    run->Run(0,100000);
    
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;
}
