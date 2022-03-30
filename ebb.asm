arch nes.cpu

// French font
define addr_main_font $5F010
org {addr_main_font}; incbin font.bin

// File selection screen
define base_file_select $2D810
define addr_win_new_game $33EC0     // original address: 339A0, now $5D free bytes there
define size_slot $1E
define offset_ptr_start_up $1C
define offset_start_up $1B
org {addr_win_new_game}; incbin window_file_select_new.bin 			// new window arrangement with more room
org $33885; dw {addr_win_new_game}-{base_file_select}				// pointer to slot 1
org $33889; dw {addr_win_new_game}-{base_file_select}+{size_slot} 	// pointer to slot 2
org $3388D; dw {addr_win_new_game}-{base_file_select}+{size_slot}*2	// pointer to slot 3
// Pointers to "Start up" IN the bin file
org {addr_win_new_game}+{offset_ptr_start_up}; dw {addr_win_new_game}-{base_file_select}+{size_slot}*2+{offset_start_up}
org {addr_win_new_game}+{offset_ptr_start_up}+{size_slot}; dw {addr_win_new_game}-{base_file_select}+{size_slot}*2+{offset_start_up}

// Translated setup menu
org $152E; incbin window_setup.bin

// OVERWORLD WINDOWS
define test $7F
define base_overworld_windows $7FF0
// Replace main menus: action, cash and status bar
define addr_win_main_menus $16B3
define addr_win_status_bar {addr_win_main_menus}+$80
org $3C27C; lda #{base_overworld_windows}+{addr_win_main_menus}&$FF; ldx #{base_overworld_windows}+{addr_win_main_menus}>>8
org $3C352; dw {base_overworld_windows}+{addr_win_status_bar}
org {addr_win_main_menus}; incbin window_main_menus.bin
// Expand items menu
org $12AC; db $03 // move 2 tiles left
org $12B4; db $17 // add 2 tiles wide
org $12BC; db $1A
org $12C4; db $1A
org $12CC; db $1A
org $12D4; db $1A
org $12DC; db $1A
// Add new larger dialog window
define addr_win_dialog_large $11E9
org {addr_win_dialog_large}; incbin window_dialog_large.bin
// Replace "who" window
define addr_win_who $17D0
org $3C3D0; lda #{base_overworld_windows}+{addr_win_who}&$FF; ldx #{base_overworld_windows}+{addr_win_who}>>8
org {addr_win_who}; incbin window_who.bin
define addr_win_item_actions $121A
org $3C3D7; lda #{base_overworld_windows}+{addr_win_item_actions}&$FF; ldx #{base_overworld_windows}+{addr_win_item_actions}>>8
org {addr_win_item_actions}; incbin window_item_actions.bin