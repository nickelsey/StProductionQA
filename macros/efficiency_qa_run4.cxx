 /* STAR Collaboration - Nick Elsey
  
    Example of how to use StEfficiencyQARun4 to produce QA histograms
  
    Requires the STAR libraries and muDST files. Defaults
    are set to a test production using the test.list file
    which should be filled with a few muDST  files before
    running :)
  
    arguments -- 
    nEvents:       number of events to reproduce
    muFileList:    list of filenames & paths to muDSTs
    nametag:       identifier used in output file name
    nFiles:        number of files to accept from the file list
*/

void efficiency_qa_run4(int nEvents = 1e9,
                        const char* muFileList = "test.list",
                        const char* nametag  = "StEfficiencyQARun4",
                        int nFiles = 3,
                        int trigSet = 1,
                        double dcaMax = 1.0,
                        int nHitMin = 20,
                        double nHitFracMin = 0.52)
{
  // load STAR libraries
  gROOT->Macro("LoadLogger.C");
  gROOT->Macro("loadMuDst.C");
  
  // load local libraries
  gSystem->Load("libStEfficiencyQARun4.so");
  gSystem->Load("libStRefMultCorr.so");
  
  StChain* chain = new StChain("StChain");
  StMuDstMaker* muDstMaker = new StMuDstMaker(0, 0, "", muFileList, "", nFiles);

  std::string outname = std::string(nametag) + ".root";
  StEfficiencyQARun4* qa_maker = new StEfficiencyQARun4(outname, "StEfficiencyQA");
  qa_maker->GetEventCuts().SetVzRange(-30, 30);
  qa_maker->GetEventCuts().SetRefMultRange(0, 7000);
  qa_maker->GetEventCuts().SetZdcEastRange(5, 9999999);
  qa_maker->GetEventCuts().SetZdcWestRange(5, 9999999);
  //qa_maker->GetEventCuts().SetZdcVzRange(-30, 30);
  //qa_maker->GetEventCuts().SetCtbSumRange(75, 999999);
  qa_maker->SetDCAMax(dcaMax);
  qa_maker->SetMinFitPoints(nHitMin);
  qa_maker->SetMinFitFrac(nHitFracMin);
  qa_maker->GetEventCuts().AddTrigger(15007);
  
  // for each event, print the memory usage
  // helpful for debugging
  StMemStat memory;
  memory.PrintMem(NULL);
	
  if (chain->Init()) { cout<<"StChain failed init: exiting"<<endl; return;}
  cout << "chain initialized" << endl;
	
  TStopwatch total;
  TStopwatch timer;
	
  int i=0;
  while ( i < nEvents && chain->Make() == kStOk ) {
    if ( i % 500 == 0 ) {
      cout<<"done with event "<<i;
      cout<<"\tcpu: "<<timer.CpuTime()<<"\treal: "<<timer.RealTime()<<"\tratio: "<<timer.CpuTime()/timer.RealTime();//<<endl;
      timer.Start();
      memory.PrintMem(NULL);
    }
    i++;
    chain->Clear();
  }
  
  chain->ls(3);
  chain->Finish();
  printf("my macro processed %i events in %s", i, nametag);
  cout << "\tcpu: " << total.CpuTime() << "\treal: " << total.RealTime() << "\tratio: " << total.CpuTime() / total.RealTime() << endl;

  cout << endl;
  cout << "--------------" << endl;
  cout << " (-: Done :-) " << endl;
  cout << "--------------" << endl;
  cout << endl;
}
