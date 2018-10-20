

int make_mudst(const int nevents=1e9,
                  const char* inputFile="/star/data01/scratch/nelsey/run4/044/5044086/st_physics_5044086_raw_1010001.event.root",
                  const char* outDir="./") {

  gROOT->Macro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");

  cout << " loading done " << endl;
  StChain* chain = new StChain("StChain"); 

  // ********************************
  // Now we add Makers to the chain... 
  // ********************************
  
  // *************
  // file handling
  // *************
  StIOMaker* ioMaker = new StIOMaker("IOMaker","r",inputFile,"bfcTree");
  
  
  ioMaker->SetIOMode("r");
  
  ioMaker->SetBranch("*",0,"0");         //deactivate all branches
  ioMaker->SetBranch("eventBranch",0,"r"); //activate evt.root Branch
  ioMaker->SetBranch("emcBranch",0,"r");   //activate evt.root Branch
  ioMaker->SetBranch("runcoBranch",0,"r"); //activate evt.root Branch
  
  // ***********************
  // the StStrangeMuDstMaker
  // ***********************
  
  //StStrangeMuDstMaker* v0dst = new StStrangeMuDstMaker("strangeMuDst");
  //v0dst->DoV0(); //Set v0MiniDstMaker to find only v0s
  //v0dst->DoXi(); //Set v0MiniDstMaker to find only v0s
  //v0dst->DoKink(); //Set v0MiniDstMaker to find only v0s
  //v0dst->SetNoKeep(); 
  //v0dst->SetWrite("depp.root"); // Set V0muDStMaker output file and Event output file
  // v0dst->SetWrite("StrangemuEventHBTPeriphdst.root","Strangemuv0HBTPeriphdst.root"); // Set V0muDStMaker output file and Event output file
  
  
  StMuDstMaker* maker = new StMuDstMaker(1,1,"./");
  //  maker->setSplit(500);
  //  maker->setBufferSize(4000);
  //  To use a different PID file than the default, uncomment and
  // modify the above
  //  maker->setProbabilityPidFile("Path/PIDTable.root");
  maker->setProbabilityPidFile();
  
  StMuL3Filter* l3Filter = new StMuL3Filter(); maker->setL3TrackFilter(l3Filter);
  StMuFilter* filter = new StMuFilter();       maker->setTrackFilter(filter);
  //  filter->addEncodedMethod(32770);
  //  filter->addEncodedMethod(8,7);
  //StMuDebug::setLevel(0);
  
  chain->Init(); // This should call the Init() method in ALL makers
  chain->PrintInfo();
  for (Int_t iev=0;iev<nevents; iev++) {
    cout << "StExample -- Working on eventNumber " << iev << endl;
    chain->Clear();
    int iret = chain->Make(iev); // This should call the Make() method in ALL makers    
    if (iret) {
      cout << "Bad return code!" << endl;
      break;
    }
  } // Event Loop
  chain->Finish(); // This should call the Finish() method in ALL makers
  
  cout << " End of Analysis " << endl;
  return 0;
}

