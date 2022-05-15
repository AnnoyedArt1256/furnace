/**
 * Furnace Tracker - multi-system chiptune tracker
 * Copyright (C) 2021-2022 tildearrow and contributors
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

// guiConst: constants used in the GUI like arrays, strings and other stuff
#include "gui.h"
#include "guiConst.h"
#include "../engine/song.h"

const int opOrder[4]={
  0, 2, 1, 3
};

const char* noteNames[180]={
  "c_5", "c+5", "d_5", "d+5", "e_5", "f_5", "f+5", "g_5", "g+5", "a_5", "a+5", "b_5",
  "c_4", "c+4", "d_4", "d+4", "e_4", "f_4", "f+4", "g_4", "g+4", "a_4", "a+4", "b_4",
  "c_3", "c+3", "d_3", "d+3", "e_3", "f_3", "f+3", "g_3", "g+3", "a_3", "a+3", "b_3",
  "c_2", "c+2", "d_2", "d+2", "e_2", "f_2", "f+2", "g_2", "g+2", "a_2", "a+2", "b_2",
  "c_1", "c+1", "d_1", "d+1", "e_1", "f_1", "f+1", "g_1", "g+1", "a_1", "a+1", "b_1",
  "C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
  "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
  "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
  "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
  "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
  "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
  "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
  "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
  "C-8", "C#8", "D-8", "D#8", "E-8", "F-8", "F#8", "G-8", "G#8", "A-8", "A#8", "B-8",
  "C-9", "C#9", "D-9", "D#9", "E-9", "F-9", "F#9", "G-9", "G#9", "A-9", "A#9", "B-9"
};

const char* noteNamesG[180]={
  "c_5", "c+5", "d_5", "d+5", "e_5", "f_5", "f+5", "g_5", "g+5", "a_5", "a+5", "h_5",
  "c_4", "c+4", "d_4", "d+4", "e_4", "f_4", "f+4", "g_4", "g+4", "a_4", "a+4", "h_4",
  "c_3", "c+3", "d_3", "d+3", "e_3", "f_3", "f+3", "g_3", "g+3", "a_3", "a+3", "h_3",
  "c_2", "c+2", "d_2", "d+2", "e_2", "f_2", "f+2", "g_2", "g+2", "a_2", "a+2", "h_2",
  "c_1", "c+1", "d_1", "d+1", "e_1", "f_1", "f+1", "g_1", "g+1", "a_1", "a+1", "h_1",
  "C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "H-0",
  "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "H-1",
  "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "H-2",
  "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "H-3",
  "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "H-4",
  "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "H-5",
  "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "H-6",
  "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "H-7",
  "C-8", "C#8", "D-8", "D#8", "E-8", "F-8", "F#8", "G-8", "G#8", "A-8", "A#8", "H-8",
  "C-9", "C#9", "D-9", "D#9", "E-9", "F-9", "F#9", "G-9", "G#9", "A-9", "A#9", "H-9"
};

const char* pitchLabel[11]={
  "1/6", "1/5", "1/4", "1/3", "1/2", "1x", "2x", "3x", "4x", "5x", "6x"
};

const int altValues[24]={
  0, 10, 1, 11, 2, 3, 12, 4, 13, 5, 14, 6, 7, 15, 8, -1, 9, -1, -1, -1, -1, -1, -1, -1
};

const int vgmVersions[6]={
  0x150,
  0x151,
  0x160,
  0x161,
  0x170,
  0x171
};

const char* insTypes[DIV_INS_MAX]={
  "Standard (SMS/NES)",
  "FM (4-operator)",
  "Game Boy",
  "C64",
  "Sample",
  "PC Engine",
  "AY-3-8910/SSG",
  "AY8930",
  "TIA",
  "SAA1099",
  "VIC",
  "PET",
  "VRC6",
  "FM (OPLL)",
  "FM (OPL)",
  "FDS",
  "Virtual Boy",
  "Namco 163",
  "Konami SCC/Bubble System WSG",
  "FM (OPZ)",
  "POKEY",
  "PC Beeper",
  "WonderSwan",
  "Atari Lynx",
  "VERA",
  "X1-010",
  "VRC6 (saw)",
  "ES5506",
  "MultiPCM",
  "SNES",
  "Sound Unit",
};

const char* sampleDepths[DIV_SAMPLE_DEPTH_MAX]={
  "1-bit PCM",
  "1-bit DPCM",
  NULL,
  NULL,
  "QSound ADPCM",
  "ADPCM-A",
  "ADPCM-B",
  "X68000 ADPCM",
  "8-bit PCM",
  NULL, // "BRR",
  "VOX",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  "16-bit PCM"
};

const char* resampleStrats[DIV_RESAMPLE_MAX]={
  "none",
  "linear",
  "cubic spline",
  "blep synthesis",
  "sinc",
  "best possible"
};

const char* loopMode[DIV_SAMPLE_LOOPMODE_MAX]={
  "Disable",
  "Foward",
  "Backward",
  "Pingpong"
};

const FurnaceGUIColors fxColors[256]={
  GUI_COLOR_PATTERN_EFFECT_MISC, // 00
  GUI_COLOR_PATTERN_EFFECT_PITCH, // 01
  GUI_COLOR_PATTERN_EFFECT_PITCH, // 02
  GUI_COLOR_PATTERN_EFFECT_PITCH, // 03
  GUI_COLOR_PATTERN_EFFECT_PITCH, // 04
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // 05
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // 06
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // 07
  GUI_COLOR_PATTERN_EFFECT_PANNING, // 08
  GUI_COLOR_PATTERN_EFFECT_SPEED, // 09
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // 0A
  GUI_COLOR_PATTERN_EFFECT_SONG, // 0B
  GUI_COLOR_PATTERN_EFFECT_TIME, // 0C
  GUI_COLOR_PATTERN_EFFECT_SONG, // 0D
  GUI_COLOR_PATTERN_EFFECT_INVALID, // 0E
  GUI_COLOR_PATTERN_EFFECT_SPEED, // 0F

  // 10-1F
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,

  // 20-2F
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,

  // 30-3F
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,

  // 40-4F
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,

  // 50-5F
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,
  GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,

  // 60-6F
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,

  // 70-7F
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,

  // 80-8F
  GUI_COLOR_PATTERN_EFFECT_PANNING,
  GUI_COLOR_PATTERN_EFFECT_PANNING,
  GUI_COLOR_PATTERN_EFFECT_PANNING,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,

  // 90-9F
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,
  GUI_COLOR_PATTERN_EFFECT_MISC,

  // A0-AF
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,

  // B0-BF
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,

  // C0-CF
  GUI_COLOR_PATTERN_EFFECT_SPEED,
  GUI_COLOR_PATTERN_EFFECT_SPEED,
  GUI_COLOR_PATTERN_EFFECT_SPEED,
  GUI_COLOR_PATTERN_EFFECT_SPEED,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,

  // D0-DF
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,
  GUI_COLOR_PATTERN_EFFECT_INVALID,

  // E0-FF extended effects
  GUI_COLOR_PATTERN_EFFECT_MISC, // E0
  GUI_COLOR_PATTERN_EFFECT_PITCH, // E1
  GUI_COLOR_PATTERN_EFFECT_PITCH, // E2
  GUI_COLOR_PATTERN_EFFECT_MISC, // E3
  GUI_COLOR_PATTERN_EFFECT_MISC, // E4
  GUI_COLOR_PATTERN_EFFECT_PITCH, // E5
  GUI_COLOR_PATTERN_EFFECT_INVALID, // E6
  GUI_COLOR_PATTERN_EFFECT_INVALID, // E7
  GUI_COLOR_PATTERN_EFFECT_INVALID, // E8
  GUI_COLOR_PATTERN_EFFECT_INVALID, // E9
  GUI_COLOR_PATTERN_EFFECT_MISC, // EA
  GUI_COLOR_PATTERN_EFFECT_MISC, // EB
  GUI_COLOR_PATTERN_EFFECT_TIME, // EC
  GUI_COLOR_PATTERN_EFFECT_TIME, // ED
  GUI_COLOR_PATTERN_EFFECT_SONG, // EE
  GUI_COLOR_PATTERN_EFFECT_SONG, // EF
  GUI_COLOR_PATTERN_EFFECT_SPEED, // F0
  GUI_COLOR_PATTERN_EFFECT_PITCH, // F1
  GUI_COLOR_PATTERN_EFFECT_PITCH, // F2
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // F3
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // F4
  GUI_COLOR_PATTERN_EFFECT_INVALID, // F5
  GUI_COLOR_PATTERN_EFFECT_INVALID, // F6
  GUI_COLOR_PATTERN_EFFECT_INVALID, // F7
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // F8
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // F9
  GUI_COLOR_PATTERN_EFFECT_VOLUME, // FA
  GUI_COLOR_PATTERN_EFFECT_INVALID, // FB
  GUI_COLOR_PATTERN_EFFECT_INVALID, // FC
  GUI_COLOR_PATTERN_EFFECT_INVALID, // FD
  GUI_COLOR_PATTERN_EFFECT_INVALID, // FE
  GUI_COLOR_PATTERN_EFFECT_SONG // FF
};

#define D FurnaceGUIActionDef
#define NOT_AN_ACTION -1

// format: ("ACTION_ENUM", "Action name", defaultBind)
const FurnaceGUIActionDef guiActions[GUI_ACTION_MAX]={
  D("GLOBAL_MIN", "---Global", NOT_AN_ACTION),
  D("OPEN", "Open file", FURKMOD_CMD|SDLK_o),
  D("OPEN_BACKUP", "Restore backup", 0),
  D("SAVE", "Save file", FURKMOD_CMD|SDLK_s),
  D("SAVE_AS", "Save as", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_s),
  D("UNDO", "Undo", FURKMOD_CMD|SDLK_z),
  D("REDO", "Redo", FURKMOD_CMD|SDLK_y),
  D("PLAY_TOGGLE", "Play/Stop (toggle)", SDLK_RETURN),
  D("PLAY", "Play", 0),
  D("STOP", "Stop", 0),
  D("PLAY_REPEAT", "Play (repeat pattern)", 0),
  D("PLAY_CURSOR", "Play from cursor", FURKMOD_SHIFT|SDLK_RETURN),
  D("STEP_ONE", "Step row", FURKMOD_CMD|SDLK_RETURN),
  D("OCTAVE_UP", "Octave up", SDLK_KP_MULTIPLY),
  D("OCTAVE_DOWN", "Octave down", SDLK_KP_DIVIDE),
  D("INS_UP", "Previous instrument", FURKMOD_SHIFT|SDLK_KP_DIVIDE),
  D("INS_DOWN", "Next instrument", FURKMOD_SHIFT|SDLK_KP_MULTIPLY),
  D("STEP_UP", "Increase edit step", FURKMOD_CMD|SDLK_KP_MULTIPLY),
  D("STEP_DOWN", "Decrease edit step", FURKMOD_CMD|SDLK_KP_DIVIDE),
  D("TOGGLE_EDIT", "Toggle edit mode", SDLK_SPACE),
  D("METRONOME", "Metronome", FURKMOD_CMD|SDLK_m),
  D("REPEAT_PATTERN", "Toggle repeat pattern", 0),
  D("FOLLOW_ORDERS", "Follow orders", 0),
  D("FOLLOW_PATTERN", "Follow pattern", 0),
  D("FULLSCREEN", "Toggle full-screen", SDLK_F11),
  D("TX81Z_REQUEST", "Request voice from TX81Z", 0),
  D("PANIC", "Panic", SDLK_F12),

  D("WINDOW_EDIT_CONTROLS", "Edit Controls", 0),
  D("WINDOW_ORDERS", "Orders", 0),
  D("WINDOW_INS_LIST", "Instrument List", 0),
  D("WINDOW_INS_EDIT", "Instrument Editor", 0),
  D("WINDOW_SONG_INFO", "Song Information", 0),
  D("WINDOW_PATTERN", "Pattern", 0),
  D("WINDOW_WAVE_LIST", "Wavetable List", 0),
  D("WINDOW_WAVE_EDIT", "Wavetable Editor", 0),
  D("WINDOW_SAMPLE_LIST", "Sample List", 0),
  D("WINDOW_SAMPLE_EDIT", "Sample Editor", 0),
  D("WINDOW_ABOUT", "About", 0),
  D("WINDOW_SETTINGS", "Settings", 0),
  D("WINDOW_MIXER", "Mixer", 0),
  D("WINDOW_DEBUG", "Debug Menu", 0),
  D("WINDOW_OSCILLOSCOPE", "Oscilloscope (master)", 0),
  D("WINDOW_VOL_METER", "Volume Meter", 0),
  D("WINDOW_STATS", "Statistics", 0),
  D("WINDOW_COMPAT_FLAGS", "Compatibility Flags", 0),
  D("WINDOW_PIANO", "Piano", 0),
  D("WINDOW_NOTES", "Song Comments", 0),
  D("WINDOW_CHANNELS", "Channels", 0),
  D("WINDOW_REGISTER_VIEW", "Register View", 0),
  D("WINDOW_LOG", "Log Viewer", 0),
  D("EFFECT_LIST", "Effect List", 0),
  D("WINDOW_CHAN_OSC", "Oscilloscope (per-channel)", 0),

  D("COLLAPSE_WINDOW", "Collapse/expand current window", 0),
  D("CLOSE_WINDOW", "Close current window", FURKMOD_SHIFT|SDLK_ESCAPE),
  D("GLOBAL_MAX", "", NOT_AN_ACTION),

  D("PAT_MIN", "---Pattern", NOT_AN_ACTION),
  D("PAT_NOTE_UP", "Transpose (+1)", FURKMOD_CMD|SDLK_F2),
  D("PAT_NOTE_DOWN", "Transpose (-1)", FURKMOD_CMD|SDLK_F1),
  D("PAT_OCTAVE_UP", "Transpose (+1 octave)", FURKMOD_CMD|SDLK_F4),
  D("PAT_OCTAVE_DOWN", "Transpose (-1 octave)", FURKMOD_CMD|SDLK_F3),
  D("PAT_VALUE_UP", "Increase values (+1)", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_F2),
  D("PAT_VALUE_DOWN", "Increase values (-1)", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_F1),
  D("PAT_VALUE_UP_COARSE", "Increase values (+16)", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_F4),
  D("PAT_VALUE_DOWN_COARSE", "Increase values (-16)", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_F3),
  D("PAT_SELECT_ALL", "Select all", FURKMOD_CMD|SDLK_a),
  D("PAT_CUT", "Cut", FURKMOD_CMD|SDLK_x),
  D("PAT_COPY", "Copy", FURKMOD_CMD|SDLK_c),
  D("PAT_PASTE", "Paste", FURKMOD_CMD|SDLK_v),
  D("PAT_PASTE_MIX", "Paste Mix (foreground)", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_v),
  D("PAT_PASTE_MIX_BG", "Paste Mix (background)", 0),
  D("PAT_PASTE_FLOOD", "Paste Flood", 0),
  D("PAT_PASTE_OVERFLOW", "Paste Overflow", 0),
  D("PAT_CURSOR_UP", "Move cursor up", SDLK_UP),
  D("PAT_CURSOR_DOWN", "Move cursor down", SDLK_DOWN),
  D("PAT_CURSOR_LEFT", "Move cursor left", SDLK_LEFT),
  D("PAT_CURSOR_RIGHT", "Move cursor right", SDLK_RIGHT),
  D("PAT_CURSOR_UP_ONE", "Move cursor up by one (override Edit Step)", FURKMOD_SHIFT|SDLK_HOME),
  D("PAT_CURSOR_DOWN_ONE", "Move cursor down by one (override Edit Step)", FURKMOD_SHIFT|SDLK_END),
  D("PAT_CURSOR_LEFT_CHANNEL", "Move cursor to previous channel", 0),
  D("PAT_CURSOR_RIGHT_CHANNEL", "Move cursor to next channel", 0),
  D("PAT_CURSOR_NEXT_CHANNEL", "Move cursor to previous channel (overflow)", 0),
  D("PAT_CURSOR_PREVIOUS_CHANNEL", "Move cursor to next channel (overflow)", 0),
  D("PAT_CURSOR_BEGIN", "Move cursor to beginning of pattern", SDLK_HOME),
  D("PAT_CURSOR_END", "Move cursor to end of pattern", SDLK_END),
  D("PAT_CURSOR_UP_COARSE", "Move cursor up (coarse)", SDLK_PAGEUP),
  D("PAT_CURSOR_DOWN_COARSE", "Move cursor down (coarse)", SDLK_PAGEDOWN),
  D("PAT_SELECTION_UP", "Expand selection upwards", FURKMOD_SHIFT|SDLK_UP),
  D("PAT_SELECTION_DOWN", "Expand selection downwards", FURKMOD_SHIFT|SDLK_DOWN),
  D("PAT_SELECTION_LEFT", "Expand selection to the left", FURKMOD_SHIFT|SDLK_LEFT),
  D("PAT_SELECTION_RIGHT", "Expand selection to the right", FURKMOD_SHIFT|SDLK_RIGHT),
  D("PAT_SELECTION_UP_ONE", "Expand selection upwards by one (override Edit Step)", 0),
  D("PAT_SELECTION_DOWN_ONE", "Expand selection downwards by one (override Edit Step)", 0),
  D("PAT_SELECTION_BEGIN", "Expand selection to beginning of pattern", 0),
  D("PAT_SELECTION_END", "Expand selection to end of pattern", 0),
  D("PAT_SELECTION_UP_COARSE", "Expand selection upwards (coarse)", FURKMOD_SHIFT|SDLK_PAGEUP),
  D("PAT_SELECTION_DOWN_COARSE", "Expand selection downwards (coarse)", FURKMOD_SHIFT|SDLK_PAGEDOWN),
  D("PAT_DELETE", "Delete", SDLK_DELETE),
  D("PAT_PULL_DELETE", "Pull delete", SDLK_BACKSPACE),
  D("PAT_INSERT", "Insert", SDLK_INSERT),
  D("PAT_MUTE_CURSOR", "Mute channel at cursor", FURKMOD_ALT|SDLK_F9),
  D("PAT_SOLO_CURSOR", "Solo channel at cursor", FURKMOD_ALT|SDLK_F10),
  D("PAT_UNMUTE_ALL", "Unmute all channels", FURKMOD_ALT|FURKMOD_SHIFT|SDLK_F9),
  D("PAT_NEXT_ORDER", "Go to next order", 0),
  D("PAT_PREV_ORDER", "Go to previous order", 0),
  D("PAT_COLLAPSE", "Collapse channel at cursor", 0),
  D("PAT_INCREASE_COLUMNS", "Increase effect columns", 0),
  D("PAT_DECREASE_COLUMNS", "Decrease effect columns", 0),
  D("PAT_INTERPOLATE", "Interpolate", 0),
  D("PAT_FADE", "Fade", 0),
  D("PAT_INVERT_VALUES", "Invert values", 0),
  D("PAT_FLIP_SELECTION", "Flip selection", 0),
  D("PAT_COLLAPSE_ROWS", "Collapse rows", 0),
  D("PAT_EXPAND_ROWS", "Expand rows", 0),
  D("PAT_COLLAPSE_PAT", "Collapse pattern", 0),
  D("PAT_EXPAND_PAT", "Expand pattern", 0),
  D("PAT_COLLAPSE_SONG", "Collapse song", 0),
  D("PAT_EXPAND_SONG", "Expand song", 0),
  D("PAT_LATCH", "Set note input latch", 0),
  D("PAT_MAX", "", NOT_AN_ACTION),

  D("INS_LIST_MIN", "---Instrument list", NOT_AN_ACTION),
  D("INS_LIST_ADD", "Add", SDLK_INSERT),
  D("INS_LIST_DUPLICATE", "Duplicate", FURKMOD_CMD|SDLK_d),
  D("INS_LIST_OPEN", "Open", 0),
  D("INS_LIST_OPEN_REPLACE", "Open (replace current)", 0),
  D("INS_LIST_SAVE", "Save", 0),
  D("INS_LIST_MOVE_UP", "Move up", FURKMOD_SHIFT|SDLK_UP),
  D("INS_LIST_MOVE_DOWN", "Move down", FURKMOD_SHIFT|SDLK_DOWN),
  D("INS_LIST_DELETE", "Delete", 0),
  D("INS_LIST_EDIT", "Edit", FURKMOD_SHIFT|SDLK_RETURN),
  D("INS_LIST_UP", "Cursor up", SDLK_UP),
  D("INS_LIST_DOWN", "Cursor down", SDLK_DOWN),
  D("INS_LIST_MAX", "", NOT_AN_ACTION),

  D("WAVE_LIST_MIN", "---Wavetable list", NOT_AN_ACTION),
  D("WAVE_LIST_ADD", "Add", SDLK_INSERT),
  D("WAVE_LIST_DUPLICATE", "Duplicate", FURKMOD_CMD|SDLK_d),
  D("WAVE_LIST_OPEN", "Open", 0),
  D("WAVE_LIST_SAVE", "Save", 0),
  D("WAVE_LIST_MOVE_UP", "Move up", FURKMOD_SHIFT|SDLK_UP),
  D("WAVE_LIST_MOVE_DOWN", "Move down", FURKMOD_SHIFT|SDLK_DOWN),
  D("WAVE_LIST_DELETE", "Delete", 0),
  D("WAVE_LIST_EDIT", "Edit", FURKMOD_SHIFT|SDLK_RETURN),
  D("WAVE_LIST_UP", "Cursor up", SDLK_UP),
  D("WAVE_LIST_DOWN", "Cursor down", SDLK_DOWN),
  D("WAVE_LIST_MAX", "", NOT_AN_ACTION),

  D("SAMPLE_LIST_MIN", "---Sample list", NOT_AN_ACTION),
  D("SAMPLE_LIST_ADD", "Add", SDLK_INSERT),
  D("SAMPLE_LIST_DUPLICATE", "Duplicate", FURKMOD_CMD|SDLK_d),
  D("SAMPLE_LIST_OPEN", "Open", 0),
  D("SAMPLE_LIST_SAVE", "Save", 0),
  D("SAMPLE_LIST_MOVE_UP", "Move up", FURKMOD_SHIFT|SDLK_UP),
  D("SAMPLE_LIST_MOVE_DOWN", "Move down", FURKMOD_SHIFT|SDLK_DOWN),
  D("SAMPLE_LIST_DELETE", "Delete", 0),
  D("SAMPLE_LIST_EDIT", "Edit", FURKMOD_SHIFT|SDLK_RETURN),
  D("SAMPLE_LIST_UP", "Cursor up", SDLK_UP),
  D("SAMPLE_LIST_DOWN", "Cursor down", SDLK_DOWN),
  D("SAMPLE_LIST_PREVIEW", "Preview", 0),
  D("SAMPLE_LIST_STOP_PREVIEW", "Stop preview", 0),
  D("SAMPLE_LIST_MAX", "", NOT_AN_ACTION),

  D("SAMPLE_MIN", "---Sample editor", NOT_AN_ACTION),
  D("SAMPLE_SELECT", "Edit mode: Select", FURKMOD_SHIFT|SDLK_i),
  D("SAMPLE_DRAW", "Edit mode: Draw", FURKMOD_SHIFT|SDLK_d),
  D("SAMPLE_CUT", "Cut", FURKMOD_CMD|SDLK_x),
  D("SAMPLE_COPY", "Copy", FURKMOD_CMD|SDLK_c),
  D("SAMPLE_PASTE", "Paste", FURKMOD_CMD|SDLK_v),
  D("SAMPLE_PASTE_REPLACE", "Paste replace", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_v),
  D("SAMPLE_PASTE_MIX", "Paste mix", FURKMOD_CMD|FURKMOD_ALT|SDLK_v),
  D("SAMPLE_SELECT_ALL", "Select all", FURKMOD_CMD|SDLK_a),
  D("SAMPLE_RESIZE", "Resize", FURKMOD_CMD|SDLK_r),
  D("SAMPLE_RESAMPLE", "Resample", FURKMOD_CMD|SDLK_e),
  D("SAMPLE_AMPLIFY", "Amplify", FURKMOD_CMD|SDLK_b),
  D("SAMPLE_NORMALIZE", "Normalize", FURKMOD_CMD|SDLK_n),
  D("SAMPLE_FADE_IN", "Fade in", FURKMOD_CMD|SDLK_i),
  D("SAMPLE_FADE_OUT", "Fade out", FURKMOD_CMD|SDLK_o),
  D("SAMPLE_SILENCE", "Apply silence", FURKMOD_SHIFT|SDLK_DELETE),
  D("SAMPLE_INSERT", "Insert silence", SDLK_INSERT),
  D("SAMPLE_DELETE", "Delete", SDLK_DELETE),
  D("SAMPLE_TRIM", "Trim", FURKMOD_CMD|SDLK_DELETE),
  D("SAMPLE_REVERSE", "Reverse", FURKMOD_CMD|SDLK_t),
  D("SAMPLE_INVERT", "Invert", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_t),
  D("SAMPLE_SIGN", "Signed/unsigned exchange", FURKMOD_CMD|SDLK_u),
  D("SAMPLE_FILTER", "Apply filter", FURKMOD_CMD|SDLK_f),
  D("SAMPLE_PREVIEW", "Preview sample", 0),
  D("SAMPLE_STOP_PREVIEW", "Stop sample preview", 0),
  D("SAMPLE_ZOOM_IN", "Zoom in", FURKMOD_CMD|SDLK_EQUALS),
  D("SAMPLE_ZOOM_OUT", "Zoom out", FURKMOD_CMD|SDLK_MINUS),
  D("SAMPLE_ZOOM_AUTO", "Toggle auto-zoom", FURKMOD_CMD|SDLK_0),
  D("SAMPLE_MAKE_INS", "Create instrument from sample", 0),
  D("SAMPLE_MAX", "", NOT_AN_ACTION),

  D("ORDERS_MIN", "---Orders", NOT_AN_ACTION),
  D("ORDERS_UP", "Previous order", SDLK_UP),
  D("ORDERS_DOWN", "Next order", SDLK_DOWN),
  D("ORDERS_LEFT", "Cursor left", SDLK_LEFT),
  D("ORDERS_RIGHT", "Cursor right", SDLK_RIGHT),
  D("ORDERS_INCREASE", "Increase value", 0),
  D("ORDERS_DECREASE", "Decrease value", 0),
  D("ORDERS_EDIT_MODE", "Switch edit mode", 0),
  D("ORDERS_LINK", "Toggle alter entire row", FURKMOD_CMD|SDLK_l),
  D("ORDERS_ADD", "Add", SDLK_INSERT),
  D("ORDERS_DUPLICATE", "Duplicate", FURKMOD_CMD|SDLK_d),
  D("ORDERS_DEEP_CLONE", "Deep clone", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_d),
  D("ORDERS_DUPLICATE_END", "Duplicate to end of song", FURKMOD_CMD|SDLK_e),
  D("ORDERS_DEEP_CLONE_END", "Deep clone to end of song", FURKMOD_CMD|FURKMOD_SHIFT|SDLK_e),
  D("ORDERS_REMOVE", "Remove", SDLK_DELETE),
  D("ORDERS_MOVE_UP", "Move up", FURKMOD_SHIFT|SDLK_UP),
  D("ORDERS_MOVE_DOWN", "Move down", FURKMOD_SHIFT|SDLK_DOWN),
  D("ORDERS_REPLAY", "Replay", 0),
  D("ORDERS_MAX", "", NOT_AN_ACTION),
};
#undef D

#define D(x,y,z) FurnaceGUIColorDef(#x,y,ImGui::ColorConvertFloat4ToU32(z))

const FurnaceGUIColorDef guiColors[GUI_COLOR_MAX]={
  D(GUI_COLOR_BACKGROUND,"Background",ImVec4(0.1f,0.1f,0.1f,1.0f)),
  D(GUI_COLOR_FRAME_BACKGROUND,"",ImVec4(0.0f,0.0f,0.0f,0.85f)),
  D(GUI_COLOR_MODAL_BACKDROP,"",ImVec4(0.0f,0.0f,0.0f,0.55f)),
  D(GUI_COLOR_HEADER,"",ImVec4(0.2f,0.2f,0.2f,1.0f)),
  D(GUI_COLOR_TEXT,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_ACCENT_PRIMARY,"",ImVec4(0.06f,0.53f,0.98f,1.0f)),
  D(GUI_COLOR_ACCENT_SECONDARY,"",ImVec4(0.26f,0.59f,0.98f,1.0f)),
  D(GUI_COLOR_BORDER,"",ImVec4(0.43f,0.43f,0.5f,0.5f)),
  D(GUI_COLOR_BORDER_SHADOW,"",ImVec4(0.0f,0.0f,0.0f,0.0f)),
  D(GUI_COLOR_TOGGLE_OFF,"",ImVec4(0.2f,0.2f,0.2f,1.0f)),
  D(GUI_COLOR_TOGGLE_ON,"",ImVec4(0.2f,0.6f,0.2f,1.0f)),
  D(GUI_COLOR_EDITING,"",ImVec4(0.2f,0.1f,0.1f,1.0f)),
  D(GUI_COLOR_SONG_LOOP,"",ImVec4(0.3f,0.5f,0.8f,0.4f)),

  D(GUI_COLOR_FILE_DIR,"",ImVec4(0.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_FILE_SONG_NATIVE,"",ImVec4(0.5f,1.0f,0.5f,1.0f)),
  D(GUI_COLOR_FILE_SONG_IMPORT,"",ImVec4(0.5f,1.0f,0.8f,1.0f)),
  D(GUI_COLOR_FILE_INSTR,"",ImVec4(1.0f,0.5f,0.5f,1.0f)),
  D(GUI_COLOR_FILE_AUDIO,"",ImVec4(1.0f,1.0f,0.5f,1.0f)),
  D(GUI_COLOR_FILE_WAVE,"",ImVec4(1.0f,0.75f,0.5f,1.0f)),
  D(GUI_COLOR_FILE_VGM,"",ImVec4(1.0f,1.0f,0.5f,1.0f)),
  D(GUI_COLOR_FILE_FONT,"",ImVec4(0.3f,1.0f,0.6f,1.0f)),
  D(GUI_COLOR_FILE_OTHER,"",ImVec4(0.7f,0.7f,0.7f,1.0f)),

  D(GUI_COLOR_OSC_BG1,"",ImVec4(0.1f,0.18f,0.3f,1.0f)),
  D(GUI_COLOR_OSC_BG2,"",ImVec4(0.1f,0.18f,0.3f,1.0f)),
  D(GUI_COLOR_OSC_BG3,"",ImVec4(0.05f,0.15f,0.25f,1.0f)),
  D(GUI_COLOR_OSC_BG4,"",ImVec4(0.05f,0.15f,0.25f,1.0f)),
  D(GUI_COLOR_OSC_BORDER,"",ImVec4(0.4f,0.6f,0.95f,1.0f)),
  D(GUI_COLOR_OSC_WAVE,"",ImVec4(0.95f,0.95f,1.0f,1.0f)),
  D(GUI_COLOR_OSC_WAVE_PEAK,"",ImVec4(0.95f,0.95f,1.0f,1.0f)),
  D(GUI_COLOR_OSC_REF,"",ImVec4(0.3,0.65f,1.0f,0.15f)),
  D(GUI_COLOR_OSC_GUIDE,"",ImVec4(0.3,0.65f,1.0f,0.13f)),

  D(GUI_COLOR_VOLMETER_LOW,"",ImVec4(0.2f,0.6f,0.2f,1.0f)),
  D(GUI_COLOR_VOLMETER_HIGH,"",ImVec4(1.0f,0.9f,0.2f,1.0f)),
  D(GUI_COLOR_VOLMETER_PEAK,"",ImVec4(1.0f,0.1f,0.1f,1.0f)),

  D(GUI_COLOR_ORDER_ROW_INDEX,"",ImVec4(0.5f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_ORDER_ACTIVE,"",ImVec4(0.4f,0.7f,1.0f,0.25f)),
  D(GUI_COLOR_ORDER_SIMILAR,"",ImVec4(0.5f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_ORDER_INACTIVE,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),

  D(GUI_COLOR_FM_ALG_BG,"",ImVec4(0.12f,0.12f,0.12f,1.0f)),
  D(GUI_COLOR_FM_ALG_LINE,"",ImVec4(1.0f,1.0f,1.0f,0.33f)),
  D(GUI_COLOR_FM_MOD,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_FM_PRIMARY_MOD,"",ImVec4(0.06f,0.53f,0.98f,1.0f)),
  D(GUI_COLOR_FM_SECONDARY_MOD,"",ImVec4(0.06f,0.53f,0.98f,1.0f)),
  D(GUI_COLOR_FM_BORDER_MOD,"",ImVec4(0.43f,0.43f,0.5f,0.5f)),
  D(GUI_COLOR_FM_BORDER_SHADOW_MOD,"",ImVec4(0.0f,0.0f,0.0f,0.0f)),
  D(GUI_COLOR_FM_CAR,"",ImVec4(0.5f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_FM_PRIMARY_CAR,"",ImVec4(0.06f,0.8f,0.98f,1.0f)),
  D(GUI_COLOR_FM_SECONDARY_CAR,"",ImVec4(0.06f,0.8f,0.98f,1.0f)),
  D(GUI_COLOR_FM_BORDER_CAR,"",ImVec4(0.43f,0.5f,0.5f,0.5f)),
  D(GUI_COLOR_FM_BORDER_SHADOW_CAR,"",ImVec4(0.0f,0.0f,0.0f,0.0f)),

  D(GUI_COLOR_FM_ENVELOPE,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_FM_ENVELOPE_SUS_GUIDE,"",ImVec4(0.3f,0.5f,0.8f,0.4f)),
  D(GUI_COLOR_FM_ENVELOPE_RELEASE,"",ImVec4(0.3f,0.5f,0.8f,0.4f)),
  D(GUI_COLOR_FM_SSG,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_FM_WAVE,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),

  D(GUI_COLOR_MACRO_VOLUME,"",ImVec4(0.2f,1.0f,0.0f,1.0f)),
  D(GUI_COLOR_MACRO_PITCH,"",ImVec4(1.0f,0.8f,0.0f,1.0f)),
  D(GUI_COLOR_MACRO_OTHER,"",ImVec4(0.0f,0.9f,1.0f,1.0f)),
  D(GUI_COLOR_MACRO_WAVE,"",ImVec4(1.0f,0.4f,0.0f,1.0f)),

  D(GUI_COLOR_INSTR_STD,"",ImVec4(0.6f,1.0f,0.5f,1.0f)),
  D(GUI_COLOR_INSTR_FM,"",ImVec4(0.6f,0.9f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_GB,"",ImVec4(1.0f,1.0f,0.5f,1.0f)),
  D(GUI_COLOR_INSTR_C64,"",ImVec4(0.85f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_AMIGA,"",ImVec4(1.0f,0.5f,0.5f,1.0f)),
  D(GUI_COLOR_INSTR_PCE,"",ImVec4(1.0f,0.8f,0.5f,1.0f)),
  D(GUI_COLOR_INSTR_AY,"",ImVec4(1.0f,0.5f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_AY8930,"",ImVec4(0.7f,0.5f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_TIA,"",ImVec4(1.0f,0.6f,0.4f,1.0f)),
  D(GUI_COLOR_INSTR_SAA1099,"",ImVec4(0.3f,0.3f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_VIC,"",ImVec4(0.2f,1.0f,0.6f,1.0f)),
  D(GUI_COLOR_INSTR_PET,"",ImVec4(1.0f,1.0f,0.8f,1.0f)),
  D(GUI_COLOR_INSTR_VRC6,"",ImVec4(1.0f,0.9f,0.5f,1.0f)),
  D(GUI_COLOR_INSTR_OPLL,"",ImVec4(0.6f,0.7f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_OPL,"",ImVec4(0.3f,1.0f,0.9f,1.0f)),
  D(GUI_COLOR_INSTR_FDS,"",ImVec4(0.8f,0.5f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_VBOY,"",ImVec4(1.0f,0.1f,0.1f,1.0f)),
  D(GUI_COLOR_INSTR_N163,"",ImVec4(1.0f,0.4f,0.1f,1.0f)),
  D(GUI_COLOR_INSTR_SCC,"",ImVec4(0.7f,1.0f,0.3f,1.0f)),
  D(GUI_COLOR_INSTR_OPZ,"",ImVec4(0.2f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_POKEY,"",ImVec4(0.5f,1.0f,0.3f,1.0f)),
  D(GUI_COLOR_INSTR_BEEPER,"",ImVec4(0.0f,1.0f,0.0f,1.0f)),
  D(GUI_COLOR_INSTR_SWAN,"",ImVec4(0.3f,0.5f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_MIKEY,"",ImVec4(0.5f,1.0f,0.3f,1.0f)),
  D(GUI_COLOR_INSTR_VERA,"",ImVec4(0.4f,0.6f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_X1_010,"",ImVec4(0.3f,0.5f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_VRC6_SAW,"",ImVec4(0.8f,0.3f,0.0f,1.0f)),
  D(GUI_COLOR_INSTR_ES5506,"",ImVec4(0.5f,0.5f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_MULTIPCM,"",ImVec4(1.0f,0.8f,0.1f,1.0f)),
  D(GUI_COLOR_INSTR_SNES,"",ImVec4(0.8f,0.7f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_SU,"",ImVec4(0.95f,0.98f,1.0f,1.0f)),
  D(GUI_COLOR_INSTR_UNKNOWN,"",ImVec4(0.3f,0.3f,0.3f,1.0f)),

  D(GUI_COLOR_CHANNEL_FM,"",ImVec4(0.2f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_CHANNEL_PULSE,"",ImVec4(0.4f,1.0f,0.2f,1.0f)),
  D(GUI_COLOR_CHANNEL_NOISE,"",ImVec4(0.8f,0.8f,0.8f,1.0f)),
  D(GUI_COLOR_CHANNEL_WAVE,"",ImVec4(1.0f,0.5f,0.2f,1.0f)),
  D(GUI_COLOR_CHANNEL_PCM,"",ImVec4(1.0f,0.9f,0.2f,1.0f)),
  D(GUI_COLOR_CHANNEL_OP,"",ImVec4(0.2f,0.4f,1.0f,1.0f)),
  D(GUI_COLOR_CHANNEL_MUTED,"",ImVec4(0.5f,0.5f,0.5f,1.0f)),

  D(GUI_COLOR_PATTERN_PLAY_HEAD,"",ImVec4(1.0f,1.0f,1.0f,0.25f)),
  D(GUI_COLOR_PATTERN_CURSOR,"",ImVec4(0.1f,0.3f,0.5f,1.0f)),
  D(GUI_COLOR_PATTERN_CURSOR_HOVER,"",ImVec4(0.2f,0.4f,0.6f,1.0f)),
  D(GUI_COLOR_PATTERN_CURSOR_ACTIVE,"",ImVec4(0.2f,0.5f,0.7f,1.0f)),
  D(GUI_COLOR_PATTERN_SELECTION,"",ImVec4(0.15f,0.15f,0.2f,1.0f)),
  D(GUI_COLOR_PATTERN_SELECTION_HOVER,"",ImVec4(0.2f,0.2f,0.3f,1.0f)),
  D(GUI_COLOR_PATTERN_SELECTION_ACTIVE,"",ImVec4(0.4f,0.4f,0.5f,1.0f)),
  D(GUI_COLOR_PATTERN_HI_1,"",ImVec4(0.6f,0.6f,0.6f,0.2f)),
  D(GUI_COLOR_PATTERN_HI_2,"",ImVec4(0.5f,0.8f,1.0f,0.2f)),
  D(GUI_COLOR_PATTERN_ROW_INDEX,"",ImVec4(0.5f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_ROW_INDEX_HI1,"",ImVec4(0.5f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_ROW_INDEX_HI2,"",ImVec4(0.5f,0.8f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_ACTIVE,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_INACTIVE,"",ImVec4(0.5f,0.5f,0.5f,1.0f)),
  D(GUI_COLOR_PATTERN_ACTIVE_HI1,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_INACTIVE_HI1,"",ImVec4(0.5f,0.5f,0.5f,1.0f)),
  D(GUI_COLOR_PATTERN_ACTIVE_HI2,"",ImVec4(1.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_INACTIVE_HI2,"",ImVec4(0.5f,0.5f,0.5f,1.0f)),
  D(GUI_COLOR_PATTERN_INS,"",ImVec4(0.4f,0.7f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_INS_WARN,"",ImVec4(1.0f,1.0f,0.1f,1.0f)),
  D(GUI_COLOR_PATTERN_INS_ERROR,"",ImVec4(1.0f,0.1f,0.1f,1.0f)),
  D(GUI_COLOR_PATTERN_VOLUME_MAX,"",ImVec4(0.0f,1.0f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_VOLUME_HALF,"",ImVec4(0.0f,0.75f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_VOLUME_MIN,"",ImVec4(0.0f,0.5f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_INVALID,"",ImVec4(1.0f,0.0f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_PITCH,"",ImVec4(1.0f,1.0f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_VOLUME,"",ImVec4(0.0f,1.0f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_PANNING,"",ImVec4(0.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_SONG,"",ImVec4(1.0f,0.0f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_TIME,"",ImVec4(0.5f,0.0f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_SPEED,"",ImVec4(1.0f,0.0f,1.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_SYS_PRIMARY,"",ImVec4(0.5f,1.0f,0.0f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_SYS_SECONDARY,"",ImVec4(0.0f,1.0f,0.5f,1.0f)),
  D(GUI_COLOR_PATTERN_EFFECT_MISC,"",ImVec4(0.3f,0.3f,1.0f,1.0f)),

  D(GUI_COLOR_LOGLEVEL_ERROR,"",ImVec4(1.0f,0.2f,0.2f,1.0f)),
  D(GUI_COLOR_LOGLEVEL_WARNING,"",ImVec4(1.0f,1.0f,0.2f,1.0f)),
  D(GUI_COLOR_LOGLEVEL_INFO,"",ImVec4(0.4f,1.0f,0.4f,1.0f)),
  D(GUI_COLOR_LOGLEVEL_DEBUG,"",ImVec4(0.3f,0.5f,1.0f,1.0f)),
  D(GUI_COLOR_LOGLEVEL_TRACE,"",ImVec4(0.8f,0.8f,0.8f,1.0f)),

  D(GUI_COLOR_EE_VALUE,"",ImVec4(0.0f,1.0f,1.0f,1.0f)),
  D(GUI_COLOR_PLAYBACK_STAT,"",ImVec4(0.6f,0.6f,0.6f,1.0f)),
};
#undef D

// define systems.
const int availableSystems[]={
  DIV_SYSTEM_YM2612,
  DIV_SYSTEM_YM2612_EXT,
  DIV_SYSTEM_SMS,
  DIV_SYSTEM_GB,
  DIV_SYSTEM_PCE,
  DIV_SYSTEM_NES,
  DIV_SYSTEM_C64_8580,
  DIV_SYSTEM_C64_6581,
  DIV_SYSTEM_YM2151,
  DIV_SYSTEM_SEGAPCM,
  DIV_SYSTEM_SEGAPCM_COMPAT,
  DIV_SYSTEM_YM2610,
  DIV_SYSTEM_YM2610_EXT,
  DIV_SYSTEM_YM2610_FULL,
  DIV_SYSTEM_YM2610_FULL_EXT,
  DIV_SYSTEM_YM2610B,
  DIV_SYSTEM_YM2610B_EXT,
  DIV_SYSTEM_AY8910,
  DIV_SYSTEM_AMIGA,
  DIV_SYSTEM_PCSPKR,
  DIV_SYSTEM_YMU759,
  DIV_SYSTEM_DUMMY,
  DIV_SYSTEM_SOUND_UNIT,
  DIV_SYSTEM_OPLL,
  DIV_SYSTEM_OPLL_DRUMS,
  DIV_SYSTEM_VRC7,
  DIV_SYSTEM_OPL,
  DIV_SYSTEM_OPL_DRUMS,
  DIV_SYSTEM_OPL2,
  DIV_SYSTEM_OPL2_DRUMS,
  DIV_SYSTEM_OPL3,
  DIV_SYSTEM_OPL3_DRUMS,
  DIV_SYSTEM_OPZ,
  DIV_SYSTEM_TIA,
  DIV_SYSTEM_SAA1099,
  DIV_SYSTEM_AY8930,
  DIV_SYSTEM_LYNX,
  DIV_SYSTEM_QSOUND,
  DIV_SYSTEM_X1_010,
  DIV_SYSTEM_SWAN,
  DIV_SYSTEM_VERA,
  DIV_SYSTEM_BUBSYS_WSG,
  DIV_SYSTEM_N163,
  DIV_SYSTEM_PET,
  DIV_SYSTEM_VIC20,
  DIV_SYSTEM_VRC6,
  DIV_SYSTEM_FDS,
  DIV_SYSTEM_MMC5,
  DIV_SYSTEM_ES5506,
  DIV_SYSTEM_SCC,
  DIV_SYSTEM_SCC_PLUS,
  0 // don't remove this last one!
};
