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
  if (accept_event != kTRUE) {mEventsFailed++; LOG_DEBUG << "Event Failed" << endm;}
  else {LOG_DEBUG << "Event Accepted" << endm;}
  return accept_event;
}

Bool_t EventCutsRun4::AcceptVx(StMuEvent* event) {
  Double_t vx = event->primaryVertexPosition().x();
  if (vx > mMaxVx || vx < mMinVx) {
    LOG_DEBUG << "Fail: Vx=" << vx << " minVx=" << mMinVx << " maxVx=" << mMaxVx << endm;
    mEventsFailedVx++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: Vx=" << vx << " minVx=" << mMinVx << " maxVx=" << mMaxVx << endm;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptVy(StMuEvent* event) {
  Double_t vy = event->primaryVertexPosition().y();
  if (vy > mMaxVy || vy < mMinVy) {
    LOG_DEBUG << "Fail: Vy=" << vy << " minVy=" << mMinVy << " maxVy=" << mMaxVy << endm;
    mEventsFailedVy++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: Vy=" << vy << " minVy=" << mMinVy << " maxVy=" << mMaxVy << endm;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptVz(StMuEvent* event) {
  Double_t vz = event->primaryVertexPosition().z();
  if (vz > mMaxVz || vz < mMinVz) {
    LOG_DEBUG << "Fail: Vz=" << vz << " minVz=" << mMinVz << " maxVz=" << mMaxVz << endm;
    mEventsFailedVz++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: Vz=" << vz << " minVz=" << mMinVz << " maxVz=" << mMaxVz << endm;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptTrigger(StMuEvent* event) {
  Bool_t accept = kFALSE;
  for (unsigned int i = 0; i < mTriggers.size(); ++i) {
    if (event->triggerIdCollection().nominal().isTrigger(mTriggers[i]) == kTRUE) {
      accept = kTRUE;
      LOG_DEBUG << "Pass: trigger accepted=" << mTriggers[i] << endm;
    } else {mEventsFailedTrigger[i]++;}
  }
  if (accept == kFALSE) {mEventsFailedTriggerTotal++; LOG_DEBUG << "Fail: no trigger accepted" << endm;}
  return accept;
}

Bool_t EventCutsRun4::AcceptRefMult(StMuEvent* event) {
  UInt_t refmult = mUseGrefMult ? event->grefmult() : event->refMult();
  if (refmult > mMaxRef || refmult < mMinRef) {
    LOG_DEBUG << "Fail: refmult=" << refmult << " min refmult=" << mMinRef << " max refmult=" << mMaxRef << endm;
    mEventsFailedRef++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: refmult=" << refmult << " min refmult=" << mMinRef << " max refmult=" << mMaxRef << endm;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptZdcEast(StMuEvent* event) {
  Double_t zdcE = event->runInfo().zdcEastRate();
  if (zdcE > mMaxZdcEast || zdcE < mMinZdcEast) {
    LOG_DEBUG << "Fail: ZDC East=" << zdcE << " minZDC-E=" << mMinZdcEast << " maxZDC-E=" << mMaxZdcEast << endm;
    mEventsFailedZdcEast++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: ZDC East=" << zdcE << " minZDC-E=" << mMinZdcEast << " maxZDC-E=" << mMaxZdcEast << endm;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptZdcWest(StMuEvent* event) {
  Double_t zdcW = event->runInfo().zdcWestRate();
  if (zdcW > mMaxZdcWest || zdcW < mMinZdcWest) {
    LOG_DEBUG << "Fail: ZDC West=" << zdcW << " minZDC-W=" << mMinZdcWest << " maxZDC-W=" << mMaxZdcWest << endm;
    mEventsFailedZdcWest++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: ZDC West=" << zdcW << " minZDC-W=" << mMinZdcWest << " maxZDC-W=" << mMaxZdcWest << endm;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptZdcVz(StMuEvent* event) {
  Double_t zdcVz = event->zdcTriggerDetector().vertexZ();
  if (zdcVz > mMaxZdcVz || zdcVz < mMinZdcVz) {
    LOG_DEBUG << "Fail: ZDC Vz=" << zdcVz << " minZDC-Vz=" << mMinZdcVz << " maxZDC-Vz=" << mMaxZdcVz << endm;
    mEventsFailedZdcVz++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: ZDC Vz=" << zdcVz << " minZDC-Vz=" << mMinZdcVz << " maxZDC-Vz=" << mMaxZdcVz << endm;
  return kTRUE;
}

Bool_t EventCutsRun4::AcceptCtbSum(StMuEvent* event) {
  Double_t ctbSum = event->ctbMultiplicity();
  if (ctbSum > mMaxCtbSum || ctbSum < mMinCtbSum) {
    LOG_DEBUG << "Fail: CTB Sum=" << ctbSum << " minCTB Sum=" << mMinCtbSum << " maxCtb Sum=" << mMaxCtbSum << endm;
    mEventsFailedCtbSum++;
    return kFALSE;
  }
  LOG_DEBUG << "Pass: CTB Sum=" << ctbSum << " minCTB Sum=" << mMinCtbSum << " maxCtb Sum=" << mMaxCtbSum << endm;
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
  LOG_INFO << "// ------------------ EventCutsRun4 Cuts ------------------ //" << endm;
  LOG_INFO << endm;
  if (mCheckVx || mCheckVy || mCheckVz) {LOG_INFO << "Vertex cuts: " << endm;}
  if (mCheckVx) {LOG_INFO << mMinVx << " < Vx < " << mMaxVx << endm;}
  if (mCheckVy) {LOG_INFO << mMinVy << " < Vy < " << mMaxVy << endm;}
  if (mCheckVz) {LOG_INFO << mMinVz << " < Vz < " << mMaxVz << endm;}
  if (mCheckRefMult) {LOG_INFO << " Refmult Cuts: " << endm;}
  if (mCheckRefMult && mUseGrefMult)  {LOG_INFO << "use grefmult" << endm;
                                          LOG_INFO << mMinRef << " < gRefMult < " << mMaxRef << endm;}
  if (mCheckRefMult && !mUseGrefMult) {LOG_INFO << "use refmult" << endm;
                                          LOG_INFO << mMinRef << " < RefMult < " << mMaxRef << endm;}
  if (mCheckZdcEast) {LOG_INFO << mMinZdcEast << " < Zdc-E < " << mMaxZdcEast << endm;}
  if (mCheckZdcWest) {LOG_INFO << mMinZdcWest << " < Zdc-W < " << mMaxZdcWest << endm;} 
  if (mCheckZdcVz) {LOG_INFO << mMinZdcVz << " < Zdc-Vz < " << mMaxZdcVz << endm;}
  if (mCheckCtbSum) {LOG_INFO << mMinCtbSum << " < Ctb-Sum < " << mMaxCtbSum << endm;}
  
  if (mCheckTrigger && mTriggers.size() > 0) {
    std::string trigger_string = "[ " + tostr(mTriggers[0]);
    for (unsigned i = 1; i < mTriggers.size(); ++i) {
      trigger_string += ", " + tostr(mTriggers[i]);
    }
    LOG_INFO << "Using triggers: " << trigger_string << endm;
  }
  LOG_INFO << " // ------------------        End Cuts        ------------------ //" << endm;
}

void  EventCutsRun4::PrintStats() {
  LOG_INFO << "// ------------------ EventCutsRun4 Stats ------------------ //" << endm;
  LOG_INFO << "number of events:   " << mNEvents << endm;
  if (mCheckVx) {LOG_INFO << "events rejected by Vx cut: " << mEventsFailedVx << endm;
                   LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedVx / mNEvents << endm;}
  if (mCheckVy) {LOG_INFO << "events rejected by Vy cut: " << mEventsFailedVy << endm;
                   LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedVy / mNEvents << endm;}
  if (mCheckVz) {LOG_INFO << "events rejected by Vz cut: " << mEventsFailedVz << endm;
                   LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedVz / mNEvents << endm;}
  if (mCheckRefMult) {
    LOG_INFO << "events rejected by RefMult cut: " << mEventsFailedVx << endm;
    LOG_INFO << "\t using grefmult: "; if (mUseGrefMult) {LOG_INFO << "true" << endm;} else {LOG_INFO << "false" << endm;}
    LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedRef / mNEvents << endm;
 }
 if (mCheckZdcEast) {LOG_INFO << "events rejected by ZDC-E cut: " << mEventsFailedZdcEast << endm;
                   LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedZdcEast / mNEvents << endm;}
 if (mCheckZdcWest) {LOG_INFO << "events rejected by ZDC-W cut: " << mEventsFailedZdcWest << endm;
                   LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedZdcWest / mNEvents << endm;}
 if (mCheckZdcVz) {LOG_INFO << "events rejected by ZDC-Vz cut: " << mEventsFailedZdcVz << endm;
                   LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedZdcVz / mNEvents << endm;}
 if (mCheckCtbSum) {LOG_INFO << "events rejected by Ctb-Sum cut: " << mEventsFailedCtbSum << endm;
                   LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailedCtbSum / mNEvents << endm;}
  
  if (mCheckTrigger && mTriggers.size() > 0) {
    std::string trigger_string = "[ " + tostr(mTriggers[0]);
    std::string loss_string = "[ " + tostr(mEventsFailedTrigger[0]);
    for (unsigned i = 1; i < mTriggers.size(); ++i) {
      trigger_string += ", " + tostr(mTriggers[i]);
      loss_string += ", " + tostr(mEventsFailedTrigger[i]);
    }
    trigger_string += " ]"; loss_string += " ]";
    LOG_INFO << "events rejected by triggers: " << endm;
    LOG_INFO << "using triggers: " << trigger_string << endm;
    LOG_INFO << "rejected: " << loss_string << endm;
    LOG_INFO << "total events rejected by trigger: " << mEventsFailedTriggerTotal << endm;
  }
  LOG_INFO << "total number of events rejected: " << mEventsFailed << endm;
  LOG_INFO << "\t percent loss: " << (Double_t) mEventsFailed / mNEvents << endm;
  LOG_INFO << " // ------------------        End Stats        ------------------ //" << endm;
  
}

