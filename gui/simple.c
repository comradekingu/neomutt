/**
 * @file
 * Simple Dialog Windows
 *
 * @authors
 * Copyright (C) 2020 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @page gui_simple Simple Dialog Windows
 *
 * Simple Dialog Windows
 */

#include "config.h"
#include <stdbool.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "core/lib.h"
#include "lib.h"
#include "menu/lib.h"

/**
 * simple_config_observer - Listen for config changes affecting a Dialog - Implements ::observer_t
 */
static int simple_config_observer(struct NotifyCallback *nc)
{
  if (!nc->event_data || !nc->global_data)
    return -1;
  if (nc->event_type != NT_CONFIG)
    return 0;

  struct EventConfig *ev_c = nc->event_data;
  if (!mutt_str_equal(ev_c->name, "status_on_top"))
    return 0;

  struct MuttWindow *dlg = nc->global_data;
  window_status_on_top(dlg, NeoMutt->sub);
  return 0;
}

/**
 * dialog_create_simple_index - Create a simple index Dialog
 * @param mtype     Menu type, e.g. #MENU_ALIAS
 * @param wtype     Dialog type, e.g. #WT_DLG_ALIAS
 * @param help_data Data for the Help Bar
 * @retval ptr New Dialog Window
 */
struct MuttWindow *dialog_create_simple_index(enum MenuType mtype, enum WindowType wtype,
                                              const struct Mapping *help_data)
{
  struct MuttWindow *dlg =
      mutt_window_new(wtype, MUTT_WIN_ORIENT_VERTICAL, MUTT_WIN_SIZE_MAXIMISE,
                      MUTT_WIN_SIZE_UNLIMITED, MUTT_WIN_SIZE_UNLIMITED);
  dlg->help_menu = mtype;
  dlg->help_data = help_data;

  struct MuttWindow *win_index = menu_new_window(mtype, NeoMutt->sub);
  dlg->focus = win_index;
  dlg->wdata = win_index->wdata;

  struct MuttWindow *win_sbar = sbar_create(dlg);
  const bool c_status_on_top = cs_subset_bool(NeoMutt->sub, "status_on_top");
  if (c_status_on_top)
  {
    mutt_window_add_child(dlg, win_sbar);
    mutt_window_add_child(dlg, win_index);
  }
  else
  {
    mutt_window_add_child(dlg, win_index);
    mutt_window_add_child(dlg, win_sbar);
  }

  notify_observer_add(NeoMutt->notify, NT_CONFIG, simple_config_observer, dlg);
  dialog_push(dlg);

  return dlg;
}

/**
 * dialog_destroy_simple_index - Destroy a simple index Dialog
 * @param ptr Dialog Window to destroy
 */
void dialog_destroy_simple_index(struct MuttWindow **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct MuttWindow *dlg = *ptr;

  dialog_pop();
  notify_observer_remove(NeoMutt->notify, simple_config_observer, dlg);
  mutt_window_free(ptr);
}
