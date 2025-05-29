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

static void getGT2PatPos(size_t orig_pat_pos, size_t pat_num, SafeReader& reader) {
  reader.seek(orig_pat_pos,SEEK_SET);  
  size_t total_patnum=(size_t)((unsigned char)reader.readC());
  if ((pat_num >= total_patnum) || (pat_num == 0)) {
    return;
  }
  for (size_t i=0; i<pat_num; i++) {
    unsigned char pat_rows=reader.readC();
    reader.seek(pat_rows*4,SEEK_CUR);
  }
}

static void readGT2Table(unsigned short *table, SafeReader& reader) {
    unsigned char table_size=reader.readC();
    for (int i=0; i<table_size; i++) {
        table[i+1]=((unsigned short)reader.readC())<<8; // left side
    }
    for (int i=0; i<table_size; i++) {
        table[i+1]|=((unsigned short)reader.readC())&0xff; // right side
        logD("%02x: %04x\n",i+1,table[i+1]);
    }
}

bool DivEngine::loadSNG(unsigned char* file, size_t len) {
  struct InvalidHeaderException {};
  bool success=false;
  int chCount=3;
  int ordCount=0;
  std::vector<int> patPtr;

  unsigned short wavetable[256];
  unsigned short pulsetable[256];
  unsigned short filtertable[256];
  unsigned short speedtable[256];
  memset(wavetable,0,256*sizeof(unsigned short));
  memset(pulsetable,0,256*sizeof(unsigned short));
  memset(filtertable,0,256*sizeof(unsigned short));
  memset(speedtable,0,256*sizeof(unsigned short));
  unsigned char* pats_unrolled = new unsigned char[6 * 4096]; // chCount
  signed char* pats_unrolled_trans = new signed char[6 * 4096]; // chCount
  memset(pats_unrolled,0,(6*4096)*sizeof(unsigned char));
  memset(pats_unrolled_trans,0,(6*4096)*sizeof(signed char));

  size_t pattern_pos_start;
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
    ds.subsong[0]->speeds.val[0]=1;
    ds.subsong[0]->speeds.len=1;

    // get instruments
    if (!reader.seek(101,SEEK_SET)) {
      logD("couldn't seek to 101");
      throw EndOfFileException(&reader,reader.tell());
    }

    // unroll orders
    for (int ch=0; ch<3; ch++) {
        unsigned char order_cnt=reader.readC();
        unsigned char rept_amt=1;
        signed char cur_trans=0;
        unsigned int pat_ind=0; 
        for (int f=0; f<order_cnt+1; f++) {
            unsigned char pat_byte=reader.readC();
            if (pat_byte <= 0xCF) {
              // pattern numbers
              for (size_t d=0; d<rept_amt; d++) {
                pats_unrolled[(pat_ind%4096)+(ch*4096)]=pat_byte;
                pats_unrolled_trans[(pat_ind%4096)+(ch*4096)]=cur_trans;
                pat_ind++;
              }
            } else if (pat_byte <= 0xDF) {
              // repeat commands
              rept_amt=(pat_byte-0xD0)+1;
              continue;
            } else {
              // transpose commands
              cur_trans=pat_byte-0xF0;
            }
            if (rept_amt != 1) rept_amt=1;
            if (pat_byte==0xFF) {
                pats_unrolled[(pat_ind%4096)+(ch*4096)]=0xFF;
                pats_unrolled_trans[(pat_ind%4096)+(ch*4096)]=reader.readC(); // loop pos
                pat_ind++;
                break;
            }
        }
        //reader.seek(order_cnt+1,SEEK_CUR);
    }

    int insCount=(int)((unsigned char)reader.readC());
    size_t ins_start_pos=reader.tell();
    reader.seek((16+9)*insCount,SEEK_CUR); // skip instruments for tables

    readGT2Table(wavetable,reader);
    readGT2Table(pulsetable,reader);
    readGT2Table(filtertable,reader);
    readGT2Table(speedtable,reader);
    pattern_pos_start=reader.tell();

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
      unsigned char ad=reader.readC();
      ins->c64.a=(ad>>4)&0xf;
      ins->c64.d=ad&0xf;
      unsigned char sr=reader.readC();
      ins->c64.s=(sr>>4)&0xf;
      ins->c64.r=sr&0xf;
      // wavetable (waveform and arpeggio)
      unsigned char wav_pointer=reader.readC();
      logD("wave pointer for ins %02x: %02x\n",i+1,wav_pointer);
      unsigned char wav_ptr_loop=wav_pointer;
      int wav_ins_loop=1;
      unsigned int arpval=0;
      DivInstrumentMacro *wave=&ins->std.waveMacro;
      DivInstrumentMacro *arp=&ins->std.arpMacro;
      DivInstrumentMacro *gate=&ins->std.ex4Macro;
      reader.readC();
      reader.readC();
      reader.readC();
      reader.readC();
      reader.readC();
      unsigned char curwav=reader.readC();
      logD("%02x",curwav);
      unsigned char delay=0;
      for (int tick=0;tick<256;tick++) {
        if (delay==0 && tick) {
            unsigned char left=(wavetable[wav_pointer]>>8)&0xff;
            unsigned char right=wavetable[wav_pointer]&0xff;
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
    ds.subsong[0]->ordersLen=ordCount=12;
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
    unsigned short pat_funk[3] = {0,0,0}; // chCount
    unsigned short pat_funk_mode[3] = {0,0,0}; // chCount
    short pat_tick[3] = {6,6,6}; // chCount
    unsigned char pat_speed[3] = {6,6,6}; // chCount
    unsigned short order_ind[3] = {0,0,0}; // chCount
    short pat_row[3] = {0,0,0}; // chCount
    int patMax = ordCount-1;
    int patLen=256;
    ds.subsong[0]->patLen=patLen;
    for (int pat=0; pat<=patMax; pat++) {
      DivPattern* chpats[DIV_MAX_CHANS];
      for (int ch=0; ch<chCount; ch++) {
        chpats[ch]=ds.subsong[0]->pat[ch].getPattern(pat,true);
      }
      for (int row=0; row<patLen; row++) {
        for (int ch=0; ch<chCount; ch++) {
          unsigned short pat_ind=order_ind[ch];
          logD("%02x/%04x: %02x",ch,pat_ind%4096,pats_unrolled[(pat_ind%4096)+(ch*4096)]);
          getGT2PatPos(pattern_pos_start,pats_unrolled[(pat_ind%4096)+(ch*4096)],reader);
          unsigned char pat_total_rows=reader.readC();
          // just in case...
          reader.seek((pat_row[ch]>=pat_total_rows?(pat_total_rows-1):pat_row[ch])*4,SEEK_CUR);
          short* dstrow=chpats[ch]->data[row];
          short* prevrow=chpats[ch]->data[row]; // stub
          if (row == 0 && pat == 0) {
              prevrow=chpats[ch]->data[row];
          } else if (row != 0) {
              prevrow=chpats[ch]->data[row-1];
          } else {
              prevrow=(ds.subsong[0]->pat[ch].getPattern(pat-1,true))->data[patLen-1];
          }
          unsigned char tick_speed = pat_speed[ch];
          if (pat_funk[ch] != 0) {
            tick_speed = (pat_funk[ch]>>(8-(pat_funk_mode[ch]<<3)))&0xff;
          }
          if ((++pat_tick[ch]) >= tick_speed) {
            if (pat_funk[ch] != 0) pat_funk_mode[ch] ^= 1;
            pat_tick[ch]=0;
            if ((++pat_row[ch]) >= pat_total_rows) {
              order_ind[ch]++;
              pat_ind=order_ind[ch];
              pat_row[ch]=1;
              getGT2PatPos(pattern_pos_start,pats_unrolled[(pat_ind%4096)+(ch*4096)],reader);
              pat_total_rows=reader.readC(); 
            }
            unsigned char notenumber=reader.readC();
            unsigned char insnum=reader.readC();
            unsigned char cmd=reader.readC();
            unsigned char cmddata=reader.readC();
            switch (cmd) {
              case 0xE: { // set funk
                if (cmddata == 0) break; // TODO: is this correct behaviour?
                for (size_t n=0;n<chCount;n++) {
                  pat_funk[n]=speedtable[cmddata];
                  pat_funk_mode[n]=0;
                }
                break;
              }
              case 0xF: { // set speed
                if ((cmddata&0x7f) >= 3) {
                  if (cmddata & 0x80) {
                    pat_speed[ch]=(cmddata&0x7f);
                    pat_funk[ch]=0;
                    pat_funk_mode[ch]=0;
                  } else {
                    for (size_t n=0;n<chCount;n++) {
                      pat_speed[n]=(cmddata&0x7f);
                      pat_funk[n]=0;
                      pat_funk_mode[n]=0;
                    }
                  }
                }
              }
              default:
                break;
            }
            if (notenumber >= 0x60 && notenumber <= 0xBC) {
              short note=notenumber-0x60;
              prevrow[0]=100;
              dstrow[0]=((note+11)%12)+1;
              dstrow[1]=(note-1)/12;
            }
            if (insnum != 0) {
              dstrow[2]=insnum-1;
            }
          }
        }
      }
    }

    ds.subsong[0]->hz=50;
    ds.systemLen=chCount/3;
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
  delete[] pats_unrolled;
  delete[] pats_unrolled_trans;
  return success;
}