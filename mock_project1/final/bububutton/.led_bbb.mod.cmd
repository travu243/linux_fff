savedcmd_drivers/led_bbb/led_bbb.mod := printf '%s\n'   led_bbb.o | awk '!x[$$0]++ { print("drivers/led_bbb/"$$0) }' > drivers/led_bbb/led_bbb.mod
