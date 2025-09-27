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

String VT2_readstrline(SafeReader *reader) {
  // account for CRLF (mostly windows) newlines
  String ret;
  unsigned char c;
  if ((*reader).isEOF()) throw EndOfFileException(reader, (*reader).tell());

  while (!((*reader).isEOF()) && (c=(*reader).readC())!=0) {
    if (c=='\n') {
      break;
    } else if (c=='\r') {
      unsigned char c2 = (*reader).readC();
      if (c2!='\n') (*reader).seek(-1,SEEK_CUR);
      break;
    }
    ret.push_back(c);
  }
  return ret;
}

int VT2_hextoint(char n) {
  if (n >= 'A' && n <= 'F') return n-'A'+0x0a;
  if (n >= '0' && n <= '9') return n-'0';
  return -1;
}

int VT2_hextoint_env(char n) {
  if (n == '.') return 0;
  if (n >= 'A' && n <= 'F') return n-'A'+0x0a;
  if (n >= '0' && n <= '9') return n-'0';
  return 0;
}

int VT2_lettertoint(char n) {
  if (n >= 'A' && n <= 'Z') return n-'A'+0x0a;
  if (n >= '0' && n <= '9') return n-'0';
  return -1;
}

bool DivEngine::loadVT2(unsigned char* file, size_t len) {
  struct InvalidHeaderException {};
  bool success=false;
  int chCount=3;
  int ordCount=0;
  std::vector<int> pat_inds;

  SafeReader reader=SafeReader(file,len);
  try {
    DivSong ds;
    ds.tuning=440.0;
    ds.version=DIV_VERSION_VT2;
    ds.linearPitch=0;
    ds.subsong[0]->speeds.val[0]=3;
    ds.subsong[0]->speeds.len=1;

    ds.name="";
    ds.author="";
    ds.copyright="";

    reader.seek(0, SEEK_SET);
    VT2_readstrline(&reader); // skip [Module]

    while (true) {
        String line = VT2_readstrline(&reader);
        int pos = line.find_first_of("=");
        String headerType = line.substr(0, pos);
        String headerVal = line.substr(pos + 1);
        if (headerType == "Speed") {
            ds.subsong[0]->speeds.val[0]=atoi(headerVal.c_str()); // i hate coding
        }
        if (headerType == "PlayOrder") {
            int pat_ind_pos = 0;
            for (int i=0; i<(int)headerVal.length(); i++) {
                char pat_char = headerVal.at(i);
                if (pat_char == 'L') {
                    // loop marker
                    pat_ind_pos=i+1;
                } else if (pat_char == ',') {
                    // delimiter
                    if (i!=0) {
                        pat_inds.push_back(atoi(headerVal.substr(pat_ind_pos,i-pat_ind_pos).c_str()));
                    }
                    pat_ind_pos=i+1;
                }
            }
        }
        logD("%s: %s", headerType.c_str(), headerVal.c_str());
        if ((line.length() == 0) || (line.at(0) == '[')) break; 
    }

    size_t old_pos = reader.tell();

    ds.subsong[0]->patLen=256;
    ds.subsong[0]->hz=50;
    ds.systemLen=chCount/3;
    for(int i=0; i<ds.systemLen; i++) {
      ds.system[i]=DIV_SYSTEM_AY8910;
      ds.systemFlags[i].set("customClock",1750000);
    }
    for(int i=0; i<chCount; i++) {
      ds.subsong[0]->chanShow[i]=true;
      ds.subsong[0]->chanShowChanOsc[i]=true;
      ds.subsong[0]->chanName[i]=fmt::sprintf("Channel %d",i+1);
      ds.subsong[0]->chanShortName[i]=fmt::sprintf("C%d",i+1);
      ds.subsong[0]->pat[i].effectCols=(i%3)==2?5:4;
    }

    int max_pat=0;
    for (int i=0; i<pat_inds.size(); i++) {
      if (pat_inds[i]>max_pat) max_pat=pat_inds[i];
    }

    ds.subsong[0]->ordersLen=ordCount=pat_inds.size();
    if (ds.subsong[0]->ordersLen<1 || ds.subsong[0]->ordersLen>128) {
      logD("invalid order count!");
      throw EndOfFileException(&reader,reader.tell());
    }

    for (int i=0; i<ordCount; i++) {
      for (int j=0; j<chCount; j++) {
        ds.subsong[0]->orders.ord[j][i]=i;//pat_inds[i];
      }
    }
                            // A,B,C,D,E,F,G
    const int note2int[7] = {9,11,0,2,4,5,7};

    DivPattern* chpats[DIV_MAX_CHANS];
    int ins[DIV_MAX_CHANS];
    int real_ins[DIV_MAX_CHANS];
    int ord[DIV_MAX_CHANS];
    int has_macro_disable[DIV_MAX_CHANS];
    for (int ch=0; ch<chCount; ch++) {
      ins[ch]=0;
      real_ins[ch]=0;
      ord[ch]=32;
      has_macro_disable[ch]=0;
    }
    std::vector<int> ins_comb;
    for (int pat=0; pat<ordCount; pat++) {
      reader.seek(old_pos, SEEK_SET);
      String pat_num_str = std::to_string(pat_inds[pat]);
      for (int tr=0;tr<2560;tr++) {
        String line = VT2_readstrline(&reader);
        if (line == ("[Pattern" + pat_num_str + "]"))
            break;
      }
      for (int ch=0; ch<chCount; ch++) {
        chpats[ch]=ds.subsong[0]->pat[ch].getPattern(pat,true);
      }
      for (int row=0; row<256; row++) {
        String line = VT2_readstrline(&reader);

        if (line.length() == 0 && row != 0) {
          short* dstrow=chpats[0]->data[row-1];
          dstrow[10]=0x0D;
          dstrow[11]=0x00;
          break;
        }

        for (int ch=0; ch<chCount; ch++) {
          short* dstrow=chpats[ch]->data[row];

          if (ch == 2) {
            String env_pitch_str = line.substr(0,4);
            if (env_pitch_str != "....") {
              unsigned char pitch_hi = (VT2_hextoint_env(env_pitch_str.at(0))<<4);
              pitch_hi |= VT2_hextoint_env(env_pitch_str.at(1));
              unsigned char pitch_lo = (VT2_hextoint_env(env_pitch_str.at(2))<<4);
              pitch_lo |= VT2_hextoint_env(env_pitch_str.at(3));
              dstrow[10]=0x23;
              dstrow[11]=pitch_lo;
              dstrow[12]=0x24;
              dstrow[13]=pitch_hi;
            }
          } else if (ch == 0) {
            String env_pitch_str = line.substr(0,4);
            if (env_pitch_str != "....") {
              dstrow[10]=0x25;
              dstrow[11]=0x00;
            }
          }

          String chpat_line = line.substr(8+(ch*14),14-1);
          if (chpat_line.at(0) == 'R') {
            dstrow[0]=100;
          } else if (chpat_line.at(0) != '-') {
            char note_char = chpat_line.at(0);
            unsigned char sharp = chpat_line.at(1)=='#'?1:0;
            char oct = chpat_line.at(2);
            if (note_char >= 'A' && note_char <= 'G') {
              char effect = chpat_line.at(9);
              char note = note2int[note_char-'A']+sharp+(oct-'0')*12;
              dstrow[0]=((note+11)%12)+1;
              dstrow[1]=(note-1)/12;
              if (effect == '.') {
                dstrow[4]=0x01;
                dstrow[5]=0x00;
              } else if (!(effect >= '1' && effect <= '3')) {
                dstrow[8]=0x01;
                dstrow[9]=0x00;
              } else {
                dstrow[8]=0xF5;
                dstrow[9]=0x01;
                has_macro_disable[ch]=2;
              }
            }
          }

          // effects
          if (chpat_line.at(9) != '.') {
            char effect = chpat_line.at(9);
            switch (effect) {
              case '1': {
                dstrow[4]=0x02;
                break;
              }
              case '2': {
                dstrow[4]=0x01;
                break;
              }
              case '3': {
                dstrow[4]=0x03;
                break;
              }
              case '9': {
                dstrow[4]=0x26;
                break;
              }
              case 'A': {
                dstrow[4]=0x25;
                break;
              }
              case 'B': {
                dstrow[4]=0x0F;
                break;
              }
            }
            unsigned char effect_val=(VT2_hextoint_env(chpat_line.at(11))<<4)|VT2_hextoint(chpat_line.at(12));;
            if (chpat_line.at(10) != '.' && effect != 'B') {
              unsigned char delay_val=VT2_hextoint_env(chpat_line.at(10));
              if (delay_val == 0) dstrow[5] = 0;
              else dstrow[5] = effect_val>>(delay_val-1);
            } else {
              dstrow[5]=effect_val;
            }
          }
  
          bool do_ins = false;

          if (chpat_line.at(5) != '.') {
            int env_val = VT2_hextoint(chpat_line.at(5));
            if (env_val == 0) env_val = -1;
            if (env_val > -1) {
              if (env_val != 15) {
                dstrow[6]=0x22;
                dstrow[7]=env_val<<4;
                dstrow[2]=ins[ch]=ins[ch]|1;
              } else {
                dstrow[2]=ins[ch]=ins[ch]^(ins[ch]&1);
              }
              ord[ch] = 32; // huh?
              do_ins = true;
            }
          }

          if (chpat_line.at(7) != '.') dstrow[3]=VT2_hextoint(chpat_line.at(7));
          if (chpat_line.at(6) != '.') {
            int ord_num = VT2_lettertoint(chpat_line.at(6))-1;
            if (ord_num > -1) {
              ord[ch]=ord_num;             
              do_ins = true;
            }
          }
          if (chpat_line.at(4) != '.') {
            int ins_num = VT2_lettertoint(chpat_line.at(4))-1;
            if (ins_num > -1) {
              real_ins[ch]=ins_num;
              do_ins=true;
            }
          }

          if (do_ins) {
            int ins_ind = real_ins[ch]+(ord[ch]*32);
            auto ins_find_result = std::find(ins_comb.begin(), ins_comb.end(), ins_ind);
            if ((ins_find_result == ins_comb.end()) || (ins_comb.size() == 0)) {
              ins_comb.push_back(ins_ind);
              ins_ind=ins_comb.size()-1;
            } else {
              ins_ind=ins_find_result-ins_comb.begin();
            }
            dstrow[2]=ins[ch]=(ins_ind<<1)|(ins[ch]&1);
          }

          if (has_macro_disable[ch] > 0) {
            if ((--has_macro_disable[ch]) == 0) {
              for (int col=4;col<12;col+=2) {
                if (dstrow[col] == -1) {
                  dstrow[col]=0xF6;
                  dstrow[col+1]=0x01;
                  break;
                }
              }
            }
          }
        }
      }
    }

    int insCount = ins_comb.size();
    // TODO: add ornaments
    // instrument creation
    logD("%d %d\n",insCount,insCount<<1);
    ds.ins.reserve(insCount<<1);
    for(int i=0; i<insCount<<1; i++) {
      int ins_num = ins_comb[i>>1];
      String samp_num_str = std::to_string((ins_num&31)+1);
      String ord_num_str = std::to_string(((ins_num>>5)&31)+1);
      reader.seek(old_pos, SEEK_SET);
      DivInstrument* ins=new DivInstrument;
      ins->type=DIV_INS_AY;
      ins->name="";
      for (int tr=0;tr<3000;tr++) {
        String line = VT2_readstrline(&reader);
        if (line == ("[Sample" + samp_num_str + "]"))
            break;
      }
      size_t samp_loop_pos = reader.tell();
      DivInstrumentMacro *vol=&ins->std.volMacro;
      DivInstrumentMacro *wave=&ins->std.waveMacro;
      DivInstrumentMacro *pitch=&ins->std.pitchMacro;
      int vol_add = 0;
      int pitch_add = 0;
      bool changed_delta = false;
      bool loop = false;
      for (int tick=0; tick<64; tick++) {
        size_t cur_samp_pos = reader.tell();
        String line = VT2_readstrline(&reader);
        if (line.length() == 0 && tick != 0) {
          if (loop && changed_delta) {
            reader.seek(samp_loop_pos,SEEK_SET);
            line = VT2_readstrline(&reader);
            cur_samp_pos = samp_loop_pos;
            wave->loop = 255; // remove loop
            vol->loop = 255; // remove loop
            pitch->loop = 255; // remove loop
          } else break;
        }
        //logD("%02x: %s\n", i, line.c_str());
        // volume macro
        char vol_add_mode = line.at(16);
        if (vol_add_mode == '+') {
          changed_delta = true;
          vol_add++;
        } else if (vol_add_mode == '-') {
          changed_delta = true;
          vol_add--; 
        }
        char vol_val = VT2_hextoint(line.at(15)) + vol_add;
        if (vol_val < 0) vol_val = 0;
        else if (vol_val > 15) vol_val = 15;
        // wave mode
        unsigned char wave_val = 0;
        if (line.at(0) == 'T') wave_val |= 1;
        if (line.at(1) == 'N') wave_val |= 2;
        if ((line.at(2) == 'E') && (i&1)) wave_val |= 4;

        int pitch_val = VT2_hextoint(line.at(7));
        pitch_val |= VT2_hextoint(line.at(6))<<4;
        pitch_val |= VT2_hextoint(line.at(5))<<8;
        if (line.at(4) == '-') pitch_val = -pitch_val;
        if (line.at(8) == '^') {
          pitch->val[tick] = -(pitch_add);
          pitch_add += pitch_val;
        } else {
          pitch->val[tick] = -(pitch_add + pitch_val);
        }

        vol->val[tick] = vol_val;
        wave->val[tick] = wave_val;
        wave->len = tick+1;
        vol->len = tick+1;
        pitch->len = tick+1;
        if (line.length() >= 19) {
          // check for loop marker
          if (line.at(18) == 'L') {
            samp_loop_pos = cur_samp_pos; 
            wave->loop = tick; // set loop to loop marker pos
            vol->loop = tick; // set loop to loop marker pos
            pitch->loop = tick; // set loop to loop marker pos
            loop = true;
          }
        }
      }
      // ornaments (basically arp macros)
      if ((ins_num>>5) != 32) {
        reader.seek(old_pos, SEEK_SET);
        for (int tr=0;tr<3000;tr++) {
          String line = VT2_readstrline(&reader);
          if (line == ("[Ornament" + ord_num_str + "]"))
              break;
        }
        String line = VT2_readstrline(&reader);
        DivInstrumentMacro *arp=&ins->std.arpMacro;
        int orn_ind_pos = 0;
        int orn_tick = 0;
        for (int p=0; p<(int)line.length(); p++) {
          char orn_char = line.at(p);
          if (orn_char == 'L') {
            // loop marker
            orn_ind_pos=p+1;
            arp->loop=orn_tick;
          } else if ((orn_char == ',') || (p == (line.length()-1))) {
            // delimiter
            unsigned int val = atoi(line.substr(orn_ind_pos,p-orn_ind_pos).c_str());
            if (p == (line.length()-1)) val = atoi(line.substr(orn_ind_pos,line.length()-orn_ind_pos).c_str());
            arp->val[orn_tick]=val;
            arp->len=orn_tick+1;
            orn_tick++;
            orn_ind_pos=p+1;
          }
        }
      }
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