void make_hit_par_file(Int_t runid)
{
  const TString runString = TString::Format("%04d",runid);

  const TString parInFilename = "../../exp/parameters/neuland_sync_s515_" + runString + "_trig.root";
  const TString parOutFilename = "../../exp/parameters/neuland_hit_" + runString + ".root";
  
  auto rtdb = FairRuntimeDb::instance();

  auto parIn = new FairParRootFileIo(false);
  parIn->open(parInFilename);
  rtdb->setFirstInput(parIn);

  auto parOut = new FairParRootFileIo(true);
  parOut->open(parOutFilename);
  rtdb->setOutput(parOut);

  auto hpars = (R3BNeulandHitPar*)rtdb->getContainer("NeulandHitPar");

  //rtdb->addRun(runid);
  //rtdb->setInputVersion(runid, (char*)"NeulandHitPar", 1, 1);  
  rtdb->initContainers(runid);

  hpars->SetDistanceToTarget(1522.5);
  hpars->SetGlobalTimeOffset(9931.8);

  //  hpars->setChanged();          // writes a new version
  //  rtdb->writeContainers();      // doesn't write FairRtdbRun
  
  rtdb->saveOutput();               // writes FairRtdbRun
  
}
