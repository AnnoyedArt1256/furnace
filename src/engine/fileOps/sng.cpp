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

static void readGT2Table(uint16_t *table, SafeReader& reader) {
    uint8_t table_size=reader.readC();
    for (int i=0; i<table_size; i++) {
        table[i+1]=((uint16_t)reader.readC())<<8; // left side
    }
    for (int i=0; i<table_size; i++) {
        table[i+1]|=((uint16_t)reader.readC())&0xff; // right side
        logD("%02x: %04x\n",i+1,table[i+1]);
    }
}

bool DivEngine::loadSNG(unsigned char* file, size_t len) {
  struct InvalidHeaderException {};
  bool success=false;
  int chCount=3;
  int ordCount=0;
  std::vector<int> patPtr;

  uint16_t wavetable[256];
  uint16_t pulsetable[256];
  uint16_t filtertable[256];
  uint16_t speedtable[256];
  memset(wavetable,0,256*sizeof(uint16_t));
  memset(pulsetable,0,256*sizeof(uint16_t));
  memset(filtertable,0,256*sizeof(uint16_t));
  memset(speedtable,0,256*sizeof(uint16_t));

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

    // skip orders for now
    for (int ch=0; ch<6; ch++) {
        uint8_t order_cnt=reader.readC();
        for (int f=0; f<order_cnt+1; f++) {
            if (((unsigned char)reader.readC())==0xFF) {
                reader.readC(); // loop pos
                break;
            }
        }
        //reader.seek(order_cnt+1,SEEK_CUR);
    }

    int insCount=(int)((uint8_t)reader.readC());
    size_t ins_start_pos=reader.tell();
    reader.seek((16+9)*insCount,SEEK_CUR); // skip instruments for tables

    readGT2Table(wavetable,reader);
    readGT2Table(pulsetable,reader);
    readGT2Table(filtertable,reader);
    readGT2Table(speedtable,reader);

    if (!reader.seek(ins_start_pos,SEEK_SET)) {
      logD("couldn't seek to instrument pos...");
      throw EndOfFileException(&reader,reader.tell());
    }

    // instrument creation
    ds.ins.reserve(insCount);
    for(int i=0; i<insCount; i++) {
      DivInstrument* ins=new DivInstrument;
      ins->type=DIV_INS_C64;
      //ins->amiga.initSample=i;
      /*
        Offset  Size    Description
        +0      byte    Attack/Decay
        +1      byte    Sustain/Release
        +2      byte    Wavepointer
        +3      byte    Pulsepointer
        +4      byte    Filterpointer
        +5      byte    Vibrato param. (speedtable pointer)
        +6      byte    Vibraro delay
        +7      byte    Gateoff timer
        +8      byte    Hard restart/1st frame waveform
        +9      16      Instrument name
      */
      uint8_t ad=reader.readC();
      ins->c64.a=(ad>>4)&0xf;
      ins->c64.d=ad&0xf;
      uint8_t sr=reader.readC();
      ins->c64.s=(sr>>4)&0xf;
      ins->c64.r=sr&0xf;
      // wavetable (waveform and arpeggio)
      uint8_t wav_pointer=reader.readC();
      logD("wave pointer for ins %02x: %02x\n",i+1,wav_pointer);
      uint8_t wav_ptr_loop=wav_pointer;
      int wav_ins_loop=1;
      uint32_t arpval=0;
      DivInstrumentMacro *wave=&ins->std.waveMacro;
      DivInstrumentMacro *arp=&ins->std.arpMacro;
      DivInstrumentMacro *gate=&ins->std.ex4Macro;
      reader.readC();
      reader.readC();
      reader.readC();
      reader.readC();
      reader.readC();
      uint8_t curwav=reader.readC();
      logD("%02x",curwav);
      uint8_t delay=0;
      for (int tick=0;tick<256;tick++) {
        if (delay==0 && tick) {
            uint8_t left=(wavetable[wav_pointer]>>8)&0xff;
            uint8_t right=wavetable[wav_pointer]&0xff;
            if (left <= 0xEF) {
                // arpeggio
                if (right <= 0x5F) {
                    // relative notes
                    arpval=right;
                } else if (right <= 0x7F) {
                    arpval=-(0x7F-right)-1;
                } else if (right >= 0x81 && right <= 0xDF) {
                    arpval=((right-0x81)+1)^0x40000000;
                }
            }
            if (left == 0) {
                // no change
                wav_pointer++;
            } else if (left >= 1 && left <= 15) {
                // delay by N frames
                delay = left&0xf;
                wav_pointer++;
            } else if (left >= 0x10 && left <= 0xDF) {
                // waveform $10-$DF
                curwav = left;
                wav_pointer++;
            } else if (left >= 0xE0 && left <= 0xEF) {
                // waveform $00-$0F (INAUDIBLE)
                curwav = 0x00;
                wav_pointer++;
            } else if (left == 0xFF) {
                // jump to $NN
                if (right == 0) {
                    break;
                } else {
                    if (right == wav_ptr_loop) {
                        wave->loop=wav_ins_loop;
                        arp->loop=wav_ins_loop;
                        gate->loop=wav_ins_loop;
                        break;
                    }
                    wav_ins_loop=tick;
                    wav_ptr_loop=right;
                    wav_pointer=right;
                }
            }
        } else if (tick) {
            delay--;
        }
        wave->len=tick+1;
        arp->len=tick+1;
        gate->len=tick+1;
        wave->val[tick]=(curwav>>4)&0xf;
        gate->val[tick]=curwav&0xf;
        arp->val[tick]=arpval;
    }
      ins->name=reader.readString(16);
      ds.ins.push_back(ins);
    }
    ds.insLen=ds.ins.size();

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