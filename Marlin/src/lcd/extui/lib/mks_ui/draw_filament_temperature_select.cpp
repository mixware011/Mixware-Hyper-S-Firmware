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
#include "../../../../sd/cardreader.h"
#include "../../../../module/temperature.h"

extern lv_group_t *g;
static lv_obj_t *scr;

enum {
  ID_FILAMENT_TEMP_200 = 200,
  ID_FILAMENT_TEMP_210 = 210,
  ID_FILAMENT_TEMP_220 = 220,
  ID_FILAMENT_TEMP_230 = 230,
  ID_FILAMENT_TEMP_240 = 240,
  ID_FILAMENT_TEMP_250 = 250,
  ID_FILAMENT_TEMP_260 = 260,
  ID_FILAMENT_TEMP_270 = 270
};
static void filament_temperature_set(int16_t t)
{
  thermalManager.temp_hotend[uiCfg.curSprayerChoose].target = t;
  thermalManager.start_watching_hotend(uiCfg.curSprayerChoose);
  gCfgItems.filament_limit_temper = t;
}

static void filament_temperature_draw_dialog(uint8_t type) {
  lv_clear_filament_temperature_select();

  if (uiCfg.print_state == IDLE) {
    uiCfg.leveling_first_time = 1;
    lv_draw_dialog(DIALOG_TYPE_FILAMENT_WAIT_START);
  }
  else if (uiCfg.print_state == WORKING) {
    #if ENABLED(SDSUPPORT)
      card.pauseSDPrint();
      stop_print_time();
      uiCfg.print_state = PAUSING;
    #endif

    lv_draw_dialog(DIALOG_TYPE_FILAMENT_PAUSING);
  }
  else {
    lv_draw_dialog(type);
  }
}

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  switch (obj->mks_obj_id) {
    case ID_FILAMENT_TEMP_200:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_200);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_200);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
    case ID_FILAMENT_TEMP_210:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_210);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_210);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
    case ID_FILAMENT_TEMP_220:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_220);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_220);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
    case ID_FILAMENT_TEMP_230:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_230);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_230);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
    case ID_FILAMENT_TEMP_240:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_240);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_240);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
    case ID_FILAMENT_TEMP_250:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_250);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_250);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
    case ID_FILAMENT_TEMP_260:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_260);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_260);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
    case ID_FILAMENT_TEMP_270:
      if (uiCfg.filament_load_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_270);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);
      }
      else if (uiCfg.filament_unload_heat_flg) {
        filament_temperature_set(ID_FILAMENT_TEMP_270);
        filament_temperature_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
      }
      break;
  }
}

void lv_draw_filament_temperature_select(void) {
  scr = lv_screen_create(FILAMENT_TEMPERATURE_SELECT_UI);

  lv_screen_menu_item(scr, filament_temp_select.temp_200, PARA_UI_POS_X, PARA_UI_POS_Y * 1, event_handler, ID_FILAMENT_TEMP_200, 0);
  lv_screen_menu_item(scr, filament_temp_select.temp_210, PARA_UI_POS_X, PARA_UI_POS_Y * 2, event_handler, ID_FILAMENT_TEMP_210, 1);
  lv_screen_menu_item(scr, filament_temp_select.temp_220, PARA_UI_POS_X, PARA_UI_POS_Y * 3, event_handler, ID_FILAMENT_TEMP_220, 2);
  lv_screen_menu_item(scr, filament_temp_select.temp_230, PARA_UI_POS_X, PARA_UI_POS_Y * 4, event_handler, ID_FILAMENT_TEMP_230, 3);
  lv_screen_menu_item(scr, filament_temp_select.temp_240, PARA_UI_POS_X, PARA_UI_POS_Y * 5, event_handler, ID_FILAMENT_TEMP_240, 4);
  lv_screen_menu_item(scr, filament_temp_select.temp_250, PARA_UI_POS_X, PARA_UI_POS_Y * 6, event_handler, ID_FILAMENT_TEMP_250, 5);
  lv_screen_menu_item(scr, filament_temp_select.temp_260, PARA_UI_POS_X, PARA_UI_POS_Y * 7, event_handler, ID_FILAMENT_TEMP_260, 6);
  lv_screen_menu_item(scr, filament_temp_select.temp_270, PARA_UI_POS_X, PARA_UI_POS_Y * 8, event_handler, ID_FILAMENT_TEMP_270, 7);
}

void lv_clear_filament_temperature_select() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
