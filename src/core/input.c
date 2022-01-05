#include "input.h"

#include <GLFW/glfw3.h>
// #include "stb/stb_ds.h"


// ---- vars ----
 
// current window
GLFWwindow* window;

#define KEY_CALLBACK_MAX 12
key_callback key_callbacks[KEY_CALLBACK_MAX];
int          key_callbacks_len = 0;

#define UTF8_CALLBACK_MAX 12
utf8_callback utf8_callbacks[UTF8_CALLBACK_MAX];
int           utf8_callbacks_len = 0;


// key state last frame 
bool
Space_st, Apostrophe_st, Comma_st, Minus_st, Period_st, Slash_st,
Alpha0_st, Alpha1_st, Alpha2_st, Alpha3_st, Alpha4_st, Alpha5_st, Alpha6_st, Alpha7_st, Alpha8_st, Alpha9_st,
SemiColon_st, Equal_st,
A_st, B_st, C_st, D_st, E_st, F_st, G_st, H_st, I_st, J_st, K_st, L_st, M_st,
N_st, O_st, P_st, Q_st, R_st, S_st, T_st, U_st, V_st, W_st, X_st, Y_st, Z_st,
LeftBracket_st, Backslash_st, RightBracket_st, GraveAccent_st, World1_st,
World2_st, Escape_st, Enter_st, Tab_st, Backspace_st, Insert_st, Delete_st,
RightArrow_st, LeftArrow_st, DownArrow_st, UpArrow_st,
PageUp_st, PageDown_st, Home_st, End_st, CapsLock_st, ScrollLock_st, NumLock_st, PrintScreen_st, Pause_st,
F1_st, F2_st, F3_st, F4_st, F5_st, F6_st, F7_st, F8_st, F9_st, F10_st, F11_st, F12_st, F13_st, F14_st, F15_st,
F16_st, F17_st, F18_st, F19_st, F20_st, F21_st, F22_st, F23_st, F24_st, F25_st,
Numpad0_st, Numpad1_st, Numpad2_st, Numpad3_st, Numpad4_st, Numpad5_st, Numpad6_st, Numpad7_st, Numpad8_st, Numpad9_st,
NumpadDecimal_st, NumpadDivide_st, NumpadMultiply_st, NumpadSubtract_st, NumpadAdd_st, NumpadEnter_st, NumpadEqual_st,
LeftShift_st, LeftControl_st, LeftAlt_st, LeftSuper_st, RightShift_st, RightControl_st, RightAlt_st, RightSuper_st, Menu_st,
LeftWinMacSymbol_st, RightWinMacSymbol_st;

// mouse_button state last frame. 
bool mouse_button1, mouse_button2, mouse_button3, mouse_button4, mouse_button5, mouse_button6, mouse_button7, mouse_button8;

f64 mouse_x = 0;
f64 mouse_y = 0;
f64 mouse_delta_x = 0;
f64 mouse_delta_y = 0;

// empty_callback* mouse_pos_callbacks = NULL;
// int mouse_pos_callbacks_len = 0;

// --------------

