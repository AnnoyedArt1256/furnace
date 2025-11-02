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

#include "gba.h"
#include "../engine.h"
#include "../ta-log.h"
#include <fmt/printf.h>
#include <array>
#include <vector>

void DivExportGBA::run() {
  int GB=-1;
  int GBA=-1;
  int IGNORED=0;

  // Locate system index.
  for (int i=0; i<e->song.systemLen; i++) {
    if (e->song.system[i] == DIV_SYSTEM_GBA_DMA) {
      if (GBA>=0) {
        IGNORED++;
        logAppendf("Ignoring duplicate GBA id %d",i);
        continue;
      }
      GBA=i;
      logAppendf("GBA (no sw mix) detected as chip id %d",i);
    } if (e->song.system[i] == DIV_SYSTEM_GB) {
      if (GB>=0) {
        IGNORED++;
        logAppendf("Ignoring duplicate GB id %d",i);
        continue;
      }
      GB=i;
      logAppendf("GB detected as chip id %d",i);
    } else {
      IGNORED++;
      logAppendf("Ignoring chip id %d, system id %d",i,(int)e->song.system[i]);
    }
  }
  if (GBA<0 || GB<0) {
    logAppendf("ERROR: Could not find non-software mixed GBA and/or GB");
    failed=true;
    running=false;
    return;
  }
  if (IGNORED>0) {
    logAppendf("WARNING: GBA export ignoring unsup sys count: %d",IGNORED);
  }

  size_t tickCount=0;

  e->stop();
  e->repeatPattern=false;
  e->setOrder(0);

  logAppend("playing and logging register writes...");


  e->synchronizedSoft([&]() {
    double origRate = e->got.rate;
    double rate = MIN(e->curSubSong->hz,1000.0);
    logAppendf("export rate is %d hz",(int)rate);
    e->got.rate=rate;

    // Determine loop point.
    int loopOrder=0;
    int loopRow=0;
    int loopEnd=0;
    e->walkSong(loopOrder,loopRow,loopEnd);
    logAppendf("loop point: %d %d",loopOrder,loopRow);
    e->warnings="";

    auto w = new SafeWriter;
    w->init(); 

    // Reset the playback state.
    e->curOrder=0;
    e->freelance=false;
    e->playing=false;
    e->extValuePresent=false;
    e->remainingLoops=-1;

    e->disCont[GB].dispatch->toggleRegisterDump(true);
    e->disCont[GBA].dispatch->toggleRegisterDump(true);

    // Prepare to write song data.
    e->playSub(false);
    bool done=false;

    logAppend("writing data...");
    progress[0].amount=0.15f;

    int wait_dur = 0;
    w->writeI(e->disCont[GBA].dispatch->getSampleMemUsage(0)+0x10);
    w->writeI(0); // 4
    w->writeI(0); // 8
    w->writeI(0); // C
    w->write(e->disCont[GBA].dispatch->getSampleMem(0), 
             e->disCont[GBA].dispatch->getSampleMemUsage(0));

    int sample_off[2];
    int sample_end[2];
    int has_reg_dump = 0;
    unsigned int sample_vol[2] = {0,0};
    uint8_t regs[0x40];

    while (!done) {
      if (e->nextTick(false,true) || !e->playing) {
        done=true;
        for (int i=0; i<e->song.systemLen; i++) {
          e->disCont[i].dispatch->getRegisterWrites().clear();
        }
      }

      // get register dumps
      // GB APU ONLY! (for now)
      has_reg_dump = 0;
      std::vector<DivRegWrite>& writes=e->disCont[GB].dispatch->getRegisterWrites();
      if (writes.size() > 0) {
        //logAppendf("GB APU: found %d messages",writes.size());
        for (DivRegWrite& write: writes) {
          if (write.addr == 0x14 || write.addr == 0x19 || write.addr == 0x23) {
            //w->writeC(write.addr & 0x3F);
            //w->writeC(write.val & 0xFF);
            //has_reg_dump = 1;
          }
        }
        for (DivRegWrite& write: writes) {
          uint8_t old_val = regs[write.addr&0x3f];
          if (write.addr < 0x40)
            regs[write.addr&0x3f] = write.val;
          if (write.addr == 0x3f) {
            w->writeC(0x30);
            w->write((void*)(&regs[0x30]),16);
            has_reg_dump = 1;
          } else if (write.addr < 0x30) {
            //if (old_val != write.val ||
            //    write.addr == 0x12 || 
            //    write.addr == 0x12+5 || 
            //    write.addr == 0x12+20) {
                w->writeC(write.addr & 0x3F);
                w->writeC(write.val & 0xFF);
                has_reg_dump = 1;
            //}
          }
        }
        writes.clear();
      }      

      std::vector<DivRegWrite>& writes_gba=e->disCont[GBA].dispatch->getRegisterWrites();
      if (writes_gba.size() > 0) {
        has_reg_dump = 1;
        for (DivRegWrite& write: writes_gba) {
          //logAppendf("GBA PCM: %08x %02x", write.addr, write.val);
          int ch = (write.addr>>8)&1;
          unsigned int val = write.val;
          switch (write.addr & 0xffff00ff) {
            case 0xfffd0000: { // get sample end
                sample_end[ch] = val;
                break;
            }
            case 0xfffe0000: { // get sample off
                sample_off[ch] = val;
                break;
            }
            case 0xfffe0001: { // set sample vol
                if (val != sample_vol[ch]) {
                    w->writeC(0x40|((2<<1)|ch));
                    w->writeC(val);
                }
                sample_vol[ch] = val;
                break;
            }
            case 0xffff0000: { // play sample
                w->writeC(0x40|((0<<1)|ch));
                w->writeI(sample_end[ch]);
                w->writeI(sample_off[ch]);
                break;
            }
            case 0xffff0001: {
                w->writeC(0x40|((1<<1)|ch));
                // 0=F/1, 1=F/64, 2=F/256, 3=F/1024
                int prescaler = 3;
                int freq_div = 1024;
                if (val<65536) { prescaler = 0; freq_div = 1; } 
                else if (val<65536*64) { prescaler = 1; freq_div = 64; }
                else if (val<65536*256) { prescaler = 2; freq_div = 256; }
                //int freq_master = (16777216>>6)*freq_div;
                w->writeS(0x10000-(val/freq_div));                
                w->writeS(0xfffc|prescaler);
                break;
            }
            case 0xffff0002: {
                //w->writeC(0x40|((0<<1)|ch));
                //w->writeS(0xff);
                break;
            }
            case 0xffff0005: {
                break;
            }
            default: break;
          }
        }
        writes_gba.clear();
      }

      // write wait
      tickCount++;
      int totalWait=e->cycles;
      if (totalWait>0 && !done) {
        while (totalWait) {
          wait_dur++;
          /*
          if (has_reg_dump || (wait_dur == 65535)) {
            w->writeC(0x80); // pitch
            w->writeS(wait_dur); // duration
            wait_dur = 0;
            has_reg_dump = 0;
          }
          */
          w->writeC(0x80);
          totalWait--;
          tickCount++;
        }
      }
      
    }
    // end of song

    // done - close out.
    e->got.rate=origRate;
    e->disCont[GB].dispatch->getRegisterWrites().clear();
    e->disCont[GBA].dispatch->getRegisterWrites().clear();
    e->disCont[GB].dispatch->toggleRegisterDump(false);
    e->disCont[GBA].dispatch->toggleRegisterDump(false);

    e->remainingLoops=-1;
    e->playing=false;
    e->freelance=false;
    e->extValuePresent=false;

    output.push_back(DivROMExportOutput("export.bin",w));
  });


  progress[0].amount=1.0f;
  
  logAppend("finished!");

  running=false;
}

bool DivExportGBA::go(DivEngine* eng) {
  progress[0].name="Progress";
  progress[0].amount=0.0f;

  e=eng;
  running=true;
  failed=false;
  mustAbort=false;
  exportThread=new std::thread(&DivExportGBA::run,this);
  return true;
}

void DivExportGBA::wait() {
  if (exportThread!=NULL) {
    logV("waiting for export thread...");
    exportThread->join();
    delete exportThread;
  }
}

void DivExportGBA::abort() {
  mustAbort=true;
  wait();
}

bool DivExportGBA::isRunning() {
  return running;
}

bool DivExportGBA::hasFailed() {
  return failed;
}

DivROMExportProgress DivExportGBA::getProgress(int index) {
  if (index<0 || index>1) return progress[1];
  return progress[index];
}
