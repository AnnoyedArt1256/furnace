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

#include "../../gui/gui.h"
#include "../engine.h"
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
  int sngRate=50;
  struct InvalidHeaderException {};
  bool success=false;
  int chCount=6;
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
  unsigned char* pats_unrolled = new unsigned char[12 * 4096]; // chCount
  signed char* pats_unrolled_trans = new signed char[12 * 4096]; // chCount
  memset(pats_unrolled,0,(12*4096)*sizeof(unsigned char));
  memset(pats_unrolled_trans,0,(12*4096)*sizeof(signed char));

  size_t pattern_pos_start;
  SafeReader reader=SafeReader(file,len);
  try {
    DivSong ds;
    ds.tuning=440.0;
    ds.version=DIV_VERSION_SNG;
    ds.linearPitch=0;
    ds.noSlidesOnFirstTick=false;
    ds.delayBehavior=0;
    ds.subsong[0]->speeds.val[0]=1;
    ds.subsong[0]->speeds.len=1;

    reader.seek(4,SEEK_CUR); // skip "GTS5" magic header (Furnace already checks for that)
    ds.name=reader.readString(32);
    ds.author=reader.readString(32);
    ds.copyright=reader.readString(32);
    unsigned char num_subtunes=reader.readC();
    logD("num_subtunes: %d",num_subtunes);

    // unroll orders
    for (int ch=0; ch<8; ch++) {
        size_t prev_file_pos=reader.tell();
        unsigned char order_cnt=reader.readC();
        unsigned char rept_amt=1;
        signed char cur_trans=0;
        unsigned int pat_ind=0; 
        unsigned char found_end_hex=0;
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
                found_end_hex=1;
                break;
            }
        }
        logD("found_end_hex for ch %d: %d",ch+1,found_end_hex);
        if (!found_end_hex) {
          reader.seek(prev_file_pos,SEEK_SET);
          chCount=ch/num_subtunes;
          break;
        }
        //reader.seek(order_cnt+1,SEEK_CUR);
    }

    chCount = 3;
  
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
      unsigned char ptr_loop=wav_pointer;
      int ins_loop=1;
      unsigned int arpval=0;
      DivInstrumentMacro *wave=&ins->std.waveMacro;
      DivInstrumentMacro *arp=&ins->std.arpMacro;
      DivInstrumentMacro *gate=&ins->std.ex4Macro;
      DivInstrumentMacro *duty=&ins->std.dutyMacro;
      DivInstrumentMacro *filter=&ins->std.algMacro;
      DivInstrumentMacro *filter_type=&ins->std.ex1Macro;
      DivInstrumentMacro *filter_res=&ins->std.ex2Macro;
      ins->c64.dutyIsAbs=true;
      ins->c64.filterIsAbs=true;
      unsigned char pulse_pointer=reader.readC();
      logD("pulse pointer for ins %02x: %02x\n",i+1,pulse_pointer);
      unsigned char filter_pointer=reader.readC();
      logD("filter pointer for ins %02x: %02x\n",i+1,filter_pointer);
      ins->c64.toFilter=filter_pointer>0?true:false;
      reader.readC();
      reader.readC();
      reader.readC();
      unsigned char curwav=reader.readC();
      logD("%02x",curwav);
      unsigned char delay=0;
      for (int tick=0;tick<256;tick++) {
do_arp_tick:
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
            } else if (left >= 0xF0 && left <= 0xFE) {
                // apply command (NOT IMPLEMENTED)
                wav_pointer++;
            }  else if (left == 0xFF) {
                // jump to $NN
                if (right == 0) {
                    break;
                } else {
                    if (right == ptr_loop) {
                        wave->loop=ins_loop;
                        arp->loop=ins_loop;
                        gate->loop=ins_loop;
                        break;
                    }
                    ins_loop=tick;
                    ptr_loop=right;
                    wav_pointer=right;
                    goto do_arp_tick; // I HAD to :P
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

      unsigned short curpulse=0x800;
      int8_t sweep_amt=0;
      delay=0;
      ptr_loop=pulse_pointer;
      ins_loop=1;
      for (int tick=0;tick<256;tick++) {
do_pulse_tick:
        if (pulse_pointer == 0) break;
        if (delay==0) {
            unsigned char left=(pulsetable[pulse_pointer]>>8)&0xff;
            unsigned char right=pulsetable[pulse_pointer]&0xff;
            if (left == 0xFF) {
                // jump to $NN
                if (right == 0) {
                    break;
                } else {
                    if (right == ptr_loop) {
                        duty->loop=ins_loop;
                        break;
                    }
                    ins_loop=tick;
                    ptr_loop=right;
                    pulse_pointer=right;
                }
                goto do_pulse_tick;
            } else if (left & 0x80) {
              // set pulse
              curpulse=pulsetable[pulse_pointer]&0xfff;
              pulse_pointer++;
              sweep_amt=0;
            } else {
              // sweep pulse
              delay=left-1;
              sweep_amt=(int8_t)right;
              pulse_pointer++;
            }
        } else if (tick) {
            delay--;
        }
        curpulse += (int8_t)sweep_amt;
        duty->len=tick+1;
        duty->val[tick]=curpulse;
      }

      unsigned char curfilt=0;
      unsigned char curfiltype=0;
      unsigned char curres=0;
      delay=0;
      ptr_loop=filter_pointer;
      ins_loop=1;
      unsigned char updated_type=0;
      unsigned char real_tick=1;
      for (int tick=0;tick<256;tick++) {
do_filter_tick:
        if (filter_pointer == 0) break;
        if (delay==0) {
            unsigned char left=(filtertable[filter_pointer]>>8)&0xff;
            unsigned char right=filtertable[filter_pointer]&0xff;
            if (left == 0xFF) {
                // jump to $NN
                if (right == 0) {
                    break;
                } else {
                    if (right == ptr_loop) {
                        filter->loop=ins_loop;
                        filter_type->loop=ins_loop;
                        filter_res->loop=ins_loop;
                        break;
                    }
                    ins_loop=tick;
                    ptr_loop=right;
                    filter_pointer=right;
                    goto do_filter_tick;
                }
            } else if (left == 0) {
              // set cutoff
              curfilt=right;
              filter_pointer++;
              sweep_amt=0;
            } else if (left & 0x80) {
              // set filter usage/resonance
              /*
              if (!updated_type) {
                filter_type->len=real_tick+1;
                for (int i=0; i<real_tick; i++)
                  filter_type->val[i]=0;
              }
              */
              updated_type=1;
              curfiltype=(left>>4)&0x7;
              curres=(right>>4)&0xf;
              filter_pointer++;
              if (updated_type) {
                filter_type->len=real_tick+1;
                filter_type->val[real_tick]=curfiltype&15;
              }
              filter_res->len=real_tick+1;
              filter_res->val[real_tick]=curres&15;
              continue;
            } else {
              // sweep pulse
              delay=left-1;
              sweep_amt=(int8_t)right;
              filter_pointer++;
            }
        } else if (tick) {
            delay--;
        }
        curfilt += (int8_t)sweep_amt;
        filter->len=real_tick+1;
        filter->val[real_tick]=((unsigned short)curfilt)<<3;
        if (updated_type) {
          filter_type->len=real_tick+1;
          filter_type->val[real_tick]=curfiltype&15;
        }
        filter_res->len=real_tick+1;
        filter_res->val[real_tick]=curres&15;
        real_tick++;
      }
      if (real_tick > 0) {
        filter->val[0]=filter->val[1];
        filter_type->val[0]=filter_type->val[1];
        filter_res->val[0]=filter_res->val[1];
      }
      ins->name=reader.readString(16);
      ds.ins.push_back(ins);
    }
    ds.insLen=ds.ins.size();
  
    // orders
    ds.subsong[0]->ordersLen=ordCount=50;
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
    unsigned short pat_funk[DIV_MAX_CHANS];
    unsigned short pat_funk_mode[DIV_MAX_CHANS];
    unsigned char pat_tick[DIV_MAX_CHANS];

    unsigned char pat_speed[DIV_MAX_CHANS];
    unsigned short order_ind[DIV_MAX_CHANS];
    unsigned char last_cmd[DIV_MAX_CHANS];
    unsigned char last_eff[DIV_MAX_CHANS];
    unsigned char last_ins[DIV_MAX_CHANS];
    unsigned char did_legato[DIV_MAX_CHANS];
    short pat_row[DIV_MAX_CHANS];
    for (size_t i=0; i<DIV_MAX_CHANS; i++) {
      pat_funk[i]=0;
      pat_funk_mode[i]=0;
      pat_tick[i]=0;
      pat_speed[i]=6;
      order_ind[i]=0;
      pat_row[i]=0;
      last_cmd[i]=0;
      last_eff[i]=0;
      last_ins[i]=0;
      did_legato[i]=0;
    }

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
          char pat_trans = pats_unrolled_trans[(pat_ind%4096)+(ch*4096)];
          //logD("%02x/%04x: %02x",ch,pat_ind%4096,pats_unrolled[(pat_ind%4096)+(ch*4096)]);
          getGT2PatPos(pattern_pos_start,pats_unrolled[(pat_ind%4096)+(ch*4096)],reader);
          unsigned char pat_total_rows=reader.readC();
          // just in case...
          reader.seek((pat_row[ch]>=pat_total_rows?(pat_total_rows-1):pat_row[ch])*4,SEEK_CUR);
          short* dstrow=chpats[ch]->data[row];
          // yes GT2 is really weird in terms of HR

          short* prevrow=chpats[ch]->data[row]; // last frame
          if (row == 0 && pat == 0) {
          } else if (row != 0) {
              prevrow=chpats[ch]->data[row-1];
          } else {
              prevrow=(ds.subsong[0]->pat[ch].getPattern(pat-1,true))->data[patLen-1];
          }

          short* prevrow2=chpats[ch]->data[row]; // second to last frame
          if (row < 2 && pat == 0) {
          } else if (row > 1) {
              prevrow2=chpats[ch]->data[row-2];
          } else {
              prevrow2=(ds.subsong[0]->pat[ch].getPattern(pat-1,true))->data[patLen-row];
          }

          unsigned char tick_speed = pat_speed[ch];
          if (pat_funk[ch] != 0) {
            tick_speed = (pat_funk[ch]>>(8-(pat_funk_mode[ch]<<3)))&0xff;
          }
          pat_tick[ch]--;
          if (pat_tick[ch] >= 0x80) {
            if (pat_funk[ch] != 0) pat_funk_mode[ch] ^= 1;
            pat_tick[ch]=tick_speed-1;
          } else if (pat_tick[ch] == 0) {
            if ((++pat_row[ch]) >= pat_total_rows) {
              order_ind[ch]++;
              pat_ind=order_ind[ch];
              pat_row[ch]=1;
              pat_trans = pats_unrolled_trans[(pat_ind%4096)+(ch*4096)];
              getGT2PatPos(pattern_pos_start,pats_unrolled[(pat_ind%4096)+(ch*4096)],reader);
              pat_total_rows=reader.readC(); 
            }
            unsigned char notenumber=reader.readC();
            unsigned char insnum=reader.readC();
            unsigned char cmd=reader.readC();
            unsigned char cmddata=reader.readC();

            switch (cmd) {
              case 0x1:
              case 0x2:
              case 0x3: { // pitch slide and portamento
                if (cmd == 3 && cmddata == 0) {
                  dstrow[8]=0x03;
                  dstrow[9]=0x00;
                  dstrow[10]=0xEA;
                  dstrow[11]=0x01;    
                  did_legato[ch]=1;
                  break;
                }
                unsigned short slide_amt = speedtable[cmddata];
                if (slide_amt > 255) slide_amt = 255;
                if (slide_amt < 0) slide_amt = 0;
                dstrow[8]=cmd;
                dstrow[9]=slide_amt;
                break;
              }
              case 0x4: { // vibrato
                short slide_amt = speedtable[cmddata];
                dstrow[8]=cmd;
                dstrow[9]=((0xff-(slide_amt>>8&0xff))&0xf0)|(slide_amt>>4&0xf);
                break;
              }
              case 0x5: { // set attack and decay
                dstrow[4]=0x20;
                dstrow[5]=cmddata;
                break;
              }
              case 0x6: { // set sustain and release
                dstrow[4]=0x21;
                dstrow[5]=cmddata;
                break;
              }
              case 0x7: { // set sustain and release
                dstrow[4]=0x10;
                dstrow[5]=cmddata>>4;
                if (cmddata < 0x10) {
                  dstrow[4]=0x21;
                  dstrow[5]=0x00;
                  dstrow[0] = 100;
                }
                break;
              }
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
            if ((last_cmd[ch] != cmd) && (last_cmd[ch] != 0)) {
              switch (last_cmd[ch]) {
                case 1:
                case 2:
                case 3:
                case 4: {
                  if (cmd >= 1 && cmd <= 4) break;
                  dstrow[6]=last_cmd[ch];
                  dstrow[7]=0x00;
                  break;
                }
                default:
                  break;
              }
            }
            last_cmd[ch]=cmd;
            last_eff[ch]=cmddata;

            if (notenumber >= 0x60 && notenumber <= 0xBC) {
              short note=notenumber-0x60+pat_trans;
              if (!(row < 2 && pat == 0) && (cmd != 3)) {
                prevrow2[0]=100; // rel (and set ADSR to $0F00)
                prevrow2[2]=(last_ins[ch]+1)%insCount; // this fixes ADSR commands from not resetting
                //prevrow[4]=0x20;
                //prevrow[5]=0x0F;
                //prevrow[6]=0x21;
                //prevrow[7]=0x00;
              }
              dstrow[0]=((note+11)%12)+1;
              dstrow[1]=(note-1)/12;
              dstrow[2]=last_ins[ch]%insCount;
            } else if (notenumber == 0xBE) {
              dstrow[0]=100;
              dstrow[2]=(last_ins[ch]+1)%insCount; // this fixes ADSR commands from not resetting
            }
            if (insnum != 0) {
              dstrow[2]=insnum-1;
              last_ins[ch]=insnum-1;
            }
          } else {
            if (did_legato[ch]) {
              dstrow[10]=0xEA;
              dstrow[11]=0x00;    
              did_legato[ch]=0;
            }
          }
        }
      }
    }

    ds.subsong[0]->hz=sngRate;
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
      ds.subsong[0]->pat[i].effectCols=4;
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