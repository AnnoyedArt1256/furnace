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

int VT2_hextoint(char n) {
  if (n >= 'A' && n <= 'F') return n-'A'+0x0a;
  if (n >= '0' && n <= '9') return n-'0';
  return 0;
}

int VT2_lettertoint(char n) {
  if (n >= 'A' && n <= 'Z') return n-'Z';
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
    reader.readStringLine(); // skip [Module]

    while (true) {
        String line = reader.readStringLine();
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

    ds.subsong[0]->patLen=64;
    ds.subsong[0]->hz=50;
    ds.systemLen=chCount/3;
    for(int i=0; i<ds.systemLen; i++) {
      ds.system[i]=DIV_SYSTEM_AY8910;
    }
    for(int i=0; i<chCount; i++) {
      ds.subsong[0]->chanShow[i]=true;
      ds.subsong[0]->chanShowChanOsc[i]=true;
      ds.subsong[0]->chanName[i]=fmt::sprintf("Channel %d",i+1);
      ds.subsong[0]->chanShortName[i]=fmt::sprintf("C%d",i+1);
      ds.subsong[0]->pat[i].effectCols=4;
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
        ds.subsong[0]->orders.ord[j][i]=pat_inds[i];
      }
    }
                            // A,B,C,D,E,F,G
    const int note2int[7] = {9,11,0,2,4,5,7};

    for (int pat=0; pat<max_pat; pat++) {
      String pat_num_str = std::to_string(pat);
      for (int tr=0;tr<2560;tr++) {
        String line = reader.readStringLine();
        if (line == ("[Pattern" + pat_num_str + "]"))
            break;
      }
      DivPattern* chpats[DIV_MAX_CHANS];
      for (int ch=0; ch<chCount; ch++) {
        chpats[ch]=ds.subsong[0]->pat[ch].getPattern(pat,true);
      }
      for (int row=0; row<64; row++) {
        String line = reader.readStringLine();

        if (line.length() == 0 && row != 0) {
          short* dstrow=chpats[3]->data[row-1];
          break;
        }

        for (int ch=0; ch<chCount; ch++) {
          short* dstrow=chpats[ch]->data[row];

          String chpat_line = line.substr(8+(ch*14),14-1);
          if (chpat_line.at(0) == 'R') {
            dstrow[0]=102;
          } else if (chpat_line.at(0) != '-') {
            char note_char = chpat_line.at(0);
            unsigned char sharp = chpat_line.at(1)=='#'?1:0;
            char oct = chpat_line.at(2);
            if (note_char >= 'A' && note_char <= 'G') {
              char note = note2int[note_char-'A']+sharp+(oct-'0')*12;
              dstrow[0]=((note+11)%12)+1;
              dstrow[1]=(note-1)/12;
            }
          }

          // TODO: add ornaments
          if (chpat_line.at(7) != '.') dstrow[3]=VT2_hextoint(chpat_line.at(7));
          if (chpat_line.at(4) != '.') {
            int ins_num = VT2_lettertoint(chpat_line.at(4));
            if (ins_num > -1) dstrow[2]=ins_num;
          }
        }
      }
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