void input_init()
{
    window = window_get();
    glfwSetKeyCallback(window, (GLFWkeyfun)input_key_callback);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)mouse_pos_callback);
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)mouse_callback);
    glfwSetCharCallback(window, (GLFWcharfun)input_utf8_callback);
}
void input_update()
{
    // wipe state vars
    Space_st = false;
    Apostrophe_st = false; Comma_st = false; Minus_st = false; Period_st = false; Slash_st = false;
    Alpha0_st = false; Alpha1_st = false; Alpha2_st = false; Alpha3_st = false; Alpha4_st = false; Alpha5_st = false; Alpha6_st = false; Alpha7_st = false; Alpha8_st = false; Alpha9_st = false;
    SemiColon_st = false; Equal_st = false;
    A_st = false; B_st = false; C_st = false; D_st = false; E_st = false; F_st = false; G_st = false; H_st = false; I_st = false; J_st = false; K_st = false; L_st = false; M_st = false;
    N_st = false; O_st = false; P_st = false; Q_st = false; R_st = false; S_st = false; T_st = false; U_st = false; V_st = false; W_st = false; X_st = false; Y_st = false; Z_st = false;
    LeftBracket_st = false; Backslash_st = false; RightBracket_st = false; GraveAccent_st = false; World1_st = false;
    World2_st = false; Escape_st = false; Enter_st = false; Tab_st = false; Backspace_st = false; Insert_st = false; Delete_st = false;
    RightArrow_st = false; LeftArrow_st = false; DownArrow_st = false; UpArrow_st = false;
    PageUp_st = false; PageDown_st = false; Home_st = false; End_st = false; CapsLock_st = false; ScrollLock_st = false; NumLock_st = false; PrintScreen_st = false; Pause_st = false;
    F1_st = false; F2_st = false; F3_st = false; F4_st = false; F5_st = false; F6_st = false; F7_st = false; F8_st = false; F9_st = false; F10_st = false; F11_st = false; F12_st = false; F13_st = false; F14_st = false; F15_st = false;
    F16_st = false; F17_st = false; F18_st = false; F19_st = false; F20_st = false; F21_st = false; F22_st = false; F23_st = false; F24_st = false; F25_st = false;
    Numpad0_st = false; Numpad1_st = false; Numpad2_st = false; Numpad3_st = false; Numpad4_st = false; Numpad5_st = false; Numpad6_st = false; Numpad7_st = false; Numpad8_st = false; Numpad9_st = false;
    NumpadDecimal_st = false; NumpadDivide_st = false; NumpadMultiply_st = false; NumpadSubtract_st = false; NumpadAdd_st = false; NumpadEnter_st = false; NumpadEqual_st = false;
    LeftShift_st = false; LeftControl_st = false; LeftAlt_st = false; LeftSuper_st = false; RightShift_st = false; RightControl_st = false; RightAlt_st = false; RightSuper_st = false; Menu_st = false;
    LeftWinMacSymbol_st = false; RightWinMacSymbol_st = false;

    mouse_button1 = false; mouse_button2 = false; mouse_button3 = false; mouse_button4 = false; 
    mouse_button5 = false; mouse_button6 = false; mouse_button7 = false; mouse_button8 = false;


    mouse_delta_x = 0.0;
    mouse_delta_y = 0.0;
}


input_state get_key_state(key _key)
{
	// "key" & "keystate" map directly to glfws key definitions
	return glfwGetKey(window, _key);
}

bool is_key_down(key _key)
{
	return get_key_state(_key) == STATE_PRESS;
}

bool is_key_released(key _key)
{
	return get_key_state(_key) == STATE_RELEASED;

}

bool is_key_pressed(key _key)
{
    return is_key_down(_key) && get_last_key_state(_key);
}

