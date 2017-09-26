// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef ADnoteUI_h
#define ADnoteUI_h
#include <FL/Fl.H>
#include "../Params/ADnoteParameters.h"
#include "ResonanceUI.h"
#include "Fl_Osc_Slider.H"
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include "Fl_Osc_Pane.H"
#include "Fl_Osc_Dial.H"
#include "Fl_Osc_Check.H"
#include "Fl_Osc_Choice.H"
#include "Fl_Osc_Slider.H"
#include "Fl_Osc_VSlider.H"
#include "Fl_Oscilloscope.h"
#include "EnvelopeUI.h"
#include "LFOUI.h"
#include "FilterUI.h"
#include "OscilGenUI.h"
#include "PresetsUI.h"

class PhaseSlider : public Fl_Osc_TSlider {
public:
  PhaseSlider(int x,int y, int w, int h, const char *label=0)
  ;
  void set_scope(Fl_Oscilloscope *newscope);
  void OSC_value(int i);
  void cb(void);
private:
  Fl_Oscilloscope *oscope = NULL; 
};
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

class ADvoicelistitem : public Fl_Osc_Group {
  Fl_Osc_Group* make_window();
  Fl_Osc_Group *ADnoteVoiceListItem;
  Fl_Osc_Group *voicelistitemgroup;
public:
  Fl_Osc_VSlider *voicevolume;
  Fl_Osc_Check *voiceresonanceenabled;
  Fl_Osc_VSlider *voicelfofreq;
  Fl_Osc_Dial *voicepanning;
  Fl_Osc_Group *voiceoscil;
  Fl_Osc_Output *detunevalueoutput;
  Fl_Osc_Slider *voicedetune;
private:
  void cb_voicedetune_i(Fl_Osc_Slider*, void*);
  static void cb_voicedetune(Fl_Osc_Slider*, void*);
public:
  Fl_Box *whitenoiselabel;
  Fl_Box *pinknoiselabel;
private:
  Fl_Osc_Check *voiceenabled;
  void cb_voiceenabled_i(Fl_Osc_Check*, void*);
  static void cb_voiceenabled(Fl_Osc_Check*, void*);
  void cb_edit_i(Fl_Button*, void*);
  static void cb_edit(Fl_Button*, void*);
public:
  ADvoicelistitem(int x,int y, int w, int h, const char *label=0);
  void init(int nvoice_, std::string loc_, Fl_Osc_Interface *osc_);
  void refreshlist();
  ~ADvoicelistitem();
private:
  int nvoice; 
  class Osc_IntModel *voice_phase;
  class Osc_IntModel *sound_type;
  class Osc_IntModel *ext_oscil;
  Fl_Oscilloscope *oscil; 
  std::string loc; 
};
#include <FL/Fl_Group.H>

