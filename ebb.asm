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

// OVERWORLD WINDOWS
define test $7F
define base_overworld_windows $7FF0
// Replace main menus: action, cash and status bar
define addr_win_main_menus $16B6
define addr_win_battle_bar {addr_win_main_menus}+$7D
define addr_win_status_bar {addr_win_main_menus}+$80
org $3C27C; lda #{base_overworld_windows}+{addr_win_main_menus}&$FF; ldx #{base_overworld_windows}+{addr_win_main_menus}>>8
org $3C352; dw {base_overworld_windows}+{addr_win_status_bar}
org $3C33D; dw {base_overworld_windows}+{addr_win_status_bar}
org $3C36D; dw {base_overworld_windows}+{addr_win_status_bar}
org $3C458; lda #{base_overworld_windows}+{addr_win_battle_bar}&$FF; ldx #{base_overworld_windows}+{addr_win_battle_bar}>>8
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
// Translate setup menu
define addr_win_setup $1531
org $3C3FD; lda #{base_overworld_windows}+{addr_win_setup}&$FF; ldx #{base_overworld_windows}+{addr_win_setup}>>8
org {addr_win_setup}; incbin window_setup.bin
// Replace "who" window
define addr_win_who $17F0
org $3C3D0; lda #{base_overworld_windows}+{addr_win_who}&$FF; ldx #{base_overworld_windows}+{addr_win_who}>>8
org {addr_win_who}; incbin window_who.bin
// Replace item actions menu
define addr_win_item_actions $121A
org $3C3D7; lda #{base_overworld_windows}+{addr_win_item_actions}&$FF; ldx #{base_overworld_windows}+{addr_win_item_actions}>>8
org {addr_win_item_actions}; incbin window_item_actions.bin
// Translate status menu
org $13C8; incbin window_status.bin

// BATTLE WINDOWS
// Expand battle message window
org $D48; incbin window_battle_messages.bin
// Relocate and translate battle action menu
define addr_win_battle_menu $1780
define offset_win_battle_1 $15
define offset_win_battle_2 {offset_win_battle_1}+$12
define offset_win_battle_3 {offset_win_battle_2}+$15
define offset_win_battle_3bis {offset_win_battle_3}+$14
org {addr_win_battle_menu}; incbin window_battle_menu.bin
org $C6B; dw {base_overworld_windows}+{addr_win_battle_menu} // was $8D93
org $C84; dw {base_overworld_windows}+{addr_win_battle_menu}
org $C6E; dw {base_overworld_windows}+{addr_win_battle_menu}+{offset_win_battle_1} // was $8DA6
org $C87; dw {base_overworld_windows}+{addr_win_battle_menu}+{offset_win_battle_1}
org $C71; dw {base_overworld_windows}+{addr_win_battle_menu}+{offset_win_battle_2} // was $8DB8
org $C8A; dw {base_overworld_windows}+{addr_win_battle_menu}+{offset_win_battle_2}
org $C74; dw {base_overworld_windows}+{addr_win_battle_menu}+{offset_win_battle_3} // was $8DCA
org $C8D; dw {base_overworld_windows}+{addr_win_battle_menu}+{offset_win_battle_3bis} // was $8DDC
// Expand battle enemy list
org $E18; db $16 // window size
org $E1E; db $16
org $E24; db $16
org $CB1; db $07 // window position
// Expand battle items/PSI menu
org $E2A; db $1A
org $E30; db $1A
org $E36; db $1A
org $CC1; db $03 // window position
// Expand “you can’t” battle window
org $D09; db $03 // window position for “can’t use”
org $D1D; db $03 // window position for “can’t equip”
org $D31; db $03 // window position for “not suitable for [character]”
org $E49; db $1A
org $E4F; db $1A
org $E55; db $1A
//Relocate “you can’t” messages because too long
org $D19; dw $8D93 // “can’t use” (relocated earlier, where the battle menu used to be)
org $D2D; dw $8DBB // “can’t equip” (relocated earlier, where the battle menu used to be)
org $D41; dw $8E48 // “not suitable for [character]”

// GRAPHICS
// Town map
org $56010; incbin gfx_map.bin
