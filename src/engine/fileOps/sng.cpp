/**
 * Furnace Tracker - multi-system chiptune tracker
 * Copyright (C) 2021-2025 tildearrow and contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "fileOpsCommon.h"

bool DivEngine::loadSNG(unsigned char* file, size_t len) {
  struct InvalidHeaderException {};
  bool success=false;
  int chCount=3;
  int ordCount=0;
  std::vector<int> patPtr;

  SafeReader reader=SafeReader(file,len);
  try {
    DivSong ds;
    ds.tuning=440.0;
    ds.version=DIV_VERSION_SNG;
    ds.linearPitch=0;
    ds.noSlidesOnFirstTick=true;
    ds.rowResetsArpPos=true;
    ds.ignoreJumpAtEnd=false;
    ds.delayBehavior=0;

    // get instruments
    if (!reader.seek(101,SEEK_SET)) {
      logD("couldn't seek to 101");
      throw EndOfFileException(&reader,reader.tell());
    }

    for (int ch=0; ch<3; ch++) {

    }

    int insCount=31;

    // orders
    ds.subsong[0]->ordersLen=ordCount=1;
    if (ds.subsong[0]->ordersLen<1 || ds.subsong[0]->ordersLen>128) {
      logD("invalid order count!");
      throw EndOfFileException(&reader,reader.tell());
    }

    for (int i=0; i<128; i++) {
      for (int j=0; j<chCount; j++) {
        ds.subsong[0]->orders.ord[j][i]=i;
      }
    }

    if (insCount==15) {
      if (!reader.seek(600,SEEK_SET)) {
        logD("couldn't seek to 600");
        throw EndOfFileException(&reader,reader.tell());
      }
    } else {
      if (!reader.seek(1084,SEEK_SET)) {
        logD("couldn't seek to 1084");
        throw EndOfFileException(&reader,reader.tell());
      }
    }

    // patterns
    int patMax = 1;
    ds.subsong[0]->patLen=64;
    for (int pat=0; pat<=patMax; pat++) {
      DivPattern* chpats[DIV_MAX_CHANS];
      for (int ch=0; ch<chCount; ch++) {
        chpats[ch]=ds.subsong[0]->pat[ch].getPattern(pat,true);
      }
      for (int row=0; row<64; row++) {
        for (int ch=0; ch<chCount; ch++) {
          short* dstrow=chpats[ch]->data[row];
        }
      }
    }

    ds.subsong[0]->hz=50;
    ds.systemLen=(chCount+3)/4;
    for(int i=0; i<ds.systemLen; i++) {
      ds.system[i]=DIV_SYSTEM_C64_8580;
      ds.systemFlags[i].set("clockSel",1); // PAL
    }
    for(int i=0; i<chCount; i++) {
      ds.subsong[0]->chanShow[i]=true;
      ds.subsong[0]->chanShowChanOsc[i]=true;
      ds.subsong[0]->chanName[i]=fmt::sprintf("Channel %d",i+1);
      ds.subsong[0]->chanShortName[i]=fmt::sprintf("C%d",i+1);
    }
    for(int i=chCount; i<ds.systemLen*4; i++) {
      ds.subsong[0]->pat[i].effectCols=1;
      ds.subsong[0]->chanShow[i]=false;
      ds.subsong[0]->chanShowChanOsc[i]=false;
    }
    
    // instrument creation
    ds.ins.reserve(insCount);
    for(int i=0; i<insCount; i++) {
      DivInstrument* ins=new DivInstrument;
      ins->type=DIV_INS_C64;
      //ins->amiga.initSample=i;
      //ins->name=ds.sample[i]->name;
      ds.ins.push_back(ins);
    }
    ds.insLen=ds.ins.size();

    // find subsongs
    ds.findSubSongs(chCount);
    
    if (active) quitDispatch();
    BUSY_BEGIN_SOFT;
    saveLock.lock();
    song.unload();
    song=ds;
    changeSong(0);
    recalcChans();
    saveLock.unlock();
    BUSY_END;
    if (active) {
      initDispatch();
      BUSY_BEGIN;
      renderSamples();
      reset();
      BUSY_END;
    }
    success=true;
  } catch (EndOfFileException& e) {
    //logE("premature end of file!");
    lastError="incomplete file";
  } catch (InvalidHeaderException& e) {
    //logE("invalid info header!");
    lastError="invalid info header!";
  }
  return success;
}