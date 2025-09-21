#pragma once

#include <lvgl.h>
#include "LGFX_setup.h"
#include <esp_heap_caps.h>

static const uint32_t screenWidth = 480;
static const uint32_t screenHeight = 480;
static lv_disp_draw_buf_t draw_buf;

/* Callback para enviar o buffer de desenho para o display */
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);
  lv_disp_flush_ready(disp);
}

/* Callback para ler os dados do touch */
static void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);

  data->point.x = touchX;
  data->point.y = touchY;
  data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

/* Inicializa os drivers do LVGL */
static void setup_lvgl_drivers(void)
{
  lv_init();

  // Otimização: Alocar os buffers de desenho na PSRAM para libertar a RAM interna.
  // Usamos double buffering para melhor performance.
  // Um tamanho de 1/10 do ecrã por buffer é um bom ponto de partida.
  const uint32_t buf_size = screenWidth * screenHeight / 10;

  // Aloca o primeiro buffer na PSRAM
  lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!buf1) {
    LV_LOG_ERROR("Falha ao alocar o buffer de desenho 1 na PSRAM!");
  }

  // Aloca o segundo buffer na PSRAM
  lv_color_t* buf2 = (lv_color_t*)heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!buf2) {
    LV_LOG_ERROR("Falha ao alocar o buffer de desenho 2 na PSRAM!");
  }
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, buf_size);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
}