bool get_last_key_state(key _key)
{
    if (_key == KEY_Unknown) { return false; }
    else if (_key == KEY_Space) { return Space_st; }
    else if (_key == KEY_Apostrophe) { return Apostrophe_st; }
    else if (_key == KEY_Comma) { return Comma_st; }
    else if (_key == KEY_Minus) { return Minus_st; }
    else if (_key == KEY_Period) { return Period_st; }
    else if (_key == KEY_Slash) { return Slash_st; }
    else if (_key == KEY_Alpha0) { return Alpha0_st; }
    else if (_key == KEY_Alpha1) { return Alpha1_st; }
    else if (_key == KEY_Alpha2) { return Alpha2_st; }
    else if (_key == KEY_Alpha3) { return Alpha3_st; }
    else if (_key == KEY_Alpha4) { return Alpha4_st; }
    else if (_key == KEY_Alpha5) { return Alpha5_st; }
    else if (_key == KEY_Alpha6) { return Alpha6_st; }
    else if (_key == KEY_Alpha7) { return Alpha7_st; }
    else if (_key == KEY_Alpha8) { return Alpha8_st; }
    else if (_key == KEY_Alpha9) { return Alpha9_st; }
    else if (_key == KEY_SemiColon) { return SemiColon_st; }
    else if (_key == KEY_Equal) { return Equal_st; }
    else if (_key == KEY_A) { return A_st; }
    else if (_key == KEY_B) { return B_st; }
    else if (_key == KEY_C) { return C_st; }
    else if (_key == KEY_D) { return D_st; }
    else if (_key == KEY_E) { return E_st; }
    else if (_key == KEY_F) { return F_st; }
    else if (_key == KEY_G) { return G_st; }
    else if (_key == KEY_H) { return H_st; }
    else if (_key == KEY_I) { return I_st; }
    else if (_key == KEY_J) { return J_st; }
    else if (_key == KEY_K) { return K_st; }
    else if (_key == KEY_L) { return L_st; }
    else if (_key == KEY_M) { return M_st; }
    else if (_key == KEY_N) { return N_st; }
    else if (_key == KEY_O) { return O_st; }
    else if (_key == KEY_P) { return P_st; }
    else if (_key == KEY_Q) { return Q_st; }
    else if (_key == KEY_R) { return R_st; }
    else if (_key == KEY_S) { return S_st; }
    else if (_key == KEY_T) { return T_st; }
    else if (_key == KEY_U) { return U_st; }
    else if (_key == KEY_V) { return V_st; }
    else if (_key == KEY_W) { return W_st; }
    else if (_key == KEY_X) { return X_st; }
    else if (_key == KEY_Y) { return Y_st; }
    else if (_key == KEY_Z) { return Z_st; }
    else if (_key == KEY_LeftBracket) { return LeftBracket_st; }
    else if (_key == KEY_Backslash) { return Backslash_st; }
    else if (_key == KEY_RightBracket) { return RightBracket_st; }
    else if (_key == KEY_GraveAccent) { return GraveAccent_st; }
    else if (_key == KEY_World1) { return World1_st; }
    else if (_key == KEY_World2) { return World1_st; }
    else if (_key == KEY_Escape) { return Escape_st; }
    else if (_key == KEY_Enter) { return Enter_st; }
    else if (_key == KEY_Tab) { return Tab_st; }
    else if (_key == KEY_Backspace) { return Backspace_st; }
    else if (_key == KEY_Insert) { return Insert_st; }
    else if (_key == KEY_Delete) { return Delete_st; }
    else if (_key == KEY_RightArrow) { return RightArrow_st; }
    else if (_key == KEY_LeftArrow) { return LeftArrow_st; }
    else if (_key == KEY_DownArrow) { return DownArrow_st; }
    else if (_key == KEY_UpArrow) { return UpArrow_st; }
    else if (_key == KEY_PageUp) { return PageUp_st; }
    else if (_key == KEY_PageDown) { return PageDown_st; }
    else if (_key == KEY_Home) { return Home_st; }
    else if (_key == KEY_End) { return End_st; }
    else if (_key == KEY_CapsLock) { return CapsLock_st; }
    else if (_key == KEY_ScrollLock) { return ScrollLock_st; }
    else if (_key == KEY_NumLock) { return NumLock_st; }
    else if (_key == KEY_PrintScreen) { return PrintScreen_st; }
    else if (_key == KEY_Pause) { return Pause_st; }
    else if (_key == KEY_F1) { return F1_st; }
    else if (_key == KEY_F2) { return F2_st; }
    else if (_key == KEY_F3) { return F3_st; }
    else if (_key == KEY_F4) { return F4_st; }
    else if (_key == KEY_F5) { return F5_st; }
    else if (_key == KEY_F6) { return F6_st; }
    else if (_key == KEY_F7) { return F7_st; }
    else if (_key == KEY_F8) { return F8_st; }
    else if (_key == KEY_F9) { return F9_st; }
    else if (_key == KEY_F10) { return F10_st; }
    else if (_key == KEY_F11) { return F11_st; }
    else if (_key == KEY_F12) { return F12_st; }
    else if (_key == KEY_F13) { return F13_st; }
    else if (_key == KEY_F14) { return F14_st; }
    else if (_key == KEY_F15) { return F15_st; }
    else if (_key == KEY_F16) { return F16_st; }
    else if (_key == KEY_F17) { return F17_st; }
    else if (_key == KEY_F18) { return F18_st; }
    else if (_key == KEY_F19) { return F19_st; }
    else if (_key == KEY_F20) { return F20_st; }
    else if (_key == KEY_F21) { return F21_st; }
    else if (_key == KEY_F22) { return F22_st; }
    else if (_key == KEY_F23) { return F23_st; }
    else if (_key == KEY_F24) { return F24_st; }
    else if (_key == KEY_F25) { return F25_st; }
    else if (_key == KEY_Numpad0) { return Numpad0_st; }
    else if (_key == KEY_Numpad1) { return Numpad1_st; }
    else if (_key == KEY_Numpad2) { return Numpad2_st; }
    else if (_key == KEY_Numpad3) { return Numpad3_st; }
    else if (_key == KEY_Numpad4) { return Numpad4_st; }
    else if (_key == KEY_Numpad5) { return Numpad5_st; }
    else if (_key == KEY_Numpad6) { return Numpad6_st; }
    else if (_key == KEY_Numpad7) { return Numpad7_st; }
    else if (_key == KEY_Numpad8) { return Numpad8_st; }
    else if (_key == KEY_Numpad9) { return Numpad9_st; }
    else if (_key == KEY_NumpadDecimal) { return NumpadDecimal_st; }
    else if (_key == KEY_NumpadDivide) { return NumpadDivide_st; }
    else if (_key == KEY_NumpadMultiply) { return NumpadMultiply_st; }
    else if (_key == KEY_NumpadSubtract) { return NumpadSubtract_st; }
    else if (_key == KEY_NumpadAdd) { return NumpadAdd_st; }
    else if (_key == KEY_NumpadEnter) { return NumpadEnter_st; }
    else if (_key == KEY_NumpadEqual) { return NumpadEqual_st; }
    else if (_key == KEY_LeftShift) { return LeftShift_st; }
    else if (_key == KEY_LeftControl) { return LeftControl_st; }
    else if (_key == KEY_LeftAlt) { return LeftAlt_st; }
    else if (_key == KEY_LeftSuper) { return LeftSuper_st; } //same as returning LeftWinMacSymbol_st
    else if (_key == KEY_RightSuper) { return RightSuper_st; } //same as returning RightWinMacSymbol_st
    else if (_key == KEY_RightShift) { return RightShift_st; }
    else if (_key == KEY_RightControl) { return RightControl_st; }
    else if (_key == KEY_RightAlt) { return RightAlt_st; }
    else if (_key == KEY_Menu) { return Menu_st; }

    return false;
}

