
#include "mapping_neuland_trig_s515.hh" // Copy this file from your upexps/202104_s515 folder

void make_nl_trig_mapping(){

  nl_trig_map_setup();

  auto rtdb = FairRuntimeDb::instance();
  
  auto parOut = new FairParRootFileIo();
  parOut->open("neuland_mapping_s515.root", "new");
  rtdb->setOutput(parOut);

  auto par3 = (R3BNeulandMappingPar*)(rtdb->getContainer("neulandMappingPar"));
  
  rtdb->addRun(1);

  for(Int_t i = 0; i < 24; i++)
  for(Int_t j = 0; j < 2; j++)
  for(Int_t k = 0; k < 50; k++)
  {  
    par3->SetTrigMap(g_nl_trig_map[i][j][k],i+1,k+1,j+1);
  }

  par3->SetNbPlanes(24);
  par3->SetNbPaddles(50);
  
  par3->setChanged();
  
  rtdb->writeContainers();

  rtdb->saveOutput();

  rtdb->print();
}
