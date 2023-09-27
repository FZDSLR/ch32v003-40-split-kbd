#ifndef _KZ_USER_KEYMAP_H
#define _KZ_USER_KEYMAP_H

#include "kz_keycode.h"
#include <stdint.h>

enum layer_names {
  _BASIC,
  _ONE,
  _FUNC,
};


const uint16_t kz_user_kbd_array1[][4*6]={
        [_BASIC] = {
            KC_TAB , KC_Q , KC_W , KC_E ,KC_R ,KC_T ,
            KC_ESC , KC_A , KC_S , KC_D ,KC_F ,KC_G ,
            KC_LSFT, KC_Z , KC_X , KC_C ,KC_V ,KC_B ,
            KC_LCTL,KC_LGUI,MO(_FUNC),KC_LALT,KC_SPC,LT(1,KC_SPC),
        },
        [_ONE] = {
            KC_GRV , KC_1, KC_2, KC_3 ,KC_4, KC_TRNS,
            KC_TRNS, KC_5, KC_6, KC_7, KC_8, KC_TRNS,
            KC_TRNS, KC_9, KC_0, KC_MINS,KC_EQL,KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,
        },
        [_FUNC] = {
            KC_TRNS, KC_F1  , KC_F2  , KC_F3  , KC_F4 ,KC_TRNS,
            KC_TRNS, KC_F5  , KC_F6  , KC_F7  , KC_F8 ,KC_TRNS,
            KC_TRNS, KC_F9  , KC_F10 , KC_F11 , KC_F12,KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,
        },
};

const uint16_t kz_user_kbd_array2[][4*6]={
        [_BASIC] = {
            KC_Y , KC_U , KC_I , KC_O ,KC_P ,KC_BSPC ,
            KC_H , KC_J , KC_K , KC_L ,KC_SCLN ,KC_QUOT ,
            KC_N , KC_M , KC_COMM , KC_DOT ,KC_SLASH ,KC_ENTER ,
            LT(1,KC_SPC),KC_SPC,KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,
        },
        [_ONE] = {
            KC_TRNS, KC_TRNS, KC_TRNS, KC_9   , KC_0   ,KC_TRNS,
            KC_LEFT, KC_DOWN, KC_UP  , KC_RGHT, KC_MINS,KC_EQL,
            KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC, KC_BSLS,KC_TRNS,
            KC_TRNS, KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP,KC_END,
        },
        [_FUNC] = {
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_DEL ,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,
        },
};

#endif
