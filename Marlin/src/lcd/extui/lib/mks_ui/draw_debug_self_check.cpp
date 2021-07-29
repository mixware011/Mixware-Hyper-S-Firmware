/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "../../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ui.h"
#include <lv_conf.h>

#include "../../../../inc/MarlinConfig.h"
#include "../../../../module/temperature.h"
#include "../../../../module/motion.h"
#include "../../../../module/planner.h"
#include "../../../../gcode/gcode.h"


extern lv_group_t *g;
static lv_obj_t *scr;

static lv_obj_t *l_debug;
static lv_obj_t *l_temp_nozzle, *l_temp_bed,
                *l_heat_nozzle, *l_heat_bed,
                *l_axis_x_motor, *l_axis_y_motor,
                *l_servo;

static millis_t h_timer;

enum {
  DEBUG_NULL = 1,
  DEBUG_CHECK_TEMP_E,
  DEBUG_CHECK_TEMP_B,
  DEBUG_CHECK_HEAT_E,
  DEBUG_CHECK_HEAT_B,
  DEBUG_CHECK_HOME_X,
  DEBUG_CHECK_HOME_Y,
  DEBUG_CHECK_SERVO
};
static uint8_t debug_state = DEBUG_NULL,
               debug_state_n = DEBUG_NULL;

enum {
  ID_DEBUG_S_CHECK = 1,
  ID_DEBUG_S_ZAXIS,
  ID_DEBUG_S_RETURN
};

#define DEBUG_LABEL_POS_X   250
#define DEBUG_LABEL_POS_Y(n)   PARA_UI_POS_Y + 30 * n
#define DEBUG_LABEL_SIZE_W  50
#define DEBUG_LABEL_SIZE_H  24

static void l_selfc_reset_label() {
  lv_label_set_text(l_temp_nozzle, "--");
  lv_label_set_text(l_temp_bed, "--");
  lv_label_set_text(l_heat_nozzle, "--");
  lv_label_set_text(l_heat_bed, "--");
  lv_label_set_text(l_axis_x_motor, "--");
  lv_label_set_text(l_axis_y_motor, "--");
  lv_label_set_text(l_servo, "--");
  lv_obj_set_style(l_temp_nozzle, &tft_style_label_rel);
  lv_obj_set_style(l_temp_bed, &tft_style_label_rel);
  lv_obj_set_style(l_heat_nozzle, &tft_style_label_rel);
  lv_obj_set_style(l_heat_bed, &tft_style_label_rel);
  lv_obj_set_style(l_axis_x_motor, &tft_style_label_rel);
  lv_obj_set_style(l_axis_y_motor, &tft_style_label_rel);
  lv_obj_set_style(l_servo, &tft_style_label_rel);
}

static void l_selfc_label_create(lv_coord_t y, const char *text) {
  lv_obj_t *l_tips = lv_label_create(scr, 40, y, text);
  lv_obj_set_size(l_tips, 200, DEBUG_LABEL_SIZE_H);
}

static void l_self_timer(millis_t t) {
  millis_t last_time = millis() + t;

  while (millis() < last_time)
    watchdog_refresh();
}

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  switch (obj->mks_obj_id) {
    case ID_DEBUG_S_CHECK:
      if (debug_state_n == DEBUG_NULL) {
        l_selfc_reset_label();
        debug_state_n = DEBUG_CHECK_TEMP_E;
        lv_label_set_text(l_debug, debug_menu.selfc_checking);
      }
      // else {
      //   debug_state = debug_state_n = DEBUG_NULL;
      //   lv_label_set_text(l_debug, debug_menu.selfc_confirm);
      // }
      break;
    case ID_DEBUG_S_ZAXIS:
      lv_clear_debug_selfc();
      lv_draw_debug_zaxis();
      break;
    case ID_DEBUG_S_RETURN:
      if (debug_state == DEBUG_CHECK_HEAT_E) {
        thermalManager.setTargetHotend(0, uiCfg.desireSprayerTempBak);
      } else if (debug_state == DEBUG_CHECK_HEAT_B) {
        thermalManager.setTargetBed(uiCfg.desireSprayerTempBak);
      } else if (debug_state == DEBUG_CHECK_HOME_X || debug_state == DEBUG_CHECK_HOME_Y) {
        break;
      }
      lv_clear_cur_ui();
      lv_draw_return_ui();
      break;
  }
}

