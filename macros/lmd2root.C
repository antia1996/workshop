struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_TPAT tpat;
    EXT_STR_h101_LOS_t los;
    EXT_STR_h101_raw_nnp_tamex_onion_t raw_nnp;
  //EXT_STR_h101_WRMASTER_t wrmaster;
};

void lmd2root(const int runID=490)
{
    const TString storage = "/lustre/r3b/202104_s515/stitched/ig4000/";
    const TString lmdfile = storage + TString::Format("main%04d_*.lmd", runID);
    //const TString outstorage = "/lustre/land/your_name/your_directory/";
    const TString outstorage = "/lustre/land/gasparic/workshop/";
    const TString outFile = outstorage + TString::Format("s515_neuland_mapped_%04d.root", runID);
    const TString ucesbPath = "/u/land/fake_cvmfs/10/jan23/upexps/202104_s515/202104_s515";
    const TString usesbCall = ucesbPath + " --allow-errors --input-buffer=100Mi";

    EXT_STR_h101 ucesbStruct;
    auto source = new R3BUcesbSource(lmdfile, "RAW", usesbCall, &ucesbStruct, sizeof(ucesbStruct));

    source->AddReader(new R3BUnpackReader(&ucesbStruct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BTrloiiTpatReader(&ucesbStruct.tpat, offsetof(EXT_STR_h101, tpat)));
    source->AddReader(new R3BLosReader(&ucesbStruct.los, offsetof(EXT_STR_h101, los)));
    source->AddReader(new R3BNeulandTamexReader(&ucesbStruct.raw_nnp, offsetof(EXT_STR_h101, raw_nnp)));
    //source->AddReader(new R3BWhiterabbitMasterReader((EXT_STR_h101_WRMASTER*)&ucesbStruct.wrmaster, offsetof(EXT_STR_h101, wrmaster), 0x1000));

    auto run = new FairRunOnline(source);
    auto EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetRunId(runID);
    run->SetSink(new FairRootFileSink(outFile));

    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    run->Run(-1,0);

    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outFile << endl;
}
