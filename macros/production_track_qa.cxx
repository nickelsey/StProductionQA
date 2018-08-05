 /* STAR Collaboration - Nick Elsey
  
    Example of how to use StProductionQA to produce QA trees
  
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

void production_track_qa(int nEvents = 1e9,
                         const char* muFileList = "test.list",
                         const char* nametag  = "StProductionTrackQA_example",
                         int nFiles = 3,
                         int trigSet = 0)
{
  // load STAR libraries
  gROOT->Macro("LoadLogger.C");
  gROOT->Macro("loadMuDst.C");
  
  // load local libraries
  gSystem->Load("libStProductionQA.so");
  
  StChain* chain = new StChain("StChain");
  StMuDstMaker* muDstMaker = new StMuDstMaker(0, 0, "", muFileList, "", nFiles);

  std::string outname = std::string(nametag) + ".root";
  StProductionQA* qa_maker = new StProductionQA(outname, "StProductionQA");
  // set triggers to be reproduced
  switch (trigSet) {
    case 0:
      qa_maker->Cuts().AddTrigger(450202);
      qa_maker->Cuts().AddTrigger(450212);
      qa_maker->Cuts().AddTrigger(450203);
      qa_maker->Cuts().AddTrigger(450213);
      break;
    case 1:
      qa_maker->Cuts().AddTrigger(450010);
      qa_maker->Cuts().AddTrigger(450020);
      break;
    case 2:
      qa_maker->Cuts().AddTrigger(450010);
      qa_maker->Cuts().AddTrigger(450020);
      qa_maker->Cuts().AddTrigger(450008);
      qa_maker->Cuts().AddTrigger(450018);
      qa_maker->Cuts().AddTrigger(450012);
      qa_maker->Cuts().AddTrigger(450022);
    default :
      break;
  }

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
