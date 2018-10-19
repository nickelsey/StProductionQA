#include "event_cuts_run4.hh"
#include "St_base/StMessMgr.h"

#include <algorithm>
#include <string>
#include <sstream>

template <typename T> string tostr(const T& t) {
  std::ostringstream os;
  os<<t;
  return os.str();
}

ClassImp(EventCutsRun4)

EventCutsRun4::EventCutsRun4()
  : mNEvents(0), mEventsFailed(0), mEventsFailedVx(0),
    mEventsFailedVy(0),  mEventsFailedVz(0),
    mEventsFailedRef(0), mEventsFailedTrigger(),
    mEventsFailedTriggerTotal(0), mCheckVx(kFALSE),
    mCheckVy(kFALSE), mCheckVz(kFALSE), mCheckRefMult(kFALSE),
    mCheckTrigger(kFALSE), mUseGrefMult(kFALSE),
    mCheckZdcEast(kFALSE), mCheckZdcWest(kFALSE),
    mCheckZdcVz(kFALSE), mCheckCtbSum(kFALSE),
    mMinVx(0),   mMaxVx(0), mMinVy(0),   mMaxVy(0),
    mMinVz(0),   mMaxVz(0), mMinRef(0),  mMaxRef(0),
    mTriggers(), mMinZdcEast(0), mMaxZdcEast(0),
    mMinZdcWest(0), mMaxZdcWest(0), mMinZdcVz(0),
    mMaxZdcVz(0), mMinCtbSum(0), mMaxCtbSum(0) {}

EventCutsRun4::~EventCutsRun4() {
  
}

bool EventCutsRun4::AcceptEvent(StMuEvent* event) {
  /* checks all cuts for all events, event if it 
     has already failed one - gives a better idea of 
     relative acceptance rates 
   */
  mNEvents++;
  bool accept_event = kTRUE;
  if (mCheckVx && !AcceptVx(event))           accept_event = kFALSE;
  if (mCheckVy && !AcceptVy(event))           accept_event = kFALSE;
  if (mCheckVz && !AcceptVz(event))           accept_event = kFALSE;
  if (mCheckRefMult && !AcceptRefMult(event)) accept_event = kFALSE;
  if (mCheckTrigger && !AcceptTrigger(event)) accept_event = kFALSE;
  if (mCheckZdcEast && !AcceptZdcEast(event)) accept_event = kFALSE;
  if (mCheckZdcWest && !AcceptZdcWest(event)) accept_event = kFALSE;
  if (mCheckZdcVz && !AcceptZdcVz(event))     accept_event = kFALSE;
  if (mCheckCtbSum && !AcceptCtbSum(event))   accept_event = kFALSE;
  if (accept_event != kTRUE) {mEventsFailed++; std::cout << "Event Failed" << std::endl;}
  else {std::cout << "Event Accepted" << std::endl;}
  return accept_event;
}