// window is type GLFWwindow*
void input_key_callback(void* window, key _key, int scancode, input_state state, int mods)
{
    if (state == STATE_PRESS && get_last_key_state(_key) == STATE_RELEASED)
    {
      for (int i = 0; i < key_callbacks_len; ++i)
      { key_callbacks[i](_key, state, mods); }
    }
    
    if (state == STATE_PRESS)
    {
        //mapping glfw keys to our keycodes
        if (_key == KEY_Unknown) { return; }
        else if (_key == KEY_Space) { Space_st = true; }
        else if (_key == KEY_Apostrophe) { Apostrophe_st = true; }
        else if (_key == KEY_Comma) { Comma_st = true; }
        else if (_key == KEY_Minus) { Minus_st = true; }
        else if (_key == KEY_Period) { Period_st = true; }
        else if (_key == KEY_Slash) { Slash_st = true; }
        else if (_key == KEY_Alpha0) { Alpha0_st = true; }
        else if (_key == KEY_Alpha1) { Alpha1_st = true; }
        else if (_key == KEY_Alpha2) { Alpha2_st = true; }
        else if (_key == KEY_Alpha3) { Alpha3_st = true; }
        else if (_key == KEY_Alpha4) { Alpha4_st = true; }
        else if (_key == KEY_Alpha5) { Alpha5_st = true; }
        else if (_key == KEY_Alpha6) { Alpha6_st = true; }
        else if (_key == KEY_Alpha7) { Alpha7_st = true; }
        else if (_key == KEY_Alpha8) { Alpha8_st = true; }
        else if (_key == KEY_Alpha9) { Alpha9_st = true; }
        else if (_key == KEY_SemiColon) { SemiColon_st = true; }
        else if (_key == KEY_Equal) { Equal_st = true; }
        else if (_key == KEY_A) { A_st = true; }
        else if (_key == KEY_B) { B_st = true; }
        else if (_key == KEY_C) { C_st = true; }
        else if (_key == KEY_D) { D_st = true; }
        else if (_key == KEY_E) { E_st = true; }
        else if (_key == KEY_F) { F_st = true; }
        else if (_key == KEY_G) { G_st = true; }
        else if (_key == KEY_H) { H_st = true; }
        else if (_key == KEY_I) { I_st = true; }
        else if (_key == KEY_J) { J_st = true; }
        else if (_key == KEY_K) { K_st = true; }
        else if (_key == KEY_L) { L_st = true; }
        else if (_key == KEY_M) { M_st = true; }
        else if (_key == KEY_N) { N_st = true; }
        else if (_key == KEY_O) { O_st = true; }
        else if (_key == KEY_P) { P_st = true; }
        else if (_key == KEY_Q) { Q_st = true; }
        else if (_key == KEY_R) { R_st = true; }
        else if (_key == KEY_S) { S_st = true; }
        else if (_key == KEY_T) { T_st = true; }
        else if (_key == KEY_U) { U_st = true; }
        else if (_key == KEY_V) { V_st = true; }
        else if (_key == KEY_W) { W_st = true; }
        else if (_key == KEY_X) { X_st = true; }
        else if (_key == KEY_Y) { Y_st = true; }
        else if (_key == KEY_Z) { Z_st = true; }
        else if (_key == KEY_LeftBracket) { LeftBracket_st = true; }
        else if (_key == KEY_Backslash) { Backslash_st = true; }
        else if (_key == KEY_RightBracket) { RightBracket_st = true; }
        else if (_key == KEY_GraveAccent) { GraveAccent_st = true; }
        else if (_key == KEY_World1) { World1_st = true; }
        else if (_key == KEY_World2) { World1_st = true; }
        else if (_key == KEY_Escape) { Escape_st = true; }
        else if (_key == KEY_Enter) { Enter_st = true; }
        else if (_key == KEY_Tab) { Tab_st = true; }
        else if (_key == KEY_Backspace) { Backspace_st = true; }
        else if (_key == KEY_Insert) { Insert_st = true; }
        else if (_key == KEY_Delete) { Delete_st = true; }
        else if (_key == KEY_RightArrow) { RightArrow_st = true; }
        else if (_key == KEY_LeftArrow) { LeftArrow_st = true; }
        else if (_key == KEY_DownArrow) { DownArrow_st = true; }
        else if (_key == KEY_UpArrow) { UpArrow_st = true; }
        else if (_key == KEY_PageUp) { PageUp_st = true; }
        else if (_key == KEY_PageDown) { PageDown_st = true; }
        else if (_key == KEY_Home) { Home_st = true; }
        else if (_key == KEY_End) { End_st = true; }
        else if (_key == KEY_CapsLock) { CapsLock_st = true; }
        else if (_key == KEY_ScrollLock) { ScrollLock_st = true; }
        else if (_key == KEY_NumLock) { NumLock_st = true; }
        else if (_key == KEY_PrintScreen) { PrintScreen_st = true; }
        else if (_key == KEY_Pause) { Pause_st = true; }
        else if (_key == KEY_F1) { F1_st = true; }
        else if (_key == KEY_F2) { F2_st = true; }
        else if (_key == KEY_F3) { F3_st = true; }
        else if (_key == KEY_F4) { F4_st = true; }
        else if (_key == KEY_F5) { F5_st = true; }
        else if (_key == KEY_F6) { F6_st = true; }
        else if (_key == KEY_F7) { F7_st = true; }
        else if (_key == KEY_F8) { F8_st = true; }
        else if (_key == KEY_F9) { F9_st = true; }
        else if (_key == KEY_F10) { F10_st = true; }
        else if (_key == KEY_F11) { F11_st = true; }
        else if (_key == KEY_F12) { F12_st = true; }
        else if (_key == KEY_F13) { F13_st = true; }
        else if (_key == KEY_F14) { F14_st = true; }
        else if (_key == KEY_F15) { F15_st = true; }
        else if (_key == KEY_F16) { F16_st = true; }
        else if (_key == KEY_F17) { F17_st = true; }
        else if (_key == KEY_F18) { F18_st = true; }
        else if (_key == KEY_F19) { F19_st = true; }
        else if (_key == KEY_F20) { F20_st = true; }
        else if (_key == KEY_F21) { F21_st = true; }
        else if (_key == KEY_F22) { F22_st = true; }
        else if (_key == KEY_F23) { F23_st = true; }
        else if (_key == KEY_F24) { F24_st = true; }
        else if (_key == KEY_F25) { F25_st = true; }
        else if (_key == KEY_Numpad0) { Numpad0_st = true; }
        else if (_key == KEY_Numpad1) { Numpad1_st = true; }
        else if (_key == KEY_Numpad2) { Numpad2_st = true; }
        else if (_key == KEY_Numpad3) { Numpad3_st = true; }
        else if (_key == KEY_Numpad4) { Numpad4_st = true; }
        else if (_key == KEY_Numpad5) { Numpad5_st = true; }
        else if (_key == KEY_Numpad6) { Numpad6_st = true; }
        else if (_key == KEY_Numpad7) { Numpad7_st = true; }
        else if (_key == KEY_Numpad8) { Numpad8_st = true; }
        else if (_key == KEY_Numpad9) { Numpad9_st = true; }
        else if (_key == KEY_NumpadDecimal) { NumpadDecimal_st = true; }
        else if (_key == KEY_NumpadDivide) { NumpadDivide_st = true; }
        else if (_key == KEY_NumpadMultiply) { NumpadMultiply_st = true; }
        else if (_key == KEY_NumpadSubtract) { NumpadSubtract_st = true; }
        else if (_key == KEY_NumpadAdd) { NumpadAdd_st = true; }
        else if (_key == KEY_NumpadEnter) { NumpadEnter_st = true; }
        else if (_key == KEY_NumpadEqual) { NumpadEqual_st = true; }
        else if (_key == KEY_LeftShift) { LeftShift_st = true; }
        else if (_key == KEY_LeftControl) { LeftControl_st = true; }
        else if (_key == KEY_LeftAlt) { LeftAlt_st = true; }
        else if (_key == KEY_LeftSuper) { LeftSuper_st = true; LeftWinMacSymbol_st = true; }
        else if (_key == KEY_RightSuper) { RightSuper_st = true; RightWinMacSymbol_st = true; }
        else if (_key == KEY_RightShift) { RightShift_st = true; }
        else if (_key == KEY_RightControl) { RightControl_st = true; }
        else if (_key == KEY_RightAlt) { RightAlt_st = true; }
        else if (_key == KEY_Menu) { Menu_st = true; }

        //additional keycodes
        else if (_key == KEY_LeftWinMacSymbol) { LeftWinMacSymbol_st = true; LeftSuper_st = true; }
        else if (_key == KEY_RightWinMacSymbol) { RightWinMacSymbol_st = true; RightSuper_st = true; }
    }

}

