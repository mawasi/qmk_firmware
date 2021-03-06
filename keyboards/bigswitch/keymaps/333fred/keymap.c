#include QMK_KEYBOARD_H

typedef enum {
    SINGLE_TAP, SINGLE_HOLD, DOUBLE, TRIPLE, QUAD
} tap_dance_state_enum;

enum {
  TD_KEY = 0
};

static tap_dance_state_enum tap_dance_state;
static bool tap_dance_active = false;
static uint16_t timer;

void dance_cycle(bool override_timer) {
  if (tap_dance_active)
  {
    if (timer_elapsed(timer) > 100 || override_timer)
    {
      switch (tap_dance_state)
      {
        case SINGLE_HOLD:
        {
          rgblight_increase_hue_noeeprom();
          break;
        }

        case DOUBLE:
        {
          rgblight_step_noeeprom();
          break;
        }

        case TRIPLE:
        {
          rgblight_toggle_noeeprom();
          break;
        }

        default:
          // Not needed
          break;
      }

      timer = timer_read();
    }
  }
}

void dance_finished(qk_tap_dance_state_t *state, void* user_data) {
  // Determine the current state
  switch (state->count)
  {
    case 1:
    {
      if (state->interrupted || state->pressed == 0) tap_dance_state = SINGLE_TAP;
      else tap_dance_state = SINGLE_HOLD;
      break;
    }
    case 2:
    {
      tap_dance_state = DOUBLE;
      break;
    }
    case 3:
    {
      tap_dance_state = TRIPLE;
      break;
    }
    default:
    {
      tap_dance_state = QUAD;
      break;
    }
  }

  switch (tap_dance_state)
  {
    case SINGLE_TAP:
    {
      // VS Build: CTRL+SHIFT+B
      send_string_with_delay_P(PSTR(SS_DOWN(X_LCTRL) SS_DOWN(X_LSHIFT) "b" SS_UP(X_LSHIFT) SS_UP(X_LCTRL)), 10);
      tap_dance_active = false;
      break;
    }

    case SINGLE_HOLD:
    case DOUBLE:
    case TRIPLE:
    {
      // These are handled by the matrix_scan, which will register the appropriate rgb
      // functions every scan
      tap_dance_active = true;
      timer = timer_read();
      dance_cycle(true);
      break;
    }

    case QUAD:
    {
      // Reprogram
      reset_keyboard();
      break;
    }
  }
}

void dance_reset(qk_tap_dance_state_t *state, void* user_data)
{
  tap_dance_active = false;
}

qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_KEY] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_finished, dance_reset)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(TD(TD_KEY))
};

void matrix_scan_user(void) {
  dance_cycle(false);
}