Bool_t EventCutsRun4::AcceptVx(StMuEvent* event) {
  Double_t vx = event->primaryVertexPosition().x();
  if (vx > mMaxVx || vx < mMinVx) {
    std::cout << "Fail: Vx=" << vx << " minVx=" << mMinVx << " maxVx=" << mMaxVx << std::endl;
    mEventsFailedVx++;
    return kFALSE;
  }
  std::cout << "Pass: Vx=" << vx << " minVx=" << mMinVx << " maxVx=" << mMaxVx << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptVy(StMuEvent* event) {
  Double_t vy = event->primaryVertexPosition().y();
  if (vy > mMaxVy || vy < mMinVy) {
    std::cout << "Fail: Vy=" << vy << " minVy=" << mMinVy << " maxVy=" << mMaxVy << std::endl;
    mEventsFailedVy++;
    return kFALSE;
  }
  std::cout << "Pass: Vy=" << vy << " minVy=" << mMinVy << " maxVy=" << mMaxVy << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptVz(StMuEvent* event) {
  Double_t vz = event->primaryVertexPosition().z();
  if (vz > mMaxVz || vz < mMinVz) {
    std::cout << "Fail: Vz=" << vz << " minVz=" << mMinVz << " maxVz=" << mMaxVz << std::endl;
    mEventsFailedVz++;
    return kFALSE;
  }
  std::cout << "Pass: Vz=" << vz << " minVz=" << mMinVz << " maxVz=" << mMaxVz << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptTrigger(StMuEvent* event) {
  Bool_t accept = kFALSE;
  for (unsigned int i = 0; i < mTriggers.size(); ++i) {
    if (event->triggerIdCollection().nominal().isTrigger(mTriggers[i]) == kTRUE) {
      accept = kTRUE;
      std::cout << "Pass: trigger accepted=" << mTriggers[i] << std::endl;
    } else {mEventsFailedTrigger[i]++;}
  }
  if (accept == kFALSE) {mEventsFailedTriggerTotal++; std::cout << "Fail: no trigger accepted" << std::endl;}
  return accept;
}

Bool_t EventCutsRun4::AcceptRefMult(StMuEvent* event) {
  UInt_t refmult = mUseGrefMult ? event->grefmult() : event->refMult();
  if (refmult > mMaxRef || refmult < mMinRef) {
    std::cout << "Fail: refmult=" << refmult << " min refmult=" << mMinRef << " max refmult=" << mMaxRef << std::endl;
    mEventsFailedRef++;
    return kFALSE;
  }
  std::cout << "Pass: refmult=" << refmult << " min refmult=" << mMinRef << " max refmult=" << mMaxRef << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptZdcEast(StMuEvent* event) {
  Double_t zdcE = event->runInfo().zdcEastRate();
  if (zdcE > mMaxZdcEast || zdcE < mMinZdcEast) {
    std::cout << "Fail: ZDC East=" << zdcE << " minZDC-E=" << mMinZdcEast << " maxZDC-E=" << mMaxZdcEast << std::endl;
    mEventsFailedZdcEast++;
    return kFALSE;
  }
  std::cout << "Pass: ZDC East=" << zdcE << " minZDC-E=" << mMinZdcEast << " maxZDC-E=" << mMaxZdcEast << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptZdcWest(StMuEvent* event) {
  Double_t zdcW = event->runInfo().zdcWestRate();
  if (zdcW > mMaxZdcWest || zdcW < mMinZdcWest) {
    std::cout << "Fail: ZDC West=" << zdcW << " minZDC-W=" << mMinZdcWest << " maxZDC-W=" << mMaxZdcWest << std::endl;
    mEventsFailedZdcWest++;
    return kFALSE;
  }
  std::cout << "Pass: ZDC West=" << zdcW << " minZDC-W=" << mMinZdcWest << " maxZDC-W=" << mMaxZdcWest << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptZdcVz(StMuEvent* event) {
  Double_t zdcVz = event->zdcTriggerDetector().vertexZ();
  if (zdcVz > mMaxZdcVz || zdcVz < mMinZdcVz) {
    std::cout << "Fail: ZDC Vz=" << zdcVz << " minZDC-Vz=" << mMinZdcVz << " maxZDC-Vz=" << mMaxZdcVz << std::endl;
    mEventsFailedZdcVz++;
    return kFALSE;
  }
  std::cout << "Pass: ZDC Vz=" << zdcVz << " minZDC-Vz=" << mMinZdcVz << " maxZDC-Vz=" << mMaxZdcVz << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptCtbSum(StMuEvent* event) {
  Double_t ctbSum = event->ctbMultiplicity();
  if (ctbSum > mMaxCtbSum || ctbSum < mMinCtbSum) {
    std::cout << "Fail: CTB Sum=" << ctbSum << " minCTB Sum=" << mMinCtbSum << " maxCtb Sum=" << mMaxCtbSum << std::endl;
    mEventsFailedCtbSum++;
    return kFALSE;
  }
  std::cout << "Pass: CTB Sum=" << ctbSum << " minCTB Sum=" << mMinCtbSum << " maxCtb Sum=" << mMaxCtbSum << std::endl;
  return kTRUE;
}

Bool_t EventCutsRun4::SetVxRange(Double_t min, Double_t max) {
  mCheckVx = kTRUE;
  mMinVx = min;
  mMaxVx = max;
  return mCheckVx;
}

Bool_t EventCutsRun4::SetVyRange(Double_t min, Double_t max) {
  mCheckVy = kTRUE;
  mMinVy = min;
  mMaxVy = max;
  return mCheckVy;
}

Bool_t EventCutsRun4::SetVzRange(Double_t min, Double_t max) {
  mCheckVz = kTRUE;
  mMinVz = min;
  mMaxVz = max;
  return mCheckVz;
}

Bool_t EventCutsRun4::SetRefMultRange(UInt_t min, UInt_t max) {
  mCheckRefMult = kTRUE;
  mMinRef = min;
  mMaxRef = max;
  return mCheckRefMult;
}

void   EventCutsRun4::UsegRefMult(bool use_gref) {
  mUseGrefMult = use_gref;
}

Bool_t EventCutsRun4::SetZdcEastRange(Double_t min, Double_t max) {
    mCheckZdcEast = kTRUE;
    mMinZdcEast = min;
    mMaxZdcEast = max;
    return mCheckZdcEast;
}

Bool_t EventCutsRun4::SetZdcWestRange(Double_t min, Double_t max) {
    mCheckZdcWest = kTRUE;
    mMinZdcWest = min;
    mMaxZdcWest = max;
    return mCheckZdcWest;
}

Bool_t EventCutsRun4::SetZdcVzRange(Double_t min, Double_t max) {
    mCheckZdcVz = kTRUE;
    mMinZdcVz = min;
    mMaxZdcVz = max;
    return mCheckZdcVz;
}

Bool_t EventCutsRun4::SetCtbSumRange(Double_t min, Double_t max) {
    mCheckCtbSum = kTRUE;
    mMinCtbSum = min;
    mMaxCtbSum = max;
    return mCheckCtbSum;
}

void EventCutsRun4::AddTrigger(unsigned int trigger) {
  if(std::find(mTriggers.begin(), mTriggers.end(), trigger) == mTriggers.end()) {
    mCheckTrigger = kTRUE;
    mTriggers.push_back(trigger);
    mEventsFailedTrigger.push_back(0);
  }
}

void EventCutsRun4::AddTrigger(std::vector<unsigned int> triggers) {
  for (unsigned id = 0; id < triggers.size(); ++id) {
    UInt_t trigger = triggers[id];
    if(std::find(mTriggers.begin(), mTriggers.end(), trigger) == mTriggers.end()) {
      mCheckTrigger = kTRUE;
      mTriggers.push_back(trigger);
      mEventsFailedTrigger.push_back(0);
    }
  }
}

void EventCutsRun4::PrintCuts() {
  std::cout << "// ------------------ EventCutsRun4 Cuts ------------------ //" << std::endl;
  std::cout << std::endl;
  if (mCheckVx || mCheckVy || mCheckVz) {std::cout << "Vertex cuts: " << std::endl;}
  if (mCheckVx) {std::cout << mMinVx << " < Vx < " << mMaxVx << std::endl;}
  if (mCheckVy) {std::cout << mMinVy << " < Vy < " << mMaxVy << std::endl;}
  if (mCheckVz) {std::cout << mMinVz << " < Vz < " << mMaxVz << std::endl;}
  if (mCheckRefMult) {std::cout << " Refmult Cuts: " << std::endl;}
  if (mCheckRefMult && mUseGrefMult)  {std::cout << "use grefmult" << std::endl;
                                          std::cout << mMinRef << " < gRefMult < " << mMaxRef << std::endl;}
  if (mCheckRefMult && !mUseGrefMult) {std::cout << "use refmult" << std::endl;
                                          std::cout << mMinRef << " < RefMult < " << mMaxRef << std::endl;}
  if (mCheckZdcEast) {std::cout << mMinZdcEast << " < Zdc-E < " << mMaxZdcEast << std::endl;}
  if (mCheckZdcWest) {std::cout << mMinZdcWest << " < Zdc-W < " << mMaxZdcWest << std::endl;} 
  if (mCheckZdcVz) {std::cout << mMinZdcVz << " < Zdc-Vz < " << mMaxZdcVz << std::endl;}
  if (mCheckCtbSum) {std::cout << mMinCtbSum << " < Ctb-Sum < " << mMaxCtbSum << std::endl;}
  
  if (mCheckTrigger && mTriggers.size() > 0) {
    std::string trigger_string = "[ " + tostr(mTriggers[0]);
    for (unsigned i = 1; i < mTriggers.size(); ++i) {
      trigger_string += ", " + tostr(mTriggers[i]);
    }
    std::cout << "Using triggers: " << trigger_string << std::endl;
  }
  std::cout << " // ------------------        End Cuts        ------------------ //" << std::endl;
}

void  EventCutsRun4::PrintStats() {
  std::cout << "// ------------------ EventCutsRun4 Stats ------------------ //" << std::endl;
  std::cout << "number of events:   " << mNEvents << std::endl;
  if (mCheckVx) {std::cout << "events rejected by Vx cut: " << mEventsFailedVx << std::endl;
                   std::cout << "\t percent loss: " << (Double_t) mEventsFailedVx / mNEvents << std::endl;}
  if (mCheckVy) {std::cout << "events rejected by Vy cut: " << mEventsFailedVy << std::endl;
                   std::cout << "\t percent loss: " << (Double_t) mEventsFailedVy / mNEvents << std::endl;}
  if (mCheckVz) {std::cout << "events rejected by Vz cut: " << mEventsFailedVz << std::endl;
                   std::cout << "\t percent loss: " << (Double_t) mEventsFailedVz / mNEvents << std::endl;}
  if (mCheckRefMult) {
    std::cout << "events rejected by RefMult cut: " << mEventsFailedVx << std::endl;
    std::cout << "\t using grefmult: "; if (mUseGrefMult) {std::cout << "true" << std::endl;} else {std::cout << "false" << std::endl;}
    std::cout << "\t percent loss: " << (Double_t) mEventsFailedRef / mNEvents << std::endl;
 }
 if (mCheckZdcEast) {std::cout << "events rejected by ZDC-E cut: " << mEventsFailedZdcEast << std::endl;
                   std::cout << "\t percent loss: " << (Double_t) mEventsFailedZdcEast / mNEvents << std::endl;}
 if (mCheckZdcWest) {std::cout << "events rejected by ZDC-W cut: " << mEventsFailedZdcWest << std::endl;
                   std::cout << "\t percent loss: " << (Double_t) mEventsFailedZdcWest / mNEvents << std::endl;}
 if (mCheckZdcVz) {std::cout << "events rejected by ZDC-Vz cut: " << mEventsFailedZdcVz << std::endl;
                   std::cout << "\t percent loss: " << (Double_t) mEventsFailedZdcVz / mNEvents << std::endl;}
 if (mCheckCtbSum) {std::cout << "events rejected by Ctb-Sum cut: " << mEventsFailedCtbSum << std::endl;
                   std::cout << "\t percent loss: " << (Double_t) mEventsFailedCtbSum / mNEvents << std::endl;}
  
  if (mCheckTrigger && mTriggers.size() > 0) {
    std::string trigger_string = "[ " + tostr(mTriggers[0]);
    std::string loss_string = "[ " + tostr(mEventsFailedTrigger[0]);
    for (unsigned i = 1; i < mTriggers.size(); ++i) {
      trigger_string += ", " + tostr(mTriggers[i]);
      loss_string += ", " + tostr(mEventsFailedTrigger[i]);
    }
    trigger_string += " ]"; loss_string += " ]";
    std::cout << "events rejected by triggers: " << std::endl;
    std::cout << "using triggers: " << trigger_string << std::endl;
    std::cout << "rejected: " << loss_string << std::endl;
    std::cout << "total events rejected by trigger: " << mEventsFailedTriggerTotal << std::endl;
  }
  std::cout << "total number of events rejected: " << mEventsFailed << std::endl;
  std::cout << "\t percent loss: " << (Double_t) mEventsFailed / mNEvents << std::endl;
  std::cout << " // ------------------        End Stats        ------------------ //" << std::endl;
  
}