void input_register_key_callback(key_callback func_ptr)
{
  ASSERT(key_callbacks_len < KEY_CALLBACK_MAX -1);
  key_callbacks[key_callbacks_len++] = func_ptr;
}

void input_utf8_callback(void* window, int code)
{
  for (int i = 0; i < utf8_callbacks_len; ++i)
  { utf8_callbacks[i](code); }
}

void input_register_utf8_callback(utf8_callback func_ptr)
{
  ASSERT(utf8_callbacks_len < UTF8_CALLBACK_MAX -1);
  utf8_callbacks[utf8_callbacks_len++] = func_ptr;
}

input_state get_mouse_state(mouse_btn btn)
{
    // "key" & "keystate" map directly to glfws key definitions
    return glfwGetMouseButton(window, btn);
}

bool is_mouse_down(mouse_btn btn)
{
    return get_mouse_state(btn) == STATE_PRESS;
}

bool is_mouse_released(mouse_btn btn)
{
    return get_mouse_state(btn) == STATE_RELEASED;

}

bool is_mouse_pressed(mouse_btn btn)
{
    return is_mouse_down(btn) == STATE_PRESS && get_last_mouse_state(btn) == STATE_PRESS;
}

bool get_last_mouse_state(mouse_btn btn)
{
    if (btn == MOUSE_button1) { return mouse_button1; }
    else if (btn == MOUSE_button2) { return mouse_button2; }
    else if (btn == MOUSE_button3) { return mouse_button3; }
    else if (btn == MOUSE_button4) { return mouse_button4; }
    else if (btn == MOUSE_button5) { return mouse_button5; }
    else if (btn == MOUSE_button6) { return mouse_button6; }
    else if (btn == MOUSE_button7) { return mouse_button7; }
    else if (btn == MOUSE_button8) { return mouse_button8; }
    return false;
}

