#pragma once

#include <LovyanGFX.hpp>

// ----------------------------------------------------------------------------
// Configuração do LovyanGFX para o ESP32-S3-Touch-LCD-5
// ----------------------------------------------------------------------------
class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7701S _panel_instance;
  lgfx::Bus_Parallel16 _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_CST816S _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.port = 0;
      cfg.freq_write = 80000000;
      cfg.pin_wr = 8;
      cfg.pin_rd = 9;
      cfg.pin_rs = 7;
      cfg.pin_d0 = 39; cfg.pin_d1 = 40; cfg.pin_d2 = 41; cfg.pin_d3 = 42;
      cfg.pin_d4 = 45; cfg.pin_d5 = 46; cfg.pin_d6 = 47; cfg.pin_d7 = 48;
      cfg.pin_d8 = 38; cfg.pin_d9 = 18; cfg.pin_d10 = 17; cfg.pin_d11 = 16;
      cfg.pin_d12 = 15; cfg.pin_d13 = 14; cfg.pin_d14 = 13; cfg.pin_d15 = 12;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = 6;
      cfg.pin_rst = 5;
      cfg.pin_busy = -1;
      cfg.panel_width = 480;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.bus_shared = false;
      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 3;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0; cfg.x_max = 479;
      cfg.y_min = 0; cfg.y_max = 479;
      cfg.pin_int = 4;
      cfg.pin_sda = 2;
      cfg.pin_scl = 1;
      cfg.i2c_addr = 0x15;
      cfg.i2c_port = 1;
      cfg.freq = 400000;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};

extern LGFX tft;

void setup_display();