void lv_draw_debug_selfc(void) {
  scr = lv_screen_create(DEBUG_SELFC_UI);

  debug_state = debug_state_n = DEBUG_NULL;
  // Create an Image button
  l_selfc_label_create(DEBUG_LABEL_POS_Y(0), debug_menu.selfc_tips_etemp);
  l_selfc_label_create(DEBUG_LABEL_POS_Y(1), debug_menu.selfc_tips_btemp);
  l_selfc_label_create(DEBUG_LABEL_POS_Y(2), debug_menu.selfc_tips_eheat);
  l_selfc_label_create(DEBUG_LABEL_POS_Y(3), debug_menu.selfc_tips_bheat);
  l_selfc_label_create(DEBUG_LABEL_POS_Y(4), debug_menu.selfc_tips_x);
  l_selfc_label_create(DEBUG_LABEL_POS_Y(5), debug_menu.selfc_tips_y);
  l_selfc_label_create(DEBUG_LABEL_POS_Y(6), debug_menu.selfc_tips_servo);

  l_temp_nozzle   = lv_label_create(scr, DEBUG_LABEL_POS_X, DEBUG_LABEL_POS_Y(0), nullptr);
  l_temp_bed      = lv_label_create(scr, DEBUG_LABEL_POS_X, DEBUG_LABEL_POS_Y(1), nullptr);
  l_heat_nozzle   = lv_label_create(scr, DEBUG_LABEL_POS_X, DEBUG_LABEL_POS_Y(2), nullptr);
  l_heat_bed      = lv_label_create(scr, DEBUG_LABEL_POS_X, DEBUG_LABEL_POS_Y(3), nullptr);
  l_axis_x_motor  = lv_label_create(scr, DEBUG_LABEL_POS_X, DEBUG_LABEL_POS_Y(4), nullptr);
  l_axis_y_motor  = lv_label_create(scr, DEBUG_LABEL_POS_X, DEBUG_LABEL_POS_Y(5), nullptr);
  l_servo         = lv_label_create(scr, DEBUG_LABEL_POS_X, DEBUG_LABEL_POS_Y(6), nullptr);

  lv_obj_set_size(l_temp_nozzle,  DEBUG_LABEL_SIZE_W, DEBUG_LABEL_SIZE_H);
  lv_obj_set_size(l_temp_bed,     DEBUG_LABEL_SIZE_W, DEBUG_LABEL_SIZE_H);
  lv_obj_set_size(l_heat_nozzle,  DEBUG_LABEL_SIZE_W, DEBUG_LABEL_SIZE_H);
  lv_obj_set_size(l_heat_bed,     DEBUG_LABEL_SIZE_W, DEBUG_LABEL_SIZE_H);
  lv_obj_set_size(l_axis_x_motor, DEBUG_LABEL_SIZE_W, DEBUG_LABEL_SIZE_H);
  lv_obj_set_size(l_axis_y_motor, DEBUG_LABEL_SIZE_W, DEBUG_LABEL_SIZE_H);
  lv_obj_set_size(l_servo,        DEBUG_LABEL_SIZE_W, DEBUG_LABEL_SIZE_H);

  l_selfc_reset_label();

  lv_obj_t* btn_check = lv_btn_create(scr, 20, 275, 280, 64, event_handler, ID_DEBUG_S_CHECK);
  l_debug = lv_label_create(btn_check, debug_menu.selfc_confirm);
  lv_btn_set_style_both(btn_check, &style_para_button);
  lv_obj_align(l_debug, btn_check, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t* btn_zaxis = lv_btn_create(scr, 20, 275+74, 280, 64, event_handler, ID_DEBUG_S_ZAXIS);
  lv_obj_t* l_zaxis = lv_label_create(btn_zaxis, debug_menu.zaxis_title);
  lv_btn_set_style_both(btn_zaxis, &style_para_button);
  lv_obj_align(l_zaxis, btn_zaxis, LV_ALIGN_CENTER, 0, 0);

  lv_screen_menu_item_return(scr, event_handler, ID_DEBUG_S_RETURN);
}

void dis_cur_debug() {
  static float cur_temp;

  if (debug_state != debug_state_n) {
    debug_state = debug_state_n;
    switch (debug_state) {
      case DEBUG_NULL:
        break;
      case DEBUG_CHECK_TEMP_E:
        if (thermalManager.temp_hotend[0].celsius > -10) {
          lv_label_set_text(l_temp_nozzle, "OK");
        }
        else {
          lv_label_set_text(l_temp_nozzle, "NG");
          lv_obj_set_style(l_temp_nozzle, &tft_style_label_HT);
        }

        debug_state_n++;
        break;
      case DEBUG_CHECK_TEMP_B:
        if (thermalManager.temp_bed.celsius > -10) {
          lv_label_set_text(l_temp_bed, "OK");
        }
        else {
          lv_label_set_text(l_temp_bed, "NG");
          lv_obj_set_style(l_temp_bed, &tft_style_label_HT);
        }

        debug_state_n++;
        break;
      case DEBUG_CHECK_HEAT_E:
        cur_temp = thermalManager.degHotend(0);

        if (cur_temp < -10) {
          lv_label_set_text(l_heat_nozzle, "NG");
          lv_obj_set_style(l_heat_nozzle, &tft_style_label_HT);

          debug_state_n++;
        }
        else {
          uiCfg.desireSprayerTempBak = thermalManager.degTargetHotend(0);
          if (uiCfg.desireSprayerTempBak < 245)
            thermalManager.temp_hotend[0].target = cur_temp+15;
          else
            thermalManager.temp_hotend[0].target = 0;

          thermalManager.start_watching_hotend(0);

          h_timer = millis() + 20000;
        }
        break;
      case DEBUG_CHECK_HEAT_B:
        cur_temp = thermalManager.degBed();

        if (cur_temp < -10) {
          lv_label_set_text(l_heat_bed, "NG");
          lv_obj_set_style(l_heat_bed, &tft_style_label_HT);

          debug_state_n++;
        }
        else {
          uiCfg.desireSprayerTempBak = thermalManager.degTargetBed();
          if (uiCfg.desireSprayerTempBak < 90)
            thermalManager.setTargetBed(100);
          else
            thermalManager.setTargetBed(0);

          h_timer = millis() + 20000;
        }
        break;
      case DEBUG_CHECK_HOME_X:
        gcode.process_subcommands_now("G91\nG1 X10 F1200\nG90");
        while(planner.has_blocks_queued());
        if (READ(X_MIN_PIN) == !X_MIN_ENDSTOP_INVERTING) {
          lv_label_set_text(l_axis_x_motor, "NG");
          lv_obj_set_style(l_axis_x_motor, &tft_style_label_HT);
          debug_state_n++;
          break;
        }

        gcode.process_subcommands_now("G28X");
        while(planner.has_blocks_queued());
        if (READ(X_MIN_PIN) == X_MIN_ENDSTOP_INVERTING) {
          lv_label_set_text(l_axis_x_motor, "NG");
          lv_obj_set_style(l_axis_x_motor, &tft_style_label_HT);
          debug_state_n++;
          break;
        }

        gcode.process_subcommands_now("G0 X5 F1500");
        while(planner.has_blocks_queued());
        if (READ(X_MIN_PIN) == !X_MIN_ENDSTOP_INVERTING) {
          lv_label_set_text(l_axis_x_motor, "NG");
          lv_obj_set_style(l_axis_x_motor, &tft_style_label_HT);
          debug_state_n++;
          break;
        }

        lv_label_set_text(l_axis_x_motor, "OK");
        debug_state_n++;
        break;
      case DEBUG_CHECK_HOME_Y:
        gcode.process_subcommands_now("G91\nG1 Y10 F1200\nG90");
        while(planner.has_blocks_queued());
        if (READ(Y_MIN_PIN) == !Y_MIN_ENDSTOP_INVERTING) {
          lv_label_set_text(l_axis_y_motor, "NG");
          lv_obj_set_style(l_axis_y_motor, &tft_style_label_HT);
          debug_state_n++;
          break;
        }

        gcode.process_subcommands_now("G28Y");
        while(planner.has_blocks_queued());
        if (READ(Y_MIN_PIN) == Y_MIN_ENDSTOP_INVERTING) {
          lv_label_set_text(l_axis_y_motor, "NG");
          lv_obj_set_style(l_axis_y_motor, &tft_style_label_HT);
          debug_state_n++;
          break;
        }

        gcode.process_subcommands_now("G0 Y5 F1500");
        while(planner.has_blocks_queued());
        if (READ(Y_MIN_PIN) == !Y_MIN_ENDSTOP_INVERTING) {
          lv_label_set_text(l_axis_y_motor, "NG");
          lv_obj_set_style(l_axis_y_motor, &tft_style_label_HT);
          debug_state_n++;
          break;
        }

        lv_label_set_text(l_axis_y_motor, "OK");
        debug_state_n++;
        break;
      case DEBUG_CHECK_SERVO:
        if (READ(Z2_MIN_PIN) == false) {
          lv_label_set_text(l_servo, "OK");
        }
        else {
          lv_label_set_text(l_servo, "NG");
          lv_obj_set_style(l_servo, &tft_style_label_HT);
        }

        debug_state = debug_state_n = DEBUG_NULL;
        lv_label_set_text(l_debug, debug_menu.selfc_confirm);
        break;
    }
  }

  if (debug_state != DEBUG_NULL && debug_state_n != DEBUG_NULL) {
    static char debug_buf[30] = { 0 };
    const char debug_checking_buf[3][8] = { ".", "..", "..."};
    static uint8_t index = 0;
    sprintf_P(debug_buf, "%s%s", debug_menu.selfc_checking, debug_checking_buf[index]);
    if (index++ >= 3)
      index = 0;
    lv_label_set_text(l_debug, debug_buf);

    if (debug_state == debug_state_n) {
      if (debug_state == DEBUG_CHECK_HEAT_E) {
        if (millis() < h_timer)
          return;

        if (abs(thermalManager.temp_hotend[0].celsius - cur_temp) >= 5) {
          lv_label_set_text(l_heat_nozzle, "OK");
        }
        else {
          lv_label_set_text(l_heat_nozzle, "NG");
          lv_obj_set_style(l_heat_nozzle, &tft_style_label_HT);
        }

        thermalManager.setTargetHotend(0, uiCfg.desireSprayerTempBak);
        debug_state_n++;
      }
      else if (debug_state == DEBUG_CHECK_HEAT_B) {
        if (millis() < h_timer)
          return;

        if ((thermalManager.temp_bed.celsius - cur_temp) >= 3) {
          lv_label_set_text(l_heat_bed, "OK");
        }
        else {
          lv_label_set_text(l_heat_bed, "NG");
          lv_obj_set_style(l_heat_bed, &tft_style_label_HT);
        }

        thermalManager.setTargetBed(uiCfg.desireSprayerTempBak);
        debug_state_n++;
      }
    }
  }
}

void lv_clear_debug_selfc() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