// window is type GLFWwindow*
void mouse_callback(void* window, mouse_btn button, input_state state, int mods)
{
    // actions GLFW_PRESS & GLFW_RELEASE map directly to bool
    if (button == MOUSE_button1)      { mouse_button1 = state; }
    else if (button == MOUSE_button2) { mouse_button2 = state; }
    else if (button == MOUSE_button3) { mouse_button3 = state; }
    else if (button == MOUSE_button4) { mouse_button4 = state; }
    else if (button == MOUSE_button5) { mouse_button5 = state; }
    else if (button == MOUSE_button6) { mouse_button6 = state; }
    else if (button == MOUSE_button7) { mouse_button7 = state; }
    else if (button == MOUSE_button8) { mouse_button8 = state; }
}

f64 get_mouse_x()
{
    return mouse_x;
}
f64 get_mouse_y()
{
    return mouse_y;
}
void get_mouse_pos(f64* x, f64* y)
{
    *x = mouse_x;
    *y = mouse_y;
}
f64 get_mouse_delta_x()
{
    return mouse_delta_x;
}
f64 get_mouse_delta_y()
{
    return mouse_delta_y;
}
void get_mouse_delta(f64* x, f64* y)
{
    *x = mouse_delta_x;
    *y = mouse_delta_y;
}

