#ifndef DMDMESSAGE_H
#define DMDMESSAGE_H

#include "DMD.h"

class DMDMessage {
public:
  DMDMessage(const String& message) : m_message(message), m_message_width(0), m_message_center_offset(0) {  
  }

  void calculcate(DMD& dmd, int total_width) {
    for (int i=0; i<m_message.length(); i++) {
      m_message_width += dmd.charWidth(m_message.charAt(i));
    }
  
    m_message_center_offset = (total_width - m_message_width) / 2;
  }

  void draw(DMD& dmd, int x, int y, byte gfx_mode) {
    dmd.drawString(x, y, m_message.c_str(), m_message.length(), gfx_mode);
  }

  void drawCenter(DMD& dmd, byte gfx_mode) {
    dmd.drawString(m_message_center_offset, 0, m_message.c_str(), m_message.length(), gfx_mode);
  }

  void drawMarquee(DMD& dmd) {
    dmd.drawMarquee(m_message.c_str(), m_message.length(), (DMD_PIXELS_ACROSS*DISPLAYS_ACROSS)-1,0);
  }

  String m_message;
  int m_message_width;
  int m_message_center_offset;
};


#endif