class ADvoiceUI : public Fl_Group {
public:
  Fl_Osc_Group* make_window();
  Fl_Osc_Group *ADnoteVoiceParameters;
  Fl_Group *voiceparametersgroup;
  Fl_Group *voicemodegroup;
  Fl_Group *voiceFMparametersgroup;
  Fl_Group *modfrequency;
  EnvelopeUI *voiceFMfreqenvgroup;
private:
  void cb_On_i(Fl_Osc_Check*, void*);
  static void cb_On(Fl_Osc_Check*, void*);
  void cb__i(Fl_Osc_Slider*, void*);
  static void cb_(Fl_Osc_Slider*, void*);
public:
  Fl_Osc_Output *fmdetunevalueoutput;
private:
  void cb_Detune_i(Fl_Osc_Choice*, void*);
  static void cb_Detune(Fl_Osc_Choice*, void*);
  void cb_440Hz_i(Fl_Osc_Check*, void*);
  static void cb_440Hz(Fl_Osc_Check*, void*);
public:
  EnvelopeUI *voiceFMampenvgroup;
private:
  void cb_On1_i(Fl_Osc_Check*, void*);
  static void cb_On1(Fl_Osc_Check*, void*);
public:
  Fl_Group *modoscil;
  Fl_Osc_Group *fmoscil;
  Fl_Button *changeFMoscilbutton;
private:
  void cb_changeFMoscilbutton_i(Fl_Button*, void*);
  static void cb_changeFMoscilbutton(Fl_Button*, void*);
public:
  Fl_Osc_Choice *extFMoscil;
private:
  void cb_extFMoscil_i(Fl_Osc_Choice*, void*);
  static void cb_extFMoscil(Fl_Osc_Choice*, void*);
public:
  Fl_Osc_Choice *extMod;
private:
  void cb_extMod_i(Fl_Osc_Choice*, void*);
  static void cb_extMod(Fl_Osc_Choice*, void*);
public:
  Fl_Osc_Choice *mod_type;
private:
  void cb_mod_type_i(Fl_Osc_Choice*, void*);
  static void cb_mod_type(Fl_Osc_Choice*, void*);
  static Fl_Menu_Item menu_mod_type[];
public:
  EnvelopeUI *voicefreqenvgroup;
private:
  void cb_On2_i(Fl_Osc_Check*, void*);
  static void cb_On2(Fl_Osc_Check*, void*);
public:
  LFOUI *voicefreqlfogroup;
private:
  void cb_On3_i(Fl_Osc_Check*, void*);
  static void cb_On3(Fl_Osc_Check*, void*);
public:
  Fl_Osc_Dial *bendadjdial;
  Fl_Osc_Dial *offsethzdial;
private:
  void cb_1_i(Fl_Osc_Slider*, void*);
  static void cb_1(Fl_Osc_Slider*, void*);
public:
  Fl_Osc_Output *detunevalueoutput;
private:
  void cb_440Hz1_i(Fl_Osc_Check*, void*);
  static void cb_440Hz1(Fl_Osc_Check*, void*);
public:
  Fl_Osc_Dial *fixedfreqetdial;
private:
  void cb_Detune1_i(Fl_Osc_Choice*, void*);
  static void cb_Detune1(Fl_Osc_Choice*, void*);
public:
  Fl_Osc_Group *voiceoscil;
  Fl_Button *changevoiceoscilbutton;
private:
  void cb_changevoiceoscilbutton_i(Fl_Button*, void*);
  static void cb_changevoiceoscilbutton(Fl_Button*, void*);
public:
  Fl_Osc_Choice *extoscil;
private:
  void cb_extoscil_i(Fl_Osc_Choice*, void*);
  static void cb_extoscil(Fl_Osc_Choice*, void*);
  void cb_Frequency_i(Fl_Osc_Slider*, void*);
  static void cb_Frequency(Fl_Osc_Slider*, void*);
public:
  Fl_Osc_Output *unisonspreadoutput;
  EnvelopeUI *voiceampenvgroup;
private:
  void cb_On4_i(Fl_Osc_Check*, void*);
  static void cb_On4(Fl_Osc_Check*, void*);
public:
  LFOUI *voiceamplfogroup;
private:
  void cb_On5_i(Fl_Osc_Check*, void*);
  static void cb_On5(Fl_Osc_Check*, void*);
public:
  Fl_Group *voicefiltergroup;
  EnvelopeUI *voicefilterenvgroup;
private:
  void cb_On6_i(Fl_Osc_Check*, void*);
  static void cb_On6(Fl_Osc_Check*, void*);
public:
  LFOUI *voicefilterlfogroup;
private:
  void cb_On7_i(Fl_Osc_Check*, void*);
  static void cb_On7(Fl_Osc_Check*, void*);
public:
  Fl_Group *activeVoiceID;
private:
  void cb_2_i(Fl_Osc_Choice*, void*);
  static void cb_2(Fl_Osc_Choice*, void*);
  static Fl_Menu_Item menu_[];
public:
  Fl_Osc_Check *bypassfiltercheckbutton;
private:
  void cb_On8_i(Fl_Osc_Check*, void*);
  static void cb_On8(Fl_Osc_Check*, void*);
public:
  Fl_Box *whitenoiselabel;
  Fl_Box *pinknoiselabel;
  Fl_Osc_Check *voiceonbutton;
private:
  void cb_voiceonbutton_i(Fl_Osc_Check*, void*);
  static void cb_voiceonbutton(Fl_Osc_Check*, void*);
public:
  ADvoiceUI(int x,int y, int w, int h, const char *label=0);
  void init(int nvoice_, std::string loc_, Fl_Osc_Interface *osc_);
  ~ADvoiceUI();
  void change_voice(int nvoice_);
private:
  int nvoice; 
  OscilEditor *oscedit; 
  Fl_Oscilloscope *osc; 
  Fl_Oscilloscope *oscFM; 
  std::string loc; 
  std::string base; 
  Fl_Osc_Interface *osc_i; 
};
#include <FL/Fl_Counter.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>