void center_cursor_pos()
{
    int w, h;
    window_get_size(&w, &h);
    glfwSetCursorPos(window_get(), (double)w / 2, (double)h / 2);
    mouse_x = (double)w / 2;
    mouse_y = (double)h / 2;
    mouse_delta_x = 0; mouse_delta_y = 0;
}

void set_cursor_visible(bool visible)
{
    if (visible)
    {
        glfwSetInputMode(window_get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(window_get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

// window is type GLFWwindow*
void mouse_pos_callback(void* window, double xpos, double ypos)
{
    /*
    mouse_delta_x = xpos - mouse_x;
    mouse_delta_y = mouse_y - ypos; // for some reason y is invers, prob because opengl is weird about coordinates

    mouse_x = xpos;
    mouse_y = ypos;

    // invoke callbacks
    for (int i = 0; i < mouse_pos_callbacks_len; ++i)
    {
        mouse_pos_callbacks[i]();
    }
    */
}

int register_mouse_pos_callback(empty_callback func)
{
    // arrput(mouse_pos_callbacks, func);
    // mouse_pos_callbacks_len++;
    // return mouse_pos_callbacks_len - 1;
    return -1;
}
void remove_mouse_pos_callback(int idx)
{
    // arrdel(mouse_pos_callbacks, idx);
    // mouse_pos_callbacks_len--;
}
