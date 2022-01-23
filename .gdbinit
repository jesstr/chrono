target remote localhost:3333
#load build/chrono-bar.elf
file build/chrono-bar.elf

monitor reset halt

define re
	monitor reset halt
end

define pprint
    printf "--------------------\n"
    printf "%s\n", $arg0
    printf "--------------------\n"
end