class ADnoteUI : public PresetsUI_ {
  Fl_Osc_Window* make_window();
public:
  Fl_Osc_Window *ADnoteGlobalParameters;
  EnvelopeUI *freqenv;
  Fl_Osc_Counter *octave;
  Fl_Osc_Counter *coarsedet;
  LFOUI *freqlfo;
  Fl_Osc_Slider *detune;
private:
  void cb_detune_i(Fl_Osc_Slider*, void*);
  static void cb_detune(Fl_Osc_Slider*, void*);
public:
  Fl_Osc_Output *detunevalueoutput;
  Fl_Osc_Choice *detunetype;
private:
  void cb_detunetype_i(Fl_Osc_Choice*, void*);
  static void cb_detunetype(Fl_Osc_Choice*, void*);
  void cb_relBW_i(Fl_Osc_Dial*, void*);
  static void cb_relBW(Fl_Osc_Dial*, void*);
public:
  Fl_Osc_VSlider *volume;
  Fl_Osc_VSlider *vsns;
  Fl_Osc_Dial *pan;
  Fl_Osc_Dial *pstr;
  Fl_Osc_Dial *pt;
  Fl_Osc_Dial *pstc;
  Fl_Osc_Dial *pvel;
  EnvelopeUI *ampenv;
  LFOUI *amplfo;
  Fl_Osc_Check *rndgrp;
  EnvelopeUI *filterenv;
  LFOUI *filterlfo;
  FilterUI *filterui;
  Fl_Osc_Check *stereo;
private:
  void cb_Show_i(Fl_Button*, void*);
  static void cb_Show(Fl_Button*, void*);
  void cb_Show1_i(Fl_Button*, void*);
  static void cb_Show1(Fl_Button*, void*);
  void cb_Close_i(Fl_Button*, void*);
  static void cb_Close(Fl_Button*, void*);
  void cb_Resonance_i(Fl_Button*, void*);
  static void cb_Resonance(Fl_Button*, void*);
  void cb_C_i(Fl_Button*, void*);
  static void cb_C(Fl_Button*, void*);
  void cb_P_i(Fl_Button*, void*);
  static void cb_P(Fl_Button*, void*);
public:
  Fl_Osc_Window *ADnoteVoice;
  ADvoiceUI *advoice;
private:
  void cb_Close1_i(Fl_Button*, void*);
  static void cb_Close1(Fl_Button*, void*);
public:
  Fl_Counter *currentvoicecounter;
private:
  void cb_currentvoicecounter_i(Fl_Counter*, void*);
  static void cb_currentvoicecounter(Fl_Counter*, void*);
  void cb_C1_i(Fl_Button*, void*);
  static void cb_C1(Fl_Button*, void*);
  void cb_P1_i(Fl_Button*, void*);
  static void cb_P1(Fl_Button*, void*);
  void cb_Show2_i(Fl_Button*, void*);
  static void cb_Show2(Fl_Button*, void*);
  void cb_Show3_i(Fl_Button*, void*);
  static void cb_Show3(Fl_Button*, void*);
public:
  Fl_Osc_Window *ADnoteVoiceList;
private:
  void cb_Close2_i(Fl_Button*, void*);
  static void cb_Close2(Fl_Button*, void*);
  void cb_Edit_i(Fl_Button*, void*);
  static void cb_Edit(Fl_Button*, void*);
public:
  ADnoteUI(std::string loc_, Fl_Osc_Interface *osc_);
  ~ADnoteUI();
  void refresh();
private:
  ResonanceUI *resui; 
  int nvoice; 
  ADvoicelistitem *voicelistitem[NUM_VOICES]; 
  std::string loc; 
  Fl_Osc_Interface *osc; 
};
#endif
