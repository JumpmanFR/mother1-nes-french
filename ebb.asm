arch nes.cpu

define addr_main_font $5F010
org {addr_main_font}; incbin font.bin

org $152E; incbin window_setup.bin

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